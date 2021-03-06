/// MIT License
///
/// uvcc/stream.h
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

#ifndef STREAM_H
#define STREAM_H

#include <uv.h>

#include "file-descriptor.h"

namespace uvcc {

class Stream : protected FileDescriptor {
 protected:
  using ReadingRawCompletionBlock = uvcc::RawCompletionBlock<uv_read_cb>;
  using ReadingCompletionBlock = std::function<ReadingRawCompletionBlock>;
  using WritingRawCompletionBlock = uvcc::RawCompletionBlock<uv_write_cb>;
  using WritingCompletionBlock = std::function<WritingRawCompletionBlock>;
  using ConnectingRawCompletionBlock = uvcc::RawCompletionBlock<uv_connect_cb>;
  using ConnectingCompletionBlock = std::function<ConnectingRawCompletionBlock>;
  using ShutdownRawCompletionBlock = uvcc::RawCompletionBlock<uv_shutdown_cb>;
  using ShutdownCompletionBlock = std::function<ShutdownRawCompletionBlock>;
  using RecevingRawCompletionBlock = uvcc::RawCompletionBlock<uv_connection_cb>;
  using RecevingCompletionBlock = std::function<RecevingRawCompletionBlock>;
  using StreamRawValueType = uv_stream_t;

 public:
  enum class TransmitType : int {
    kDefault = UV_STREAM,
    kTCP = UV_TCP,
    kTTY = UV_TTY,
    kNamedPipe = UV_NAMED_PIPE,
    kFile = UV_FILE,
    kUnknown = UV_UNKNOWN_HANDLE,
    //    kHandleTypeMax = UV_HANDLE_TYPE_MAX,
  };

  explicit Stream(const TransmitType &type = TransmitType::kDefault,
                  ClosingCompletionBlock &&block = {}) _NOEXCEPT {
    closing_completion_block_ = std::move(block);
    switch (type) {
      case TransmitType::kTCP:
        raw_->tcp = decltype(raw_->tcp)();
        break;
      case TransmitType::kTTY:
        raw_->tty = decltype(raw_->tty)();
        break;
      case TransmitType::kNamedPipe:
        raw_->pipe = decltype(raw_->pipe)();
        break;
      case TransmitType::kDefault:
        raw_->stream = decltype(raw_->stream)();
        break;
      default:
        raw_ = uvcc::make_unique<Self>();
        break;
    }
  }
  Stream(const Self &self, ClosingCompletionBlock &&block)
      : FileDescriptor(self, std::move(block)) {}
  Stream(Self &&self, ClosingCompletionBlock &&block = {}) _NOEXCEPT
      : FileDescriptor(std::move(self), std::move(block)) {}
  Stream(Stream &&) _NOEXCEPT = default;
  Stream &operator=(Stream &&) _NOEXCEPT = default;
  virtual ~Stream() = default;

  bool isReadable() const _NOEXCEPT { return uv_is_readable(_someStream()); }

  bool isWritable() const _NOEXCEPT { return uv_is_writable(_someStream()); }

  void setBlocking(bool enabled) {
    uvcc::expr_throws(
        uv_stream_set_blocking(_someStream(), static_cast<int>(enabled)), true);
  }

  std::size_t writeQueueSize() const _NOEXCEPT {
    return uv_stream_get_write_queue_size(_someStream());
  }

 protected:
  inline virtual bool _validateType() const _NOEXCEPT {
    auto t = _someStream()->type;
    return t == UV_STREAM || t == UV_TCP || t == UV_TTY || t == UV_NAMED_PIPE ||
           t == UV_FILE;
  }

  inline auto _someStream() const _NOEXCEPT ->
      typename std::add_pointer<StreamRawValueType>::type {
    return reinterpret_cast<
        typename std::add_pointer<StreamRawValueType>::type>(raw_.get());
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

#endif  // STREAM_H
