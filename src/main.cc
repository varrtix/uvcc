#include <uvcc/basic-fd.h>
#include <uvcc/core.h>
#include <uvcc/basic-req.h>
#include <uvcc/basic-stream.h>
#include <uvcc/event-loop.h>
#include <uvcc/exception.h>
#include <uvcc/utilities.h>

#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

// int main() {
//  auto loop = uvcc::EventLoop(std::move(*uv_default_loop()));
//    auto fd =
//    uvcc::FileDescriptor(uvcc::FileDescriptor::TransmitType::kUDP);

//    auto stream = uvcc::Stream();
//  auto ptr = uvcc::make_unique<uv_any_handle>();
//    *ptr = {.udp = uv_udp_t()};
//  ptr->udp = uv_udp_t();
//  uv_udp_init(uv_default_loop(), &ptr->udp);
//  auto get = reinterpret_cast<uv_handle_t *>(ptr.get());

//  std::cout << "handle type: " << uv_handle_type_name(get->type) <<
//  std::endl;

//  auto fd = uvcc::FileDescriptor(uvcc::FileDescriptor::TransmitType::kUDP);
//  std::cout << "size" << uv_handle_size(UV_UNKNOWN_HANDLE) << std::endl;

//  return 0;
//}

#define DEFAULT_PORT 7000
#define DEFAULT_BACKLOG 128

uv_loop_t *loop;
struct sockaddr_in addr;

typedef struct {
  uv_write_t req;
  uv_buf_t buf;
} write_req_t;

void free_write_req(uv_write_t *req) {
  write_req_t *wr = (write_req_t *)req;
  free(wr->buf.base);
  free(wr);
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  buf->base = (char *)malloc(suggested_size);
  buf->len = suggested_size;
}

void on_close(uv_handle_t *handle) { free(handle); }

void echo_write(uv_write_t *req, int status) {
  if (status) {
    fprintf(stderr, "Write error %s\n", uv_strerror(status));
  }
  free_write_req(req);
}

void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
  if (nread > 0) {
    write_req_t *req = (write_req_t *)malloc(sizeof(write_req_t));
    req->buf = uv_buf_init(buf->base, nread);
    uv_write((uv_write_t *)req, client, &req->buf, 1, echo_write);
    return;
  }
  if (nread < 0) {
    if (nread != UV_EOF) fprintf(stderr, "Read error %s\n", uv_err_name(nread));
    uv_close((uv_handle_t *)client, on_close);
  }

  free(buf->base);
}

void on_new_connection(uv_stream_t *server, int status) {
  if (status < 0) {
    fprintf(stderr, "New connection error %s\n", uv_strerror(status));
    // error!
    return;
  }

  uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
  uv_tcp_init(loop, client);
  if (uv_accept(server, (uv_stream_t *)client) == 0) {
    uv_read_start((uv_stream_t *)client, alloc_buffer, echo_read);
  } else {
    uv_close((uv_handle_t *)client, on_close);
  }
}

int main() {
  auto loop = uvcc::event_loop::standard();
  loop->run();
  loop->destroy();

//  auto any_h = new uv_any_handle();
  uv_any_handle any_h;
  any_h.tcp = uv_tcp_t();
  any_h.tcp.data = static_cast<void *>(loop);
//  tcp.data = static_cast<void *>(loop);
  uv_handle_t *h;
  h = reinterpret_cast<uv_handle_t *>(&any_h.tcp);
  uv_tcp_t *tcp;
  tcp = reinterpret_cast<uv_tcp_t *>(h);
  uv_tcp_init(uv_default_loop(), tcp);
  tcp->connection_cb = on_new_connection;
  uv_handle_t *h2;
  h2 = reinterpret_cast<uv_handle_t *>(tcp);
  uv_tcp_t *tcp2;
  tcp2 = reinterpret_cast<uv_tcp_t *>(h2);
  std::cout << tcp2 << std::endl;
//  std::string str;
  //  uv_tcp_t server;
  //  uv_tcp_init(loop, &server);
  //
  //  uv_ip4_addr("0.0.0.0", DEFAULT_PORT, &addr);
  //
  //  uv_tcp_bind(&server, (const struct sockaddr *)&addr, 0);
  //  int r = uv_listen((uv_stream_t *)&server, DEFAULT_BACKLOG,
  //  on_new_connection); if (r) {
  //    fprintf(stderr, "Listen error %s\n", uv_strerror(r));
  //    return 1;
  //  }
  //  return uv_run(, UV_RUN_DEFAULT);
  std::cout << "Hello world!" << std::endl;
  return 0;
}
