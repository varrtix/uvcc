#ifndef HANDLE_H
#define HANDLE_H

#include <uv.h>
#include <uvcc/utilities.h>

#include <memory>

namespace uvcc {

class BaseHandleDelegate {
 public:
};

class BaseHandle {
  using Element = uv_handle_t;

 public:
  BaseHandle() _NOEXCEPT = default;
  virtual ~BaseHandle() _NOEXCEPT {}

 protected:
  virtual bool _isActive(const Element &element) const {
    //    return !UV_FUNC_SUCC_ASSERT(uv_is_active(&element));
    //    return !
    return
  }

  virtual bool _isClosing(const Element &element) const _NOEXCEPT {
    return !UV_FUNC_SUCC_ASSERT(uv_is_closing(&element));
  }

  //  virtual

 private:
  int err = 0;
};

class Handle {
 public:
  Handle() = default;
  virtual ~Handle() {}

  virtual bool isActive() const {
    return !UV_FUNC_SUCC_ASSERT(uv_is_active(context_.get()));
  }

  virtual bool isClosing() const {
    return !UV_FUNC_SUCC_ASSERT(uv_is_closing(context_.get()));
  }

 private:
  std::unique_ptr<uv_handle_t> context_;
  int err_;
};

}  // namespace uvcc

#endif  // HANDLE_H
