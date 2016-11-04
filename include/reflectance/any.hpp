#pragma once

#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>

namespace reflectance {
template <typename T>
struct in_place_type_t {};

class bad_any_cast : public std::bad_cast {
public:
	virtual ~bad_any_cast() = default;
	virtual const char* what() const noexcept {
		return "type mismatch";
	}
};

class any_wrapper {
public:
	virtual ~any_wrapper() = default;
	virtual std::unique_ptr<any_wrapper> copy() = 0;
	virtual std::unique_ptr<any_wrapper> move() = 0;
	virtual void* get() = 0;
	virtual const void* get() const = 0;
};

template <typename ValueType>
class typed_any_wrapper : public any_wrapper {
private:
	std::shared_ptr<void> _contained_value;

public:
	virtual ~typed_any_wrapper() = default;

	typed_any_wrapper(ValueType&& value) {
		_contained_value = std::make_shared<ValueType>(std::forward<ValueType>(value));
	}

	template <typename... Args>
	typed_any_wrapper(Args&&... args) {
		_contained_value = std::make_shared<ValueType>(std::forward<Args>(args)...);
	}

	virtual std::unique_ptr<any_wrapper> copy() {
		auto& typed_contained_value = *static_cast<ValueType*>(_contained_value.get());
		return std::make_unique<typed_any_wrapper<ValueType>>(typed_contained_value);
	}

	virtual std::unique_ptr<any_wrapper> move() {
		auto& typed_contained_value = *static_cast<ValueType*>(_contained_value.get());
		auto moved_value = std::make_unique<typed_any_wrapper<ValueType>>(std::move(typed_contained_value));
		_contained_value = nullptr;
		return std::move(moved_value);
	}

	virtual void* get() noexcept {
		return _contained_value.get();
	}

	virtual const void* get() const noexcept {
		return _contained_value.get();
	}
};

class any {
private:
	std::unique_ptr<any_wrapper> _heap_allocated_conatined_value;
	std::aligned_storage_t<3 * sizeof(size_t)> _stack_allocated_contained_value{};
	bool _stack_allocated = false;
	std::reference_wrapper<const std::type_info> _type = typeid(void);

	template <typename ValueType>
	friend const ValueType* any_cast(const any*) noexcept;

	template <typename ValueType>
	friend ValueType* any_cast(any*) noexcept;

public:
	constexpr any() = default;

	any(const any& other) : _type{other._type} {
		if(other._stack_allocated) {
			std::memcpy(&_stack_allocated_contained_value, &other._stack_allocated_contained_value, 3 * sizeof(size_t));
			_stack_allocated = true;
		} else {
			_heap_allocated_conatined_value = other._heap_allocated_conatined_value->copy();
			_stack_allocated = false;
		}
	}

	any& operator=(const any& other) {
		if(other._stack_allocated) {
			std::memcpy(&_stack_allocated_contained_value, &other._stack_allocated_contained_value, 3 * sizeof(size_t));
			_stack_allocated = true;
		} else {
			_heap_allocated_conatined_value = other._heap_allocated_conatined_value->copy();
			_stack_allocated = false;
		}
		_type = other._type;
		return *this;
	}

	any(any&& other) {
		if(other._stack_allocated) {
			std::memcpy(&_stack_allocated_contained_value, &other._stack_allocated_contained_value, 3 * sizeof(size_t));
			_stack_allocated = true;
			other._stack_allocated = false;
		} else {
			_heap_allocated_conatined_value = std::move(other._heap_allocated_conatined_value);
			_stack_allocated = false;
			other._heap_allocated_conatined_value = nullptr;
		}
		_type = std::move(other._type);
	}

	any& operator=(any&& other) {
		if(other._stack_allocated) {
			std::memcpy(&_stack_allocated_contained_value, &other._stack_allocated_contained_value, 3 * sizeof(size_t));
			_stack_allocated = true;
			other._stack_allocated = false;
		} else {
			_heap_allocated_conatined_value = std::move(other._heap_allocated_conatined_value);
			_stack_allocated = false;
			other._heap_allocated_conatined_value = nullptr;
		}
		_type = std::move(other._type);
		return *this;
	}

	template <typename ValueType>
	any(std::enable_if_t<!std::is_same<std::decay_t<ValueType>, any>::value, ValueType>&& value) {
		using decayed_value_type = std::decay_t<ValueType>;
		static_assert(std::is_copy_constructible<decayed_value_type>::value);
		decayed_value_type contained_value{std::forward<ValueType>(value)};
		if(sizeof(decayed_value_type) <= 3 * sizeof(size_t) && std::is_trivially_copyable<decayed_value_type>::value) {
			new(&_stack_allocated_contained_value) decayed_value_type{std::forward<ValueType>(value)};
			_stack_allocated = true;
		} else {
			_heap_allocated_conatined_value =
					std::make_unique<typed_any_wrapper<decayed_value_type>>(std::forward<decayed_value_type>(value));
			_stack_allocated = false;
		}
		_type = typeid(decayed_value_type);
	}

