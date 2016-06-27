#include <g3log/g3log.hpp>
#include "server_channel.hpp"
#include "azrpc/reply.hpp"
#include "azrpc.pb.h"


namespace azrpc {
ServerChannel::ServerChannel() :
	m_zsock(zsock_new(ZMQ_ROUTER))
{
}

ServerChannel::~ServerChannel() {
	stop();
	zsock_destroy(&m_zsock);
}

void ServerChannel::registerService(const std::shared_ptr<IService>& service) {
	LOG(INFO) << "Register service:" << service->GetDescriptor()->full_name();
	m_service_map.insert(std::make_pair(service->GetDescriptor()->full_name(),service));
}

int ServerChannel::sendReply(zmsg_t** p_envelope,uint64_t event_id,RpcError err,const std::string& err_msg,const std::string& response) {
	azrpc::AzRpcResponse msg;
	msg.set_event_id(event_id);
	msg.set_error((azrpc::AzRpcResponse_RpcError)err);
	if( !err_msg.empty() ) {
		msg.set_err_msg(err_msg);
	}
	if( !response.empty() ) {
		msg.set_response(response);
	}

	if( -1 == zmsg_sendm(p_envelope,m_zsock) ) {
		return -1;
	}
	return m_zp.send(m_zsock,&msg,false,0);
}


int ServerChannel::bind(const std::string& address) {
	return zsock_bind(m_zsock,"%s",address.c_str());
}

int ServerChannel::start(const std::shared_ptr<ILooperAdapter>& adapter) {
	if( m_loop_adapter ) {
		return -1;
	}
	adapter->registerChannel(m_zsock,
			std::bind(&ServerChannel::handleReadable,this),
			std::bind(&ServerChannel::handleTimeout,this)
			);
	m_loop_adapter = adapter;
	return 0;
}

void ServerChannel::stop() {
	if( m_loop_adapter != nullptr ) {
		m_loop_adapter->unregisterChannel();
		m_loop_adapter.reset();
	}
}

void ServerChannel::handleReadable() {
	zmsg_t* envelope = m_zp.envelope(m_zsock);
	std::unique_ptr<azrpc::AzRpcRequest> request( (azrpc::AzRpcRequest*)m_zp.recv(m_zsock,azrpc::AzRpcRequest::descriptor()));
	zsock_flush(m_zsock);

	do {
		if( request == nullptr ) {
			LOG(WARNING) << "failed to read AzRpcRequest message";
			break;
		}
		if( !request->has_service_name() || !request->has_method_name() || !request->has_event_id() ) {
			LOG(WARNING) << "incompleted AzRpcRequest message";
			break;
		}
		auto it = m_service_map.find(request->service_name());
		if( it == m_service_map.end() ) {
			LOG(WARNING) << "can not found service: " << request->service_name();
			break;
		}
		auto service = it->second;
		auto method = service->GetDescriptor()->FindMethodByName(request->method_name());
		if( method == nullptr ) {
			LOG(WARNING) << "can not found method: " << request->method_name();
			break;
		}

		auto input_type = method->input_type();
		auto output_type = method->output_type();
		if( (input_type && !request->has_argument()) || (input_type == nullptr && request->has_argument()) ) {
			LOG(WARNING) << "unpaired argument request and suply";
			return;
		}
		std::shared_ptr<google::protobuf::Message> input;
		if( input_type ) {
			input.reset(etutils::build_message(input_type,request->argument().c_str(),request->argument().size()));
			if( input == nullptr ) {
				LOG(WARNING) << "unserialize argument failed: " << input_type->full_name();
				break;
			}
		}
		std::shared_ptr<Reply> reply(new Reply(this,&envelope,output_type,request->event_id()));
		service->callMethod(method,input,reply);
		return;
	} while(0);

	if( envelope ) {
		zmsg_destroy(&envelope);
	}
}

void ServerChannel::handleTimeout() {
	// nothing to do
}


}

