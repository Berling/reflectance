#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <reflectance/any.hpp>
#include <reflectance/object_factories.hpp>
#include <reflectance/properties.hpp>

struct node {
	std::string name;
	std::string type;
	std::string value;
	std::vector<node> children;
};

class object_description_converter {
private:
	const reflectance::object_factories& _object_factories;

public:
	object_description_converter(const reflectance::object_factories& factories)
	: _object_factories{factories} {
	}

	reflectance::any convert_fundamental_type(const node& ast) const {
		if (ast.type == "int") {
			return reflectance::make_any<int>(std::stoi(ast.value));
		} else if (ast.type == "float") {
			return reflectance::make_any<float>(std::stof(ast.value));
		} else if (ast.type == "string") {
			return reflectance::make_any<std::string>(ast.value);
		} else {
			return reflectance::any{};
		}
	}

	reflectance::any convert(const node& ast) const {
		if (ast.children.empty()) {
			return convert_fundamental_type(ast);
		}

		reflectance::named_argument_list arguments;
		for (auto&& node : ast.children) {
			arguments[node.name] = convert(node);
		}

		return _object_factories.construct(ast.type, arguments);
	}
};

struct vec2 {
	float x;
	float y;

	static reflectance::properties<vec2>& properties() noexcept {
		static reflectance::properties<vec2> properties{[](reflectance::properties<vec2>& properties) {
			properties("x", &vec2::x);
			properties("y", &vec2::y);
		}};
		return properties;
	}
};

class widget {
private:
	std::string _name;
	vec2 _position;

public:
	auto& name() const noexcept {
		return _name;
	}

	void rename(const std::string& name) noexcept {
		_name = name;
	}

	auto& position() const noexcept {
		return _position;
	}

	void relocate(const vec2& position) noexcept {
		_position = position;
	}

	static reflectance::properties<widget>& properties() {
		static reflectance::properties<widget> properties{[](reflectance::properties<widget>& properties) {
			properties("name", reflectance::getter<std::string, widget>{&widget::name},
								 reflectance::setter<std::string, widget>{&widget::rename});
			properties("position", reflectance::getter<vec2, widget>{&widget::position},
								 reflectance::setter<vec2, widget>{&widget::relocate});
		}};
		return properties;
	}
};

int main(int, char**) { // clang-format off
	node ast = {
		"",
		"widget",
		"",
		{
			{
				"name",
				"string",
				"test_widget",
				{}
			},
			{
				"position",
				"vec2",
				"",
				{
					{
						"x",
						"float",
						"10.f",
						{}
					},
					{
						"y",
						"float",
						"25.f",
						{}
					}
				}
			}
		}
	};
	// clang-format on

	reflectance::object_factories factories{};
	factories.register_type<vec2>("vec2");
	factories.register_type<widget>("widget");

	object_description_converter converter{factories};

	auto instance = converter.convert(ast);
	if (instance.type() == typeid(void)) {
		std::cout << "type could not be constructed" << std::endl;
	} else if (instance.type() == typeid(widget)) {
		auto& typed_referenc = reflectance::any_cast<widget&>(instance);

		std::cout << "widget" << std::endl;
		std::cout << "name: " << typed_referenc.name() << std::endl;
		std::cout << "position: vec2{" << typed_referenc.position().x << ", "
							<< typed_referenc.position().y << "}" << std::endl;
	}

	return 0;
}
