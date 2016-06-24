#pragma once

#include <memory>
#include <czmq.h>
#include "azrpc/i_service.hpp"

namespace azrpc {

class IServerChannel {
public:
	virtual ~IServerChannel() {}

	virtual void registerService(const std::shared_ptr<IService>& service) = 0;

	virtual int sendReply(zmsg_t** envelop,int64_t event_id,RpcError err,const std::string& err_msg,const std::string& response) = 0;
};

}

