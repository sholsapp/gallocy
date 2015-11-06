#include "gtest/gtest.h"


#include "request.h"


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
  "{\"foo\": \"bar\"}\r\n"
  "\r\n"
);


TEST(RequestTests, SimpleGetRequest) {
  Request request(GET_REQUEST);
  ASSERT_EQ(request.method, "GET");
  ASSERT_EQ(request.path, "/get");
  ASSERT_EQ(request.protocol, "HTTP/1.1");
  ASSERT_EQ(request.headers.size(), static_cast<uint64_t>(2));
  ASSERT_EQ(request.headers["Host"], "127.0.0.1");
  ASSERT_EQ(request.headers["User-Agent"], "gallocy");
}


TEST(RequestTests, SimpleGetRequestQuery) {
  Request request(GET_REQUEST_QUERY);
  ASSERT_EQ(request.method, "GET");
  ASSERT_EQ(request.path, "/get?query=1");
  ASSERT_EQ(request.protocol, "HTTP/1.1");
  ASSERT_EQ(request.headers.size(), static_cast<uint64_t>(2));
  ASSERT_EQ(request.headers["Host"], "127.0.0.1");
  ASSERT_EQ(request.headers["User-Agent"], "gallocy");
  // TODO(sholsapp): Add functionality to parse the query string and inspect it
  // here.
}


TEST(RequestTests, SimplePostRequest) {
  Request request(POST_REQUEST);
  ASSERT_EQ(request.method, "POST");
  ASSERT_EQ(request.path, "/post");
  ASSERT_EQ(request.protocol, "HTTP/1.1");
  ASSERT_EQ(request.headers.size(), static_cast<uint64_t>(5));
  ASSERT_EQ(request.headers["Host"], "127.0.0.1");
  ASSERT_EQ(request.headers["Connection"], "Keep-Alive");
  ASSERT_EQ(request.headers["Content-Length"], "14");
  ASSERT_EQ(request.headers["Content-Type"], "application/json");
  ASSERT_EQ(request.headers["User-Agent"], "gallocy");
  ASSERT_EQ(request.raw_body, "{\"foo\": \"bar\"}");
}
