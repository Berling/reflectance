#pragma once

#include <stdexcept>

#include <reflectance/abstract_object_factory.hpp>

namespace reflectance {
template <typename T>
class object_factory : public abstract_object_factory {
public:
	virtual any construct(const argument_list& argument_list) {
		auto& properties = T::properties();
		if(argument_list.size() != properties.size()) {
			throw std::invalid_argument{"invalid length of argument list"};
		}
		auto instance = reflectance::make_any<T>();
		for(auto index = argument_list::size_type{0}; index < argument_list.size(); ++index) {
			auto& property = properties.at(index);
			auto& argument = argument_list[index];
			auto& assigner = properties.assigner(property.type().hash_code());
			assigner.assign(property, instance, argument);
		}
		return instance;
	}

	virtual any construct(const named_argument_list& argument_list) {
		auto& properties = T::properties();
		auto instance = reflectance::make_any<T>();
		for(auto& argument_pair : argument_list) {
			auto& property_name = argument_pair.first;
			auto& argument = argument_pair.second;
			auto& property = properties.at(property_name);
			auto& assigner = properties.assigner(property.type().hash_code());
			assigner.assign(property, instance, argument);
		}
		return instance;
	}
};
}
