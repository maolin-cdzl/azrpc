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

}

