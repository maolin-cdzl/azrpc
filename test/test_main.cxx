#include "etutils/common/time.hpp"
#include "test_common.h"
#include "azrpc/client_channel_builder.hpp"
#include "azrpc/server_channel_builder.hpp"


static const size_t PARALISM_HINT = 5;
size_t g_count = 0;
std::shared_ptr<test::SearchService_Stub> g_ev_stub;
std::shared_ptr<IAzRpcCallback> g_ev_callback;

class EvSearchCallback : public azrpc::IAzRpcCallback {
public:
	virtual void onResponse(RpcError err,const std::string& err_msg,const std::shared_ptr<google::protobuf::Message>& response) {
		g_count += 1;
		test::SearchRequest request;
		request.set_query("gold");
		g_ev_stub->Search(&request,g_ev_callback);
	}
};


void test_withEv() {
	struct ev_loop* loop = EV_DEFAULT;

	ev_signal signal_watcher;
	ev_signal_init (&signal_watcher, sigint_cb, SIGINT);
	ev_signal_start (loop, &signal_watcher);

	auto serverChannel = ServerChannelBuilder().withEvLoop(loop).bind("tcp://127.0.0.1:5555").build();
	CHECK(serverChannel != nullptr) << "serverChannel build failed.";

	std::shared_ptr<IService> svc(new SearchServiceImpl());
	serverChannel->registerService(svc);

	g_ev_stub.reset(new test::SearchService_Stub(ClientChannelBuilder().withEvLoop(loop).connect("tcp://127.0.0.1:5555").build()));
	g_ev_callback.reset(new EvSearchCallback());
	
	g_count = 0;
	uint64_t tv_start = etutils::up_time();
	for(size_t i=0; i < PARALISM_HINT; ++i) {
		test::SearchRequest request;
		request.set_query("gold");
		g_ev_stub->Search(&request,g_ev_callback);
	}

	ev_run(loop,0);

	uint64_t tv_end = etutils::up_time();

	LOG(INFO) << "paralism: " << PARALISM_HINT << ", performent " << g_count << " in " << (tv_end - tv_start) << " ms";
	g_ev_stub.reset();
	g_ev_callback.reset();
}

std::shared_ptr<test::SearchService_Stub> g_z_stub;
std::shared_ptr<IAzRpcCallback> g_z_callback;

class ZSearchCallback : public azrpc::IAzRpcCallback {
public:
	virtual void onResponse(RpcError err,const std::string& err_msg,const std::shared_ptr<google::protobuf::Message>& response) {
		g_count += 1;
		test::SearchRequest request;
		request.set_query("gold");
		g_z_stub->Search(&request,g_z_callback);
	}
};


void test_withZ() {
	zloop_t* loop = zloop_new();
	auto serverChannel = ServerChannelBuilder().withZLoop(loop).bind("tcp://127.0.0.1:5555").build();
	CHECK(serverChannel != nullptr) << "serverChannel build failed.";

	
	serverChannel->registerService(std::shared_ptr<IService>(new SearchServiceImpl()));

	g_z_stub.reset(new test::SearchService_Stub(ClientChannelBuilder().withZLoop(loop).connect("tcp://127.0.0.1:5555").build()));
	g_z_callback.reset(new ZSearchCallback());
	
	g_count = 0;
	uint64_t tv_start = etutils::up_time();
	for(size_t i=0; i < PARALISM_HINT; ++i) {
		test::SearchRequest request;
		request.set_query("gold");
		g_z_stub->Search(&request,g_z_callback);
	}

	zloop_start(loop);

	uint64_t tv_end = etutils::up_time();

	LOG(INFO) << "paralism: " << PARALISM_HINT << ", performent " << g_count << " in " << (tv_end - tv_start) << " ms";
	g_z_stub.reset();
	g_z_callback.reset();
}

void test_Timeout() {
	zloop_t* loop = zloop_new();
	auto serverChannel = ServerChannelBuilder().withZLoop(loop).bind("tcp://127.0.0.1:5555").build();
	CHECK(serverChannel != nullptr) << "serverChannel build failed.";

	
	serverChannel->registerService(std::shared_ptr<IService>(new NonSearchServiceImpl()));

	test::SearchService_Stub stub(ClientChannelBuilder().withZLoop(loop).connect("tcp://127.0.0.1:5555").build());
	std::shared_ptr<SearchCallback> callback(new SearchCallback());
	
	test::SearchRequest request;
	request.set_query("gold");
	stub.Search(&request,callback,1000);

	zloop_start(loop);
}

int main(int argc,char* argv[]) {
	std::unique_ptr<g3::LogWorker> logworker {g3::LogWorker::createLogWorker()};
	auto sinkHandle = logworker->addSink(std2::make_unique<ColorCoutSink >(),&ColorCoutSink ::ReceiveLogMessage);
	initializeLogging(logworker.get());
	LOG(INFO) << "Test start...";
	zsys_init();

	test_Timeout();

	//test_withEv();
	//sleep(1);
	//test_withZ();
	return 0;
}


