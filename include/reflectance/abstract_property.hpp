#pragma once

#include <string>
#include <typeinfo>

namespace reflectance {
	class abstract_property {
	private:
		std::string _name;

	public:
		abstract_property(const std::string& name) : _name{name} {
		}
		virtual ~abstract_property() = 0;

		auto& name() const noexcept {
			return _name;
		}

		virtual const std::type_info& base_type() const noexcept = 0;
		virtual const std::type_info& type() const noexcept = 0;
	};

	abstract_property::~abstract_property() {
	}
}
