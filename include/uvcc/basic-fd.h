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
class basic_fd
    : virtual protected basic_uv_union_object<uv_handle_t, _Tp, uv_any_handle> {
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
                    std::is_same<_Tp, uv_stream_t>::value ||
                    std::is_same<_Tp, uv_tcp_t>::value ||
                    std::is_same<_Tp, uv_tty_t>::value ||
                    std::is_same<_Tp, uv_pipe_t>::value,
                "The '_Tp' should be a legal uv handle type.");

 public:
  typedef _Tp mem_type;
  typedef _Tp &mem_reference;
  typedef const _Tp &const_mem_reference;

  //  typedef uvcc::completion_block<uv_alloc_cb>::c_type
  //  c_alloc_completion_block; typedef
  //  uvcc::completion_block<uv_alloc_cb>::type alloc_completion_block;

  //  typedef uvcc::completion_block<void(uvcc::fd *)> close_completion_block;

  // TODO
  typedef typename uvcc::completion_block<void(basic_fd<mem_type> *,
                                               std::size_t, uv_buf_t *)>::type
      alloc_completion_block;
  typedef typename uvcc::completion_block<void(basic_fd<mem_type> *)>::type
      close_completion_block;
  typedef
      typename uvcc::completion_block<void(basic_fd<mem_type> *, void *)>::type
          map_completion_block;

  explicit basic_fd(event_loop *loop,
                    close_completion_block &&block = {}) _NOEXCEPT
      : basic_uv_union_object<uv_handle_t, mem_type, union_type>(),
        loop_ptr_(loop),
        close_completion_block_ptr_(
            uvcc::make_unique<close_completion_block>(std::move(block))) {}
  //  basic_fd(const basic_fd &) = delete;
  //  basic_fd(basic_fd &&other) _NOEXCEPT { *this = std::move(other); }
  //  basic_fd &operator=(const basic_fd &) = delete;
  //  basic_fd &operator=(basic_fd &&other) _NOEXCEPT {
  //    if (this != &other) {
  //      this->obj_ptr_.reset(other.obj_ptr_.release());
  //    }
  //    return *this;
  //  }
  virtual ~basic_fd() {
#ifdef DEBUG
    uvcc::log::debug(this, __PRETTY_FUNCTION__).print();
#endif
    _force_close();
  }

  bool is_active() const _NOEXCEPT {
    return uvcc::expr::ne(uv_is_active(this->_basic_ptr())).eval();
  }

  bool is_closing() const _NOEXCEPT {
    return uvcc::expr::ne(uv_is_closing(this->_basic_ptr())).eval();
  }

  void reference() _NOEXCEPT { uv_ref(this->_basic_ptr()); }

  void unreference() _NOEXCEPT { uv_unref(this->_basic_ptr()); }

  bool has_reference() const _NOEXCEPT {
    return uvcc::expr::ne(uv_has_ref(this->_basic_ptr())).eval();
  }

  const event_loop *loop() _NOEXCEPT { return loop_ptr_; }

 protected:
  friend class event_loop;

  typedef uv_handle_t basic_type;
  typedef uv_any_handle union_type;

  typedef basic_type &basic_reference;
  typedef const basic_type &const_basic_reference;

  typedef uvcc::completion_block<uv_close_cb>::c_type c_close_completion_block;
  typedef uvcc::completion_block<uv_close_cb>::type f_close_completion_block;

  //  virtual elem_type *_elem_ptr() const _NOEXCEPT {
  //    return reinterpret_cast<elem_type *>(this->_obj_ptr());
  //  }

  //  basic_type *_basic_ptr() const _NOEXCEPT {
  //    return reinterpret_cast<basic_type *>(this->_obj_ptr());
  //  }

  event_loop *loop_ptr_;
  std::unique_ptr<close_completion_block> close_completion_block_ptr_;

 private:
  void _force_close() _NOEXCEPT {
    if (this->_basic_ptr() != nullptr && !is_closing()) {
      uv_close(this->_basic_ptr(),
               _close_func_cast(close_completion_block_ptr_));
      //          close_completion_block_ptr_
      //              ?
      //              close_completion_block_ptr_->target<c_close_completion_block>()
      //              : nullptr);
      this->obj_ptr_.reset();
    }
  }

  inline c_close_completion_block *_close_func_cast() const _NOEXCEPT {
    f_close_completion_block block = [this](uv_handle_t *handle) {
      this->close_completion_block_ptr_();
    };
    return block.target<c_close_completion_block>();
  }

  virtual std::size_t _obj_size() const _NOEXCEPT {
    return uv_handle_size(uv_handle_type::UV_HANDLE);
  }
};

}  // namespace uvcc

#endif  // UVCC_BASIC_FD_H
