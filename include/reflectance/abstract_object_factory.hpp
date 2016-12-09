#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <reflectance/any.hpp>

namespace reflectance {
	using argument_list = std::vector<any>;
	using named_argument_list = std::unordered_map<std::string, any>;

	class abstract_object_factory {
	public:
		virtual ~abstract_object_factory() = default;
		virtual any construct(const argument_list& argument_list) = 0;
		virtual any construct(const named_argument_list& argument_list) = 0;
	};
}
