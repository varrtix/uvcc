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
                "The '_enumT' should be an enum type.");
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
                "The '_classT' should be a function.");

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

// template <typename _successT, typename _failureT>
// class result {
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
  static_assert(
      // Any handle type
      std::is_same<_Tp, uv_any_handle>::value ||
          std::is_same<_Tp, uv_async_t>::value ||
          std::is_same<_Tp, uv_check_t>::value ||
          std::is_same<_Tp, uv_fs_event_t>::value ||
          std::is_same<_Tp, uv_fs_poll_t>::value ||
          std::is_same<_Tp, uv_handle_t>::value ||
          std::is_same<_Tp, uv_idle_t>::value ||
          std::is_same<_Tp, uv_pipe_t>::value ||
          std::is_same<_Tp, uv_poll_t>::value ||
          std::is_same<_Tp, uv_prepare_t>::value ||
          std::is_same<_Tp, uv_process_t>::value ||
          std::is_same<_Tp, uv_stream_t>::value ||
          std::is_same<_Tp, uv_tcp_t>::value ||
          std::is_same<_Tp, uv_timer_t>::value ||
          std::is_same<_Tp, uv_tty_t>::value ||
          std::is_same<_Tp, uv_udp_t>::value ||
          std::is_same<_Tp, uv_signal_t>::value ||
          // Any request type
          std::is_same<_Tp, uv_any_req>::value ||
          std::is_same<_Tp, uv_req_t>::value ||
          std::is_same<_Tp, uv_connect_t>::value ||
          std::is_same<_Tp, uv_write_t>::value ||
          std::is_same<_Tp, uv_shutdown_t>::value ||
          std::is_same<_Tp, uv_udp_send_t>::value ||
          std::is_same<_Tp, uv_fs_t>::value ||
          std::is_same<_Tp, uv_work_t>::value ||
          std::is_same<_Tp, uv_getaddrinfo_t>::value ||
          std::is_same<_Tp, uv_getnameinfo_t>::value ||
          std::is_same<_Tp, uv_random_t>::value ||
          // loop type
          std::is_same<_Tp, uv_loop_t>::value,
      "The '_Tp' should be an uv type.");

 public:
  typedef _Tp obj_type;
  typedef _Tp &reference;
  typedef const _Tp &const_reference;

 protected:
  basic_uv_object() _NOEXCEPT : obj_ptr_(uvcc::make_unique<obj_type>()) {}
  basic_uv_object(const basic_uv_object &) = delete;
  basic_uv_object(basic_uv_object &&other) _NOEXCEPT {
    *this = std::move(other);
  }
  basic_uv_object(obj_type &&other) _NOEXCEPT
      : obj_ptr_(uvcc::make_unique<obj_type>(other)) {}
  basic_uv_object &operator=(const basic_uv_object &) = delete;
  basic_uv_object &operator=(basic_uv_object &&other) _NOEXCEPT {
    if (this != &other) {
      obj_ptr_.reset(other.obj_ptr_.release());
    }
    return *this;
  }
  basic_uv_object &operator=(obj_type &&other) _NOEXCEPT {
    if (_obj_ptr() != &other) {
      obj_ptr_.reset(&other);
    }
    return *this;
  }
  virtual ~basic_uv_object() {
    // DEBUG
    std::cout << __FUNCTION__ << std::endl;
    //
//    obj_ptr_.release();
      obj_ptr_.reset();
  }

  // protected:

  obj_type *_obj_ptr() const _NOEXCEPT { return obj_ptr_.get(); }

  virtual std::size_t _obj_size() const _NOEXCEPT = 0;

  std::unique_ptr<obj_type> obj_ptr_;
};

