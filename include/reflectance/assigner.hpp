#pragma once

#include <reflectance/abstract_assigner.hpp>
#include <reflectance/property.hpp>

namespace reflectance {
template <typename ValueType, typename OwnerType, typename BaseType = OwnerType>
class assigner : public abstract_assigner {
public:
	virtual void assign(abstract_property& property, any& instance, const any& value) {
		auto& typed_property = reflectance::property_cast<ValueType, BaseType>(property);
		auto& typed_instance = reflectance::any_cast<OwnerType&>(instance);
		auto& typed_value = reflectance::any_cast<const ValueType&>(value);
		typed_property.set(typed_instance, typed_value);
	}
};
}
