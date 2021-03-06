#pragma once

#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <ev.h>
#include <czmq.h>
#include "etutils/zep/zprotobuf.hpp"
#include "azrpc/i_client_channel.hpp"
#include "eventid_generator.hpp"
#include "azrpc.pb.h"

namespace azrpc {

class IAzRpcCallback;
class ILooperAdapter;
class ClientChannel;

struct RemoteCallEntry {
	uint64_t									event_id;
	uint64_t									deadline;
	const google::protobuf::Descriptor*			response_descriptor;
	std::weak_ptr<IAzRpcCallback>				callback;
};

class ClientChannel : public IClientChannel {
public:
	virtual ~ClientChannel();
	ClientChannel(const ClientChannel&) = delete;
	ClientChannel& operator = (const ClientChannel&) = delete;

	virtual int callMethod(
			const google::protobuf::MethodDescriptor* method,
			const google::protobuf::Message* argument,
			const std::shared_ptr<IAzRpcCallback>& callback,
			int32_t timeout);

	friend class ClientChannelBuilder;
private:
	ClientChannel();

	int connect(const std::string& address);
	int start(const std::shared_ptr<ILooperAdapter>& adapter);
	void stop();

	void handleReadable();
	void handleTimeout();
	uint64_t getTimeout();
	void checkSetTimer();

	void handleResponse(const azrpc::AzRpcResponse& response);
private:
	std::shared_ptr<ILooperAdapter>				m_loop_adapter;
	zsock_t*									m_zsock;
	etutils::ZProtobuf							m_zp;
	std::string									m_msg_cache;
	EventIdGenerator							m_eid_generator;
	std::map<uint64_t,uint64_t>					m_deadline_map;
	std::unordered_map<uint64_t,std::shared_ptr<RemoteCallEntry>>	m_request_map;
};


}

