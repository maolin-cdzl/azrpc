protobuf_generate_cpp(AZRPC_PB_SRCS AZRPC_PB_HDRS proto/azrpc.proto)

add_library(azrpc STATIC
	clock.cxx
	ev_looper_adapter.cxx
	client_channel.cxx
	eventid_generator.cxx
	client_channel_builder.cxx
	clock.hpp
	ev_looper_adapter.hpp
	client_channel.hpp
	eventid_generator.hpp
	${CMAKE_SOURCE_DIR}/include/azrpc/i_azrpc_callback.hpp
	${CMAKE_SOURCE_DIR}/include/azrpc/i_client_channel.hpp
	${CMAKE_SOURCE_DIR}/include/azrpc/i_looper_adapter.hpp
	${CMAKE_SOURCE_DIR}/include/azrpc/client_channel_builder.hpp
	${AZRPC_PB_SRCS}
	${AZRPC_PB_SRCS}
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(azrpc
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(azrpc PRIVATE _POSIX_C_SOURCE=200809L )

target_link_libraries(azrpc PUBLIC
	zeputils
	czmq
	zmq
	protobuf
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
install(TARGETS azrpc DESTINATION lib/)

