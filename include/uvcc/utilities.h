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

#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <type_traits>

#include "exception.h"

namespace uvcc {

template <typename _Tp, typename... _Ts>
std::unique_ptr<_Tp> make_unique(_Ts &&...params) {
    return std::unique_ptr<_Tp>(new _Tp(std::forward<_Ts>(params)...));
}

template <typename _enumT,
typename _underlyingT = typename std::underlying_type<_enumT>::type>
constexpr auto enum_cast(const _enumT &enum_case) _NOEXCEPT -> _underlyingT {
    static_assert(std::is_enum<_enumT>::value,
                  "The '_enumT' must be a enum type.");
    return static_cast<_underlyingT>(enum_case);
}

template <
typename ClassType,
typename std::enable_if<std::is_pointer<ClassType>::value, int>::type = 0>
using RawCompletionBlock = typename std::remove_pointer<ClassType>::type;

template <typename _funcT>
struct completion_block {
private:
    typedef typename std::conditional<std::is_pointer<_funcT>::value,
    typename std::remove_pointer<_funcT>::type,
    _funcT>::type _c_style_type;
    static_assert(std::is_function<_c_style_type>::value,
                  "The '_classT' must be a function.");
    
public:
    typedef _c_style_type c_type;
    typedef std::function<c_type> type;
};

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

//template <typename _successT, typename _failureT>
//class result {
//  static_assert(std::is_same<std::exception, _failureT>::value ||
//                    std::is_base_of<std::exception, _failureT>::value,
//                "The '_failureT' should be a std::exception or inherited by "
//                "std::exception");
//
// public:
//  struct type {
//    typedef _successT success;
//    typedef _failureT failure;
//  };
//  typedef typename type::success success_type;
//  typedef typename type::failure failure_type;
//
//  explicit result(
//      const std::function<success_type()> &catching_body) _NOEXCEPT {
//    try {
//      auto func = catching_body();
//      success_.reset(&func);
//      failure_.reset();
//    } catch (const failure_type &failure) {
//      failure_ = uvcc::make_unique<failure_type>(failure);
//      success_ = nullptr;
//    }
//  }
//
//  bool as_success() const _NOEXCEPT { return success_ != nullptr; }
//
//  bool as_failure() const _NOEXCEPT { return failure_.get() != nullptr; }
//
// private:
//  std::unique_ptr<success_type> success_;
//  //    success_type *success_;
//  std::unique_ptr<failure_type> failure_;
//};

inline bool expr_assert(int err, bool abs = false) _NOEXCEPT {
    return abs ? (err == 0) : (err >= 0);
}

inline void expr_cerr(const uvcc::Exception &exception) _NOEXCEPT {
    std::cerr << "uvcc error: " << exception.what() << std::endl;
}

inline bool expr_cerr_r(int err, bool abs = false) _NOEXCEPT {
    if (expr_assert(err, abs)) return true;
    expr_cerr(uvcc::Exception(err));
    return false;
}

inline void expr_throws(int err, bool abs = false) {
    if (!expr_assert(err, abs)) throw uvcc::Exception(err);
}

// enum class LoopOption : int {
//    kBlockSignal = UV_LOOP_BLOCK_SIGNAL,
//    kMetricsIDLETime = UV_METRICS_IDLE_TIME,
//};
//
// enum class RunOption : int {
//    kDefault = UV_RUN_DEFAULT,
//    kOnce = UV_RUN_ONCE,
//    kNoWait = UV_RUN_NOWAIT,
//};

template <typename _Tp>
class basic_uv_object {
public:
    typedef _Tp element_type;
    typedef _Tp &reference;
    typedef const _Tp &const_reference;
    
    basic_uv_object() _NOEXCEPT : obj_ptr_(uvcc::make_unique<element_type>()) {}
    basic_uv_object(const basic_uv_object &) = delete;
    basic_uv_object(basic_uv_object &&other) _NOEXCEPT {
        *this = std::move(other);
    }
    basic_uv_object(element_type &&other) _NOEXCEPT
    : obj_ptr_(uvcc::make_unique<element_type>(other)) {}
    basic_uv_object &operator=(const basic_uv_object &) = delete;
    basic_uv_object &operator=(basic_uv_object &&other) _NOEXCEPT {
        if (this != &other) {
            obj_ptr_.reset(other.obj_ptr_.release());
        }
        return *this;
    }
    basic_uv_object &operator=(element_type &&other) _NOEXCEPT {
        if (_get() != &other) {
            obj_ptr_.reset(&other);
        }
        return *this;
    }
    virtual ~basic_uv_object() { obj_ptr_.release(); }
    
protected:
    std::unique_ptr<element_type> obj_ptr_;
    
    element_type *_get() const _NOEXCEPT { return obj_ptr_.get(); }
    
    virtual const std::size_t element_size() const _NOEXCEPT = 0;
};

template <typename _Tp, typename _Tu>
class basic_uv_union_object : virtual protected basic_uv_object<_Tu> {
    static_assert(std::is_union<_Tu>::value, "The '_Tu' must be union type.");
    
public:
    typedef _Tp element_type;
    typedef _Tp &reference;
    typedef const _Tp &const_reference;
    
    typedef _Tu union_element_type;
    //    typedef _Tu& reference_union;
    //    typedef const _Tu& const_reference_union;
    
