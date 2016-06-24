#pragma once

#include <stdint.h>

namespace azrpc {
class EventIdGenerator {
public:
	const static uint64_t kLargePrime = (1ULL << 63) - 165;
	const static uint64_t kGenerator = 2;

public:
	EventIdGenerator(const EventIdGenerator&) = delete;
	EventIdGenerator& operator = (const EventIdGenerator&) = delete;

	EventIdGenerator();

	uint64_t next();
private:
	uint64_t m_seed;
};

}

