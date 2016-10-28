#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>

#include <reflectance/abstract_object_factory.hpp>
#include <reflectance/any.hpp>
#include <reflectance/object_factory.hpp>

namespace reflectance {
class object_factories {
private:
	std::unordered_map<std::string, std::unique_ptr<abstract_object_factory>> _factories;

public:
	template <typename T>
	void register_type(const std::string& name) {
		using type = std::remove_pointer_t<std::remove_reference_t<std::decay_t<T>>>;
		_factories[name] = std::make_unique<object_factory<type>>();
	}

	any construct(const std::string& type, const argument_list& argument_list) const {
		auto factory = _factories.find(type);
		if(factory != _factories.end()) {
			return factory->second->construct(argument_list);
		}
		return any{};
	}

	any construct(const std::string& type, const named_argument_list& argument_list) const {
		auto factory = _factories.find(type);
		if(factory != _factories.end()) {
			return factory->second->construct(argument_list);
		}
		return any{};
	}

	template <typename T>
	T construct(const std::string& type, const argument_list& argument_list) const {
		static_assert(!std::is_pointer_v<T> && !std::is_reference_v<T>);
		return any_cast<T>(construct(type, argument_list));
	}

	template <typename T>
	T construct(const std::string& type, const named_argument_list& argument_list) const {
		static_assert(!std::is_pointer_v<T> && !std::is_reference_v<T>);
		return any_cast<T>(construct(type, argument_list));
	}
};
}
