/// MIT License
///
/// uvcc/basic-req.h
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

#ifndef UVCC_BASIC_REQ_H
#define UVCC_BASIC_REQ_H

#include <uv.h>

#include "core.h"
#include "utilities.h"

namespace uvcc {

template <typename _Tp>
class basic_req : virtual protected basic_uv_union_object<_Tp, uv_any_req> {
  static_assert(std::is_same<_Tp, uv_connect_t>::value ||
                    std::is_same<_Tp, uv_write_t>::value ||
                    std::is_same<_Tp, uv_shutdown_t>::value ||
                    std::is_same<_Tp, uv_udp_send_t>::value ||
                    std::is_same<_Tp, uv_fs_t>::value ||
                    std::is_same<_Tp, uv_work_t>::value ||
                    std::is_same<_Tp, uv_getaddrinfo_t>::value ||
                    std::is_same<_Tp, uv_getnameinfo_t>::value ||
                    std::is_same<_Tp, uv_random_t>::value,
                "The '_Tp' should be a legal uv request type.");

 public:
  typedef _Tp elem_type;
  typedef _Tp &elem_reference;
  typedef const _Tp &const_elem_reference;

  void cancel() _NOEXCEPT(false) {
    uvcc::expr(uv_cancel(_basic_ptr())).throws();
  }

 protected:
  typedef uv_req_t basic_type;
  typedef uv_any_req union_type;

  typedef basic_type &basic_reference;
  typedef const basic_type &const_basic_reference;

  explicit basic_req() _NOEXCEPT
      : basic_uv_union_object<elem_type, union_type>() {}
  basic_req(const basic_req &) = delete;
  basic_req(basic_req &&other) _NOEXCEPT { *this = std::move(other); }
  basic_req &operator=(const basic_req &) = delete;
  basic_req &operator=(basic_req &&other) _NOEXCEPT {
    if (this != &other) {
      this->obj_ptr_.reset(other.obj_ptr_.release());
    }
    return *this;
  }
  virtual ~basic_req() {
    // DEBUG
    std::cout << __FUNCTION__ << std::endl;
    //
  }

  virtual elem_type *_elem_ptr() const _NOEXCEPT {
    return reinterpret_cast<elem_type *>(this->_obj_ptr());
  }

  basic_type *_basic_ptr() const _NOEXCEPT {
    return reinterpret_cast<basic_type *>(this->_obj_ptr());
  }

 private:
  virtual std::size_t _obj_size() const _NOEXCEPT {
    return uv_req_size(uv_req_type::UV_REQ);
  }
};

class shutdown final : protected basic_req<uv_shutdown_t> {
 public:
  typedef uvcc::completion_block<void(shutdown *, bool)>::type completion_block;

  explicit shutdown(completion_block &&block = {}) _NOEXCEPT
      : basic_req<elem_type>() {
    _obj_ptr()->shutdown = decltype(_obj_ptr()->shutdown)();
    _obj_ptr()->shutdown.data = static_cast<void *>(this);
    completion_block_ =
        block ? uvcc::make_unique<shutdown_completion_block>(
                    [&block](uv_shutdown_t *req, int status) {
                      auto shutdown_req = uvcc::shutdown(std::move(*req));
                      block(&shutdown_req, static_cast<bool>(!!status));
                    })
              : nullptr;
  }
  shutdown(const shutdown &) = delete;
  shutdown(shutdown &&other) _NOEXCEPT { *this = std::move(other); }
  shutdown(elem_type &&other_elem) _NOEXCEPT { *this = std::move(other_elem); }
  shutdown &operator=(const shutdown &) = delete;
  shutdown &operator=(shutdown &&other) _NOEXCEPT {
    if (this != &other) {
      obj_ptr_.reset(other.obj_ptr_.release());
    }
    return *this;
  }
  shutdown &operator=(elem_type &&other_elem) _NOEXCEPT {
    if (&this->_obj_ptr()->shutdown != &other_elem) {
      _obj_ptr()->shutdown = std::move(other_elem);
    }
    return *this;
  }
  ~shutdown() {
    // DEBUG
    std::cout << __FUNCTION__ << std::endl;
    //
  }

