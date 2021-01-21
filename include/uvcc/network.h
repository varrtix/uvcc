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
//#include <uvcc/utilities.h>

//#include <memory>
//#include <string>
//#include <type_traits>
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

class Endpoint {
 public:
  template <typename AddressType>
  class BaseIPAddress : protected BaseObject<AddressType, AnyRawSocketAddress> {
   public:
    BaseIPAddress(const AddressType &self)
        : BaseObject<AddressType, AnyRawSocketAddress>(self) {}
    BaseIPAddress(AddressType &&self) _NOEXCEPT
        : BaseObject<AddressType, AnyRawSocketAddress>(std::move(self)) {}
    BaseIPAddress(const BaseIPAddress &) = delete;
    BaseIPAddress(BaseIPAddress &&) _NOEXCEPT = default;
  };

  //  class IPAddress : protected BaseIPAddress<> {
  //   public:
  //    IPAddress(const IPAddress &) = delete;
  //    IPAddress &operator=(const IPAddress &) = delete;
  //    virtual ~IPAddress() = default;
  //  };

  class IPv4Address : protected BaseIPAddress<struct in_addr> {
   public:
    //    IPv4Address(const std::string &addr_str) : BaseIPAddress() {
    //    explicit IPv4Address(const std::string &addr_str) {
    //      uvcc::expr_throws(
    //          uv_inet_pton(AF_INET, addr_str.c_str(), &addr_->sin_addr));
    //        _someRaw()->sin_addr
    //      auto a6 = sockaddr_in6();
    //      a6.sin6_addr
    //    }
    //    IPv4Address(IPv4Address &&addr) _NOEXCEPT { addr_ =
    //    std::move(addr.addr_); }

    //    IPv4Address &operator=(IPv4Address &&addr) {
    //      addr_ = std::move(addr.addr_);
    //      return *this;
    //    }
    //    IPv4Address() = delete;
    explicit IPv4Address(const std::string &addr_str) {
      raw_->addr_4_ = decltype(raw_->addr_4_)();
      uvcc::expr_throws(uv_inet_pton(AF_INET, addr_str.c_str(), _someRaw()));
    }
    IPv4Address(Self &&self) _NOEXCEPT
        : BaseIPAddress<struct in_addr>(std::move(self)) {}
    //    IPv4Address(const Self &self)
    //        : BaseIPAddress<struct in_addr, AnyRawSocketAddress>(self) {}

    //    ~IPv4Address() {}

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

    //   private:
    //    Address *_sockaddr() const _NOEXCEPT override { return addr_.get(); }

    //    const Family *_family() const _NOEXCEPT override {
    //      return &addr_->sin_family;
    //    }
    //    RawIPv4Address *_sockaddr() const _NOEXCEPT {
    //      return reinterpret_cast<RawIPv4Address *>(storage_.get());
    //    }
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

  template <typename Address, typename RawAddress,
            typename std::enable_if<
                std::is_base_of<BaseIPAddress<RawAddress>, Address>::value,
                int>::type = 0>
  Endpoint(const Address &address, const Port &port) {
    //    switch (static_cast<IPAddress>(address).family()) {
    //      case AF_INET:
    //        uvcc::expr_throws(uv_ip4_addr());
    //      default:
    //        throw uvcc::Exception(UV_EAFNOSUPPORT);
    //    }
  }
};

}  // namespace network

}  // namespace uvcc

#endif  // NETWORK_H
