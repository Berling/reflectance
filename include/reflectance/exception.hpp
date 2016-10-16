#pragma once

#include <stdexcept>

namespace reflectance {
class bad_accessor_call : public std::runtime_error {
public:
	bad_accessor_call(const std::string& what_arg) : std::runtime_error{what_arg} {}
	virtual ~bad_accessor_call() = default;
};
}
