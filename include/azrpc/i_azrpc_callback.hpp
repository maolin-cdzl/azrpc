#pragma once

#include <google/protobuf/message.h>
#include "azrpc/azrpc_types.hpp"

namespace azrpc {

class IAzRpcCallback {
public:
	virtual ~IAzRpcCallback() { }
	virtual void onResponse(RpcError err,const std::string* err_msg,const google::protobuf::Message* response) = 0;
};

}

