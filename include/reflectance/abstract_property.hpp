#pragma once

#include <string>

namespace reflectance {
class abstract_property {
private:
	std::string _name;

public:
	abstract_property(const std::string& name) : _name{name} {}
	virtual ~abstract_property() = 0;

	auto& name() const noexcept {
		return _name;
	}

	virtual size_t base_type_hash_code() const noexcept = 0;
	virtual size_t hash_code() const noexcept = 0;
};

abstract_property::~abstract_property() {}
}
