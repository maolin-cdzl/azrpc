#pragma once

#include <string>
#include <exception>
#include <google/protobuf/descriptor.h>
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

class BadReplyMessage : public std::exception {
public:
	BadReplyMessage(const std::string& msg);
	BadReplyMessage(const std::string& expect,const std::string& suply);

	virtual const char* what() const throw();
private:
	std::string error_msg;
};

class Reply{
public:
	Reply(const google::protobuf::Descriptor* descriptor,int64_t id); 

	int64_t event_id() const;
	const google::protobuf::Descriptor* descriptor() const;
	RpcError error() const;
	const std::string& err_msg() const;

	void reply(const google::protobuf::Message* res) throw(BadReplyMessage);
	void error(RpcError err);
	void error(RpcError err,const std::string& err_msg);
	
private:
	const google::protobuf::Descriptor*			m_descriptor;
	const int64_t								m_event_id;
	RpcError									m_err;
	std::string									m_err_msg;
	std::string									m_reply_buf;
};

}

