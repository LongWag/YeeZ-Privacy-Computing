#include "ypc/datahub/tool.h"

using stx_status = stbox::stx_status;
using namespace ypc;
using ntt = ypc::nt<ypc::bytes>;
using crypto_ptr_t = std::shared_ptr<ypc::datahub::crypto_base>;

boost::program_options::variables_map parse_command_line(int argc,
                                                         char *argv[]) {
  namespace bp = boost::program_options;
  bp::options_description all("YeeZ Privacy Data Hub options");
  bp::options_description general("General Options");
  bp::options_description seal_data_opts("Seal Data Options");

  // clang-format off
  seal_data_opts.add_options()
    ("crypto", bp::value<std::string>()->default_value("stdeth"), "choose the crypto, stdeth/gmssl")
    ("data-url", bp::value<std::string>(), "Data URL")
    ("plugin-path", bp::value<std::string>(), "shared library for reading data")
    ("use-publickey-file", bp::value<std::string>(), "public key file")
    ("use-publickey-hex", bp::value<std::string>(), "public key")
    ("sealed-data-url", bp::value<std::string>(), "Sealed data URL")
    ("output", bp::value<std::string>(), "output meta file path");


  general.add_options()
    ("help", "help message")
    ("version", "show version");

  // clang-format on

  all.add(general).add(seal_data_opts);

  boost::program_options::variables_map vm;
  boost::program_options::store(
      boost::program_options::parse_command_line(argc, argv, all), vm);

  if (vm.count("help") != 0u) {
    std::cout << all << std::endl;
    exit(-1);
  }
  if (vm.count("version") != 0u) {
    std::cout << ypc::get_ypc_version() << std::endl;
    exit(-1);
  }
  return vm;
}

int main(int argc, char *argv[]) {
  boost::program_options::variables_map vm;
  try {
    vm = parse_command_line(argc, argv);
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << "invalid cmd line parameters!" << std::endl;
    return -1;
  }
  if (vm.count("data-url") == 0u) {
    std::cerr << "data not specified!" << std::endl;
    return -1;
    }
  if (vm.count("sealed-data-url") == 0u) {
    std::cerr << "sealed data url not specified" << std::endl;
    return -1;
  }
  if (vm.count("output") == 0u) {
    std::cerr << "output not specified" << std::endl;
    return -1;
  }
  if (vm.count("plugin-path") == 0u) {
    std::cerr << "library not specified" << std::endl;
    return -1;
  }
  if ((vm.count("use-publickey-hex") == 0u) && (vm.count("use-publickey-file") == 0u)) {
    std::cerr << "missing public key, use 'use-publickey-file' or "
                 "'use-publickey-hex'"
              << std::endl;
    return -1;
  }
  if (vm.count("crypto") == 0u) {
    std::cerr << "crypto not specified" << std::endl;
    return -1;
  }

  ypc::bytes public_key;
  if (vm.count("use-publickey-hex") != 0u) {
    public_key = ypc::hex_bytes(vm["use-publickey-hex"].as<std::string>())
                     .as<ypc::bytes>();
  } else if (vm.count("use-publickey-file") != 0u) {
    boost::property_tree::ptree pt;
    boost::property_tree::json_parser::read_json(
        vm["use-publickey-file"].as<std::string>(), pt);
    public_key = pt.get<ypc::bytes>("public-key");
  }

  std::string plugin = vm["plugin-path"].as<std::string>();
  std::string data_file = vm["data-url"].as<std::string>();
  std::string output = vm["output"].as<std::string>();
  std::string sealed_data_file = vm["sealed-data-url"].as<std::string>();
  std::string crypto = vm["crypto"].as<std::string>();

  ypc::bytes data_hash;
  std::ofstream ofs;
  ofs.open(output);
  if (!ofs.is_open()) {
    std::cout << "Cannot open file " << output << "\n";
    return -1;
  }
  ofs.close();

  crypto_ptr_t crypto_ptr;
  if (crypto == "stdeth") {
    crypto_ptr = std::make_shared<ypc::datahub::crypto_tool<ypc::crypto::eth_sgx_crypto>>();
  } else if (crypto == "gmssl") {
    crypto_ptr = std::make_shared<ypc::datahub::crypto_tool<ypc::crypto::gmssl_sgx_crypto>>();
  } else {
    throw std::runtime_error("Unsupperted crypto type!");
  }
  ypc::datahub::tool m_instance(crypto_ptr.get());
  auto status = m_instance.seal_file(plugin, data_file, sealed_data_file,
                          public_key, data_hash);
  // auto status = seal_file(crypto_ptr, plugin, data_file, sealed_data_file,
  //                         public_key, data_hash);
  if (status != 0u) {
    return -1;
  }

  ofs.open(output);
  if (!ofs.is_open()) {
    std::cout << "Cannot open file " << output << "\n";
    return -1;
  }
  ofs << "data_url"
      << " = " << data_file << "\n";
  ofs << "sealed_data_url"
      << " = " << sealed_data_file << "\n";
  ofs << "public_key"
      << " = " << public_key << "\n";
  ofs << "data_id"
      << " = " << data_hash << "\n";

  privacy_data_reader reader(plugin, data_file);
  ofs << "item_num"
      << " = " << reader.get_item_number() << "\n";

  // sample and format are optional
  bytes sample = reader.get_sample_data();
  if (!sample.empty()) {
    ofs << "sample_data"
        << " = " << sample << "\n";
  }
  std::string format = reader.get_data_format();
  if (!format.empty()) {
    ofs << " data_fromat"
        << " = " << format << "\n";
  }
  ofs.close();

  std::cout << "done sealing" << std::endl;
  return 0;
}
