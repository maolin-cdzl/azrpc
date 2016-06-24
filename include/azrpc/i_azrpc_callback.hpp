#pragma once

#include <google/protobuf/message.h>

namespace azrpc {

typedef int RpcError;

// must same as protocol defined
static const RpcError OK					= 0;
static const RpcError TIMEOUT				= 1;	
static const RpcError WOULD_BLOCK			= 2;
static const RpcError CANCELLED				= 3;
static const RpcError TERMINATED			= 4;
static const RpcError NO_SUCH_SERVICE		= 5;
static const RpcError NO_SUCH_METHOD		= 6;
static const RpcError NO_IMPLEMENTED		= 7;
static const RpcError PROTOCOL_ERROR		= 8;

class IAzRpcCallback {
public:
	virtual ~IAzRpcCallback() { }
	virtual void onResponse(RpcError err,const std::string* err_msg,const google::protobuf::Message* response) = 0;
};

}

