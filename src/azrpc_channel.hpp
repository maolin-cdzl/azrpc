#pragma once

#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <ev.h>
#include <czmq.h>
#include "zeputils/zprotobuf.hpp"
#include "azrpc/i_azrpc_channel.hpp"
#include "eventid_generator.hpp"
#include "azrpc.pb.h"

namespace azrpc {

class IAzRpcCallback;
class ILooperAdapter;
class AzRpcChannel;

struct RemoteCallEntry {
	int64_t										event_id;
	int64_t										deadline;
	const google::protobuf::Descriptor*			response_descriptor;
	std::weak_ptr<IAzRpcCallback>				callback;
};

class AzRpcChannel : public IAzRpcChannel {
public:
	virtual ~AzRpcChannel();
	AzRpcChannel(const AzRpcChannel&) = delete;
	AzRpcChannel& operator = (const AzRpcChannel&) = delete;

	virtual int callMethod(
			const google::protobuf::MethodDescriptor* method,
			const google::protobuf::Message* argument,
			const std::shared_ptr<IAzRpcCallback>& callback,
			int32_t timeout);

	friend class AzRpcChannelBuilder;
private:
	AzRpcChannel();

	int connect(const std::string& address);
	int start(const std::shared_ptr<ILooperAdapter>& adapter);
	void stop();

	void handleReadable();
	void handleTimeout();
	int64_t getTimeout();
	void checkSetTimer();

	void handleResponse(const azrpc::AzRpcResponse& response);
private:
	std::shared_ptr<ILooperAdapter>				m_loop_adapter;
	zsock_t*									m_zsock;
	ZProtobuf									m_zp;
	std::string									m_msg_cache;
	EventIdGenerator							m_eid_generator;
	std::map<int64_t,int64_t>					m_deadline_map;
	std::unordered_map<int64_t,std::shared_ptr<RemoteCallEntry>>	m_request_map;
};


}

