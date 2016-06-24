#include "azrpc/client_channel_builder.hpp"
#include "ev_looper_adapter.hpp"
#include "client_channel.hpp"

namespace azrpc {

ClientChannelBuilder& ClientChannelBuilder::bindLoop(const std::shared_ptr<ILooperAdapter>& adapter) {
	m_loop_adapter = adapter;
	return *this;
}

ClientChannelBuilder& ClientChannelBuilder::bindEvLoop(struct ev_loop* loop) {
	m_loop_adapter = std::shared_ptr<ILooperAdapter>(new EvLooperAdapter(loop));
	return *this;
}


ClientChannelBuilder& ClientChannelBuilder::connect(const std::string& address) {
	m_addresses.push_back(address);
	return *this;
}

std::shared_ptr<IClientChannel> ClientChannelBuilder::build() {
	if( m_loop_adapter == nullptr || m_addresses.empty() ) {
		return nullptr;
	}

	std::shared_ptr<ClientChannel> channel(new ClientChannel());
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
