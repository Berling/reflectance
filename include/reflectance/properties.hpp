#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include <reflectance/abstract_property.hpp>
#include <reflectance/directly_linked_property.hpp>
#include <reflectance/linked_property.hpp>

namespace reflectance {
template <typename ValueType, typename OwnerType>
struct getter {
	getter_type<ValueType, OwnerType> value;

	getter(const getter_type<ValueType, OwnerType>& value) : value{value} {}
};

template <typename ValueType, typename OwnerType>
struct setter {
	setter_type<ValueType, OwnerType> value;

	setter(const setter_type<ValueType, OwnerType>& value) : value{value} {}
};

template <typename BaseType>
class properties {
private:
	std::vector<std::unique_ptr<abstract_property>> _properties;
	std::unordered_map<std::string, std::vector<std::unique_ptr<abstract_property>>::size_type> _property_names;

public:
	properties(const std::function<void(properties<BaseType>&)>& initializer) {
		initializer(*this);
	}

	template <typename ValueType, typename OwnerType = BaseType>
	void emplace(const std::string& name, const getter<ValueType, OwnerType>& getter,
							 const setter<ValueType, OwnerType>& setter) {
		_property_names[name] = _properties.size();
		_properties.emplace_back(make_linked_property<ValueType, OwnerType, BaseType>(name, getter.value, setter.value));
	}

	template <typename ValueType, typename OwnerType = BaseType>
	void emplace(const std::string& name, const member_type<ValueType, OwnerType>& member) {
		_property_names[name] = _properties.size();
		_properties.emplace_back(make_directly_linked_property<ValueType, OwnerType, BaseType>(name, member));
	}

	template <typename ValueType, typename OwnerType = BaseType>
	properties& operator()(const std::string& name, const getter<ValueType, OwnerType>& getter,
												 const setter<ValueType, OwnerType>& setter) {
		emplace(name, getter, setter);
		return *this;
	}

	template <typename ValueType, typename OwnerType = BaseType>
	properties& operator()(const std::string& name, const member_type<ValueType, OwnerType>& member) {
		emplace(name, member);
		return *this;
	}

	auto size() const noexcept {
		return _properties.size();
	}

	auto begin() noexcept {
		return _properties.begin();
	}

	auto begin() const noexcept {
		return _properties.begin();
	}

	auto cbegin() const noexcept {
		return _properties.cbegin();
	}

	auto end() noexcept {
		return _properties.end();
	}

	auto end() const noexcept {
		return _properties.end();
	}

	auto cend() const noexcept {
		return _properties.cend();
	}

	auto& get() const noexcept {
		return _properties;
	}

	auto& at(const std::string& property_name) {
		return _properties.at(_property_names.at(property_name));
	}

	auto& at(const std::string& property_name) const {
		return _properties.at(_property_names.at(property_name));
	}

	auto find(const std::string& property_name) {
		auto property_name_it = _property_names.find(property_name);
		if(property_name_it == _property_names.end()) {
			return _properties.end();
		}
		return _properties.begin() + property_name_it->second;
	}

	auto find(const std::string& property_name) const {
		auto property_name_it = _property_names.find(property_name);
		if(property_name_it == _property_names.end()) {
			return _properties.end();
		}
		return _properties.begin() + property_name_it->second;
	}

	auto empty() const noexcept {
		return _properties.empty();
	}
};
}
