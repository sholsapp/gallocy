#ifndef GALLOCY_UTILS_HTTP_H_
#define GALLOCY_UTILS_HTTP_H_

#include "gallocy/allocators/internal.h"

typedef uint64_t FutureResponse;

namespace utils {
/**
 * Make many requests asynchronously.
 *
 * Upon receiving a majority of responses this function will signal the caller
 * using the supplied pthread_cond_t parameter.
 */
int multirequest(const gallocy::vector<gallocy::string> &peers, uint16_t port);

}  // namespace utils

#endif  // GALLOCY_UTILS_HTTP_H_
