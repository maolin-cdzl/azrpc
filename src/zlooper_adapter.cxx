#include <g3log/g3log.hpp>
#include "zlooper_adapter.hpp"
#include "etutils/common/time.hpp"

namespace azrpc {

int ZLooperAdapter::ReadableCallback(zloop_t* loop,zsock_t* sock,void* arg) {
	(void)loop;
	ZLooperAdapter* self = (ZLooperAdapter*)arg;
	self->m_cb_read();
	return 0;
}

int ZLooperAdapter::TimerCallback(zloop_t* loop,int timer_id,void* arg) {
	(void)loop;
	ZLooperAdapter* self = (ZLooperAdapter*)arg;
	if( self->m_timer_timeout != 0 ) {
		const uint64_t now = etutils::up_time();
		if( now >= self->m_timer_timeout ) {
			self->m_timer_timeout = 0;
			self->m_cb_timer();
		}
	}
	return 0;
}

ZLooperAdapter::ZLooperAdapter(zloop_t* loop) :
	m_loop(loop),
	m_zsock(nullptr),
	m_timer_id(-1),
	m_timer_timeout(0)
{
}

ZLooperAdapter::~ZLooperAdapter() {
	unregisterChannel();
}

void ZLooperAdapter::registerChannel(zsock_t* zsock,
		const std::function<void()>& readable,
		const std::function<void()>& onTimeout) {
	CHECK( m_zsock == nullptr );
	m_zsock = zsock;
	m_cb_read = readable;
	m_cb_timer = onTimeout;

	zloop_reader(m_loop,m_zsock,&ReadableCallback,this);
	m_timer_id = zloop_timer(m_loop,5,0,&TimerCallback,this);
	CHECK( m_timer_id != -1 );
}

void ZLooperAdapter::unregisterChannel() {
	if( m_zsock ) {
		zloop_reader_end(m_loop,m_zsock);
		zloop_timer_end(m_loop,m_timer_id);
		m_zsock = nullptr;
		m_timer_id = -1;
	}
}

void ZLooperAdapter::setTimer(uint64_t timeout) {
	m_timer_timeout = etutils::up_time() + timeout;
}

void ZLooperAdapter::cancelTimer() {
	m_timer_timeout = 0;
}

}

