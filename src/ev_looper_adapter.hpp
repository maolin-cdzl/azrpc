#pragma once

#include <ev.h>
#include "zeputils/ev_zmq.h"
#include "azrpc/i_looper_adapter.hpp"

namespace azrpc {

class EvLooperAdapter : public ILooperAdapter {
public:
	EvLooperAdapter(struct ev_loop* loop);
	virtual ~EvLooperAdapter();

	virtual void registerChannel(zsock_t* zsock,
			const std::function<void()>& readable,
			const std::function<void()>& onTimeout);

	virtual void unregisterChannel();

	virtual void setTimer(int64_t timeout);
	virtual void cancelTimer();
private:
	static void ZmqWatcherCallback(struct ev_loop* loop,ev_zmq* w,int events);
	static void TimerWatcherCallback(struct ev_loop* loop,ev_timer* w,int events);

private:
	struct ev_loop*				m_ev_loop;
	zsock_t*					m_zsock;
	std::function<void()>		m_cb_read;
	std::function<void()>		m_cb_timer;
	
	struct ev_zmq				m_zmq_watcher;
	struct ev_timer				m_timer_watcher;
};


}

