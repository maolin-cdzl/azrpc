
set(AZRPC_PLUGIN_ROOT ${CMAKE_BINARY_DIR}/src/plugin)

message(${CMAKE_BINARY_DIR})

function(PROTOBUF_GENERATE_AZRPC SRCS HDRS)
	set(PLUGIN_BIN ${AZRPC_PLUGIN_ROOT}/protoc-gen-cpp_azrpc)

	PROTOBUF_GENERATE_MULTI(PLUGIN "cpp_azrpc" PROTOS ${ARGN}
                          OUTPUT_STRUCT "_SRCS:.azrpc.cc;_HDRS:.azrpc.h"
                          FLAGS "--plugin=protoc-gen-cpp_azrpc=${PLUGIN_BIN}"
                          DEPENDS ${PLUGIN_BIN})
	set(${SRCS} ${_SRCS} PARENT_SCOPE)
	set(${HDRS} ${_HDRS} PARENT_SCOPE)
endfunction()

PROTOBUF_GENERATE_CPP(PROTO_PB_SRCS PROTO_PB_HDRS search.proto)
PROTOBUF_GENERATE_AZRPC(PROTO_AZRPC_SRCS PROTO_AZRPC_HDRS search.proto)

set(TEST_SRCS test_main.cxx ${PROTO_PB_SRCS} ${PROTO_PB_HDRS} ${PROTO_AZRPC_SRCS} ${PROTO_AZRPC_HDRS})
add_executable(test_main ${TEST_SRCS})
target_include_directories(test_main PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(test_main azrpc g3logger ${PROTOBUF_LIBRARIES} ev pthread)
