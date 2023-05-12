#pragma once
#include "ypc/common/crypto_prefix.h"
#include "ypc/common/limits.h"
#include "ypc/core/ntobject_file.h"
#include "ypc/core/privacy_data_reader.h"
#include "ypc/core/sealed_file.h"
#include "ypc/core/version.h"
#include "ypc/core/status.h"
#include "ypc/core/byte.h"
#include "ypc/corecommon/crypto/gmssl.h"
#include "ypc/corecommon/crypto/stdeth.h"
#include "ypc/corecommon/nt_cols.h"
#include <boost/program_options.hpp>
#include <boost/progress.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <exception>
#include <fstream>
#include <iostream>
#include <thread>

//using stx_status = stbox::stx_status;
//using ntt = ypc::nt<ypc::bytes>;
// using crypto_ptr_t = std::shared_ptr<crypto_base>;
namespace ypc {
namespace datahub {
class crypto_base {
public:
  virtual uint32_t encrypt_message_with_prefix(const ypc::bytes &public_key,
                                               const ypc::bytes &data,
                                               uint32_t prefix,
                                               ypc::bytes &cipher) = 0;
  virtual uint32_t hash_256(const ypc::bytes &msg, ypc::bytes &hash) = 0;
};

template <typename Crypto> class crypto_tool : public crypto_base {
public:
  using crypto_t = Crypto;
  virtual uint32_t encrypt_message_with_prefix(const ypc::bytes &public_key,
                                               const ypc::bytes &data,
                                               uint32_t prefix,
                                               ypc::bytes &cipher) {
    return crypto_t::encrypt_message_with_prefix(public_key, data, prefix, cipher);
  }
  virtual uint32_t hash_256(const ypc::bytes &msg, ypc::bytes &hash) {
    return crypto_t::hash_256(msg, hash);
  }
};

std::unique_ptr<crypto_base> intel_sgx_and_eth_compatible();
std::unique_ptr<crypto_base> sm_compatible();

}//namespace datahub
}//namespace ypc