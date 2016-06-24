#pragma once

#include <string>
#include <memory>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

#include "azrpc/i_azrpc_callback.hpp"

namespace azrpc {

class IAzRpcChannel {
public:
	virtual ~IAzRpcChannel() {}

	// Return 0 means success, -1 if any error
	virtual int callMethod(
			const google::protobuf::MethodDescriptor* method,
			const google::protobuf::Message* argument,
			const std::shared_ptr<IAzRpcCallback>& callback,
			int32_t timeout = 0
			) = 0;
};

}

