#include "azrpc/reply.hpp"
#include "azrpc/i_server_channel.hpp"

namespace azrpc {

Reply::Reply(IServerChannel* channel,zmsg_t** p_envelope,const google::protobuf::Descriptor* descriptor,int64_t id) :
	m_channel(channel),
	m_descriptor(descriptor),
	m_event_id(id),
	m_envelope(*p_envelope)
{
	*p_envelope = nullptr;
}

Reply::~Reply() {
	if( m_envelope ) {
		zmsg_destroy(&m_envelope);
	}
}

int64_t Reply::event_id() const {
	return m_event_id;
}

const google::protobuf::Descriptor* Reply::descriptor() const {
	return m_descriptor;
}

int Reply::sendReply(const google::protobuf::Message* res) throw(BadReplyMessage) {
	if( nullptr == res ) {
		if( m_descriptor != nullptr ) {
			throw BadReplyMessage(m_descriptor->full_name(),"None");
		}
	} else if( m_descriptor == nullptr ) {
		throw BadReplyMessage("None",res->GetTypeName());
	}

	if( res->GetTypeName().compare(m_descriptor->full_name()) != 0 ) {
		throw BadReplyMessage(m_descriptor->full_name(),res->GetTypeName());
	}
	std::string buf;
	if( ! res->SerializeToString(&buf) ) {
		throw BadReplyMessage("Error: SerializeToString");
	}
	return m_channel->sendReply(&m_envelope,m_event_id,OK,"",buf);
}

int Reply::sendError(RpcError err) {
	return m_channel->sendReply(&m_envelope,m_event_id,err,"","");
}

int Reply::sendError(RpcError err,const std::string& err_msg) {
	return m_channel->sendReply(&m_envelope,m_event_id,err,err_msg,"");
}

}

