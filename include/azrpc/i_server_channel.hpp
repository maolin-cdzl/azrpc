#pragma once

#include <memory>

#include "azrpc/i_service.hpp"

namespace azrpc {

class IServerChannel {
public:
	virtual ~IServerChannel() {}

	virtual void registerService(const std::shared_ptr<IService>& service);
};

}

