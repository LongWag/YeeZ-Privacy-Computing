#include "gtest_common.h"

ypc::bytes random_string(size_t len) {
  std::string ret(len, '0');
  static std::default_random_engine generator;
  static std::uniform_int_distribution<int> distribution(int('a'), int('z'));
  static auto rand = std::bind(distribution, generator);

  for (size_t i = 0; i < len; i++) {
    ret[i] = rand();
  }
  return ypc::bytes(ret.data(), ret.size());
}
