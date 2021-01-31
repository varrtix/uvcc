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

//#include <uv.h>

//#include "utilities.h"

//namespace uvcc {

//class EventLoop : virtual protected BaseObject<uv_loop_t> {
// protected:
//  using MappingRawCompletionBlock = uvcc::RawCompletionBlock<uv_walk_cb>;
//  using MappingCompletionBlock = std::function<MappingRawCompletionBlock>;
//
// public:
//  EventLoop() : BaseObject<Self>() {
//    uvcc::expr_throws(uv_loop_init(raw_.get()));
//  }
//  EventLoop(const Self &self) : BaseObject<Self>(self) {}
//  EventLoop(Self &&self) _NOEXCEPT : BaseObject<Self>(self) {}
//  EventLoop(EventLoop &&) _NOEXCEPT = default;
//  EventLoop &operator=(EventLoop &&) _NOEXCEPT = default;
//  ~EventLoop() _NOEXCEPT {
//    try {
//      _close();
//    } catch (const uvcc::Exception &exception) {
//      uvcc::expr_cerr(exception);
//    }
//  }
//
//  bool configure(const uvcc::LoopOption &option) _NOEXCEPT {
//    return uvcc::expr_cerr_r(
//        uv_loop_configure(raw_.get(), uv_loop_option(option)), true);
//  }
//
//  void run(const uvcc::RunOption &option) {
//    uvcc::expr_throws(uv_run(raw_.get(), uv_run_mode(option)));
//  }
//
//  bool isAlive() const _NOEXCEPT {
//    return uvcc::expr_assert(uv_loop_alive(raw_.get()), true);
//  }
//
//  void stop() _NOEXCEPT { uv_stop(raw_.get()); }
//
//  int fd() const _NOEXCEPT { return uv_backend_fd(raw_.get()); }
//
//  int timeout() const _NOEXCEPT { return uv_backend_timeout(raw_.get()); }
//
//  std::uint64_t now() const _NOEXCEPT { return uv_now(raw_.get()); }
//
//  void updateTime() _NOEXCEPT { uv_update_time(raw_.get()); }
//
//  template <typename T>
//  EventLoop &map(std::unique_ptr<T> arg,
//                 MappingCompletionBlock &&block) _NOEXCEPT {
//    uv_walk(raw_.get(), block.target<MappingRawCompletionBlock>(), arg.get());
//    return *this;
//  }
//
//  void fork() { uvcc::expr_throws(uv_loop_fork(raw_.get())); }
//
//  template <typename T>
//  const std::unique_ptr<const T> data() const _NOEXCEPT {
//    return uv_loop_get_data(raw_.get());
//  }
//
//  template <typename T>
//  std::unique_ptr<const T> setData(std::unique_ptr<T> data) _NOEXCEPT {
//    return uv_loop_set_data(raw_.get(), data);
//  }
//
//  static const std::shared_ptr<const EventLoop> standard() _NOEXCEPT {
//    return std::make_shared<const EventLoop>(std::move(*uv_default_loop()));
//  }
//
// protected:
//  static std::size_t _loopSize() _NOEXCEPT { return uv_loop_size(); }
//
// private:
//  void _close() { uvcc::expr_throws(uv_loop_close(raw_.get())); }
//};

//}  // namespace uvcc

#endif  // EVENTLOOP_H
