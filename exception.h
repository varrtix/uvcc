#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <uv.h>

#include <exception>

namespace uvcc {

class Exception : public std::exception {
 public:
  explicit Exception(const int &error_code) _NOEXCEPT
      : error_code_(error_code) {}

  const char *what() const _NOEXCEPT { return uv_strerror(error_code_); }

  virtual const char *code() const _NOEXCEPT {
    return uv_err_name(error_code_);
  }

  virtual const int &rawCode() const _NOEXCEPT { return error_code_; }

 private:
  int error_code_;
};

}  // namespace uvcc

#endif  // EXCEPTION_H
