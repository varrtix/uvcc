#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <uv.h>

#include <memory>

#include "utilities.h"

namespace uvcc {

class EventLoop {
 public:
  explicit EventLoop() : context_(uvcc::make_unique<uv_loop_t>()), err(0) {
    UV_FUNC_THROWS(uv_loop_init(context_.get()), err)
  }
  ~EventLoop() { uv_loop_close(context_.get()); }

  void run(const uvcc::RunOption &option) {
    UV_FUNC_THROWS(uv_run(context_.get(), uv_run_mode(option)), err)
  }

  void stop() _NOEXCEPT { uv_stop(context_.get()); }

  static std::shared_ptr<EventLoop> standard() {
    return std::make_shared<EventLoop>();
  }

 private:
  std::unique_ptr<uv_loop_t> context_;
  int err;
};

}  // namespace uvcc

#endif  // EVENTLOOP_H
