#pragma once

#include "azrpc/i_looper_adapter.hpp"
#include "azrpc/i_client_channel.hpp"

namespace azrpc {

class ClientChannelBuilder {
public:
	// only one loop adapter make sense
	ClientChannelBuilder& withLoop(const std::shared_ptr<ILooperAdapter>& adapter);

	ClientChannelBuilder& withEvLoop(struct ev_loop* loop);

	ClientChannelBuilder& connect(const std::string& address);

	std::shared_ptr<IClientChannel> build();

private:
	std::shared_ptr<ILooperAdapter>		m_loop_adapter;
	std::vector<std::string>			m_addresses;
};

}

