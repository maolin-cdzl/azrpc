#include <iostream>
#include <ev.h>
#include "azrpc/client_channel_builder.hpp"
#include "azrpc/server_channel_builder.hpp"

#include "search.azrpc.h"

using namespace azrpc;

class SearchServiceImpl : public test::SearchService {
	virtual void Search(const std::shared_ptr<test::SearchRequest>& request,const std::shared_ptr<azrpc::Reply>& reply) {
		std::cout << "Got request for '" << request->query() << "'" << std::endl;
		test::SearchResponse response;
		response.add_results("result1 for " + request->query());
		response.add_results("this is result2");
		reply->sendReply(&response);
	}
};

class SearchCallback : public azrpc::IAzRpcCallback {
public:
	virtual void onResponse(RpcError err,const std::string* err_msg,const google::protobuf::Message* response) {
		std::cout << "onResponse" << std::endl;
	}
};


int main(int argc,char* argv[]) {
	struct ev_loop* loop = ev_loop_new();

	auto serverChannel = ServerChannelBuilder().withEvLoop(loop).bind("tcp://localhost:5555").build();
	serverChannel->registerService(std::shared_ptr<IService>(new SearchServiceImpl()));

	test::SearchService_Stub search_stub(ClientChannelBuilder().withEvLoop(loop).connect("tcp://localhost:5555").build());
	test::SearchRequest request;
	request.set_query("gold");
	search_stub.Search(&request,std::shared_ptr<IAzRpcCallback>(new SearchCallback()));

	ev_run(loop,0);
	return 0;
}


