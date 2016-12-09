#pragma once

#include <functional>
#include <memory>
#include <string>
#include <type_traits>

#include <reflectance/exception.hpp>
#include <reflectance/property.hpp>

namespace reflectance {
	template <typename ValueType, typename OwnerType>
	using getter_type = accessor_type<ValueType> (OwnerType::*)() const;

	template <typename ValueType, typename OwnerType>
	using setter_type = void (OwnerType::*)(accessor_type<ValueType>);

	template <typename ValueType, typename OwnerType, typename BaseType = OwnerType>
	class linked_property : public property<ValueType, BaseType> {
	private:
		getter_type<ValueType, OwnerType> _getter;
		setter_type<ValueType, OwnerType> _setter;

	public:
		linked_property(const std::string& name,
										const getter_type<ValueType, OwnerType>& getter,
										const setter_type<ValueType, OwnerType>& setter)
		: property<ValueType, BaseType>{name}, _getter{getter}, _setter{setter} {
		}
		virtual ~linked_property() = default;

		virtual accessor_type<ValueType> get(const BaseType& instance) const {
			auto& typed_reference = static_cast<const OwnerType&>(instance);
			if (!_getter) {
				throw bad_accessor_call{"no accesor of type 'getter' specified in property "
																+ abstract_property::name()};
			}
			return std::invoke(_getter, typed_reference);
		}

		virtual void set(BaseType& instance, accessor_type<ValueType> value) {
			auto& typed_reference = static_cast<OwnerType&>(instance);
			if (!_setter) {
				throw bad_accessor_call{"no accessor of type 'setter' specified in property "
																+ abstract_property::name()};
			}
			std::invoke(_setter, typed_reference, value);
		}

		virtual bool getable() const noexcept {
			return _getter != nullptr;
		}

		virtual bool setable() const noexcept {
			return _setter != nullptr;
		}
	};

	template <typename ValueType, typename OwnerType, typename BaseType = OwnerType>
	using linked_property_ptr = std::unique_ptr<linked_property<ValueType, OwnerType, BaseType>>;

	template <typename ValueType, typename OwnerType, typename BaseType = OwnerType>
	auto make_linked_property(const std::string& name,
														const getter_type<ValueType, OwnerType>& getter,
														const setter_type<ValueType, OwnerType>& setter) {
		return std::make_unique<linked_property<ValueType, OwnerType, BaseType>>(name, getter, setter);
	}
}
