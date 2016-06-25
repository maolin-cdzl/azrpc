#pragma once

#include <czmq.h>
#include "zeputils/ev_zmq.h"
#include "azrpc/i_looper_adapter.hpp"

namespace azrpc {

class ZLooperAdapter : public ILooperAdapter {
public:
	ZLooperAdapter(zloop_t* loop);
	virtual ~ZLooperAdapter();

	virtual void registerChannel(zsock_t* zsock,
			const std::function<void()>& readable,
			const std::function<void()>& onTimeout);

	virtual void unregisterChannel();

	virtual void setTimer(int64_t timeout);
	virtual void cancelTimer();
private:
	static int ReadableCallback(zloop_t* loop,zsock_t* sock,void* arg);
	static int TimerCallback(zloop_t* loop,int timer_id,void* arg);

private:
	zloop_t*					m_loop;
	zsock_t*					m_zsock;
	std::function<void()>		m_cb_read;
	std::function<void()>		m_cb_timer;
	
	int							m_timer_id;
	uint64_t					m_timer_timeout;
};


}


