#include <cstring>
#include <cstdlib>

#include "gtest/gtest.h"

#include "gallocy/utils/logging.h"


TEST(LoggingTests, Logging) {
  LOG_DEBUG("Testing " << "DEBUG");
  LOG_INFO("Testing " << "INFO");
  LOG_WARNING("Testing " << "WARNING");
  LOG_ERROR("Testing " << "ERROR");
  LOG_APP("Testing " << "APP");
}
