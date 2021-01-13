#ifndef STREAM_H
#define STREAM_H

#include <uv.h>

#include "handle.h"

namespace uvcc {

class Stream : public Handle {
 public:
  enum class Type : int {
    kDefault = UV_STREAM,
    kUnknown = UV_UNKNOWN_HANDLE,
    kTCP = UV_TCP,
    kTTY = UV_TTY,
    kNamedPipe = UV_NAMED_PIPE,
    kFile = UV_FILE,
    kHandleTypeMax = UV_HANDLE_TYPE_MAX,
  };

  explicit Stream() _NOEXCEPT { Stream::Type type = Stream::Type::kStream; }
};

}  // namespace uvcc

#endif  // STREAM_H
