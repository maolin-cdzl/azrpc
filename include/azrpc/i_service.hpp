#pragma once

#include <google/protobuf/descriptor.h>
#include "azrpc/azrpc_types.hpp"

namespace azrpc {

class Reply;

class IService {
public:
	virtual ~IService() { }

	virtual const google::protobuf::ServiceDescriptor* GetDescriptor() = 0;

	virtual void callMethod(
		const google::protobuf::MethodDescriptor* method,
		const std::shared_ptr<google::protobuf::Message>& request,
		const std::shared_ptr<Reply>& reply
		) = 0;

};


}

