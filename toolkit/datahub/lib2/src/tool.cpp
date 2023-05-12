#include "ypc/datahub/tool.h"
namespace ypc {
namespace datahub {

using stx_status = stbox::stx_status;
using ntt = ypc::nt<ypc::bytes>;
using crypto_ptr_t = std::shared_ptr<crypto_base>;
tool::tool(crypto_base *crypto) : m_crypto_ptr(crypto) {}

void tool::write_batch(ypc::simple_sealed_file &sf,
                       const std::vector<ypc::bytes> &batch,
                       const ypc::bytes &public_key) {
    ntt::batch_data_pkg_t pkg;
    ypc::bytes s;
    ypc::bytes batch_str =
    ypc::make_bytes<ypc::bytes>::for_package<ntt::batch_data_pkg_t,
                                             ntt::batch_data>(batch);
    uint32_t status = m_crypto_ptr->encrypt_message_with_prefix(
    public_key, batch_str, ypc::utc::crypto_prefix_arbitrary, s);
    if (status != 0u) {
        std::stringstream ss;
        ss << "encrypt "
        << " data fail: " << stbox::status_string(status);
        LOG(ERROR) << ss.str();
        std::cerr << ss.str();
        exit(1);
    }
    sf.write_item(s);
}
uint32_t tool::seal_file(const std::string &plugin,
                   const std::string &file, const std::string &sealed_file_path,
                   const ypc::bytes &public_key, ypc::bytes &data_hash) {
    // Read origin file use sgx to seal file
    privacy_data_reader reader(plugin, file);
    ypc::simple_sealed_file sf(sealed_file_path, false);
    // std::string k(file);
    // k = k + std::string(sealer_path);

    // magic string here!
    m_crypto_ptr->hash_256(bytes("Fidelius"), data_hash);

    bytes item_data = reader.read_item_data();
    if (item_data.size() > ypc::utc::max_item_size) {
        std::cerr << "only support item size that smaller than "
                  << ypc::utc::max_item_size << " bytes!" << std::endl;
        return 1;
    }
    uint64_t item_number = reader.get_item_number();

    std::cout << "Reading " << item_number << " items ..." << std::endl;
    boost::progress_display pd(item_number);
    uint counter = 0;
    std::vector<ypc::bytes> batch;
    size_t batch_size = 0;
    while (!item_data.empty() && counter < item_number) {
        batch.push_back(item_data);
        batch_size += item_data.size();
        if (batch_size >= ypc::utc::max_item_size) {
            write_batch(sf, batch, public_key);
            batch.clear();
            batch_size = 0;
        }

        ypc::bytes k = data_hash + item_data;
        m_crypto_ptr->hash_256(k, data_hash);

        item_data = reader.read_item_data();
        if (item_data.size() > ypc::utc::max_item_size) {
            std::cerr << "only support item size that smaller than "
                      << ypc::utc::max_item_size << " bytes!" << std::endl;
            return 1;
        }
        ++pd;
        ++counter;
    }
    if (!batch.empty()) {
        write_batch(sf, batch, public_key);
        batch.clear();
        batch_size = 0;
    }

    std::cout << "data hash: " << data_hash << std::endl;
    std::cout << "\nDone read data count: " << pd.count() << std::endl;
    return 0;
}

} // namespace datahub
} // namespace ypc