 protected:
  typedef uvcc::completion_block<uv_shutdown_cb>::c_type
      c_shutdown_completion_block;
  typedef uvcc::completion_block<uv_shutdown_cb>::type
      shutdown_completion_block;

  std::unique_ptr<shutdown_completion_block> completion_block_;

  elem_type *_elem_ptr() const _NOEXCEPT override {
    return &obj_ptr_->shutdown;
  }
};

// class Request : virtual protected BaseObject<uv_req_t, uv_any_req> {
// public:
//  enum class TransmitType : int {
//    kDefault = UV_REQ,
//    kConnect = UV_CONNECT,
//    kWrite = UV_WRITE,
//    kShutdown = UV_SHUTDOWN,
//    kUDPSend = UV_UDP_SEND,
//    kFS = UV_FS,
//    kWork = UV_WORK,
//    kGetAddrInfo = UV_GETADDRINFO,
//    kGetNameInfo = UV_GETNAMEINFO,
//    kRandom = UV_RANDOM,
//    kUnknown = UV_UNKNOWN_REQ,
//    //    kRequestTypeMax = UV_REQ_TYPE_MAX,
//  };

//  explicit Request(
//      const TransmitType &type = TransmitType::kDefault) _NOEXCEPT {
//    switch (type) {
//      case TransmitType::kConnect:
//        raw_->connect = decltype(raw_->connect)();
//        break;
//      case TransmitType::kWrite:
//        raw_->write = decltype(raw_->write)();
//        break;
//      case TransmitType::kShutdown:
//        raw_->shutdown = decltype(raw_->shutdown)();
//        break;
//      case TransmitType::kUDPSend:
//        raw_->udp_send = decltype(raw_->udp_send)();
//        break;
//      case TransmitType::kFS:
//        raw_->fs = decltype(raw_->fs)();
//        break;
//      case TransmitType::kWork:
//        raw_->work = decltype(raw_->work)();
//        break;
//      case TransmitType::kGetAddrInfo:
//        raw_->getaddrinfo = decltype(raw_->getaddrinfo)();
//        break;
//      case TransmitType::kGetNameInfo:
//        raw_->getnameinfo = decltype(raw_->getnameinfo)();
//        break;
//      case TransmitType::kRandom:
//        raw_->random = decltype(raw_->random)();
//        break;
//      case TransmitType::kDefault:
//        raw_->req = decltype(raw_->req)();
//        break;
//      default:
//        raw_ = uvcc::make_unique<Self>();
//        break;
//    }
//  }
//  Request(const Self &self) : BaseObject<uv_req_t, uv_any_req>(self) {}
//  Request(Self &&self) _NOEXCEPT
//      : BaseObject<uv_req_t, uv_any_req>(std::move(self)) {}
//  Request(Request &&) _NOEXCEPT = default;
//  Request &operator=(Request &&) _NOEXCEPT = default;
//  virtual ~Request() = default;

//  void cancel() { uvcc::expr_throws(uv_cancel(_someRaw()), true); }

//  std::size_t size() const _NOEXCEPT {
//    return _validateType() ? uv_req_size(_someRaw()->type) : 0;
//  }

//  template <typename T>
//  const std::unique_ptr<const T> data() const _NOEXCEPT {
//    return uv_req_get_data(_someRaw());
//  }

//  template <typename T>
//  std::unique_ptr<const T> setData(std::unique_ptr<T> data) _NOEXCEPT {
//    return uv_req_set_data(_someRaw(), data);
//  }

//  virtual TransmitType type() const _NOEXCEPT {
//    return _type(_someRaw()->type);
//  }

//  virtual const std::string typeName() const _NOEXCEPT {
//    return std::string(uv_req_type_name(_someRaw()->type));
//  }

// protected:
//  inline virtual bool _validateType() const _NOEXCEPT {
//    return _someRaw()->type > UV_UNKNOWN_REQ &&
//           _someRaw()->type < UV_REQ_TYPE_MAX;
//  }

// private:
//  inline TransmitType _type(const uv_req_type &raw_type) const _NOEXCEPT {
//    return static_cast<TransmitType>(raw_type);
//  }

//  inline uv_req_type _rawType(const TransmitType &type) const _NOEXCEPT {
//    return static_cast<uv_req_type>(type);
//  }
//};

}  // namespace uvcc

#endif  // UVCC_BASIC_REQ_H
