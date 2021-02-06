/// MIT License
///
/// uvcc/basic-stream.h
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

#ifndef UVCC_BASIC_STREAM_H
#define UVCC_BASIC_STREAM_H

#include <uv.h>

//#include "file-descriptor.h"
#include "core.h"
#include "utilities.h"

namespace uvcc {
/*
 * Maximum queue length specifiable by listen.
 * [FIX] Needs to support multi-platforms.
 */
static constexpr auto K_SOMAXCONN = 128;


// class Stream : protected FileDescriptor {
// protected:
//  using ReadingRawCompletionBlock = uvcc::RawCompletionBlock<uv_read_cb>;
//  using ReadingCompletionBlock = std::function<ReadingRawCompletionBlock>;
//  using WritingRawCompletionBlock = uvcc::RawCompletionBlock<uv_write_cb>;
//  using WritingCompletionBlock = std::function<WritingRawCompletionBlock>;
//  using ConnectingRawCompletionBlock =
//  uvcc::RawCompletionBlock<uv_connect_cb>; using ConnectingCompletionBlock =
//  std::function<ConnectingRawCompletionBlock>; using
//  ShutdownRawCompletionBlock = uvcc::RawCompletionBlock<uv_shutdown_cb>; using
//  ShutdownCompletionBlock = std::function<ShutdownRawCompletionBlock>; using
//  RecevingRawCompletionBlock = uvcc::RawCompletionBlock<uv_connection_cb>;
//  using RecevingCompletionBlock = std::function<RecevingRawCompletionBlock>;
//  using StreamRawValueType = uv_stream_t;

// public:
//  enum class TransmitType : int {
//    kDefault = UV_STREAM,
//    kTCP = UV_TCP,
//    kTTY = UV_TTY,
//    kNamedPipe = UV_NAMED_PIPE,
//    kFile = UV_FILE,
//    kUnknown = UV_UNKNOWN_HANDLE,
//    //    kHandleTypeMax = UV_HANDLE_TYPE_MAX,
//  };

//  explicit Stream(const TransmitType &type = TransmitType::kDefault,
//                  ClosingCompletionBlock &&block = {}) _NOEXCEPT {
//    closing_completion_block_ = std::move(block);
//    switch (type) {
//      case TransmitType::kTCP:
//        raw_->tcp = decltype(raw_->tcp)();
//        break;
//      case TransmitType::kTTY:
//        raw_->tty = decltype(raw_->tty)();
//        break;
//      case TransmitType::kNamedPipe:
//        raw_->pipe = decltype(raw_->pipe)();
//        break;
//      case TransmitType::kDefault:
//        raw_->stream = decltype(raw_->stream)();
//        break;
//      default:
//        raw_ = uvcc::make_unique<Self>();
//        break;
//    }
//  }
//  Stream(const Self &self, ClosingCompletionBlock &&block)
//      : FileDescriptor(self, std::move(block)) {}
//  Stream(Self &&self, ClosingCompletionBlock &&block = {}) _NOEXCEPT
//      : FileDescriptor(std::move(self), std::move(block)) {}
//  Stream(Stream &&) _NOEXCEPT = default;
//  Stream &operator=(Stream &&) _NOEXCEPT = default;
//  virtual ~Stream() = default;

//  bool isReadable() const _NOEXCEPT { return uv_is_readable(_someStream()); }

//  bool isWritable() const _NOEXCEPT { return uv_is_writable(_someStream()); }

//  void setBlocking(bool enabled) {
//    uvcc::expr_throws(
//        uv_stream_set_blocking(_someStream(), static_cast<int>(enabled)),
//        true);
//  }

//  std::size_t writeQueueSize() const _NOEXCEPT {
//    return uv_stream_get_write_queue_size(_someStream());
//  }

// protected:
//  inline virtual bool _validateType() const _NOEXCEPT {
//    auto t = _someStream()->type;
//    return t == UV_STREAM || t == UV_TCP || t == UV_TTY || t == UV_NAMED_PIPE
//    ||
//           t == UV_FILE;
//  }

//  inline auto _someStream() const _NOEXCEPT ->
//      typename std::add_pointer<StreamRawValueType>::type {
//    return reinterpret_cast<
//        typename std::add_pointer<StreamRawValueType>::type>(raw_.get());
//  }

// private:
//  inline TransmitType _type(const uv_handle_type &raw_type) const _NOEXCEPT {
//    return static_cast<TransmitType>(raw_type);
//  }

//  inline uv_handle_type _rawType(const TransmitType &type) const _NOEXCEPT {
//    return static_cast<uv_handle_type>(type);
//  }
//};

template <typename _Tp>
class basic_stream_fd : virtual protected basic_fd<_Tp> {
  static_assert(std::is_same<_Tp, uv_tcp_t>::value ||
                    std::is_same<_Tp, uv_tty_t>::value ||
                    std::is_same<_Tp, uv_pipe_t>::value,
                "The '_Tp' should be a legal uv type.");

