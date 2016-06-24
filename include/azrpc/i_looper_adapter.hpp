#pragma once

#include <functional>
#include <czmq.h>

namespace azrpc {

class ILooperAdapter {
public:
	virtual ~ILooperAdapter() {}

	virtual void registerChannel(zsock_t* zsock,
			const std::function<void()>& readable,
			const std::function<void()>& onTimeout) = 0;

	virtual void unregisterChannel() = 0;

	// note,there is only 1 timer needed,but setTimer will be called frequently
	virtual void setTimer(int64_t timeout) = 0;
	virtual void cancelTimer() = 0;
};

}

