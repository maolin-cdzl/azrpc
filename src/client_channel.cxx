#include <stdint.h>
#include <vector>
#include <memory>
#include <g3log/g3log.hpp>
#include "client_channel.hpp"
#include "clock.hpp"
#include "azrpc.pb.h"

#include "azrpc/i_azrpc_callback.hpp"
#include "azrpc/i_looper_adapter.hpp"

namespace azrpc {

ClientChannel::ClientChannel()
	: m_zsock(zsock_new(ZMQ_DEALER))
{
}

ClientChannel::~ClientChannel() {
	stop();
	zsock_destroy(&m_zsock);
}

int ClientChannel::connect(const std::string& address) {
	return zsock_connect(m_zsock,"%s",address.c_str());
}

int ClientChannel::start(const std::shared_ptr<ILooperAdapter>& adapter) {
	if( m_loop_adapter != nullptr ) {
		return -1;
	}
	adapter->registerChannel(m_zsock,
			std::bind(&ClientChannel::handleReadable,this),
			std::bind(&ClientChannel::handleTimeout,this)
			);
	m_loop_adapter = adapter;
	return 0;
}

void ClientChannel::stop() {
	if( m_loop_adapter != nullptr ) {
		m_loop_adapter->unregisterChannel();
		m_loop_adapter.reset();
	}
}

void ClientChannel::handleReadable() {
	LOG(DEBUG) << "ClientChannel::handleReadable";
	int zevents = zsock_events(m_zsock);

	while( zevents & ZMQ_POLLIN ) {
		std::unique_ptr<azrpc::AzRpcResponse> response((azrpc::AzRpcResponse*)m_zp.recv(m_zsock));
		if( response ) {
			handleResponse(*response);
		} else {
			LOG(WARNING) << "Error when read AzRpcResponse";
		}
		zevents = zsock_events(m_zsock);
	}
	checkSetTimer();
}

void ClientChannel::handleTimeout() {
	const int64_t now = clock_time();
	std::vector<std::shared_ptr<RemoteCallEntry>> events;
	for(auto it = m_deadline_map.begin(); it != m_deadline_map.end();) {
		if( it->first > now ) {
			break;
		}
		auto itrc = m_request_map.find(it->second);
		if( itrc != m_request_map.end() ) {
			events.push_back(itrc->second);
			m_request_map.erase(itrc);
		}
		it = m_deadline_map.erase(it);
	}
	
	for(auto it = events.begin(); it != events.end(); ++it) {
		auto cb = (*it)->callback.lock();
		if( cb ) {
			cb->onResponse(TIMEOUT,nullptr,nullptr);
		}
	}

	checkSetTimer();
}

int64_t ClientChannel::getTimeout() {
	if( !m_deadline_map.empty() ) {
		const int64_t now = clock_time();
		auto it = m_deadline_map.begin();
		if( it->first > now ) {
			return it->first - now;
		} else {
			return 1L;
		}
	} else {
		return 0;
	}
}

void ClientChannel::checkSetTimer() {
	int64_t timeout = getTimeout();
	if( timeout > 0 ) {
		m_loop_adapter->setTimer(timeout);
	} else {
		m_loop_adapter->cancelTimer();
	}
}

int ClientChannel::callMethod(
		const google::protobuf::MethodDescriptor* method,
		const google::protobuf::Message* argument,
		const std::shared_ptr<IAzRpcCallback>& callback,
		int32_t timeout) {
	LOG(DEBUG) << "ClientChannel::callMethod";
	azrpc::AzRpcRequest request;
	request.set_event_id(m_eid_generator.next());
	request.set_service_name(method->service()->full_name());
	request.set_method_name(method->name());
	if( argument ) {
		if( ! argument->SerializeToString(&m_msg_cache) ) {
			LOG(WARNING) << "argument serialize failed";
			return -1;
		}
		if( ! m_msg_cache.empty() ) {
			request.set_argument(m_msg_cache);
		}
	}

	if( -1 == m_zp.send(m_zsock,&request,true,ZMQ_DONTWAIT) ) {
		LOG(WARNING) << "Send request failed";
		return -1;
	}

	std::shared_ptr<RemoteCallEntry> rc(new RemoteCallEntry());
	rc->event_id = request.event_id();
	rc->response_descriptor = method->output_type();
	if( callback != nullptr ) {
		rc->callback = callback;
	}
	if( timeout != 0 ) {
		rc->deadline = clock_time() + timeout;
		m_deadline_map.insert(std::make_pair(rc->deadline,rc->event_id));
		LOG(DEBUG) << "success request with timeout in " << timeout;
	} else {
		rc->deadline = 0;
		LOG(DEBUG) << "success request with no timeout";
	}
	m_request_map.insert(std::make_pair(rc->event_id,rc));
	return 0;
}

void ClientChannel::handleResponse(const azrpc::AzRpcResponse& response) {
	LOG(DEBUG) << "handleResponse";
	if( !response.has_event_id() || !response.has_error() ) {
		LOG(WARNING) << "incompleted AzRpcResponse";
		return;
	}

	auto it = m_request_map.find(response.event_id());
	if( it == m_request_map.end() ) {
		LOG(WARNING) << "Can not found pending event_id";
		// maybe timeout
		return;
	}

	// clearnup
	std::shared_ptr<RemoteCallEntry> rc = it->second;
	m_request_map.erase(it);
	if( rc->deadline ) {
		m_deadline_map.erase(rc->deadline);
	}

	std::shared_ptr<IAzRpcCallback> cb = rc->callback.lock();
	if( cb == nullptr ) {
		LOG(WARNING) << "Can not get callback";
		// callback object already destroy or there is no callback setup
		return;
	}

	RpcError err = response.error();
	const std::string* err_msg = nullptr;
	google::protobuf::Message* rep = nullptr;

	if( response.has_err_msg() ) {
		err_msg = &response.err_msg();
	}
	if( it->second->response_descriptor && response.has_response() ) {
		const std::string& bytes = response.response();
		rep = build_message(it->second->response_descriptor,bytes.c_str(),bytes.size());
		if( rep == nullptr ) {
			err = PROTOCOL_ERROR;
		}
	}

	cb->onResponse(err,err_msg,rep);

	if( rep ) {
		delete rep;
	}
}

}