    basic_uv_union_object() _NOEXCEPT : basic_uv_object<union_element_type>() {}
    basic_uv_union_object(const basic_uv_union_object &) = delete;
    basic_uv_union_object(basic_uv_union_object &&other) _NOEXCEPT {
        *this = std::move(other);
    }
    basic_uv_union_object(union_element_type &&other) _NOEXCEPT
    : basic_uv_object<union_element_type>(other) {}
    basic_uv_union_object &operator=(const basic_uv_union_object &) = delete;
    basic_uv_union_object &operator=(basic_uv_union_object &&other) _NOEXCEPT {
        if (this != &other) {
            this->obj_ptr_.reset(other.obj_ptr_.release());
        }
        return *this;
    }
    basic_uv_union_object &operator=(union_element_type &&other) _NOEXCEPT {
        if (_get() != &other) {
            this->obj_ptr_.reset(&other);
        }
        return *this;
    }
    
protected:
    virtual element_type *_get() const _NOEXCEPT = 0;
};

class event_loop final : protected basic_uv_object<uv_loop_t> {
public:
    typedef uvcc::completion_block<uv_walk_cb>::c_type c_map_completion_block;
    typedef uvcc::completion_block<uv_walk_cb>::type map_completion_block;
    
    enum class options : int {
        block_signal = UV_LOOP_BLOCK_SIGNAL,
        metrics_idle_time = UV_METRICS_IDLE_TIME,
    };
    
    enum class run_mode : int {
        standard = UV_RUN_DEFAULT,
        once = UV_RUN_ONCE,
        no_wait = UV_RUN_NOWAIT,
    };
    
    explicit event_loop() _NOEXCEPT(false) : basic_uv_object<element_type>() {
        uvcc::expr_throws(uv_loop_init(_get()));
        _get()->data = static_cast<void *>(this);
    }
    event_loop(const event_loop &) = delete;
    event_loop(event_loop &&other) _NOEXCEPT { *this = std::move(other); }
    event_loop &operator=(const event_loop &) = delete;
    event_loop &operator=(event_loop &&other) _NOEXCEPT {
        if (this != &other) {
            this->obj_ptr_.reset(other.obj_ptr_.release());
        }
        return *this;
    }
    ~event_loop() _NOEXCEPT final {
        while (is_alive() && _get()->stop_flag == static_cast<decltype(_get()->stop_flag)>(false)) {
            stop();
        }
        uvcc::expr_cerr_r(uv_loop_close(_get()));
    }
    
    static event_loop *standard() _NOEXCEPT {
        const std::lock_guard<std::mutex> lock(singleton_mutex_);
        if (standard_ == nullptr) standard_ = new event_loop(uv_default_loop());
        return standard_;
    }
    
    bool configure(const uvcc::event_loop::options &option) _NOEXCEPT {
        return uvcc::expr_cerr_r(
                                 uv_loop_configure(_get(), uv_loop_option(option), true));
    }
    
    void run(const uvcc::event_loop::run_mode &mode = run_mode::standard) _NOEXCEPT(false) {
        uvcc::expr_throws(uv_run(_get(), uv_run_mode(mode)));
    }
    
    bool is_alive() const _NOEXCEPT {
        return uvcc::expr_assert(uv_loop_alive(_get()), true);
    }
    
    void stop() _NOEXCEPT { uv_stop(_get()); }
    
    int fd() const _NOEXCEPT { return uv_backend_fd(_get()); }
    
    int timeout() const _NOEXCEPT { return uv_backend_timeout(_get()); }
    
    std::uint64_t now() const _NOEXCEPT { return uv_now(_get()); }
    
    void update_time() _NOEXCEPT { uv_update_time(_get()); }
    
    template <typename argT>
    event_loop &map(std::unique_ptr<argT> arg,
                    map_completion_block &&block) _NOEXCEPT {
        uv_walk(_get(), block.target<c_map_completion_block>(), arg.get());
        return *this;
    }
    
    void fork() { uvcc::expr_throws(uv_loop_fork(_get())); }
    
private:
    static event_loop *standard_;
    static std::mutex singleton_mutex_;
    
    event_loop(element_type *loop) _NOEXCEPT(false)
    : basic_uv_object<element_type>() {
        if (loop != nullptr && _get() != loop) {
            obj_ptr_.reset(loop);
            _get()->data = static_cast<void *>(this);
        } else {
            // TODO: exception process.
            std::cerr << "uvcc error: standard uvcc::event_loop allocation failure."
            << std::endl;
        }
    }
    
    const std::size_t element_size() const _NOEXCEPT final {
        return uv_loop_size();
    }
};

event_loop *event_loop::standard_{nullptr};
std::mutex event_loop::singleton_mutex_;

class basic_fd : virtual protected basic_uv_union_object<uv_handle_t, uv_any_handle> {
public:
    typedef uvcc::completion_block<uv_alloc_cb>::c_type c_alloc_completion_block;
    typedef uvcc::completion_block<uv_alloc_cb>::type alloc_completion_block;
    typedef uvcc::completion_block<uv_close_cb>::c_type c_close_completion_block;
    typedef uvcc::completion_block<uv_close_cb>::type close_completion_block;
    
    explicit basic_fd(close_completion_block &&block = {}) _NOEXCEPT {}
    ~basic_fd() { std::cout << "basic fd" << std::endl; }
    
    element_type *_get() const _NOEXCEPT {
        return reinterpret_cast<element_type *>(_get());
    }
};

class stream_fd : public basic_uv_union_object<uv_stream_t, uv_any_handle> {};

class tcp_fd : public basic_uv_union_object<uv_tcp_t, uv_any_handle> {};

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
