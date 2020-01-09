/*
 * pc_identifier.cpp
 *
 *  Created on: Dec 22, 2019
 *      Author: GC
 */

#include <algorithm>
#include "pc_identifier.hpp"
#include "../base/base64.h"

namespace license {
using namespace std;

PcIdentifier::PcIdentifier() {}

PcIdentifier::PcIdentifier(const std::string& param) {
	// TODO Auto-generated constructor stub
}

PcIdentifier::~PcIdentifier() {}

PcIdentifier::PcIdentifier(const PcIdentifier& other) : m_data(other.m_data) {}

void PcIdentifier::set_identification_strategy(IDENTIFICATION_STRATEGY strategy) {
	if (strategy == STRATEGY_UNKNOWN || strategy == STRATEGY_DEFAULT) {
		throw logic_error("Only known strategies are permitted");
	}
	uint8_t stratMov = (strategy << 5);
	m_data[1] = (m_data[1] & 0x1F) | stratMov;
}

void PcIdentifier::set_use_environment_var(bool use_env_var) {
	if (use_env_var) {
		m_data[0] = m_data[0] | 0x40;
	} else {
		m_data[0] = m_data[0] & ~0x40;
	}
}

void PcIdentifier::set_virtual_environment(VIRTUALIZATION virt) {
	// 110000 0x30
	m_data[0] = (m_data[0] && ~0x30) | virt << 4;
}

void PcIdentifier::set_virtualization(VIRTUALIZATION_DETAIL virtualization_detail) {}

void PcIdentifier::set_cloud_provider(CLOUD_PROVIDER cloud_provider) {}

void PcIdentifier::set_data(const std::array<uint8_t, 6>& data) {}

std::string PcIdentifier::print() const {
	string result = base64(m_data.data(), m_data.size(), 4);
	std::replace(result.begin(), result.end(), '\n', '-');
	return result;
}

} /* namespace license */
