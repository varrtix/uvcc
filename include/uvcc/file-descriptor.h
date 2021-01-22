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

#include "event-loop.h"
#include "utilities.h"

namespace uvcc {

class FileDescriptor
    : virtual protected BaseObject<uv_handle_t, uv_any_handle> {
 protected:
  using AllocatingRawCompletionBlock = uvcc::RawCompletionBlock<uv_alloc_cb>;
  using AllocatingCompletionBlock = std::function<AllocatingRawCompletionBlock>;
  using ClosingRawCompletionBlock = uvcc::RawCompletionBlock<uv_close_cb>;
  using ClosingCompletionBlock = std::function<ClosingRawCompletionBlock>;

 public:
  enum class TransmitType : int {
    kDefault = UV_HANDLE,
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
    kUnknown = UV_UNKNOWN_HANDLE,
    //    kDescriptorTypeMax = UV_HANDLE_TYPE_MAX,
  };

  explicit FileDescriptor(const TransmitType &type = TransmitType::kDefault,
                          ClosingCompletionBlock &&block = {}) _NOEXCEPT
      : closing_completion_block_(std::move(block)) {
    switch (type) {
      case TransmitType::kAsync: {
        raw_->async = decltype(raw_->async)();
        break;
      }
      case TransmitType::kCheck:
        raw_->check = decltype(raw_->check)();
        break;
      case TransmitType::kFSEvent:
        raw_->fs_event = decltype(raw_->fs_event)();
        break;
      case TransmitType::kFSPoll:
        raw_->fs_poll = decltype(raw_->fs_poll)();
        break;
      case TransmitType::kIDLE:
        raw_->idle = decltype(raw_->idle)();
        break;
      case TransmitType::kPoll:
        raw_->poll = decltype(raw_->poll)();
        break;
      case TransmitType::kPrepare:
        raw_->prepare = decltype(raw_->prepare)();
        break;
      case TransmitType::kProcess:
        raw_->process = decltype(raw_->process)();
        break;
      case TransmitType::kTimer:
        raw_->timer = decltype(raw_->timer)();
        break;
      case TransmitType::kUDP:
        raw_->udp = decltype(raw_->udp)();
        break;
      case TransmitType::kSignal:
        raw_->signal = decltype(raw_->signal)();
        break;
      case TransmitType::kDefault:
        raw_->handle = decltype(raw_->handle)();
        break;
      default:
        raw_ = uvcc::make_unique<Self>();
        break;
    }
  }
  FileDescriptor(const Self &self, ClosingCompletionBlock &&block = {})
      : BaseObject<uv_handle_t, uv_any_handle>(self),
        closing_completion_block_(std::move(block)) {}
  FileDescriptor(Self &&self, ClosingCompletionBlock &&block = {}) _NOEXCEPT
      : BaseObject<uv_handle_t, uv_any_handle>(std::move(self)),
        closing_completion_block_(std::move(block)) {}
  FileDescriptor(FileDescriptor &&) _NOEXCEPT = default;
  FileDescriptor &operator=(FileDescriptor &&) _NOEXCEPT = default;
  virtual ~FileDescriptor() { _close(); }

  bool isActive() const _NOEXCEPT {
    return !uvcc::expr_assert(uv_is_active(_someRaw()), true);
  }

  bool isClosing() const _NOEXCEPT {
    return !uvcc::expr_assert(uv_is_closing(_someRaw()), true);
  }

  void reference() _NOEXCEPT { uv_ref(_someRaw()); }

  void unreference() _NOEXCEPT { uv_unref(_someRaw()); }

  bool hasReference() const _NOEXCEPT {
    return !uvcc::expr_assert(uv_has_ref(_someRaw()), true);
  }

  std::size_t size() const _NOEXCEPT {
    return _validateType() ? uv_handle_size(_someRaw()->type) : 0;
  }

  const std::unique_ptr<const EventLoop> loop() _NOEXCEPT {
    return uvcc::make_unique<const EventLoop>(*_someRaw()->loop);
  }

  template <typename T>
  const std::unique_ptr<const T> data() const _NOEXCEPT {
    return uv_handle_get_data(_someRaw());
  }

  template <typename T>
  std::unique_ptr<const T> setData(std::unique_ptr<T> data) _NOEXCEPT {
    return uv_handle_set_data(_someRaw(), data);
  }

  virtual TransmitType type() const _NOEXCEPT {
    return _type(_someRaw()->type);
  }

  virtual const std::string typeName() const _NOEXCEPT {
    return std::string(uv_handle_type_name(_someRaw()->type));
  }

 protected:
  ClosingCompletionBlock closing_completion_block_;

  void _close() _NOEXCEPT {
    if (_validateType())
      uv_close(
          _someRaw(),
          closing_completion_block_
              ? closing_completion_block_.target<ClosingRawCompletionBlock>()
              : nullptr);
    else
      return;
  }

  inline virtual bool _validateType() const _NOEXCEPT {
    return _someRaw()->type > UV_UNKNOWN_HANDLE &&
           _someRaw()->type < UV_HANDLE_TYPE_MAX;
  }

 private:
  inline TransmitType _type(const uv_handle_type &raw_type) const _NOEXCEPT {
    return static_cast<TransmitType>(raw_type);
  }

  inline uv_handle_type _rawType(const TransmitType &type) const _NOEXCEPT {
    return static_cast<uv_handle_type>(type);
  }
};

}  // namespace uvcc

#endif  // FILEDESCRIPTOR_H
