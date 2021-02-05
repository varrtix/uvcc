/// MIT License
///
/// uvcc/utilities.h
/// uvcc
///
/// created by varrtix on 2021/01/10.
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

#ifndef UTILITIES_H
#define UTILITIES_H

#include <uv.h>

#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <type_traits>

#include "exception.h"

namespace uvcc {

template <typename _Tp, typename... _Ts>
std::unique_ptr<_Tp> make_unique(_Ts &&...params) {
  return std::unique_ptr<_Tp>(new _Tp(std::forward<_Ts>(params)...));
}

template <typename _enumT,
          typename _underlyingT = typename std::underlying_type<_enumT>::type>
constexpr auto enum_cast(const _enumT &enum_case) _NOEXCEPT -> _underlyingT {
  static_assert(std::is_enum<_enumT>::value,
                "The '_enumT' should be an enum type.");
  return static_cast<_underlyingT>(enum_case);
}

template <typename _funcT>
struct completion_block {
 private:
  typedef typename std::conditional<std::is_pointer<_funcT>::value,
                                    typename std::remove_pointer<_funcT>::type,
                                    _funcT>::type _c_style_type;
  static_assert(std::is_function<_c_style_type>::value,
                "The '_classT' should be a function.");

 public:
  typedef _c_style_type c_type;
  typedef std::function<c_type> type;
};

template <typename _successT, typename _failureT = std::exception>
class result {
  static_assert(std::is_same<std::exception, _failureT>::value ||
                    std::is_base_of<std::exception, _failureT>::value,
                "The '_failureT' should be a std::exception or inherited by "
                "std::exception");

 public:
  struct type {
    typedef _successT success;
    typedef _failureT failure;
  };
  typedef typename type::success success_type;
  typedef typename type::failure failure_type;

  explicit result(std::function<std::unique_ptr<success_type>()>
                      &&catching_body) _NOEXCEPT {
    try {
      auto res = catching_body();
      success_.reset(res.release());
      failure_ = nullptr;
    } catch (const failure_type &failure) {
      failure_ = std::make_shared<failure_type>(failure);
      success_ = nullptr;
    }
  }
  result(const result &) = delete;
  result(result &&other) _NOEXCEPT { *this = std::move(other); }
  result &operator=(const result &) = delete;
  result &operator=(result &&other) _NOEXCEPT {
    if (this != &other) {
      this->success_ = std::move(other.success_);
      this->failure_ = std::move(other.failure_);
    }
    return *this;
  }

  bool as_success() const _NOEXCEPT {
    return success_ != nullptr && failure_ == nullptr;
  }

  bool as_failure() const _NOEXCEPT { return !as_success(); }

  const std::shared_ptr<success_type> success() const _NOEXCEPT {
    return success_;
  }

  const std::shared_ptr<failure_type> failure() const _NOEXCEPT {
    return failure_;
  }

 private:
  std::shared_ptr<success_type> success_;
  std::shared_ptr<failure_type> failure_;
};

struct expr {
  explicit expr(const int &err) _NOEXCEPT : expr(err, ops::eq) {}

  inline static expr eq(const int &err) _NOEXCEPT { return expr(err, ops::eq); }
  inline static expr ne(const int &err) _NOEXCEPT { return expr(err, ops::ne); }
  inline static expr gt(const int &err) _NOEXCEPT { return expr(err, ops::gt); }
  inline static expr lt(const int &err) _NOEXCEPT { return expr(err, ops::lt); }
  inline static expr ge(const int &err) _NOEXCEPT { return expr(err, ops::ge); }
  inline static expr le(const int &err) _NOEXCEPT { return expr(err, ops::le); }

  inline static expr equal(const int &err) _NOEXCEPT { return eq(err); }
  inline static expr not_equal(const int &err) _NOEXCEPT { return ne(err); }
  inline static expr greater_than(const int &err) _NOEXCEPT { return gt(err); }
  inline static expr less_than(const int &err) _NOEXCEPT { return lt(err); }
  inline static expr greater_than_or_equal(const int &err) _NOEXCEPT {
    return ge(err);
  }
  inline static expr less_than_or_equal(const int &err) _NOEXCEPT {
    return le(err);
  }

  inline bool eval() const _NOEXCEPT { return eval_; }

  inline bool failed() const _NOEXCEPT { return !eval_; };

  inline expr &throws() _NOEXCEPT(false) {
    if (failed() && calc(ops::ne, err_)) throw uvcc::exception(err_);
    return *this;
  }

  inline const std::string str() const _NOEXCEPT {
    return (failed() && calc(ops::ne, err_))
               ? uvcc::exception(err_).description()
               : "";
  }

 private:
  enum class ops : int {
    eq,
    ne,
    gt,
    lt,
    ge,
    le,
  };

  int err_;

  bool eval_;

  expr(const int &err, const ops &sign) _NOEXCEPT : err_(err),
                                                    eval_(calc(sign, err)) {}

  inline static bool calc(const ops &sign, const int &err) _NOEXCEPT {
    switch (sign) {
      case ops::eq:
        return err == 0;
      case ops::ne:
        return err != 0;
      case ops::gt:
        return err > 0;
      case ops::lt:
        return err < 0;
      case ops::ge:
        return err >= 0;
      case ops::le:
        return err <= 0;
      default:
        return false;
    }
  }
};

struct log {
  inline static log info(const void *addr = nullptr,
                         const char *des = nullptr) _NOEXCEPT {
    return log(lv::info, addr, des);
  }
  inline static log debug(const void *addr = nullptr,
                          const char *des = nullptr) _NOEXCEPT {
    return log(lv::debug, addr, des);
  }
  inline static log warning(const void *addr = nullptr,
                            const char *des = nullptr) _NOEXCEPT {
    return log(lv::warning, addr, des);
  }
  inline static log error(const void *addr = nullptr,
                          const char *des = nullptr) _NOEXCEPT {
    return log(lv::error, addr, des);
  }

  inline const std::string str() const _NOEXCEPT { return description_; }

  inline void print() const _NOEXCEPT { print(nullptr); }

  inline void print(const char *cmt) const _NOEXCEPT {
    ((level_ == lv::info || level_ == lv::debug) ? std::cout : std::cerr)
        << (cmt == nullptr ? description_
                           : (description_ + "\ndetails >> " + cmt))
        << std::endl;
  }

  inline void print(const std::string &cmt) const _NOEXCEPT {
    print(cmt.empty() ? nullptr : cmt.c_str());
  }

 private:
  enum class lv : int {
    info,
    debug,
    warning,
    error,
  };

  std::string description_;

  lv level_;

  log(const lv &level, const void *addr, const char *des) _NOEXCEPT
      : description_(format_str(level, addr, des)),
        level_(level) {}

  inline static const char *lv_str(const lv &level) _NOEXCEPT {
    switch (level) {
      case lv::info:
        return "info";
      case lv::debug:
        return "debug";
      case lv::warning:
        return "warning";
      case lv::error:
        return "error";
      default:
        return "";
    }
  }

  inline static std::string format_str(const lv &level, const void *addr,
                                       const char *des) _NOEXCEPT {
    std::stringstream s;
    (addr == nullptr ? s : s << "[" << addr << "] ")
        << "uvcc " << lv_str(level) << ": "
        << (des == nullptr ? "<unknown>" : des);
    return s.str();
  }
};

}  // namespace uvcc

#endif  // UTILITIES_H
