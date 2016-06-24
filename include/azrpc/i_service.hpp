#pragma once

#include <google/protobuf/descriptor.h>
#include "azrpc/azrpc_types.hpp"

namespace azrpc {


class IService {
public:
	virtual ~IService() { }

	virtual const google::protobuf::ServiceDescriptor* GetDescriptor() = 0;

	virtual const google::protobuf::Message& GetRequestPrototype(const google::protobuf::MethodDescriptor*) const = 0;

	virtual const google::protobuf::Message& GetResponsePrototype(const google::protobuf::MethodDescriptor*) const = 0;

	virtual void callMethod(
		const google::protobuf::MethodDescriptor* method,
		const google::protobuf::Message* request,
		) = 0;

};


}

