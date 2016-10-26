#pragma once

#include <vector>

#include <reflectance/any.hpp>

namespace reflectance {
using argument_list = std::vector<any>;

class abstract_object_factory {
public:
	virtual ~abstract_object_factory() = default;
	virtual any construct(const argument_list& argument_list) = 0;
};
}
