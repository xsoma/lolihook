#pragma once

namespace pattern {
	// container
	using patterns_t = std::vector< Address >;
	using pattern_byte_t = std::pair< uint8_t, bool >;

	Address find(Address start, size_t len, const std::string& pat);

	Address find(const PE::Module& module, const std::string& pat);

	patterns_t FindAll(Address start, size_t len, const std::string& pat);

	patterns_t FindAll(const PE::Module& module, const std::string& pat);
}