template <typename _Tp, typename _Tu>
class basic_uv_union_object : virtual protected basic_uv_object<_Tu> {
  static_assert(std::is_union<_Tu>::value,
                "The '_Tu' should be an union type.");
  static_assert(
      // Any handle type with union type 'uv_any_handle'
      std::is_same<_Tp, uv_async_t>::value ||
          std::is_same<_Tp, uv_check_t>::value ||
          std::is_same<_Tp, uv_fs_event_t>::value ||
          std::is_same<_Tp, uv_fs_poll_t>::value ||
          std::is_same<_Tp, uv_handle_t>::value ||
          std::is_same<_Tp, uv_idle_t>::value ||
          std::is_same<_Tp, uv_pipe_t>::value ||
          std::is_same<_Tp, uv_poll_t>::value ||
          std::is_same<_Tp, uv_prepare_t>::value ||
          std::is_same<_Tp, uv_process_t>::value ||
          std::is_same<_Tp, uv_stream_t>::value ||
          std::is_same<_Tp, uv_tcp_t>::value ||
          std::is_same<_Tp, uv_timer_t>::value ||
          std::is_same<_Tp, uv_tty_t>::value ||
          std::is_same<_Tp, uv_udp_t>::value ||
          std::is_same<_Tp, uv_signal_t>::value ||
          // Any handle type with union type 'uv_any_handle'
          std::is_same<_Tp, uv_req_t>::value ||
          std::is_same<_Tp, uv_connect_t>::value ||
          std::is_same<_Tp, uv_write_t>::value ||
          std::is_same<_Tp, uv_shutdown_t>::value ||
          std::is_same<_Tp, uv_udp_send_t>::value ||
          std::is_same<_Tp, uv_fs_t>::value ||
          std::is_same<_Tp, uv_work_t>::value ||
          std::is_same<_Tp, uv_getaddrinfo_t>::value ||
          std::is_same<_Tp, uv_getnameinfo_t>::value ||
          std::is_same<_Tp, uv_random_t>::value,
      "The '_Tp' should be a member type of the '_Tu'.");

 public:
  typedef _Tp elem_type;
  typedef _Tp &reference;
  typedef const _Tp &const_reference;

  typedef _Tu union_elem_type;
  //    typedef _Tu& reference_union;
  //    typedef const _Tu& const_reference_union;

 protected:
  basic_uv_union_object() _NOEXCEPT : basic_uv_object<union_elem_type>() {}
  basic_uv_union_object(const basic_uv_union_object &) = delete;
  basic_uv_union_object(basic_uv_union_object &&other) _NOEXCEPT {
    *this = std::move(other);
  }
//  basic_uv_union_object(union_elem_type &&other) _NOEXCEPT
//      : basic_uv_object<union_elem_type>(other) {}
  basic_uv_union_object &operator=(const basic_uv_union_object &) = delete;
  basic_uv_union_object &operator=(basic_uv_union_object &&other) _NOEXCEPT {
    if (this != &other) {
      this->obj_ptr_.reset(other.obj_ptr_.release());
    }
    return *this;
  }
//  basic_uv_union_object &operator=(union_elem_type &&other) _NOEXCEPT {
//    if (_elem_ptr() != &other) {
//      this->obj_ptr_.reset(&other);
//    }
//    return *this;
//  }

  // protected:
  virtual elem_type *_elem_ptr() const _NOEXCEPT = 0;
};


class basic_event_loop final : protected basic_uv_object<uv_loop_t> {
public:
    typedef obj_type basic_type;

    explicit basic_event_loop() _NOEXCEPT(false) : basic_uv_object<basic_type>() {
        uvcc::expr_throws(uv_loop_init(_obj_ptr()));
        _obj_ptr()->data = static_cast<void *>(this);
    }
    basic_event_loop(const basic_event_loop &) = delete;
    basic_event_loop(basic_event_loop &&other) _NOEXCEPT {
        *this = std::move(other);
    }
    explicit basic_event_loop(basic_type *basic_loop_ptr) _NOEXCEPT {
    }
    basic_event_loop &operator=(const basic_event_loop &) = delete;
    basic_event_loop &operator=(basic_event_loop &&other) _NOEXCEPT {
        return *this;
    }
    
private:
    bool _is_alive() const _NOEXCEPT {
      return uvcc::expr_assert(uv_loop_alive(_obj_ptr()), true);
    }