  friend class shutdown;

  typedef uvcc::completion_block<uv_connection_cb>::c_type
      c_connection_completion_block;
  typedef uvcc::completion_block<uv_connection_cb>::type
      connection_completion_block;

 public:
  typedef _Tp elem_type;
  typedef _Tp &reference;
  typedef const _Tp &const_reference;

  typedef uvcc::completion_block<uv_read_cb>::c_type c_read_completion_block;
  typedef uvcc::completion_block<uv_read_cb>::type read_completion_block;
  typedef uvcc::completion_block<uv_write_cb>::c_type c_write_completion_block;
  typedef uvcc::completion_block<uv_write_cb>::type write_completion_block;
  typedef uvcc::completion_block<uv_connect_cb>::c_type
      c_connect_completion_block;
  typedef uvcc::completion_block<uv_connect_cb>::type connect_completion_block;

//  void shutdown(const shutdown &req) _NOEXCEPT(false) {
//    uvcc::expr(
//        uv_shutdown(req._elem_ptr(), _basic_stream_ptr(),
//                    req.completion_block_
//                        ? req.completion_block_
//                              ->target<shutdown::c_shutdown_completion_block>()
//                    : nullptr)).throws();
//  }

//  void listen(uvcc::completion_block<void(basic_fd<elem_type> *, bool)> &&block,
//              int backlog) _NOEXCEPT {
//    connect_completion_block cb = [&block](uv_stream_t *stream, int status) {
//      // TODO
//    };
//    auto output = uvcc::expr(uv_listen(_basic_stream_ptr(), backlog,
//                         cb.target<c_connect_completion_block>()));
//    if (output.failed())
//      uvcc::log::warning(this, __PRETTY_FUNCTION__).print(output.str());
//  }
  void listen(uvcc::completion_block<void(basic_stream_fd<elem_type> *, bool)> &&block, const int &backlog = K_SOMAXCONN) _NOEXCEPT(false) {
    connect_completion_block cb = [&block](uv_stream_t *stream, int status) {
      block();
    };
    auto output = uvcc::expr(uv_listen(_basic_stream_ptr(), backlog, cb.target<c_connect_completion_block>()));
    if (output.failed())
      uvcc::log::warning(this, __PRETTY_FUNCTION__).print(output.str());
  }

 protected:
  typedef uv_stream_t basic_stream_type;
  typedef basic_stream_type &basic_stream_reference;
  typedef const basic_stream_type &const_basic_stream_reference;

  virtual elem_type *_elem_ptr() const _NOEXCEPT {
    return reinterpret_cast<elem_type *>(this->_obj_ptr());
  }

  basic_stream_type *_basic_stream_ptr() const _NOEXCEPT {
    return reinterpret_cast<basic_stream_type *>(this->_obj_ptr());
  }

  virtual std::size_t _obj_size() const _NOEXCEPT {
    return uv_handle_size(uv_handle_type::UV_STREAM);
  }
};

// class tcp_fd : public basic_uv_union_object<uv_tcp_t, uv_any_handle> {};
class tcp_fd final : protected basic_stream_fd<uv_tcp_t> {};

}  // namespace uvcc

#endif  // UVCC_BASIC_STREAM_H
