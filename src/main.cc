#include <uvcc/event-loop.h>
#include <uvcc/file-descriptor.h>
#include <uvcc/stream.h>
#include <uvcc/utilities.h>

#include <iostream>
#include <string>

int main() {
  //  auto loop = uvcc::EventLoop(std::move(*uv_default_loop()));
//    auto fd = uvcc::FileDescriptor(uvcc::FileDescriptor::TransmitType::kUDP);
    
//    auto stream = uvcc::Stream();
    auto ptr = uvcc::make_unique<uv_any_handle>();
//    *ptr = {.udp = uv_udp_t()};
    ptr->udp = uv_udp_t();
    uv_udp_init(uv_default_loop(), &ptr->udp);
    auto get = reinterpret_cast<uv_handle_t *>(ptr.get());

    std::cout << "handle type: " << uv_handle_type_name(get->type) << std::endl;

  return 0;
}
