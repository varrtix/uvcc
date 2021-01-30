/// MIT License
///
/// uvcc/utilities.h
/// uvcc
///
/// created by varrtix on 2021/01/10.
/// Copyright (c) 2021 varrtix. All rights reserved.
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to
/// deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
/// IN THE SOFTWARE.

#ifndef UTILITIES_H
#define UTILITIES_H

#include <uv.h>

#include <iostream>
#include <memory>

#include "exception.h"

namespace uvcc {

template <typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts &&...params) {
  return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

template <typename EnumType,
          typename std::enable_if<std::is_enum<EnumType>::value, int>::type = 0>
constexpr auto enum_cast(const EnumType &enum_case) _NOEXCEPT ->
    typename std::underlying_type<EnumType>::type {
  return static_cast<typename std::underlying_type<EnumType>::type>(enum_case);
}

template <
    typename ClassType,
    typename std::enable_if<std::is_pointer<ClassType>::value, int>::type = 0>
using RawCompletionBlock = typename std::remove_pointer<ClassType>::type;

template <
    typename Success, typename Failure,
    typename std::enable_if<std::is_same<std::exception, Failure>::value ||
                                std::is_base_of<std::exception, Failure>::value,
                            int>::type = 0>
class Result {
 public:
  enum class Type : bool {
    kSuccess = true,
    kFailure = false,
  };

  explicit Result(std::function<Success()> &&catching_body) _NOEXCEPT {
    try {
      success_ = uvcc::make_unique<Success>(catching_body());
      type_ = Type::kSuccess;
    } catch (const Failure &failure) {
      failure_ = uvcc::make_unique<Failure>(failure);
    }
  }

  Result(Result &&) _NOEXCEPT = default;

  Result &operator=(Result &&) _NOEXCEPT = default;

  Result(const Result &) = delete;

  Result &operator=(const Result &) = delete;

  ~Result() = default;

  inline bool as(const Result::Type &type) const _NOEXCEPT {
    return type == type_;
  }

  inline bool asSuccess() const _NOEXCEPT { return uvcc::enum_cast(type_); }

  inline bool asFailure() const _NOEXCEPT { return !uvcc::enum_cast(type_); }

  const std::unique_ptr<Success> &success() const _NOEXCEPT { return success_; }

  const std::unique_ptr<Failure> &failure() const _NOEXCEPT { return failure_; }

 private:
  std::unique_ptr<Success> success_ = nullptr;
  std::unique_ptr<Failure> failure_ = nullptr;

  Result::Type type_ = Type::kFailure;
};

inline bool expr_assert(int err, bool abs = false) _NOEXCEPT {
  return abs ? (err == 0) : (err >= 0);
}

inline void expr_cerr(const uvcc::Exception &exception) _NOEXCEPT {
  std::cerr << exception.what() << std::endl;
}

inline bool expr_cerr_r(int err, bool abs = false) _NOEXCEPT {
  if (expr_assert(err, abs)) return true;
  expr_cerr(uvcc::Exception(err));
  return false;
}

inline void expr_throws(int err, bool abs = false) {
  if (!expr_assert(err, abs)) throw uvcc::Exception(err);
}

enum class LoopOption : int {
  kBlockSignal = UV_LOOP_BLOCK_SIGNAL,
  kMetricsIDLETime = UV_METRICS_IDLE_TIME,
};

enum class RunOption : int {
  kDefault = UV_RUN_DEFAULT,
  kOnce = UV_RUN_ONCE,
  kNoWait = UV_RUN_NOWAIT,
};

template <typename _Tp>
class basic_uv_object {
public:
    typedef _Tp element_type;
    typedef _Tp& reference;
    typedef const _Tp& const_reference;

    basic_uv_object() _NOEXCEPT : obj_ptr_(uvcc::make_unique<element_type>()) {}
    basic_uv_object(const basic_uv_object &) = delete;
    basic_uv_object(basic_uv_object &&other) _NOEXCEPT {
        *this = std::move(other);
    }
    basic_uv_object(element_type &&other) _NOEXCEPT : obj_ptr_(uvcc::make_unique<element_type>(other)) {}
    basic_uv_object &operator=(const basic_uv_object &) = delete;
    basic_uv_object &operator=(basic_uv_object &&other) _NOEXCEPT {
        if (this != &other) {
            obj_ptr_.reset(other.obj_ptr_.release());
        }
        return *this;
    }
    basic_uv_object &operator=(element_type &&other) _NOEXCEPT {
        if (this->obj_ptr_.get() != &other) {
            obj_ptr_.reset(other);
        }
        return *this;
    }
    virtual ~basic_uv_object() { obj_ptr_.release(); }
    
protected:
    std::unique_ptr<element_type> obj_ptr_;
    
    element_type *_get() const _NOEXCEPT { return obj_ptr_.get(); }
};

template <typename _Tp, typename _Tu>
class basic_uv_union_object: virtual protected basic_uv_object<_Tu> {
    static_assert(std::is_union<_Tu>::value, "The template must be union type.");
public:
    typedef _Tp element_type;
    typedef _Tp& reference;
    typedef const _Tp& const_reference;
    
    typedef _Tu union_element_type;
//    typedef _Tu& reference_union;
//    typedef const _Tu& const_reference_union;
    
    basic_uv_union_object() _NOEXCEPT : basic_uv_object<union_element_type>() {}
    basic_uv_union_object(const basic_uv_union_object &&) = delete;
    basic_uv_union_object(basic_uv_union_object &&other) _NOEXCEPT {
        *this = std::move(other);
    }
    basic_uv_union_object(union_element_type &&other) _NOEXCEPT : basic_uv_object<union_element_type>(other) {}
    basic_uv_union_object &operator=(const basic_uv_union_object &) = delete;
    basic_uv_union_object &operator=(basic_uv_union_object &&other) _NOEXCEPT {
        if (this != &other) {
            this->obj_ptr_.reset(other.obj_ptr_.release());
        }
        return *this;
    }
    basic_uv_union_object &operator=(union_element_type &&other) _NOEXCEPT {
        if (this->obj_ptr_.get() != &other) {
            this->obj_ptr_.reset(other);
        }
        return *this;
    }
    
protected:
    virtual element_type *_get() const _NOEXCEPT = 0;
};

class basic_fd: public basic_uv_union_object<uv_handle_t, uv_any_handle> {
public:
    basic_fd() {}
    ~basic_fd() { std::cout << "basic fd" << std::endl; }
    
    element_type *_get() const _NOEXCEPT { return reinterpret_cast<element_type *>(_get()); }
};

class stream_fd: public basic_uv_union_object<uv_stream_t, uv_any_handle> {};

class tcp_fd: public basic_uv_union_object<uv_tcp_t, uv_any_handle> {};

template <typename Type, typename UnionType = void,
          typename std::enable_if<std::is_union<UnionType>::value ||
                                      (std::is_void<UnionType>::value &&
                                       !std::is_union<Type>::value),
                                  int>::type = 0>
class BaseObject {
 public:
  using Self = typename std::conditional<std::is_void<UnionType>::value, Type,
                                         UnionType>::type;
  using UnionSelf = UnionType;

  BaseObject() : raw_(uvcc::make_unique<Self>()) {}
  BaseObject(const Self &self) : raw_(uvcc::make_unique<Self>(self)) {}
  BaseObject(Self &&self) _NOEXCEPT : raw_(uvcc::make_unique<Self>(self)) {}
  //  BaseObject(const BaseObject &) = default;
  BaseObject(BaseObject &&) _NOEXCEPT = default;
  //  BaseObject &operator=(const BaseObject &) = default;
  BaseObject &operator=(BaseObject &&) _NOEXCEPT = default;
  BaseObject &operator=(const Self &self) {
    if (&self != this->_someRaw()) raw_ = uvcc::make_unique<Self>(self);
    return *this;
  }
  BaseObject &operator=(Self &&self) _NOEXCEPT {
    if (&self != this->_someRaw()) *raw_ = std::move(self);
    return *this;
  }
  virtual ~BaseObject() = default;

 protected:
  std::unique_ptr<Self> raw_ = 0;

  template <typename RawValuePointer = typename std::add_pointer<Type>::type,
            typename std::enable_if<std::is_pointer<RawValuePointer>::value,
                                    int>::type = 0>
  inline RawValuePointer _someRaw() const _NOEXCEPT {
    return reinterpret_cast<RawValuePointer>(raw_.get());
  }
};

}  // namespace uvcc

#endif  // UTILITIES_H
