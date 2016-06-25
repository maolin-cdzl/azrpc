#pragma once

#include <ev.h>
#include <czmq.h>
#include "azrpc/i_looper_adapter.hpp"
#include "azrpc/i_server_channel.hpp"

namespace azrpc {

class ServerChannelBuilder {
public:
	// only one loop adapter make sense
	ServerChannelBuilder& withLoop(const std::shared_ptr<ILooperAdapter>& adapter);

	ServerChannelBuilder& withEvLoop(struct ev_loop* loop);

	ServerChannelBuilder& withZLoop(zloop_t* loop);

	ServerChannelBuilder& bind(const std::string& address);

	std::shared_ptr<IServerChannel> build();

private:
	std::shared_ptr<ILooperAdapter>		m_loop_adapter;
	std::string							m_address;
};

}