    void _stop() _NOEXCEPT { uv_stop(_obj_ptr()); }
    

    bool _close(basic_type *basic_loop_ptr) _NOEXCEPT {
        while (_is_alive() &&
               basic_loop_ptr->stop_flag ==
               static_cast<decltype(basic_loop_ptr->stop_flag)>(false)) {
            _stop();
        }
        return uvcc::expr_cerr_r(uv_loop_close(basic_loop_ptr), true);
    }
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

  explicit event_loop() _NOEXCEPT(false) : basic_uv_object<obj_type>() {
    uvcc::expr_throws(uv_loop_init(_obj_ptr()));
    _obj_ptr()->data = static_cast<void *>(this);
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
    // DEBUG
    std::cout << __FUNCTION__ << std::endl;
    //
    while (is_alive() &&
           _obj_ptr()->stop_flag ==
               static_cast<decltype(_obj_ptr()->stop_flag)>(false)) {
      stop();
    }
    uvcc::expr_cerr_r(uv_loop_close(_obj_ptr()));
  }

  static event_loop *standard() _NOEXCEPT {
    const std::lock_guard<std::mutex> lock(default_loop_mutex_);
    if (default_loop_ptr_ == nullptr)
      default_loop_ptr_ = new event_loop(uv_default_loop());
    return default_loop_ptr_;
  }

  bool configure(const uvcc::event_loop::options &option) _NOEXCEPT {
    return uvcc::expr_cerr_r(
        uv_loop_configure(_obj_ptr(), uv_loop_option(option), true));
  }

  void run(const uvcc::event_loop::run_mode &mode = run_mode::standard)
      _NOEXCEPT(false) {
    uvcc::expr_throws(uv_run(_obj_ptr(), uv_run_mode(mode)));
  }

  bool is_alive() const _NOEXCEPT {
    return uvcc::expr_assert(uv_loop_alive(_obj_ptr()), true);
  }

  void stop() _NOEXCEPT { uv_stop(_obj_ptr()); }

  int fd() const _NOEXCEPT { return uv_backend_fd(_obj_ptr()); }

  int timeout() const _NOEXCEPT { return uv_backend_timeout(_obj_ptr()); }

  std::uint64_t now() const _NOEXCEPT { return uv_now(_obj_ptr()); }

  void update_time() _NOEXCEPT { uv_update_time(_obj_ptr()); }

  template <typename argT>
  event_loop &map(std::unique_ptr<argT> arg,
                  map_completion_block &&block) _NOEXCEPT {
    uv_walk(_obj_ptr(), block.target<c_map_completion_block>(), arg.get());
    return *this;
  }

  void fork() { uvcc::expr_throws(uv_loop_fork(_obj_ptr())); }

 private:
  static event_loop *default_loop_ptr_;
  static std::mutex default_loop_mutex_;

  event_loop(obj_type *loop) _NOEXCEPT(false) : basic_uv_object<obj_type>() {
    if (loop != nullptr && _obj_ptr() != loop) {
      obj_ptr_.reset(loop);
      _obj_ptr()->data = static_cast<void *>(this);
    } else {
      // TODO: exception process.
      std::cerr << "uvcc error: uvcc::event_loop allocation failure."
                << std::endl;
    }
  }

  std::size_t _obj_size() const _NOEXCEPT final { return uv_loop_size(); }
};

event_loop *event_loop::default_loop_ptr_{nullptr};
std::mutex event_loop::default_loop_mutex_;

template <typename _Tp>
class basic_fd : virtual protected basic_uv_union_object<_Tp, uv_any_handle> {
  static_assert(std::is_same<_Tp, uv_stream_t>::value ||
                    std::is_same<_Tp, uv_async_t>::value ||
                    std::is_same<_Tp, uv_check_t>::value ||
                    std::is_same<_Tp, uv_fs_event_t>::value ||
                    std::is_same<_Tp, uv_fs_poll_t>::value ||
                    std::is_same<_Tp, uv_idle_t>::value ||
                    std::is_same<_Tp, uv_poll_t>::value ||
                    std::is_same<_Tp, uv_prepare_t>::value ||
                    std::is_same<_Tp, uv_process_t>::value ||
                    std::is_same<_Tp, uv_timer_t>::value ||
                    std::is_same<_Tp, uv_udp_t>::value ||
                    std::is_same<_Tp, uv_signal_t>::value,
                "The '_Tp' should be a legal uv type.");

