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

#include <functional>

#include "file-descriptor.h"

namespace uvcc {

class Stream : protected FileDescriptor {
 public:
  enum class TransmitType : int {
    kDefault = UV_STREAM,
    kTCP = UV_TCP,
    kTTY = UV_TTY,
    kNamedPipe = UV_NAMED_PIPE,
    kUnknown = UV_UNKNOWN_HANDLE,
    kFile = UV_FILE,
    kHandleTypeMax = UV_HANDLE_TYPE_MAX,
  };

  explicit Stream(const TransmitType &type = TransmitType::kDefault,
                  std::function<void(uv_stream_t *)> &&closing_callback = {})
      _NOEXCEPT : stream_type_(type) {
    //    closing_callback_ = std::move(closing_callback);
    closing_callback_ = [&closing_callback](uv_handle_t *handle) {
      closing_callback(reinterpret_cast<uv_stream_t *>(handle));
    };
    //        std::bind(closing_callback,
    //                  reinterpret_cast<uv_handle_t
    //                  *>(std::placeholders::_1));
    switch (stream_type_) {
      case TransmitType::kTCP:
        raw_->tcp = uv_tcp_t();
        break;
      case TransmitType::kDefault:
      default:
        raw_->stream = uv_stream_t();
        break;
    }
  }

  void shutdown() {}

 private:
  TransmitType stream_type_;

  uv_stream_t *_someStream() const _NOEXCEPT {
    switch (stream_type_) {
      case TransmitType::kTCP:
        return reinterpret_cast<uv_stream_t *>(&raw_->tcp);
      case TransmitType::kDefault:
      default:
        return reinterpret_cast<uv_stream_t *>(&raw_->stream);
    }
  }
};

}  // namespace uvcc

#endif  // STREAM_H
