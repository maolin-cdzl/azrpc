#pragma once

#include "azrpc/i_server_channel.hpp"

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
	int start(const std::shared_ptr<ILoopAdapter>& adapter);
	void stop();

	void handleReadable();
	void handleTimeout();

	void handleRequest(const azrpc::AzRpcRequest& request);
private:
	std::unordered_map<std::string,std::shared_ptr<IService>>	m_service_map;
	std::shared_ptr<ILooperAdapter>				m_loop_adapter;
	zsock_t*									m_zsock;
	ZProtobuf									m_zp;
};


}

