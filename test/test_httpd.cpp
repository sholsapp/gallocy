#include "gtest/gtest.h"


#include "httpd.h"


gallocy::string GET_REQUEST(
  "GET /foo HTTP/1.1\r\n"
  "Host: 127.0.0.1\r\n"
  "User-Agent: gallocy\r\n"
  "\r\n"
  "\r\n"
);


TEST(RequestTests, SimpleGetRequest) {
  Request request(GET_REQUEST);
  ASSERT_EQ(request.method, "GET");
  ASSERT_EQ(request.path, "/foo");
  ASSERT_EQ(request.protocol, "HTTP/1.1");
  ASSERT_EQ(request.headers.size(), 2);
  ASSERT_EQ(request.headers["Host"], "127.0.0.1");
  ASSERT_EQ(request.headers["User-Agent"], "gallocy");
}
