/// MIT License
///
/// uvcc/network.h
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

#ifndef NETWORK_H
#define NETWORK_H

#include <sys/socket.h>
#include <uv.h>

#include "utilities.h"

namespace uvcc {

namespace network {

typedef union {
  sockaddr addr_;
  sockaddr_in addr_in_4_;
  sockaddr_in6 addr_in_6_;
} AnyRawSocketEndpoint;

typedef union {
  struct in_addr addr_4_;
  struct in6_addr addr_6_;
} AnyRawSocketAddress;

class Endpoint : protected BaseObject<sockaddr, AnyRawSocketEndpoint> {
 public:
  template <typename AddressType>
  class BaseIPAddress : protected BaseObject<AddressType, AnyRawSocketAddress> {
  };

  class IPv4Address : protected BaseIPAddress<struct in_addr> {
    friend class Endpoint;

   public:
    IPv4Address() = delete;
    explicit IPv4Address(const std::string &addr_str)
        : BaseIPAddress<struct in_addr>() {
      raw_->addr_4_ = decltype(raw_->addr_4_)();
      uvcc::expr_throws(uv_inet_pton(AF_INET, addr_str.c_str(), _someRaw()));
    }
    IPv4Address(const struct in_addr &addr) : BaseIPAddress<struct in_addr>() {
      raw_->addr_4_ = addr;
    }
    IPv4Address(struct in_addr &&addr) _NOEXCEPT
        : BaseIPAddress<struct in_addr>() {
      raw_->addr_4_ = std::move(addr);
    }
    IPv4Address(const IPv4Address &addr) {
      raw_ = uvcc::make_unique<Self>(*addr.raw_);
    }
    IPv4Address(IPv4Address &&) _NOEXCEPT = default;
    IPv4Address &operator=(const IPv4Address &addr) {
      if (&addr != this) this->raw_.reset(addr.raw_.get());
      return *this;
    }
    IPv4Address &operator=(IPv4Address &&) _NOEXCEPT = default;
    IPv4Address &operator=(const struct in_addr &addr) {
      if (&addr != this->_someRaw()) raw_->addr_4_ = addr;
      return *this;
    }
    IPv4Address &operator=(struct in_addr &&addr) _NOEXCEPT {
      if (&addr != this->_someRaw()) raw_->addr_4_ = std::move(addr);
      return *this;
    }
    ~IPv4Address() = default;

    static const IPv4Address any() _NOEXCEPT { return IPv4Address("0.0.0.0"); }

    static const IPv4Address broadcast() _NOEXCEPT {
      return IPv4Address("255.255.255.255");
    }
    static const IPv4Address loopback() _NOEXCEPT {
      return IPv4Address("127.0.0.1");
    }
    static const IPv4Address allHostsGroup() _NOEXCEPT {
      return IPv4Address("224.0.0.1");
    }
    static const IPv4Address allRoutersGroup() _NOEXCEPT {
      return IPv4Address("224.0.0.2");
    }
    static const IPv4Address allReportsGroup() _NOEXCEPT {
      return IPv4Address("224.0.0.22");
    }
    static const IPv4Address mdnsGroup() _NOEXCEPT {
      return IPv4Address("224.0.0.251");
    }
  };

  class IPv6Address : protected BaseIPAddress<struct in6_addr> {
    friend class Endpoint;
  };

  typedef enum : int {
    kAny = 0,
    kSSH = 22,
    kSTMP = 25,
    kHTTP = 80,
    kPOP = 110,
    kIMAP = 143,
    kHTTPs = 443,
    kIMAPs = 993,
    kSocks = 1080,
  } Port;

  explicit Endpoint(const IPv4Address &address, const Port &port) {
    Endpoint(address, static_cast<std::uint16_t>(port));
  }
  explicit Endpoint(const IPv4Address &address, const std::uint16_t port) {
    raw_->addr_in_4_ = decltype(raw_->addr_in_4_)();
    raw_->addr_in_4_.sin_family = AF_INET;
    raw_->addr_in_4_.sin_addr = *address._someRaw();
    raw_->addr_in_4_.sin_port = htons(port);
  }
  Endpoint(const Endpoint &ep) { raw_ = uvcc::make_unique<Self>(*ep.raw_); }
  Endpoint(Endpoint &&) _NOEXCEPT = default;
  Endpoint &operator=(const Endpoint &ep) {
    if (&ep != this) this->raw_.reset(ep.raw_.get());
    return *this;
  }
  Endpoint &operator=(Endpoint &&) = default;
  ~Endpoint() = default;
};

class Listener {};

class Connection {};

}  // namespace network

}  // namespace uvcc

#endif  // NETWORK_H
