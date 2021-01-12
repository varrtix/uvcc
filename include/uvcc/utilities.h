#ifndef UTILITIES_H
#define UTILITIES_H

#include <uv.h>

#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>

#include "exception.h"

namespace uvcc {

template <typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts &&...params) {
  return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

template <typename EnumType,
          typename std::enable_if<std::is_enum<EnumType>::value, int>::type = 0>
constexpr auto enum_cast(const EnumType &enum_case) ->
    typename std::underlying_type<EnumType>::type {
  return static_cast<typename std::underlying_type<EnumType>::type>(enum_case);
}

template <
    typename Success, typename Failure,
    typename std::enable_if<std::is_same<std::exception, Failure>::value ||
                                std::is_base_of<std::exception, Failure>::value,
                            int>::type = 0>
class Result {
 public:
  enum class Type : bool {
    kSuccess = true,
    kFailure = false,
  };

  explicit Result(std::function<Success()> &&catching_body) _NOEXCEPT {
    try {
      success_ = uvcc::make_unique<Success>(catching_body());
      type_ = Type::kSuccess;
    } catch (const Failure &failure) {
      failure_ = uvcc::make_unique<Failure>(failure);
    }
  }

  inline bool as(const Result::Type &type) const _NOEXCEPT {
    return type == type_;
  }

  inline bool asSuccess() const _NOEXCEPT { return uvcc::enum_cast(type_); }

  inline bool asFailure() const _NOEXCEPT { return !uvcc::enum_cast(type_); }

  const std::unique_ptr<Success> &success() const _NOEXCEPT { return success_; }

  const std::unique_ptr<Failure> &failure() const _NOEXCEPT { return failure_; }

 private:
  std::unique_ptr<Success> success_ = nullptr;
  std::unique_ptr<Failure> failure_ = nullptr;

  Result::Type type_ = Type::kFailure;
};

inline bool expr_assert(const int &err, const bool &abs = false) _NOEXCEPT {
  return abs ? (err == 0) : (err < 0);
}

inline void expr_cerr(const uvcc::Exception &exception) _NOEXCEPT {
  std::cerr << exception.what() << std::endl;
}

inline bool expr_cerr(const int &err, const bool &abs = false) _NOEXCEPT {
  if (expr_assert(err, abs)) return true;
  expr_cerr(uvcc::Exception(err));
  return false;
}

inline void expr_throws(const int &err, const bool &abs = false) {
  if (!expr_assert(err, abs)) throw uvcc::Exception(err);
}

enum class LoopOption : int {
  kBlockSignal = UV_LOOP_BLOCK_SIGNAL,
  kMetricsIDLETime = UV_METRICS_IDLE_TIME,
};

enum class RunOption : int {
  kDefault = UV_RUN_DEFAULT,
  kOnce = UV_RUN_ONCE,
  kNoWait = UV_RUN_NOWAIT,
};

}  // namespace uvcc

#endif  // UTILITIES_H
