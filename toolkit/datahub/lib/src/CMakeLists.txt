set(src
  crypto_base.cpp
  tool.cpp
)

add_library(datahub SHARED ${src})

target_link_libraries(datahub stbox_common core core_stdeth core_gmssl)

target_include_directories(datahub PUBLIC
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
  "$<BUILD_INTERFACE:${FF_INCLUDE_DIR}>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
  )

install(TARGETS datahub EXPORT mod_datahub
  DESTINATION "${lib_install_dir}"
  COMPONENT datahub)
install(EXPORT mod_datahub
  DESTINATION "${config_install_dir}/toolkit"
  NAMESPACE "${namespace}"
  COMPONENT datahub
  )

AddClangTidy(datahub)
EnableCoverage(datahub)
