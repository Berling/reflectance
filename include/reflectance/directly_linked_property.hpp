#pragma once

#include <memory>
#include <string>

#include <reflectance/exception.hpp>
#include <reflectance/property.hpp>

namespace reflectance {
template <typename ValueType, typename OwnerType>
using member_type = ValueType OwnerType::*;

template <typename ValueType, typename OwnerType, typename BaseType = OwnerType>
class directly_linked_property : public property<ValueType, BaseType> {
private:
	member_type<ValueType, OwnerType> _member;

public:
	directly_linked_property(const std::string& name, const member_type<ValueType, OwnerType>& member)
			: property<ValueType, BaseType>{name}, _member{member} {
		if(_member == nullptr) {
			throw bad_accessor_call{"no accessor of type 'member' found in property " + name};
		}
	}

	virtual ~directly_linked_property() = default;

	virtual accessor_type<ValueType> get(const BaseType& instance) const {
		auto& typed_reference = static_cast<const OwnerType&>(instance);
		return typed_reference.*_member;
	}

	virtual void set(BaseType& instance, accessor_type<ValueType> value) {
		auto& typed_reference = static_cast<OwnerType&>(instance);
		typed_reference.*_member = value;
	}

	virtual bool getable() const noexcept {
		return true;
	}

	virtual bool setable() const noexcept {
		return true;
	}
};

template <typename ValueType, typename OwnerType, typename BaseType = OwnerType>
using directly_linked_property_ptr = std::unique_ptr<directly_linked_property<ValueType, OwnerType, BaseType>>;

template <typename ValueType, typename OwnerType, typename BaseType = OwnerType>
auto make_directly_linked_property(const std::string& name, const member_type<ValueType, OwnerType>& member) {
	return std::make_unique<directly_linked_property<ValueType, OwnerType, BaseType>>(name, member);
}
}
