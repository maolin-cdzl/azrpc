#pragma once

#include <string>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <czmq.h>

#include "azrpc/azrpc_types.hpp"

namespace azrpc {

class IServerChannel;

class Reply{
public:
	Reply(IServerChannel* channel,zmsg_t** p_envelope,const google::protobuf::Descriptor* descriptor,uint64_t id); 
	~Reply();

	Reply(const Reply&) = delete;
	Reply& operator = (const Reply&) = delete;

	uint64_t event_id() const;
	const google::protobuf::Descriptor* descriptor() const;

	int sendReply(const google::protobuf::Message* res) throw(BadReplyMessage);
	int sendError(RpcError err);
	int sendError(RpcError err,const std::string& err_msg);
	
private:
	IServerChannel*								m_channel;
	const google::protobuf::Descriptor*			m_descriptor;
	const uint64_t								m_event_id;
	zmsg_t*										m_envelope;
};

}

