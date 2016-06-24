find_package(ProtobufPlugin REQUIRED)
include_directories($PROTOBUF_INCLUDE_DIR)

set(AZRPC_SRCS azrpc_cpp_generator.cxx file_generator.cxx azrpc_plugin_main.cxx azrpc_cpp_service.cxx)
add_executable(protoc-gen-cpp_azrpc ${AZRPC_SRCS})
target_link_libraries(protoc-gen-cpp_azrpc ${PROTOBUF_LIBRARIES} ${PROTOBUF_PROTOC_LIBRARIES})
install(TARGETS protoc-gen-cpp_azrpc
    RUNTIME DESTINATION bin)

