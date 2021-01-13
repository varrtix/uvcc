/// MIT License
///
/// uvcc/handle.h
/// uvcc
///
/// created by varrtix on 2021/01/12.
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

#ifndef HANDLE_H
#define HANDLE_H
#include <uv.h>
#include <uvcc/utilities.h>

#include <memory>

namespace uvcc {

class Handle {
 public:
  using Storage = uv_any_handle;

  enum class Type : int {
    kUnknown = UV_UNKNOWN_HANDLE,
    kAsync = UV_ASYNC,
    kCheck = UV_CHECK,
    kFSEvent = UV_FS_EVENT,
    kFSPoll = UV_FS_POLL,
    kHandle = UV_HANDLE,
    kIDLE = UV_IDLE,
    kPoll = UV_POLL,
    kPrepare = UV_PREPARE,
    kProcess = UV_PROCESS,
    kTimer = UV_TIMER,
    kUDP = UV_UDP,
    kSignal = UV_SIGNAL,
    kFile = UV_FILE,
    kHandleTypeMax = UV_HANDLE_TYPE_MAX,
  };

  explicit Handle(const Handle::Type &type = Handle::Type::kHandle) _NOEXCEPT
      : err_(0),
        handle_type_(type) {}
  //  virtual ~Handle() _NOEXCEPT {}

 protected:
  //  virtual bool _isActive(const Element &element) const {
  //    return !UV_FUNC_SUCC_ASSERT(uv_is_active(&element));
  //    return !
  //    return true;
  //  }

  //  virtual bool _isClosing(const Element &element) const _NOEXCEPT {
  //    return !UV_FUNC_SUCC_ASSERT(uv_is_closing(&element));
  //  }

  //  virtual

 private:
  //  std::unique_ptr<HandleStorage> context_;
  Handle::Type handle_type_;
  int err_;
};

// class Handle {
// public:
//  Handle() = default;
//  virtual ~Handle() {}

//  virtual bool isActive() const {
//    return !UV_FUNC_SUCC_ASSERT(uv_is_active(context_.get()));
//  }

//  virtual bool isClosing() const {
//    return !UV_FUNC_SUCC_ASSERT(uv_is_closing(context_.get()));
//  }

// private:
//  std::unique_ptr<uv_handle_t> context_;
//  int err_;
//};

}  // namespace uvcc

#endif  // HANDLE_H
