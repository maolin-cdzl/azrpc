#include "eventid_generator.hpp"
#include <sys/types.h>
#include <unistd.h>

namespace azrpc {

EventIdGenerator::EventIdGenerator() {
	m_seed = (reinterpret_cast<uint64_t>(this) << 32);
	m_seed += getpid();
}

uint64_t EventIdGenerator::next() {
	m_seed = (m_seed * kGenerator) % kLargePrime;
	return m_seed;
}

}

