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
  ~EventLoop() _NOEXCEPT {
    try {
      _close();
    } catch (const uvcc::Exception &exception) {
      uvcc::expr_cerr(exception);
    }
  }

  bool configure(const uvcc::LoopOption &option) _NOEXCEPT {
    return uvcc::expr_cerr(
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
