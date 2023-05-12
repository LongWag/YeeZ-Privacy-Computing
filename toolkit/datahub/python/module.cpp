#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "ypc/datahub/tool.h"
#include "ypc/datahub/crypto_base.h"
namespace py = pybind11;

enum crypto_base_type { intel_sgx_and_eth, chinese_sm };

auto cast = [](const py::bytes &p) -> ypc::bytes {
  std::string b = std::string(p);
  ypc::bytes sk(b.c_str(), b.size());
  return sk;
};

auto castVector = [](const std::vector<py::bytes> &p) -> std::vector<ypc::bytes> {
    int size = p.size();
    std::vector<ypc::bytes> result(size);
    for(int i = 0; i < size; i++)
    {
        const py::bytes tmp = p[i];
        std::string b = std::string(tmp);
        ypc::bytes sk(b.c_str(), b.size());
        result[i] = sk;
    }
    return result;
};

class crypto_base_wrapper {
public:
    crypto_base_wrapper(crypto_base_type cbt) : m_type(cbt) {
        switch (cbt) {
        case crypto_base_type::intel_sgx_and_eth:
            m_crypto = ypc::datahub::intel_sgx_and_eth_compatible();
            break;
        case crypto_base_type::chinese_sm:
            m_crypto = ypc::datahub::sm_compatible();
            break;
        default:
            throw std::runtime_error("not support type");
        }
    }

    uint32_t encrypt_message_with_prefix(const py::bytes &public_key, const py::bytes &data, uint32_t prefix,
                                         py::bytes &cipher) {
        std::string a(cipher);
        ypc::bytes tmp(a.c_str(), a.size());
        return m_crypto->encrypt_message_with_prefix(cast(public_key), cast(data), prefix, tmp);
    }

    uint32_t hash_256(const py::bytes &msg, py::bytes &hash) {
        std::string a(hash);
        ypc::bytes tmp(a.c_str(), a.size());
        return m_crypto->hash_256(cast(msg), tmp);
    }

    std::unique_ptr<ypc::datahub::crypto_base> m_crypto;
protected:
    crypto_base_type m_type;
};
class tool_wrapper {
public:
    tool_wrapper(std::shared_ptr<crypto_base_wrapper> crypto)
        : m_crypto(crypto), m_instance(crypto.get()->m_crypto.get()){}
    
    void write_batch(ypc::simple_sealed_file &sf, const std::vector<py::bytes> &batch,
                     const py::bytes &public_key) {
        m_instance.write_batch(sf, castVector(batch), cast(public_key));
    }

    uint32_t seal_file(const std::string &plugin, const std::string &file, const std::string &sealed_file_path,
                       const py::bytes &public_key, py::bytes &data_hash) {
        std::string a(data_hash);
        ypc::bytes tmp(a.c_str(), a.size());
        return m_instance.seal_file(plugin, file, sealed_file_path, cast(public_key), tmp);
    }

    void test(std::vector<int> a)
    {}
    
protected:
    std::shared_ptr<crypto_base_wrapper> m_crypto;
    ypc::datahub::tool m_instance;
};

PYBIND11_MODULE(pydatahub, m) {

    m.doc() = "Python module of Fidelius toolkit - terminus";
    py::enum_<crypto_base_type>(m, "CryptoBaseType")
        .value("IntelSGXAndEthCompatible", crypto_base_type::intel_sgx_and_eth)
        .value("ChineseSM", crypto_base_type::chinese_sm)
        .export_values();

    py::class_<crypto_base_wrapper, std::shared_ptr<crypto_base_wrapper>>(m, "CryptoBase")
        .def(py::init<crypto_base_type>())
        .def("encrypt_message_with_prefix", &crypto_base_wrapper::encrypt_message_with_prefix)
        .def("hash_256", &crypto_base_wrapper::hash_256);

    py::class_<tool_wrapper, std::shared_ptr<tool_wrapper>> tool(m, "Tool");
    tool.def(py::init<std::shared_ptr<crypto_base_wrapper>>())
        .def("write_batch", &tool_wrapper::write_batch,
            "Write batch", py::arg("simple_sealed_file"), py::arg("batch"), py::arg("public_key"))
        .def("test", &tool_wrapper::test)
        .def("seal_file", &tool_wrapper::seal_file,
            "Seal file", py::arg("plugin"), py::arg("file"), py::arg("sealed_file_path"), 
            py::arg("public_key"), py::arg("data_hash"));
    py::class_<ypc::internal::sealed_file_base, std::shared_ptr<ypc::internal::sealed_file_base>>(m, "SealedFileBase");
    py::class_<ypc::simple_sealed_file, ypc::internal::sealed_file_base, std::shared_ptr<ypc::simple_sealed_file>>(m, "SimpleSealedFile")
        .def(py::init<const std::string, bool>());
}