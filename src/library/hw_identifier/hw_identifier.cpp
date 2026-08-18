/*
 * hw_identifier.cpp
 *
 *  Created on: Dec 22, 2019
 *      Author: GC
 */

#include <algorithm>
#include <cstring>
#include "hw_identifier.hpp"
#include "../base/base64.h"

namespace license {
namespace hw_identifier {

using namespace std;

void encode_string_to_buffer(const std::string& input, unsigned char* buffer_out, size_t out_size) {
	memset(buffer_out, 0, out_size);
	for (size_t i = 0; i < input.size(); i++) {
		unsigned char current_byte = static_cast<unsigned char>(input[i]);
		if (i % 2 == 1) {
			current_byte = (current_byte << 4) | (current_byte >> 4);
		}
		buffer_out[i % out_size] ^= current_byte;
	}
}

HwIdentifier::HwIdentifier() {}

HwIdentifier::HwIdentifier(const std::string& param) {
	string tmp_str(param);
	std::replace(tmp_str.begin(), tmp_str.end(), '-', '\n');
	vector<uint8_t> decoded = unbase64(tmp_str);
	if (decoded.size() != HW_IDENTIFIER_PROPRIETARY_DATA + 1) {
		throw logic_error("wrong identifier size " + param);
	}
	std::copy_n(decoded.begin(), HW_IDENTIFIER_PROPRIETARY_DATA + 1, m_data.begin());
}

HwIdentifier::~HwIdentifier() {}

HwIdentifier::HwIdentifier(const HwIdentifier& other) : m_data(other.m_data) {}

void HwIdentifier::set_identification_strategy(LCC_API_HW_IDENTIFICATION_STRATEGY strategy) {
	if (strategy == STRATEGY_NONE || strategy == STRATEGY_DEFAULT) {
		throw logic_error("Only known strategies are permitted");
	}
	uint8_t stratMov = (strategy << 3);
	m_data[0] = (m_data[0] & 0xE7) | stratMov;	// 0xE7 = 11100111 (clears bits 5-3)
}

void HwIdentifier::set_use_environment_var(bool use_env_var) {
	if (use_env_var) {
		m_data[0] = m_data[0] | 0x40;
	} else {
		m_data[0] = m_data[0] & ~0x40;
	}
}

void HwIdentifier::set_is_cloud(bool is_cloud) {
	if (is_cloud) {
		m_data[0] = m_data[0] | 0x04;
	} else {
		m_data[0] = m_data[0] & ~0x04;
	}
}

bool HwIdentifier::is_cloud() const { return (m_data[0] & 0x04) != 0; }

void HwIdentifier::set_virtualization_summary(LCC_API_VIRTUALIZATION_SUMMARY summary) {
	uint8_t summary_val = static_cast<uint8_t>(summary) & 0x03;
	m_data[0] = (m_data[0] & 0xFC) | summary_val;  // 0xFC = 11111100 (clears bits 1-0)
}

LCC_API_VIRTUALIZATION_SUMMARY HwIdentifier::get_virtualization_summary() const {
	uint8_t summary_val = m_data[0] & 0x03;	 // extract bits 1-0
	return static_cast<LCC_API_VIRTUALIZATION_SUMMARY>(summary_val);
}

void HwIdentifier::set_data(const std::array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA>& data) {
	for (int i = 0; i < HW_IDENTIFIER_PROPRIETARY_DATA; i++) {
		m_data[i + 1] = data[i];
	}
}

void HwIdentifier::set_data(const std::string& data) {
	encode_string_to_buffer(data, m_data.data() + 1, HW_IDENTIFIER_PROPRIETARY_DATA);
}

const std::array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA + 1>& HwIdentifier::get_data() const { return m_data; }

std::string HwIdentifier::print() const {
	string result = base64(m_data.data(), m_data.size(), 5);
	std::replace(result.begin(), result.end(), '\n', '-');
	return result.substr(0, result.size() - 1);
}

LCC_API_HW_IDENTIFICATION_STRATEGY HwIdentifier::get_identification_strategy() const {
	uint8_t stratMov = (m_data[0] & 0x38) >> 3;	 // 0x38 = 00111000 (extracts bits 5-3)
	return static_cast<LCC_API_HW_IDENTIFICATION_STRATEGY>(stratMov);
}

bool HwIdentifier::data_match(const std::array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA>& data) const {
	bool equals = true;
	for (int i = 0; i < HW_IDENTIFIER_PROPRIETARY_DATA && equals; i++) {
		equals = (data[i] == m_data[i + 1]);
	}
	return equals;
}

bool operator==(const HwIdentifier& lhs, const HwIdentifier& rhs) {
	bool equals = lhs.get_identification_strategy() == rhs.get_identification_strategy();
	for (int i = 0; i < HW_IDENTIFIER_PROPRIETARY_DATA && equals; i++) {
		equals = (lhs.m_data[i + 1] == rhs.m_data[i + 1]);
	}
	return equals;
}

bool operator!=(const HwIdentifier& lhs, const HwIdentifier& rhs) { return !(lhs == rhs); }

}  // namespace hw_identifier
} /* namespace license */
