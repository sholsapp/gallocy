#ifndef GALLOCY_UTILS_HTTP_H_
#define GALLOCY_UTILS_HTTP_H_

#include "gallocy/allocators/internal.h"
#include "restclient.h"

typedef uint64_t FutureResponse;

namespace utils {
/**
 * Make many get requests asynchronously.
 *
 * Upon receiving a majority of responses this function will signal the caller
 * using the supplied pthread_cond_t parameter.
 */
int get_many(const gallocy::string &path, const gallocy::vector<gallocy::string> &peers, uint16_t port);
/**
 * Make many post requests asynchronously.
 *
 * Upon receiving a majority of responses this function will signal the caller
 * using the supplied pthread_cond_t parameter.
 */
int post_many(const gallocy::string &path, const gallocy::vector<gallocy::string> &peers, uint16_t port, gallocy::string json_body, std::function<bool(const RestClient::Response &)>);

}  // namespace utils

#endif  // GALLOCY_UTILS_HTTP_H_
