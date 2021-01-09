#ifndef UTILITIES_H
#define UTILITIES_H

#include <uv.h>

#include <memory>

#include "exception.h"

#define UV_FUNC_THROWS(expr, err) \
  if ((err = expr) < 0) throw uvcc::Exception(err);

namespace uvcc {

template <typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts&&... params) {
  return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

enum RunOption {
  kDefault = UV_RUN_DEFAULT,
  kOnce = UV_RUN_ONCE,
  kNoWait = UV_RUN_NOWAIT,
};

}  // namespace uvcc

#endif  // UTILITIES_H
