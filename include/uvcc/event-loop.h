/// MIT License
///
/// uvcc/event-loop.h
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

#ifndef UVCC_EVENTLOOP_H
#define UVCC_EVENTLOOP_H

#include <uv.h>

#include <mutex>

#include "core.h"
//#include "basic-fd.h"
#include "utilities.h"

namespace uvcc {

class event_loop final : protected basic_uv_object<uv_loop_t> {
//  typedef uvcc::completion_block<uv_walk_cb>::c_type c_map_completion_block;
//  typedef uvcc::completion_block<uv_walk_cb>::type func_map_completion_block;
  
public:
  // TODO: It will be deleted.
  typedef uvcc::completion_block<uv_walk_cb>::c_type c_map_completion_block;
  typedef uvcc::completion_block<uv_walk_cb>::type f_map_completion_block;
  //  template <typename _Tp>
  //  using map_completion_block = uvcc::completion_block<void(uvcc::basic_fd<_Tp> *, void *)>::;type;
  
  enum class options : int {
    block_signal = UV_LOOP_BLOCK_SIGNAL,
    metrics_idle_time = UV_METRICS_IDLE_TIME,
  };
  
  enum class run_mode : int {
    standard = UV_RUN_DEFAULT,
    once = UV_RUN_ONCE,
    no_wait = UV_RUN_NOWAIT,
  };
  
  event_loop() _NOEXCEPT(false) : basic_uv_object<obj_type>() {
    uvcc::expr(uv_loop_init(_obj_ptr())).throws();
    _obj_ptr()->data = static_cast<void *>(this);
  }
  event_loop(const event_loop &) = delete;
  event_loop(obj_type &&other) _NOEXCEPT { *this = std::move(other); }
  event_loop &operator=(const event_loop &) = delete;
  event_loop &operator=(event_loop &&other) _NOEXCEPT {
    if (this != &other) {
      this->obj_ptr_.reset(other.obj_ptr_.release());
    }
    return *this;
  }
  
  inline static event_loop *standard() _NOEXCEPT {
    const std::lock_guard<std::mutex> lock(default_loop_mutex_);
    
    if (default_loop_ptr_ == nullptr) try {
      default_loop_ptr_ = new event_loop();
    } catch (const uvcc::exception &ex) {
//      delete default_loop_ptr_;
      default_loop_ptr_ = nullptr;
#if DEBUG
      uvcc::log::warning(nullptr, __PRETTY_FUNCTION__)
      .print(ex.description());
#endif
    }
    
    return default_loop_ptr_;
  }
  
  void destroy() _NOEXCEPT { delete this; }
  
  bool configure(const uvcc::event_loop::options &option) _NOEXCEPT {
    auto output =
    uvcc::expr(uv_loop_configure(_obj_ptr(), uv_loop_option(option)));
    if (output.failed())
      uvcc::log::warning(this, __PRETTY_FUNCTION__).print(output.str());
    
    return output.eval();
  }
  
  void run(const uvcc::event_loop::run_mode &mode = run_mode::standard)
  _NOEXCEPT(false) {
    uvcc::expr::ne(uv_run(_obj_ptr(), uv_run_mode(mode))).throws();
  }
  
  bool is_alive() const _NOEXCEPT {
    return uvcc::expr(uv_loop_alive(_obj_ptr())).eval();
  }
  
  void stop() _NOEXCEPT { uv_stop(_obj_ptr()); }
  
  int fd() const _NOEXCEPT { return uv_backend_fd(_obj_ptr()); }
  
  int timeout() const _NOEXCEPT { return uv_backend_timeout(_obj_ptr()); }
  
  std::uint64_t now() const _NOEXCEPT { return uv_now(_obj_ptr()); }
  
  void update_time() _NOEXCEPT { uv_update_time(_obj_ptr()); }
  
  // TODO: Fix it.
  template <typename argT>
  event_loop &map(std::unique_ptr<argT> arg,
                  f_map_completion_block &&block) _NOEXCEPT {
    uv_walk(_obj_ptr(), block.target<c_map_completion_block>(), arg.get());
    return *this;
  }
  
  void fork() { uvcc::expr::ne(uv_loop_fork(_obj_ptr())).throws(); }
  
private:
  static event_loop *default_loop_ptr_;
  static std::mutex default_loop_mutex_;
  
//  event_loop(event_loop &&other) _NOEXCEPT { *this = std::move(other); }
//  event_loop &operator=(obj_type &&other) _NOEXCEPT {
//    if(this->_obj_ptr() != &other) {
//      if (uv_default_loop() == &other) {
//        obj_ptr_ = uvcc::make_unique<obj_type>();
//        auto output = uvcc::expr(uv_loop_init(_obj_ptr()));
//        if (output.failed())
//          uvcc::log::error(this, __PRETTY_FUNCTION__).print(output.str());
//      } else {
//        obj_ptr_.reset(std::move(&other));
//      }
//      _obj_ptr()->data = static_cast<void *>(this);
//    }
//    return *this;
//  }
  ~event_loop() _NOEXCEPT { _force_close(); }
  
  bool _force_close() _NOEXCEPT {
    if (_obj_ptr() == nullptr) return false;
    
    while (is_alive() && _obj_ptr()->stop_flag == 0) {
      stop();
    }
#ifdef DEBUG
    uvcc::log::debug(this, __PRETTY_FUNCTION__)
    .print("an event_loop will be closed.");
#endif
    auto output = uvcc::expr::ne(uv_loop_close(_obj_ptr()));
    if (output.failed())
      uvcc::log::warning(this, __PRETTY_FUNCTION__).print(output.str());
    
    return output.eval();
  }
  
  std::size_t _obj_size() const _NOEXCEPT final { return uv_loop_size(); }
};

event_loop *event_loop::default_loop_ptr_{nullptr};
std::mutex event_loop::default_loop_mutex_;

}  // namespace uvcc

#endif  // UVCC_EVENTLOOP_H
