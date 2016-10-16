#pragma once

#include <string>
#include <type_traits>
#include <typeinfo>

#include <reflectance/abstract_property.hpp>

namespace reflectance {
template <typename ValueType>
using accessor_type = std::conditional_t<std::is_fundamental_v<ValueType>, ValueType, const ValueType&>;

template <typename ValueType, typename BaseType>
class property : public abstract_property {
public:
	property(const std::string& name) : abstract_property{name} {}
	virtual ~property() = default;
	virtual accessor_type<ValueType> get(const BaseType& instance) const = 0;
	virtual void set(BaseType& instance, accessor_type<ValueType> value) = 0;
	virtual bool getable() const noexcept = 0;
	virtual bool setable() const noexcept = 0;

	virtual size_t hash_code() const noexcept {
		return typeid(ValueType).hash_code();
	}
};
}
