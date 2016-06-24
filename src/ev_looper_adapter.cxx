#include <assert.h>
#include "ev_looper_adapter.hpp"

namespace azrpc {

EvLooperAdapter::EvLooperAdapter(struct ev_loop* loop) :
	m_ev_loop(loop),
	m_zsock(nullptr)
{
}

EvLooperAdapter::~EvLooperAdapter() {
	unregisterChannel();
}

void EvLooperAdapter::registerChannel(zsock_t* zsock,
			const std::function<void()>& readable,
			const std::function<void()>& onTimeout) {
	assert( m_zsock == nullptr );
	m_zsock = zsock;
	m_cb_read = readable;
	m_cb_timer = onTimeout;

	ev_zmq_init(&m_zmq_watcher,&ZmqWatcherCallback,zsock,EV_READ);
	ev_zmq_start(m_ev_loop,&m_zmq_watcher);
	m_zmq_watcher.data = this;

	ev_init(&m_timer_watcher,&TimerWatcherCallback);
	m_timer_watcher.data = this;
}

void EvLooperAdapter::unregisterChannel() {
	if( m_zsock ) {
		ev_zmq_stop(m_ev_loop,&m_zmq_watcher);
		ev_timer_stop(m_ev_loop,&m_timer_watcher);
		m_zsock = nullptr;
	}
}

void EvLooperAdapter::setTimer(int64_t timeout) {
	double evtime = timeout / 1000.0f;
	m_timer_watcher.repeat = evtime;
	ev_timer_again(m_ev_loop,&m_timer_watcher);
}

void EvLooperAdapter::cancelTimer() {
	ev_timer_stop(m_ev_loop,&m_timer_watcher);
}

void EvLooperAdapter::ZmqWatcherCallback(struct ev_loop* loop,ev_zmq* w,int events) {
	(void)loop;
	if( EV_READ & events ) {
		EvLooperAdapter* self = (EvLooperAdapter*)w->data;
		self->m_cb_read();
	}
}

void EvLooperAdapter::TimerWatcherCallback(struct ev_loop* loop,ev_timer* w,int events) {
	(void) loop;
	if( EV_TIMEOUT & events ) {
		EvLooperAdapter* self = (EvLooperAdapter*)w->data;
		self->m_cb_timer();
	}
}

}

