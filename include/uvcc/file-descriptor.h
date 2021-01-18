/// MIT License
///
/// uvcc/file-desciptor.h
/// uvcc
///
/// created by varrtix on 2021/01/13.
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

#ifndef FILEDESCRIPTOR_H
#define FILEDESCRIPTOR_H

#include <uv.h>

#include <memory>
#include <string>

#include "event-loop.h"
#include "utilities.h"

namespace uvcc {

class FileDescriptor : protected BaseObject<uv_any_handle> {
 public:
  enum class TransmitType : int {
    kDefault = UV_HANDLE,
    kUnknown = UV_UNKNOWN_HANDLE,
    kAsync = UV_ASYNC,
    kCheck = UV_CHECK,
    kFSEvent = UV_FS_EVENT,
    kFSPoll = UV_FS_POLL,
    kIDLE = UV_IDLE,
    kPoll = UV_POLL,
    kPrepare = UV_PREPARE,
    kProcess = UV_PROCESS,
    kTimer = UV_TIMER,
    kUDP = UV_UDP,
    kSignal = UV_SIGNAL,
    kFile = UV_FILE,
    kDescriptorTypeMax = UV_HANDLE_TYPE_MAX,
  };

  explicit FileDescriptor(
      const TransmitType &type = TransmitType::kDefault,
      std::function<void(uv_handle_t *)> &&closing_callback = {}) _NOEXCEPT
      : closing_callback_(std::move(closing_callback)),
        fd_type_(type) {
    switch (fd_type_) {
      case TransmitType::kUDP:
        raw_->udp = uv_udp_t();
        break;
      case TransmitType::kDefault:
      default:
        raw_->handle = uv_handle_t();
        break;
    }
  }
  FileDescriptor(const Self &self) : BaseObject<Self>(self) {}
  FileDescriptor(Self &&self) _NOEXCEPT : BaseObject<Self>(self) {}
  FileDescriptor(FileDescriptor &&) _NOEXCEPT = default;
  FileDescriptor &operator=(FileDescriptor &&) _NOEXCEPT = default;

  bool isActive() const _NOEXCEPT {
    return !uvcc::expr_assert(uv_is_active(_someHandle()), true);
  }

  bool isClosing() const _NOEXCEPT {
    return !uvcc::expr_assert(uv_is_closing(_someHandle()), true);
  }

  void close() _NOEXCEPT {
    uv_close(_someHandle(),
             closing_callback_ ? closing_callback_.target<void(uv_handle_t *)>()
                               : nullptr);
  }

  void reference() _NOEXCEPT { uv_ref(_someHandle()); }

  void unreference() _NOEXCEPT { uv_unref(_someHandle()); }

  bool hasReference() const _NOEXCEPT {
    return !uvcc::expr_assert(uv_has_ref(_someHandle()), true);
  }

  std::size_t size() const _NOEXCEPT {
    return uv_handle_size(_someHandle()->type);
  }

  const std::unique_ptr<const EventLoop> loop() _NOEXCEPT {
    return uvcc::make_unique<const EventLoop>(*_someHandle()->loop);
  }

  template <typename T>
  const std::unique_ptr<const T> data() const _NOEXCEPT {
    return uv_handle_get_data(_someHandle());
  }

  template <typename T>
  std::unique_ptr<const T> setData(std::unique_ptr<T> data) _NOEXCEPT {
    return uv_handle_set_data(_someHandle(), data);
  }

  virtual const TransmitType &type() const _NOEXCEPT { return fd_type_; }

  virtual const std::string typeName() const _NOEXCEPT {
    return std::string(uv_handle_type_name(_someHandle()->type));
  }

 protected:
  std::function<void(uv_handle_t *)> closing_callback_;

 private:
  TransmitType fd_type_;

  uv_handle_t *_someHandle() const _NOEXCEPT {
    switch (fd_type_) {
      case TransmitType::kUDP:
        return reinterpret_cast<uv_handle_t *>(&raw_->udp);
      case TransmitType::kDefault:
      default:
        return reinterpret_cast<uv_handle_t *>(&raw_->handle);
    }
  }
};  // namespace uvcc
}  // namespace uvcc

#endif  // FILEDESCRIPTOR_H
