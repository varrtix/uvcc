/// MIT License
///
/// uvcc/basic-fd.h
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

#ifndef UVCC_BASIC_FD_H
#define UVCC_BASIC_FD_H

#include <uv.h>

#include "core.h"
#include "event-loop.h"
#include "utilities.h"

namespace uvcc {

template <typename _Tp>
class basic_fd : virtual protected basic_uv_union_object<_Tp, uv_any_handle> {
  static_assert(std::is_same<_Tp, uv_async_t>::value ||
                    std::is_same<_Tp, uv_check_t>::value ||
                    std::is_same<_Tp, uv_fs_event_t>::value ||
                    std::is_same<_Tp, uv_fs_poll_t>::value ||
                    std::is_same<_Tp, uv_idle_t>::value ||
                    std::is_same<_Tp, uv_poll_t>::value ||
                    std::is_same<_Tp, uv_prepare_t>::value ||
                    std::is_same<_Tp, uv_process_t>::value ||
                    std::is_same<_Tp, uv_timer_t>::value ||
                    std::is_same<_Tp, uv_udp_t>::value ||
                    std::is_same<_Tp, uv_signal_t>::value ||
                    // stream type
                    std::is_same<_Tp, uv_tcp_t>::value ||
                    std::is_same<_Tp, uv_tty_t>::value ||
                    std::is_same<_Tp, uv_pipe_t>::value,
                "The '_Tp' should be a legal uv handle type.");

 public:
  typedef _Tp elem_type;
  typedef _Tp &elem_reference;
  typedef const _Tp &const_elem_reference;

  typedef uvcc::completion_block<uv_alloc_cb>::c_type c_alloc_completion_block;
  typedef uvcc::completion_block<uv_alloc_cb>::type alloc_completion_block;
  typedef uvcc::completion_block<uv_close_cb>::c_type c_close_completion_block;
  typedef uvcc::completion_block<uv_close_cb>::type close_completion_block;

  bool is_active() const _NOEXCEPT {
    return uvcc::expr::ne(uv_is_active(_basic_ptr())).eval();
  }

  bool is_closing() const _NOEXCEPT {
    return uvcc::expr::ne(uv_is_closing(_basic_ptr())).eval();
  }

  void reference() _NOEXCEPT { uv_ref(_basic_ptr()); }

  void unreference() _NOEXCEPT { uv_unref(_basic_ptr()); }

  bool has_reference() const _NOEXCEPT {
    return uvcc::expr::ne(uv_has_ref(_basic_ptr())).eval();
  }

  //  event_loop *loop() _NOEXCEPT { return loop_ptr_.get(); }
  const event_loop **loop() _NOEXCEPT { return loop_p_ptr_.get(); }

 protected:
  friend class event_loop;

  typedef uv_handle_t basic_type;
  typedef uv_any_handle union_type;

  typedef basic_type &basic_reference;
  typedef const basic_type &const_basic_reference;

  explicit basic_fd(const event_loop *const loop,
                    close_completion_block &&block = {}) _NOEXCEPT
      : basic_uv_union_object<elem_type, union_type>(),
        close_completion_block_ptr_(
            uvcc::make_unique<close_completion_block>(std::move(block))) {
    this->loop_p_ptr_.reset(&loop);
  }
  basic_fd(const basic_fd &) = delete;
  basic_fd(basic_fd &&other) _NOEXCEPT { *this = std::move(other); }
  basic_fd &operator=(const basic_fd &) = delete;
  basic_fd &operator=(basic_fd &&other) _NOEXCEPT {
    if (this != &other) {
      this->obj_ptr_.reset(other.obj_ptr_.release());
    }
    return *this;
  }
  virtual ~basic_fd() {
    // DEBUG
    std::cout << __FUNCTION__ << std::endl;
    //
    _force_close();
  }

  virtual elem_type *_elem_ptr() const _NOEXCEPT {
    return reinterpret_cast<elem_type *>(this->_obj_ptr());
  }

  basic_type *_basic_ptr() const _NOEXCEPT {
    return reinterpret_cast<basic_type *>(this->_obj_ptr());
  }

  // protected:
  std::unique_ptr<close_completion_block> close_completion_block_ptr_;
  std::unique_ptr<event_loop *> loop_p_ptr_;

 private:
  void _force_close() _NOEXCEPT {
    if (_basic_ptr() != nullptr && !is_closing()) {
      uv_close(
          _basic_ptr(),
          close_completion_block_ptr_
              ? close_completion_block_ptr_->target<c_close_completion_block>()
              : nullptr);
      this->obj_ptr_.reset();
    }
  }

