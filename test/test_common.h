#pragma once

#include <iostream>
#include <ev.h>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include "azrpc/reply.hpp"
#include "search.azrpc.h"

using namespace azrpc;

static void sigint_cb (struct ev_loop *loop, ev_signal *w, int revents) {
	ev_break (loop, EVBREAK_ALL);
}

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

class SearchServiceImpl : public test::SearchService {
	virtual void Search(const std::shared_ptr<test::SearchRequest>& request,const std::shared_ptr<azrpc::Reply>& reply) {
		test::SearchResponse response;
		response.add_results("result1 for " + request->query());
		response.add_results("this is result2");
		reply->sendReply(&response);
	}
};

class NonSearchServiceImpl : public test::SearchService {
	virtual void Search(const std::shared_ptr<test::SearchRequest>& request,const std::shared_ptr<azrpc::Reply>& reply) {
		LOG(INFO) << "got request for " << request->query();
	}
};

class SearchCallback : public azrpc::IAzRpcCallback {
public:
	virtual void onResponse(RpcError err,const std::string& err_msg,const std::shared_ptr<google::protobuf::Message>& response) {
		LOG(INFO) << "got callback,err:" << err << " " << err_msg;
	}
};


