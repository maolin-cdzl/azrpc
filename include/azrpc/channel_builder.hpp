#pragma once

#include "azrpc/i_looper_adapter.hpp"
#include "azrpc/i_azrpc_channel.hpp"

namespace azrpc {

class AzRpcChannelBuilder {
public:
	// only one loop adapter make sense
	AzRpcChannelBuilder& bindLoop(const std::shared_ptr<ILooperAdapter>& adapter);

	AzRpcChannelBuilder& bindEvLoop(struct ev_loop* loop);

	AzRpcChannelBuilder& connect(const std::string& address);

	std::shared_ptr<IAzRpcChannel> build();

private:
	std::shared_ptr<ILooperAdapter>		m_loop_adapter;
	std::vector<std::string>			m_addresses;
};

}

