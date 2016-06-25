#include <iostream>
#include <ev.h>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include "azrpc/client_channel_builder.hpp"
#include "azrpc/server_channel_builder.hpp"

#include "search.azrpc.h"

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
	test::SearchRequest request;
	request.set_query("gold");
	auto callback = std::shared_ptr<IAzRpcCallback>(new SearchCallback());
	search_stub.Search(&request,callback);

	ev_run(loop,0);

	return 0;
}


