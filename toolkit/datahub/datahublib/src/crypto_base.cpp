#include "ypc/corecommon/crypto/gmssl.h"
#include "ypc/corecommon/crypto/stdeth.h"
#include "ypc/datahub/crypto_base.h"

namespace ypc {
namespace datahub {

std::unique_ptr<crypto_base> intel_sgx_and_eth_compatible() {
  return std::unique_ptr<crypto_base>(new crypto_tool<::ypc::crypto::eth_sgx_crypto>());
}

std::unique_ptr<crypto_base> sm_compatible() {
  return std::unique_ptr<crypto_base>(new crypto_tool<::ypc::crypto::gmssl_sgx_crypto>());
}
}
} // namespace ypc
