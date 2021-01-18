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
#include <uvcc/utilities.h>

#include <memory>
#include <string>

namespace uvcc {

class Request {
 public:
  using Storage = uv_any_req;

  enum class Type : int {
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
    kRequestTypeMax = UV_REQ_TYPE_MAX,
  };

  explicit Request(const Type &type = Type::kDefault) _NOEXCEPT
      : req_type_(type) {
    switch (req_type_) {
      case Type::kDefault:
      default:
        req_ = uvcc::make_unique<Storage>(uv_req_t());
    }
  }

  void cancel() { uvcc::expr_throws(uv_cancel(_someRequest()), true); }

  std::size_t size() const _NOEXCEPT {
    return uv_req_size(_someRequest()->type);
  }

  template <typename T>
  const std::unique_ptr<const T> data() const _NOEXCEPT {
    return uv_req_get_data(_someRequest());
  }

  template <typename T>
  std::unique_ptr<const T> setData(std::unique_ptr<T> data) _NOEXCEPT {
    return uv_req_set_data(_someRequest(), data);
  }

  virtual const Type &type() const _NOEXCEPT { return req_type_; }

  virtual const std::string typeName() const _NOEXCEPT {
    return std::string(uv_req_type_name(_someRequest()->type));
  }

 protected:
  std::unique_ptr<Storage> req_;

  inline uv_req_t *_someRequest() const _NOEXCEPT {
    switch (req_type_) {
      case Type::kDefault:
      default:
        return reinterpret_cast<uv_req_t *>(&req_->req);
    }
  }

 private:
  Type req_type_;
};

}  // namespace uvcc

#endif  // REQUEST_H
