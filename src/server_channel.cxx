#include "server_channel.hpp"


namespace azrpc {
ServerChannel::ServerChannel() :
	m_zsock(zsock_new(ZMQ_ROUTER))
{
}

ServerChannel::~ServerChannel() {
	stop();
	zsock_destroy(&m_zsock);
}

void ServerChannel::registerService(const std::shared_ptr<IService>& service) {
}

int ServerChannel::sendReply(zmsg_t** envelop,int64_t event_id,RpcError err,const std::string& err_msg,const std::string& response) {
}


int ServerChannel::bind(const std::string& address) {
}

int ServerChannel::start(const std::shared_ptr<ILoopAdapter>& adapter) {
	if( m_loop_adpater ) {
		return -1;
	}
	adapter->registerChannel(m_zsock,
			std::bind(&ClientChannel::handleReadable,this),
			std::bind(&ClientChannel::handleTimeout,this)
			);
	m_loop_adapter = adapter;
	return 0;
}

void ServerChannel::stop() {
	if( m_loop_adapter != nullptr ) {
		m_loop_adapter->unregisterChannel();
		m_loop_adapter.reset();
	}
}

void ServerChannel::handleReadable() {
}

void ServerChannel::handleTimeout() {
}

void ServerChannel::handleRequest(const azrpc::AzRpcRequest& request) {
}


}

