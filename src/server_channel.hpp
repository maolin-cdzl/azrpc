#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "etutils/zep/zprotobuf.hpp"
#include "azrpc/i_server_channel.hpp"
#include "azrpc/i_looper_adapter.hpp"

namespace azrpc {

class ServerChannelBuilder;

class ServerChannel : public IServerChannel {
public:
	virtual ~ServerChannel();
	ServerChannel(const ServerChannel&) = delete;
	ServerChannel& operator = (const ServerChannel&) = delete;

	virtual void registerService(const std::shared_ptr<IService>& service);

	virtual int sendReply(zmsg_t** envelop,int64_t event_id,RpcError err,const std::string& err_msg,const std::string& response);

	friend class ServerChannelBuilder;
private:
	ServerChannel();

	int bind(const std::string& address);
	int start(const std::shared_ptr<ILooperAdapter>& adapter);
	void stop();

	void handleReadable();
	void handleTimeout();
private:
	std::unordered_map<std::string,std::shared_ptr<IService>>	m_service_map;
	std::shared_ptr<ILooperAdapter>				m_loop_adapter;
	zsock_t*									m_zsock;
	etutils::ZProtobuf							m_zp;
};


}

