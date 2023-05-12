pybind11_add_module(pydatahub module.cpp)
target_link_libraries(pydatahub PRIVATE boost_system datahub glog)
install(TARGETS pydatahub
  DESTINATION "${lib_install_dir}"
  COMPONENT c_ydatahub OPTIONAL)