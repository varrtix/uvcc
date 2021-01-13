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

#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <uv.h>

#include <functional>
#include <iostream>
#include <memory>

#include "utilities.h"

namespace uvcc {

class EventLoop {
 public:
  explicit EventLoop() : context_(uvcc::make_unique<uv_loop_t>()) {
    uvcc::expr_throws(uv_loop_init(context_.get()));
  }

  explicit EventLoop(uv_loop_t &&loop) _NOEXCEPT
      : context_(uvcc::make_unique<uv_loop_t>(std::move(loop))) {}

  ~EventLoop() _NOEXCEPT {
    try {
      _close();
    } catch (const uvcc::Exception &exception) {
      uvcc::expr_cerr(exception);
    }
  }

  bool configure(const uvcc::LoopOption &option) _NOEXCEPT {
    return uvcc::expr_cerr_r(
        uv_loop_configure(context_.get(), uv_loop_option(option)), true);
  }

  void run(const uvcc::RunOption &option) {
    uvcc::expr_throws(uv_run(context_.get(), uv_run_mode(option)));
  }

  bool isAlive() const _NOEXCEPT {
    return uvcc::expr_assert(uv_loop_alive(context_.get()), true);
  }

  void stop() _NOEXCEPT { uv_stop(context_.get()); }

  int fd() const _NOEXCEPT { return uv_backend_fd(context_.get()); }

  int timeout() const _NOEXCEPT { return uv_backend_timeout(context_.get()); }

  std::uint64_t now() const _NOEXCEPT { return uv_now(context_.get()); }

  void updateTime() _NOEXCEPT { uv_update_time(context_.get()); }

  template <typename T>
  EventLoop &map(
      std::unique_ptr<T> arg,
      const std::function<void(uv_handle_t *, void *)> &callback) _NOEXCEPT {
    uv_walk_cb callback_ptr = callback.target<void(uv_handle_t *, void *)>();
    uv_walk(context_.get(), callback_ptr, arg.get());
    return *this;
  }

  void fork() { uvcc::expr_throws(uv_loop_fork(context_.get())); }

  template <typename T>
  std::shared_ptr<T> data() const _NOEXCEPT {
    return uv_loop_get_data(context_.get());
  }

  template <typename T>
  std::shared_ptr<T> setData(std::unique_ptr<T> data) _NOEXCEPT {
    return uv_loop_set_data(context_.get(), data.get());
  }

  static std::shared_ptr<EventLoop> standard() {
    return std::make_shared<EventLoop>();
  }

 protected:
  static std::size_t _loopSize() _NOEXCEPT { return uv_loop_size(); }

 private:
  std::unique_ptr<uv_loop_t> context_;

  void _close() { uvcc::expr_throws(uv_loop_close(context_.get())); }
};

}  // namespace uvcc

#endif  // EVENTLOOP_H