  //  elem_type *_elem_ptr() const _NOEXCEPT final {
  //    return &_obj_ptr()->handle;
  //  }
  friend class event_loop;

 public:
  typedef _Tp elem_type;
  typedef _Tp &elem_reference;
  typedef const _Tp &const_elem_reference;

  typedef uvcc::completion_block<uv_alloc_cb>::c_type c_alloc_completion_block;
  typedef uvcc::completion_block<uv_alloc_cb>::type alloc_completion_block;
  typedef uvcc::completion_block<uv_close_cb>::c_type c_close_completion_block;
  typedef uvcc::completion_block<uv_close_cb>::type close_completion_block;

  bool is_active() const _NOEXCEPT {
    return !uvcc::expr_assert(uv_is_active(_basic_ptr()), true);
  }

  bool is_closing() const _NOEXCEPT {
    return !uvcc::expr_assert(uv_is_closing(_basic_ptr()), true);
  }

  void reference() _NOEXCEPT { uv_ref(_basic_ptr()); }

  void unreference() _NOEXCEPT { uv_unref(_basic_ptr()); }

  bool has_reference() const _NOEXCEPT {
    return !uvcc::expr_assert(uv_has_ref(_basic_ptr()), true);
  }

  event_loop *loop() _NOEXCEPT {}

  // protected:
  typedef uv_handle_t basic_type;
  typedef uv_any_handle union_type;

  typedef basic_type &basic_reference;
  typedef const basic_type &const_basic_reference;

  explicit basic_fd(event_loop *loop,
                    close_completion_block &&block = {}) _NOEXCEPT
      : basic_uv_union_object<elem_type, union_type>(),
        close_completion_block_ptr_(
            uvcc::make_unique<close_completion_block>(std::move(block))) {
    this->loop_ptr_.reset(loop);
  }
  basic_fd(const basic_fd &) = delete;
  basic_fd(basic_fd &&other) _NOEXCEPT { *this = std::move(other); }
  basic_fd &operator=(const basic_fd &) = delete;
  basic_fd &operator=(basic_fd &&other) _NOEXCEPT {
    if (this != &other) {
      this->obj_ptr_.reset(other.obj_ptr_.release());
    }
    return *this;
  }
  virtual ~basic_fd() {
    // DEBUG
    std::cout << __FUNCTION__ << std::endl;
  }

  virtual elem_type *_elem_ptr() const _NOEXCEPT {
    return reinterpret_cast<elem_type *>(this->_obj_ptr());
  }

  basic_type *_basic_ptr() const _NOEXCEPT {
    return reinterpret_cast<basic_type *>(this->_obj_ptr());
  }

  // protected:
  std::unique_ptr<close_completion_block> close_completion_block_ptr_;
  std::unique_ptr<event_loop> loop_ptr_;

 private:
  virtual std::size_t _obj_size() const _NOEXCEPT {
    return uv_handle_size(uv_handle_type::UV_HANDLE);
  }
};

// class basic_stream : public basic_uv_union_object<uv_stream_t, uv_any_handle>
// {};
template <typename _Tp>
class basic_stream_fd : virtual protected basic_fd<uv_stream_t> {
  static_assert(std::is_same<_Tp, uv_tcp_t>::value ||
                    std::is_same<_Tp, uv_tty_t>::value ||
                    std::is_same<_Tp, uv_pipe_t>::value,
                "The '_Tp' should be a legal uv type.");

 public:
};

typedef basic_stream_fd<uv_stream_t> stream_fd;

// class tcp_fd : public basic_uv_union_object<uv_tcp_t, uv_any_handle> {};
class tcp_fd : protected basic_stream_fd<uv_tcp_t> {};

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
