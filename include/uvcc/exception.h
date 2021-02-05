/// MIT License
///
/// uvcc/exception.h
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

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <uv.h>

#include <exception>
#include <string>

namespace uvcc {

class basic_exception : virtual public std::exception {
 public:
  explicit basic_exception(const int &error_code) _NOEXCEPT
      : error_code_(error_code) {}

  const int &code() const _NOEXCEPT { return error_code_; }

  virtual const char *what() const _NOEXCEPT { return what_.c_str(); }

  virtual const char *description() const _NOEXCEPT {
    return description_.c_str();
  }

  virtual const char *abbr() const _NOEXCEPT = 0;

 protected:
  int error_code_;
  std::string what_;
  std::string description_;
};

class exception final : public basic_exception {
 public:
  explicit exception(const int &error_code) _NOEXCEPT
      : basic_exception(error_code) {
    what_ = uv_strerror(error_code);
    description_ = std::string(abbr()) + ": " + what();
  }

  const char *abbr() const _NOEXCEPT { return uv_err_name(error_code_); }
};

}  // namespace uvcc

#endif  // EXCEPTION_H
