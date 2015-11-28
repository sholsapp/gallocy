#include "gtest/gtest.h"


#include "gallocy/server.h"
#include "gallocy/http_router.h"
#include "gallocy/request.h"


gallocy::string GET_REQUEST(
  "GET /get HTTP/1.1\r\n"
  "Host: 127.0.0.1\r\n"
  "User-Agent: gallocy\r\n"
  "\r\n"
  "\r\n"
);


gallocy::string GET_REQUEST_QUERY(
  "GET /get?query=1 HTTP/1.1\r\n"
  "Host: 127.0.0.1\r\n"
  "User-Agent: gallocy\r\n"
  "\r\n"
  "\r\n"
);


gallocy::string POST_REQUEST(
  "POST /post HTTP/1.1\r\n"
  "Host: 127.0.0.1:8000\r\n"
  "Connection: Keep-Alive\r\n"
  "Content-Length: 14\r\n"
  "Content-Type: application/json\r\n"
  "User-Agent: gallocy\r\n"
  "\r\n"
  "{\"foo\":\"bar\"}\r\n"
  "\r\n"
);


gallocy::string RESPONSE(
  "HTTP/1.0 200 OK\r\n"
  "Content-Type: application/json\r\n"
  "Server: Gallocy-Httpd\r\n"
  "\r\n"
  "{\"foo\":\"bar\"}\r\n"
  "\r\n"
);


TEST(RequestTests, Constructors) {
  Request request1(GET_REQUEST);
  ASSERT_EQ(request1.method, "GET");
#if 0
  Request request2 = request1;
  ASSERT_EQ(request2.method, "GET");
  Request request3(request1);
  ASSERT_EQ(request3.method, "GET");
#endif
}


TEST(RequestTests, SimpleGetRequest) {
  Request request(GET_REQUEST);
  ASSERT_EQ(request.method, "GET");
  ASSERT_EQ(request.uri, "/get");
  ASSERT_EQ(request.protocol, "HTTP/1.1");
  ASSERT_EQ(request.headers.size(), static_cast<uint64_t>(2));
  ASSERT_EQ(request.headers["Host"], "127.0.0.1");
  ASSERT_EQ(request.headers["User-Agent"], "gallocy");
}


TEST(RequestTests, SimpleGetRequestQuery) {
  Request request(GET_REQUEST_QUERY);
  ASSERT_EQ(request.method, "GET");
  ASSERT_EQ(request.uri, "/get?query=1");
  ASSERT_EQ(request.protocol, "HTTP/1.1");
  ASSERT_EQ(request.headers.size(), static_cast<uint64_t>(2));
  ASSERT_EQ(request.headers["Host"], "127.0.0.1");
  ASSERT_EQ(request.headers["User-Agent"], "gallocy");
  ASSERT_EQ(request.get_params()["query"], "1");
}


TEST(RequestTests, SimplePostRequest) {
  Request request(POST_REQUEST);
  ASSERT_EQ(request.method, "POST");
  ASSERT_EQ(request.uri, "/post");
  ASSERT_EQ(request.protocol, "HTTP/1.1");
  ASSERT_EQ(request.headers.size(), static_cast<uint64_t>(5));
  ASSERT_EQ(request.headers["Host"], "127.0.0.1");
  ASSERT_EQ(request.headers["Connection"], "Keep-Alive");
  ASSERT_EQ(request.headers["Content-Length"], "14");
  ASSERT_EQ(request.headers["Content-Type"], "application/json");
  ASSERT_EQ(request.headers["User-Agent"], "gallocy");
  ASSERT_EQ(request.raw_body, "{\"foo\":\"bar\"}");
  ASSERT_EQ(request.get_json()["foo"], "bar");
}


TEST(ResponseTests, Constructors) {
  Response response1;
  response1.status_code = 200;
  response1.headers["foo"] = "bar";
  ASSERT_EQ(response1.status_code, static_cast<uint64_t>(200));
  ASSERT_EQ(response1.headers["foo"], "bar");
#if 0
  Response response2 = response1;
  ASSERT_EQ(response2.status_code, static_cast<uint64_t>(200));
  ASSERT_EQ(response2.headers["foo"], "bar");
  Response response3(response1);
  ASSERT_EQ(response3.status_code, static_cast<uint64_t>(200));
  ASSERT_EQ(response3.headers["foo"], "bar");
#endif
}


TEST(ResponseTests, SimpleResponse) {
  Response response;
  response.status_code = 200;
  response.headers["Server"] = "Gallocy-Httpd";
  response.headers["Content-Type"] = "application/json";
  gallocy::json j = { { "foo", "bar" } };
  // There is no known conversion from std::string to
  // gallocy::string... we should fix this.
  response.body = j.dump().c_str();
  ASSERT_EQ(response.str().c_str(), RESPONSE);
}


TEST(RoutingTableTests, Functors) {
  using ArgList = gallocy::vector<gallocy::string>;
  RoutingTable<std::function<Response *(ArgList *, Request *)> > t;
  Response *response =
    new (internal_malloc(sizeof(Response))) Response();
  auto f = [response](ArgList *a, Request *request) {
    gallocy::stringstream s;
    s << a->size();
    response->body = s.str();
    return response;
  };
  t.register_handler("/foo", f);
  t.register_handler("/foo/<x>", f);
  t.register_handler("/foo/<x>/bar", f);
  t.register_handler("/foo/<x>/bar/<y>", f);
  t.register_handler("/foo/<x>/baz", f);
  t.register_handler("/foo/<x>/baz/<y>", f);
  ASSERT_EQ(t.match("/foo")(nullptr)->body, "0");
  ASSERT_EQ(t.match("/foo/arg1")(nullptr)->body, "1");
  ASSERT_EQ(t.match("/foo/arg1/bar")(nullptr)->body, "1");
  ASSERT_EQ(t.match("/foo/arg1/bar/arg2")(nullptr)->body, "2");
  ASSERT_EQ(t.match("/foo/arg1/baz")(nullptr)->body, "1");
  ASSERT_EQ(t.match("/foo/arg1/baz/arg2")(nullptr)->body, "2");
  internal_free(response);
}
