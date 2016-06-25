#include <iostream>
#include <ev.h>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include "azrpc/client_channel_builder.hpp"
#include "azrpc/server_channel_builder.hpp"

#include "search.azrpc.h"

static int64_t get_time_now(void) {
	struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC,&tv);
    return (int64_t) (tv.tv_sec * 1000 + tv.tv_nsec / 1000000);
}
struct ColorCoutSink {
	// Linux xterm color
	// http://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
	enum FG_Color {YELLOW = 33, RED = 31, GREEN=32, WHITE = 97};

	FG_Color GetColor(const LEVELS level) const {
		if (level.value == WARNING.value) { return YELLOW; }
		if (level.value == DEBUG.value) { return GREEN; }
		if (g3::internal::wasFatal(level)) { return RED; }

		return WHITE;
	}

	void ReceiveLogMessage(g3::LogMessageMover logEntry) {
		auto level = logEntry.get()._level;
		auto color = GetColor(level);
		std::cout << "\033[" << color << "m" << logEntry.get().toString() << "\033[m" << std::endl;
	}
};

using namespace azrpc;

class SearchServiceImpl : public test::SearchService {
	virtual void Search(const std::shared_ptr<test::SearchRequest>& request,const std::shared_ptr<azrpc::Reply>& reply) {
		test::SearchResponse response;
		response.add_results("result1 for " + request->query());
		response.add_results("this is result2");
		reply->sendReply(&response);
	}
};

test::SearchService_Stub* g_client_stub = nullptr;
std::shared_ptr<IAzRpcCallback> g_callback;
size_t g_count = 0;

class SearchCallback : public azrpc::IAzRpcCallback {
public:
	virtual void onResponse(RpcError err,const std::string& err_msg,const std::shared_ptr<google::protobuf::Message>& response) {
		g_count += 1;
		test::SearchRequest request;
		request.set_query("gold");
		g_client_stub->Search(&request,g_callback);
	}
};

static void sigint_cb (struct ev_loop *loop, ev_signal *w, int revents) {
	ev_break (loop, EVBREAK_ALL);
}

int main(int argc,char* argv[]) {
	std::unique_ptr<g3::LogWorker> logworker {g3::LogWorker::createLogWorker()};
	auto sinkHandle = logworker->addSink(std2::make_unique<ColorCoutSink >(),&ColorCoutSink ::ReceiveLogMessage);
	initializeLogging(logworker.get());
	LOG(INFO) << "Test start...";

	zsys_init();
	struct ev_loop* loop = EV_DEFAULT;

	ev_signal signal_watcher;
	ev_signal_init (&signal_watcher, sigint_cb, SIGINT);
	ev_signal_start (loop, &signal_watcher);

	auto serverChannel = ServerChannelBuilder().withEvLoop(loop).bind("tcp://127.0.0.1:5555").build();
	CHECK(serverChannel != nullptr) << "serverChannel build failed.";

	std::shared_ptr<IService> svc(new SearchServiceImpl());
	serverChannel->registerService(svc);

	test::SearchService_Stub search_stub(ClientChannelBuilder().withEvLoop(loop).connect("tcp://127.0.0.1:5555").build());
	g_client_stub = &search_stub;
	g_callback.reset(new SearchCallback());

	static const size_t PARALISM_HINT = 5;
	uint64_t tv_start = get_time_now();
	for(size_t i=0; i < PARALISM_HINT; ++i) {
		test::SearchRequest request;
		request.set_query("gold");
		search_stub.Search(&request,g_callback);
	}

	ev_run(loop,0);

	uint64_t tv_end = get_time_now();

	LOG(INFO) << "paralism: " << PARALISM_HINT << ", performent " << g_count << " in " << (tv_end - tv_start) << " ms";
	return 0;
}


