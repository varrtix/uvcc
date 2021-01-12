#include <iostream>

//#include "event-loop.h"
//#include "version.h"
#include <uvcc/event-loop.h>
#include <uvcc/utilities.h>
#include <uvcc/version.h>

int test_func(const int &err) {
  if (err < 0) throw uvcc::Exception(err);
  return err;
}

int main() {
  std::cout << "libuv version: " << uvcc::Version::description() << std::endl;

  //  auto loop = uvcc::EventLoop::standard();
  //  loop->run(uvcc::RunOption::kDefault);

  auto result =
      uvcc::Result<int, uvcc::Exception>([] { return test_func(-2); });
  if (result.asFailure()) std::cerr << result.failure()->what() << std::endl;

  return 0;
}