	template <typename T, typename... Args>
	any(in_place_type_t<T>, Args... args) {
		static_assert(std::is_constructible<T, Args...>::value);
		if(sizeof(T) <= 3 * sizeof(size_t) && std::is_trivially_copyable<T>::value) {
			new(&_stack_allocated_contained_value) T{std::forward<Args>(args)...};
			_stack_allocated = true;
		} else {
			_heap_allocated_conatined_value = std::make_unique<typed_any_wrapper<T>>(std::forward<Args>(args)...);
			_stack_allocated = false;
		}
		_type = typeid(T);
	}

	void swap(any& other) {
		auto tmp = std::move(*this);
		*this = std::move(other);
		other = std::move(tmp);
	}

	template <typename ValueType>
	any& operator=(std::enable_if_t<!std::is_same<std::decay_t<ValueType>, any>::value, ValueType>&& rhs) {
		static_assert(std::is_copy_constructible<std::decay_t<ValueType>>::value);
		any(std::forward<ValueType>(rhs)).swap(*this);
		rhs._stack_allocated = false;
		rhs._heap_allocated_conatined_value = nullptr;
		return *this;
	}

	template <typename T, typename... Args>
	void emplace(Args&&... args) {
		static_assert(std::is_constructible<T, Args...>::value);
		if(sizeof(T) <= 3 * sizeof(size_t) && std::is_trivially_copyable<T>::value) {
			new(&_stack_allocated_contained_value) T{std::forward<Args>(args)...};
			_stack_allocated = true;
		} else {
			_heap_allocated_conatined_value = std::make_unique<T>(std::forward<Args>(args)...);
			_stack_allocated = false;
		}
		_type = typeid(T);
	}

	bool has_value() const noexcept {
		if(!_stack_allocated && !_heap_allocated_conatined_value) {
			return false;
		}
		return true;
	}

	void reset() {
		if(has_value()) {
			_stack_allocated = false;
			_heap_allocated_conatined_value = nullptr;
		}
	}

	const std::type_info& type() const noexcept {
		if(has_value()) {
			return _type;
		}
		return typeid(void);
	}

private:
	void* contained_value() {
		if(_stack_allocated) {
			return &_stack_allocated_contained_value;
		} else {
			return _heap_allocated_conatined_value->get();
		}
	}

	const void* contained_value() const {
		if(_stack_allocated) {
			return &_stack_allocated_contained_value;
		} else {
			return _heap_allocated_conatined_value->get();
		}
	}
};

template <typename ValueType>
ValueType* any_cast(any* operand) noexcept {
	if(operand) {
		if(operand->type().hash_code() == typeid(ValueType).hash_code()) {
			return reinterpret_cast<ValueType*>(operand->contained_value());
		}
	}
	return nullptr;
}

template <typename ValueType>
const ValueType* any_cast(const any* operand) noexcept {
	if(operand) {
		if(operand->type().hash_code() == typeid(ValueType).hash_code()) {
			return reinterpret_cast<const ValueType*>(operand->contained_value());
		}
	}
	return nullptr;
}

template <typename ValueType>
ValueType any_cast(const any& operand) {
	static_assert(std::is_reference<ValueType>::value || std::is_copy_constructible<ValueType>::value);
	if(operand.type().hash_code() == typeid(ValueType).hash_code()) {
		return *any_cast<std::add_const_t<std::remove_reference_t<ValueType>>>(&operand);
	}
	throw bad_any_cast{};
}

template <typename ValueType>
ValueType any_cast(any& operand) {
	static_assert(std::is_reference<ValueType>::value || std::is_copy_constructible<ValueType>::value);
	if(operand.type().hash_code() == typeid(ValueType).hash_code()) {
		return *any_cast<std::remove_reference_t<ValueType>>(&operand);
	}
	throw bad_any_cast{};
}

template <typename ValueType>
ValueType any_cast(any&& operand) {
	static_assert(std::is_reference<ValueType>::value || std::is_copy_constructible<ValueType>::value);
	if(operand.type().hash_code() == typeid(ValueType).hash_code()) {
		return *any_cast<std::remove_reference_t<ValueType>>(&operand);
	}
	throw bad_any_cast{};
}

template <typename T, typename... Args>
any make_any(Args&&... args) {
	return any{in_place_type_t<T>(), std::forward<Args>(args)...};
}
}

namespace std {
template <>
void swap(reflectance::any& lhs, reflectance::any& rhs) {
	lhs.swap(rhs);
}
}
