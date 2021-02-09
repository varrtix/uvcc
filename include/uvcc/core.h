/// MIT License
///
/// uvcc/core.h
/// uvcc
///
/// created by varrtix on 2021/02/04.
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

#ifndef UVCC_CORE_H
#define UVCC_CORE_H

#include <uv.h>

//#include <functional>
#include <memory>
#include <type_traits>

#include "utilities.h"

namespace uvcc {

template <typename _Tp>
class basic_uv_object {
  static_assert(
                // Any handle type
                std::is_same<_Tp, uv_any_handle>::value ||
                std::is_same<_Tp, uv_async_t>::value ||
                std::is_same<_Tp, uv_check_t>::value ||
                std::is_same<_Tp, uv_fs_event_t>::value ||
                std::is_same<_Tp, uv_fs_poll_t>::value ||
                std::is_same<_Tp, uv_handle_t>::value ||
                std::is_same<_Tp, uv_idle_t>::value ||
                std::is_same<_Tp, uv_pipe_t>::value ||
                std::is_same<_Tp, uv_poll_t>::value ||
                std::is_same<_Tp, uv_prepare_t>::value ||
                std::is_same<_Tp, uv_process_t>::value ||
                std::is_same<_Tp, uv_stream_t>::value ||
                std::is_same<_Tp, uv_tcp_t>::value ||
                std::is_same<_Tp, uv_timer_t>::value ||
                std::is_same<_Tp, uv_tty_t>::value ||
                std::is_same<_Tp, uv_udp_t>::value ||
                std::is_same<_Tp, uv_signal_t>::value ||
                // Any request type
                std::is_same<_Tp, uv_any_req>::value ||
                std::is_same<_Tp, uv_req_t>::value ||
                std::is_same<_Tp, uv_connect_t>::value ||
                std::is_same<_Tp, uv_write_t>::value ||
                std::is_same<_Tp, uv_shutdown_t>::value ||
                std::is_same<_Tp, uv_udp_send_t>::value ||
                std::is_same<_Tp, uv_fs_t>::value ||
                std::is_same<_Tp, uv_work_t>::value ||
                std::is_same<_Tp, uv_getaddrinfo_t>::value ||
                std::is_same<_Tp, uv_getnameinfo_t>::value ||
                std::is_same<_Tp, uv_random_t>::value ||
                // loop type
                std::is_same<_Tp, uv_loop_t>::value,
                "The '_Tp' should be an uv structure type.");
  
public:
  typedef _Tp obj_type;
  typedef _Tp &reference;
  typedef const _Tp &const_reference;
  
protected:
  basic_uv_object() _NOEXCEPT : obj_ptr_(uvcc::make_unique<obj_type>()) {}
  virtual ~basic_uv_object() _NOEXCEPT {
#ifdef DEBUG
    uvcc::log::debug(this, __PRETTY_FUNCTION__).print();
#endif
  }
  
  obj_type *_obj_ptr() const _NOEXCEPT { return obj_ptr_.get(); }
  
  virtual std::size_t _obj_size() const _NOEXCEPT = 0;
  
  std::unique_ptr<obj_type> obj_ptr_;
};

template <typename _Tb, typename _Tp, typename _Tu>
class basic_uv_union_object : virtual protected basic_uv_object<_Tu> {
  static_assert(std::is_same<_Tb, uv_handle_t>::value ||
                std::is_same<_Tb, uv_req_t>::value,
                "The '_Tb' should be the basic type for a '_Tp'.");
  static_assert(
                // Any handle type with union type 'uv_any_handle'
                std::is_same<_Tp, uv_async_t>::value ||
                std::is_same<_Tp, uv_check_t>::value ||
                std::is_same<_Tp, uv_fs_event_t>::value ||
                std::is_same<_Tp, uv_fs_poll_t>::value ||
                //          std::is_same<_Tp, uv_handle_t>::value ||
                std::is_same<_Tp, uv_idle_t>::value ||
                std::is_same<_Tp, uv_pipe_t>::value ||
                std::is_same<_Tp, uv_poll_t>::value ||
                std::is_same<_Tp, uv_prepare_t>::value ||
                std::is_same<_Tp, uv_process_t>::value ||
                std::is_same<_Tp, uv_stream_t>::value ||
                std::is_same<_Tp, uv_tcp_t>::value ||
                std::is_same<_Tp, uv_timer_t>::value ||
                std::is_same<_Tp, uv_tty_t>::value ||
                std::is_same<_Tp, uv_udp_t>::value ||
                std::is_same<_Tp, uv_signal_t>::value ||
                // Any handle type with union type 'uv_any_handle'
                //          std::is_same<_Tp, uv_req_t>::value ||
                std::is_same<_Tp, uv_connect_t>::value ||
                std::is_same<_Tp, uv_write_t>::value ||
                std::is_same<_Tp, uv_shutdown_t>::value ||
                std::is_same<_Tp, uv_udp_send_t>::value ||
                std::is_same<_Tp, uv_fs_t>::value ||
                std::is_same<_Tp, uv_work_t>::value ||
                std::is_same<_Tp, uv_getaddrinfo_t>::value ||
                std::is_same<_Tp, uv_getnameinfo_t>::value ||
                std::is_same<_Tp, uv_random_t>::value,
                "The '_Tp' should be a member type of the '_Tu'.");
  static_assert(std::is_union<_Tu>::value &&
                (std::is_same<_Tu, uv_any_handle>::value ||
                 std::is_same<_Tu, uv_any_req>::value),
                "The '_Tu' should be an union type.");
  
public:
  typedef _Tb basic_obj_type;
  
  typedef _Tp obj_type;
  typedef _Tp &reference;
  typedef const _Tp &const_reference;
  
  typedef _Tu union_obj_type;
  
protected:
  virtual obj_type *_elem_ptr() const _NOEXCEPT = 0;
  basic_obj_type *_basic_ptr() const _NOEXCEPT {
    return reinterpret_cast<basic_obj_type *>(_elem_ptr());
  }
};

}  // namespace uvcc

#endif  // UVCC_CORE_H
