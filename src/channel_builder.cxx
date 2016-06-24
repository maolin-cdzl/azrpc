#include "azrpc/channel_builder.hpp"
#include "ev_looper_adapter.hpp"
#include "azrpc_channel.hpp"

namespace azrpc {

AzRpcChannelBuilder& AzRpcChannelBuilder::bindLoop(const std::shared_ptr<ILooperAdapter>& adapter) {
	m_loop_adapter = adapter;
	return *this;
}

AzRpcChannelBuilder& AzRpcChannelBuilder::bindEvLoop(struct ev_loop* loop) {
	m_loop_adapter = std::shared_ptr<ILooperAdapter>(new EvLooperAdapter(loop));
	return *this;
}


AzRpcChannelBuilder& AzRpcChannelBuilder::connect(const std::string& address) {
	m_addresses.push_back(address);
	return *this;
}

std::shared_ptr<IAzRpcChannel> AzRpcChannelBuilder::build() {
	if( m_loop_adapter == nullptr || m_addresses.empty() ) {
		return nullptr;
	}

	std::shared_ptr<AzRpcChannel> channel(new AzRpcChannel());
	for(auto it=m_addresses.begin(); it != m_addresses.end(); ++it) {
		if( -1 == channel->connect(*it) ) {
			return nullptr;
		}
	}
	if( -1 == channel->start(m_loop_adapter) ) {
	   return nullptr;
	}
	return channel;
}

}
