#ifndef UTILITIES_H
#define UTILITIES_H

#include <uv.h>

#include <memory>

#include "exception.h"

#define UV_FUNC_FAIL(code) ((code) < 0)
#define UV_FUNC_SUCC(code) ((code) == 0)
#define UV_FUNC_EXPR_ASSERT(result, flag) \
  flag ? UV_FUNC_SUCC(result) : UV_FUNC_FAIL(result)

//#define UV_FUNC_EXPR_ASSERT(expr, condi) ((expr)(condi))
//#define UV_FUNC_ERR_ASSERT(expr, condi, err) ((err = (expr))(condi))

//#define UV_FUNC_THROWS(expr, condi, err) \
//  if (UV_FUNC_ERR_ASSERT(expr, condi, err)) throw uvcc::Exception(err);

//#define UV_FUNC_FAIL_ASSERT(expr) UV_FUNC_EXPR_ASSERT(expr,
// UV_FUNC_FAIL_CONDI) #define UV_FUNC_SUCC_ASSERT(expr)
// UV_FUNC_EXPR_ASSERT(expr, UV_FUNC_SUCC_CONDI)

//#define UV_FUNC_FAIL_THROWS(expr, err) \
//  UV_FUNC_THROWS(expr, UV_FUNC_FAIL_CONDI, err)
//#define UV_FUNC_SUCC_THROWS(expr, err) \
//  UV_FUNC_THROWS(expr, UV_FUNC_SUCC_CONDI, err)

namespace uvcc {

template <typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts &&...params) {
  return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

inline static bool uv_expr_assert(const int &err,
                                  const bool &abs_succeed) _NOEXCEPT {
  return UV_FUNC_EXPR_ASSERT(err, abs_succeed);
}

inline static bool uv_eval_assert(const int &err, const bool &abs_succeed,
                                  int *res) _NOEXCEPT {
  if (res == nullptr) return false;
  *res = err;
  return uv_expr_assert(err, abs_succeed);
}

inline static void uv_expr_throws(const int &err) {
  if (uv_expr_assert(err, false)) throw uvcc::Exception(err);
}

enum LoopOption {
  kBlockSignal = UV_LOOP_BLOCK_SIGNAL,
  kMetricsIDLETime = UV_METRICS_IDLE_TIME,
};

enum RunOption {
  kDefault = UV_RUN_DEFAULT,
  kOnce = UV_RUN_ONCE,
  kNoWait = UV_RUN_NOWAIT,
};

}  // namespace uvcc

#undef UV_FUNC_FAIL
#undef UV_FUNC_SUCC
#undef UV_FUNC_EXPR_ASSERT

#endif  // UTILITIES_H
