/// MIT License
///
/// uvcc/request.h
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

#ifndef REQUEST_H
#define REQUEST_H

#include <uv.h>

#include "utilities.h"

namespace uvcc {

class Request : protected BaseObject<uv_req_t, uv_any_req> {
 public:
  enum class TransmitType : int {
    kDefault = UV_REQ,
    kConnect = UV_CONNECT,
    kWrite = UV_WRITE,
    kShutdown = UV_SHUTDOWN,
    kUDPSend = UV_UDP_SEND,
    kFS = UV_FS,
    kWork = UV_WORK,
    kGetAddrInfo = UV_GETADDRINFO,
    kGetNameInfo = UV_GETNAMEINFO,
    kRandom = UV_RANDOM,
    kUnknown = UV_UNKNOWN_REQ,
    //    kRequestTypeMax = UV_REQ_TYPE_MAX,
  };

  explicit Request(
      const TransmitType &type = TransmitType::kDefault) _NOEXCEPT {
    switch (type) {
      case TransmitType::kConnect:
        raw_->connect = decltype(raw_->connect)();
        break;
      case TransmitType::kWrite:
        raw_->write = decltype(raw_->write)();
        break;
      case TransmitType::kShutdown:
        raw_->shutdown = decltype(raw_->shutdown)();
        break;
      case TransmitType::kUDPSend:
        raw_->udp_send = decltype(raw_->udp_send)();
        break;
      case TransmitType::kFS:
        raw_->fs = decltype(raw_->fs)();
        break;
      case TransmitType::kWork:
        raw_->work = decltype(raw_->work)();
        break;
      case TransmitType::kGetAddrInfo:
        raw_->getaddrinfo = decltype(raw_->getaddrinfo)();
        break;
      case TransmitType::kGetNameInfo:
        raw_->getnameinfo = decltype(raw_->getnameinfo)();
        break;
      case TransmitType::kRandom:
        raw_->random = decltype(raw_->random)();
        break;
      case TransmitType::kDefault:
        raw_->req = decltype(raw_->req)();
        break;
      default:
        raw_ = uvcc::make_unique<Self>();
        break;
    }
  }
  Request(const Self &self) : BaseObject<uv_req_t, uv_any_req>(self) {}
  Request(Self &&self) _NOEXCEPT
      : BaseObject<uv_req_t, uv_any_req>(std::move(self)) {}
  Request(Request &&) _NOEXCEPT = default;
  Request &operator=(Request &&) _NOEXCEPT = default;
  virtual ~Request() = default;

  void cancel() { uvcc::expr_throws(uv_cancel(_someRaw()), true); }

  std::size_t size() const _NOEXCEPT {
    return _validateType() ? uv_req_size(_someRaw()->type) : 0;
  }

  template <typename T>
  const std::unique_ptr<const T> data() const _NOEXCEPT {
    return uv_req_get_data(_someRaw());
  }

  template <typename T>
  std::unique_ptr<const T> setData(std::unique_ptr<T> data) _NOEXCEPT {
    return uv_req_set_data(_someRaw(), data);
  }

  virtual TransmitType type() const _NOEXCEPT {
    return _type(_someRaw()->type);
  }

  virtual const std::string typeName() const _NOEXCEPT {
    return std::string(uv_req_type_name(_someRaw()->type));
  }

 protected:
  inline virtual bool _validateType() const _NOEXCEPT {
    return _someRaw()->type > UV_UNKNOWN_REQ &&
           _someRaw()->type < UV_REQ_TYPE_MAX;
  }

 private:
  inline TransmitType _type(const uv_req_type &raw_type) const _NOEXCEPT {
    return static_cast<TransmitType>(raw_type);
  }

  inline uv_req_type _rawType(const TransmitType &type) const _NOEXCEPT {
    return static_cast<uv_req_type>(type);
  }
};

}  // namespace uvcc

#endif  // REQUEST_H
