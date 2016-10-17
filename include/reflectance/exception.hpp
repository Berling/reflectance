#pragma once

#include <stdexcept>

namespace reflectance {
class bad_accessor_call : public std::runtime_error {
public:
	bad_accessor_call(const std::string& what_arg) : std::runtime_error{what_arg} {}
	virtual ~bad_accessor_call() = default;
};

class bad_property_cast : public std::bad_cast {
private:
	std::string _what_arg;

public:
	bad_property_cast(const std::string& property_name) {
		_what_arg = "attempted cast to wrong type with property " + property_name;
	}

	virtual ~bad_property_cast() = default;

	virtual const char* what() const noexcept {
		return _what_arg.c_str();
	}
};
}
