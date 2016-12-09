#pragma once

#include <reflectance/abstract_property.hpp>
#include <reflectance/any.hpp>

namespace reflectance {
	class abstract_assigner {
	public:
		virtual ~abstract_assigner() = default;
		virtual void assign(abstract_property& property, any& instance, const any& value) = 0;
	};
}
