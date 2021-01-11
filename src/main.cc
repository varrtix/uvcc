#include <iostream>

//#include "event-loop.h"
//#include "version.h"
#include <uvcc/event-loop.h>
#include <uvcc/version.h>

int main() {
  std::cout << "libuv version: " << uvcc::Version::description() << std::endl;

  auto loop = uvcc::EventLoop::standard();
  loop->run(uvcc::RunOption::kDefault);

  return 0;
}
