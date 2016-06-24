#include "azrpc/azrpc_types.hpp"

namespace azrpc {

BadReplyMessage::BadReplyMessage(const std::string& msg) :
	error_msg(msg)
{
}

BadReplyMessage::BadReplyMessage(const std::string& expect,const std::string& suply) {
	error_msg = "expect: " + ( expect.empty() ? "None" : expect ) + ", suply: " + (suply.empty() ? "None" : suply);
}

const char* BadReplyMessage::what() const throw() {
	return error_msg.c_str();
}

// class Reply

Reply::Reply(const google::protobuf::Descriptor* descriptor,int64_t id) :
	m_descriptor(descriptor),
	m_event_id(id),
	m_err(OK)
{
}


int64_t Reply::event_id() const {
	return m_event_id;
}

const google::protobuf::Descriptor* Reply::descriptor() const {
	return m_descriptor;
}

RpcError Reply::error() const {
	return m_err;
}

const std::string& Reply::err_msg() const {
	return m_err_msg;
}

void Reply::reply(const google::protobuf::Message* res) throw(BadReplyMessage) {
	if( nullptr == res ) {
		if( m_descriptor != nullptr ) {
			throw BadReplyMessage(m_descriptor->full_name(),"None");
		} else {
			m_err = OK;
		}
		return;
	} else if( m_descriptor == nullptr ) {
		throw BadReplyMessage("None",res->GetTypeName());
	}

	if( res->GetTypeName().compare(m_descriptor->full_name()) != 0 ) {
		throw BadReplyMessage(m_descriptor->full_name(),res->GetTypeName());
	}
	if( ! res->SerializeToString(&m_reply_buf) ) {
		throw BadReplyMessage("Error: SerializeToString");
	}
}

void Reply::error(RpcError err) {
	m_err = err;
}

void Reply::error(RpcError err,const std::string& err_msg) {
	m_err = err;
	m_err_msg = err_msg;
}

}

