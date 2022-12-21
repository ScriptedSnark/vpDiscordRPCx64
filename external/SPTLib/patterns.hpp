/**
 * Copyright - xWhitey ~ ScriptedSnark, 2022.
 * patterns.hpp - Our pattern-system
 */

#ifdef PATTERNS_HPP_RECURSE_GUARD
#error Recursive header files inclusion detected in patterns.hpp
#else //PATTERNS_HPP_RECURSE_GUARD

#define PATTERNS_HPP_RECURSE_GUARD

#ifndef PATTERNS_HPP_GUARD
#define PATTERNS_HPP_GUARD
#pragma once

#include "MemUtils.hpp"

#define PATTERNS(name, ...) const auto name = new CIntelligentPattern(__VA_ARGS__);

typedef unsigned char uint8_t;

const static std::vector<std::string> split(const std::string& s, char seperator) {
	std::vector<std::string> output;

	int prev_pos = 0, pos = 0;

	while ((pos = s.find(seperator, pos)) != std::string::npos) {
		std::string substring(s.substr(prev_pos, pos - prev_pos));

		output.push_back(substring);

		prev_pos = ++pos;
	}

	output.push_back(s.substr(prev_pos, pos - prev_pos));

	return output;
}

constexpr const uint8_t hex(char c) {
	return (c >= '0' && c <= '9') ? static_cast<uint8_t>(c - '0')
		: (c >= 'a' && c <= 'f') ? static_cast<uint8_t>(c - 'a' + 10)
		: (c >= 'A' && c <= 'F') ? static_cast<uint8_t>(c - 'A' + 10)
		: throw std::domain_error("not a hex digit");
}

typedef struct CIntelligentPattern {
	explicit CIntelligentPattern(std::string peidPattern, std::string version) {
		std::vector<uint8_t> pattern = {};
		std::string mask = "";
		std::map<std::vector<uint8_t>, std::string> _pattern;
		auto splitten = split(peidPattern, ' ');
		for (unsigned int idx = 0; idx < splitten.size(); idx++) {
			if (splitten[idx] == "??") {
				mask.append("?");
				pattern.push_back(0xCC);
			}
			else {
				mask.append("x");
				pattern.push_back(hex(splitten[idx][0]) * 16 + hex(splitten[idx][1]));
			}
		}
		_pattern.insert(std::make_pair(pattern, mask));
		std::map<decltype(_pattern), std::string> final_pattern;
		final_pattern.insert(std::make_pair(_pattern, version));
		patterns.push_back(final_pattern);
	}
	explicit CIntelligentPattern(std::vector<std::string> peidPatterns, std::vector<std::string> versions) {
		for (unsigned int idx = 0; idx < peidPatterns.size(); idx++) {
			auto peidPattern = peidPatterns.at(idx);
			auto version = versions.at(idx);
			std::vector<uint8_t> pattern = {};
			std::string mask = "";
			std::map<std::vector<uint8_t>, std::string> _pattern;
			auto splitten = split(peidPattern, ' ');
			for (unsigned int idx = 0; idx < splitten.size(); idx++) {
				if (splitten[idx] == "??") {
					mask.append("?");
					pattern.push_back(0xCC);
				}
				else {
					mask.append("x");
					pattern.push_back(hex(splitten[idx][0]) * 16 + hex(splitten[idx][1]));
				}
			}
			_pattern.insert(std::make_pair(pattern, mask));
			std::map<decltype(_pattern), std::string> final_pattern;
			final_pattern.insert(std::make_pair(_pattern, version));
			patterns.push_back(final_pattern);
		}
	}
	std::vector<std::map<std::map<std::vector<uint8_t>, std::string>, std::string>> patterns;
	//vector<map<map<pattern, mask>, version>
} CIntelligentPattern;

/**
 * @author: xWhitey
 * @comment: _Module can't be nullptr!
 */
_Success_(return == true) template<class ResultType> bool FindAsync(_In_ void* _Module, _In_ std::vector<uint8_t> _Pattern, _In_ const char* _Mask, _Out_ ResultType& _Result) {
	if (!_Module) return false;
	if (!_Mask[0] || !_Mask) return false;

	auto module_size = MemUtils::GetModuleSize(_Module);

	unsigned int pattern_length = strlen(_Mask);
	unsigned char* based = reinterpret_cast<unsigned char*>(_Module);

	for (unsigned int idx = 0; idx < module_size - pattern_length; ++idx) {
		bool found = true;
		for (unsigned int i = 0; i < pattern_length; i++) {
			if (_Mask[i] != '?' && based[idx + i] != _Pattern[i]) {
				found = false;
				break;
			}
		}

		if (found) {
			_Result = ((ResultType)&based[idx]);

			return true;
		}
	}

	return false;
}

template<class ResultType> void FindAsync(_In_ void* _Module, _In_ CIntelligentPattern* _Pattern, _Out_ ResultType& _Result, _In_ std::function<void(std::string)> _ToDoUponFound, _In_ std::function<void()> _ToDoIfCouldntFind) {
	bool found = false;
	for (auto p : _Pattern->patterns) {
		for (auto val : p) {
			auto pattern = val.first;
			auto version = val.second;
			for (auto m : pattern) {
				auto bytes = m.first;
				auto mask = m.second;
				if (found = FindAsync(_Module, bytes, mask.c_str(), _Result)) {
					_ToDoUponFound(version);
					return;
				}
			}
		}
	}
	if (!found) _ToDoIfCouldntFind();
}

template<class ResultType> bool FindAsync(_In_ void* _Module, _In_ CIntelligentPattern* _Pattern, _Out_ ResultType& _Result) {
	for (auto p : _Pattern->patterns) {
		for (auto val : p) {
			auto pattern = val.first;
			auto version = val.second;
			for (auto m : pattern) {
				auto bytes = m.first;
				auto mask = m.second;
				return FindAsync(_Module, bytes, mask.c_str(), _Result);
			}
		}
	}
}

/**
 * Warning! You must follow this example in case of creating custom pattern: pattern1, pattern2, mask1, mask2, version1, version2
 * Otherwise you'll break the whole pattern-system!
 */
namespace patterns
{
	PATTERNS(JACK_ConsolePrint, "48 89 4C 24 ?? 48 89 54 24 ?? 4C 89 44 24 ?? 4C 89 4C 24 ?? 48 83 EC 48 48 C7 44 24 ?? FE FF FF FF 48 83 3D ?? ?? ?? ?? 00 0F 84 ?? ?? ?? ?? 48 85 C9 0F 84 ?? ?? ?? ?? 48 8D 44 24 ??", "NonSteam");
}

#endif //PATTERNS_HPP_GUARD

#undef PATTERNS_HPP_RECURSE_GUARD
#endif //PATTERNS_HPP_RECURSE_GUARD
