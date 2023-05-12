#include "ypc/datahub/crypto_base.h"
#include "ypc/core/sealed_file.h"
namespace ypc {
namespace datahub {
//using crypto_ptr_t = std::shared_ptr<crypto_base>;
class tool {
public:
    explicit tool(crypto_base *crypto);
    void write_batch(ypc::simple_sealed_file &sf,
                     const std::vector<ypc::bytes> &batch,
                     const ypc::bytes &public_key);
    uint32_t seal_file(const std::string &plugin,
                       const std::string &file, const std::string &sealed_file_path,
                       const ypc::bytes &public_key, ypc::bytes &data_hash);
protected:
    crypto_base *m_crypto_ptr;//TODO
};
}
}