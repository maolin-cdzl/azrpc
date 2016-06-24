#include "azrpc/server_channel_builder.hpp"
#include "ev_looper_adapter.hpp"
#include "server_channel.hpp"

namespace azrpc {

ServerChannelBuilder& ServerChannelBuilder::withLoop(const std::shared_ptr<ILooperAdapter>& adapter) {
	m_loop_adapter = adapter;
	return *this;
}

ServerChannelBuilder& ServerChannelBuilder::withEvLoop(struct ev_loop* loop) {
	m_loop_adapter = std::shared_ptr<ILooperAdapter>(new EvLooperAdapter(loop));
	return *this;
}


ServerChannelBuilder& ServerChannelBuilder::bind(const std::string& address) {
	m_address = address;
	return *this;
}

std::shared_ptr<IServerChannel> ServerChannelBuilder::build() {
	if( m_loop_adapter == nullptr || m_address.empty() ) {
		return nullptr;
	}

	std::shared_ptr<ServerChannel> channel(new ServerChannel());
	if( -1 == channel->bind(m_address) ) {
		return nullptr;
	}
	if( -1 == channel->start(m_loop_adapter) ) {
	   return nullptr;
	}
	return channel;
}

}

