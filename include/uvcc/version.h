#ifndef VERSION_H
#define VERSION_H

#include <uv.h>

#include <string>

namespace uvcc {

class Version {
 public:
  static const std::string description() _NOEXCEPT {
    return uv_version_string();
  }
};

}  // namespace uvcc

#endif  // VERSION_H