  virtual std::size_t _obj_size() const _NOEXCEPT {
    return uv_handle_size(uv_handle_type::UV_HANDLE);
  }
};
}  // namespace uvcc
//
// namespace uvcc {
//
// class FileDescriptor
//    : virtual protected BaseObject<uv_handle_t, uv_any_handle> {
// protected:
//  using AllocatingRawCompletionBlock = uvcc::RawCompletionBlock<uv_alloc_cb>;
//  using AllocatingCompletionBlock =
//  std::function<AllocatingRawCompletionBlock>; using ClosingRawCompletionBlock
//  = uvcc::RawCompletionBlock<uv_close_cb>; using ClosingCompletionBlock =
//  std::function<ClosingRawCompletionBlock>;
//
// public:
//  enum class TransmitType : int {
//    kDefault = UV_HANDLE,
//    kAsync = UV_ASYNC,
//    kCheck = UV_CHECK,
//    kFSEvent = UV_FS_EVENT,
//    kFSPoll = UV_FS_POLL,
//    kIDLE = UV_IDLE,
//    kPoll = UV_POLL,
//    kPrepare = UV_PREPARE,
//    kProcess = UV_PROCESS,
//    kTimer = UV_TIMER,
//    kUDP = UV_UDP,
//    kSignal = UV_SIGNAL,
//    kFile = UV_FILE,
//    kUnknown = UV_UNKNOWN_HANDLE,
//    //    kDescriptorTypeMax = UV_HANDLE_TYPE_MAX,
//  };
//
//  explicit FileDescriptor(const TransmitType &type = TransmitType::kDefault,
//                          ClosingCompletionBlock &&block = {}) _NOEXCEPT
//      : closing_completion_block_(std::move(block)) {
//    switch (type) {
//      case TransmitType::kAsync: {
//        raw_->async = decltype(raw_->async)();
//        break;
//      }
//      case TransmitType::kCheck:
//        raw_->check = decltype(raw_->check)();
//        break;
//      case TransmitType::kFSEvent:
//        raw_->fs_event = decltype(raw_->fs_event)();
//        break;
//      case TransmitType::kFSPoll:
//        raw_->fs_poll = decltype(raw_->fs_poll)();
//        break;
//      case TransmitType::kIDLE:
//        raw_->idle = decltype(raw_->idle)();
//        break;
//      case TransmitType::kPoll:
//        raw_->poll = decltype(raw_->poll)();
//        break;
//      case TransmitType::kPrepare:
//        raw_->prepare = decltype(raw_->prepare)();
//        break;
//      case TransmitType::kProcess:
//        raw_->process = decltype(raw_->process)();
//        break;
//      case TransmitType::kTimer:
//        raw_->timer = decltype(raw_->timer)();
//        break;
//      case TransmitType::kUDP:
//        raw_->udp = decltype(raw_->udp)();
//        break;
//      case TransmitType::kSignal:
//        raw_->signal = decltype(raw_->signal)();
//        break;
//      case TransmitType::kDefault:
//        raw_->handle = decltype(raw_->handle)();
//        break;
//      default:
//        raw_ = uvcc::make_unique<Self>();
//        break;
//    }
//  }
//  FileDescriptor(const Self &self, ClosingCompletionBlock &&block = {})
//      : BaseObject<uv_handle_t, uv_any_handle>(self),
//        closing_completion_block_(std::move(block)) {}
//  FileDescriptor(Self &&self, ClosingCompletionBlock &&block = {}) _NOEXCEPT
//      : BaseObject<uv_handle_t, uv_any_handle>(std::move(self)),
//        closing_completion_block_(std::move(block)) {}
//  FileDescriptor(FileDescriptor &&) _NOEXCEPT = default;
//  FileDescriptor &operator=(FileDescriptor &&) _NOEXCEPT = default;
//  virtual ~FileDescriptor() { _close(); }
//
//  bool isActive() const _NOEXCEPT {
//    return !uvcc::expr_assert(uv_is_active(_someRaw()), true);
//  }
//
//  bool isClosing() const _NOEXCEPT {
//    return !uvcc::expr_assert(uv_is_closing(_someRaw()), true);
//  }
//
//  void reference() _NOEXCEPT { uv_ref(_someRaw()); }
//
//  void unreference() _NOEXCEPT { uv_unref(_someRaw()); }
//
//  bool hasReference() const _NOEXCEPT {
//    return !uvcc::expr_assert(uv_has_ref(_someRaw()), true);
//  }
//
//  std::size_t size() const _NOEXCEPT {
//    return _validateType() ? uv_handle_size(_someRaw()->type) : 0;
//  }
//
//  const std::unique_ptr<const EventLoop> loop() _NOEXCEPT {
//    return uvcc::make_unique<const EventLoop>(*_someRaw()->loop);
//  }
//
//  template <typename T>
//  const std::unique_ptr<const T> data() const _NOEXCEPT {
//    return uv_handle_get_data(_someRaw());
//  }
//
//  template <typename T>
//  std::unique_ptr<const T> setData(std::unique_ptr<T> data) _NOEXCEPT {
//    return uv_handle_set_data(_someRaw(), data);
//  }
//
//  virtual TransmitType type() const _NOEXCEPT {
//    return _type(_someRaw()->type);
//  }
//
//  virtual const std::string typeName() const _NOEXCEPT {
//    return std::string(uv_handle_type_name(_someRaw()->type));
//  }
//
// protected:
//  ClosingCompletionBlock closing_completion_block_;
//
//  void _close() _NOEXCEPT {
//    if (_validateType())
//      uv_close(
//          _someRaw(),
//          closing_completion_block_
//              ? closing_completion_block_.target<ClosingRawCompletionBlock>()
//              : nullptr);
//    else
//      return;
//  }
//
//  inline virtual bool _validateType() const _NOEXCEPT {
//    return _someRaw()->type > UV_UNKNOWN_HANDLE &&
//           _someRaw()->type < UV_HANDLE_TYPE_MAX;
//  }
//
// private:
//  inline TransmitType _type(const uv_handle_type &raw_type) const _NOEXCEPT {
//    return static_cast<TransmitType>(raw_type);
//  }
//
//  inline uv_handle_type _rawType(const TransmitType &type) const _NOEXCEPT {
//    return static_cast<uv_handle_type>(type);
//  }
//};
//
//}  // namespace uvcc

#endif  // UVCC_BASIC_FD_H
