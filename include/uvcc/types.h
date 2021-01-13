#ifndef TYPES_H
#define TYPES_H

#include <uv.h>

//#define HANDLE_TYPE_RENAMED_MAP(type)                                         \
//  type(ASYNC, Async) type(CHECK, Check) type(FS_EVENT, FSEvent)               \
//      type(FS_POLL, FSPoll) type(HANDLE, Handle) type(IDLE, IDLE)             \
//          type(NAMED_PIPE, NamedPipe) type(POLL, Poll) type(PREPARE, Prepare) \
//              type(PROCESS, Process) type(STREAM, Stream) type(TCP, TCP)      \
//                  type(TIMER, Timer) type(TTY, TTY) type(UDP, UDP)            \
//                      type(SIGNAL, Signal)

namespace uvcc {

// using HandleStorage = uv_any_handle;
// using RequestStorage = uv_any_req;

// typedef union handle_storage_t {
// Base
//  uv_handle_t handle_;

// Inherited by uv_handle_t
//  uv_udp_t udp_;
//  uv_poll_t poll_;
//  uv_async_t async_;
//  uv_idle_t idle_;
//  uv_check_t check_;
//  uv_prepare_t prepare_;
//  uv_timer_t timer_;
//  uv_process_t process_;
//  uv_fs_event_t fs_event_;
//  uv_fs_poll_t fs_poll_;
//  uv_stream_t stream_;
//  uv_signal_t signal_;
//  // Inherited by uv_stream_t
//  uv_tcp_t tcp_;
//  uv_tty_t tty_;
//  uv_pipe_t pipe_;

//} HandleStorage;

// typedef union request_storage_t {
// Base
// uv_req_t request_;

//// Inherited by uv_req_t
// uv_shutdown_t shutdown_;
// uv_write_t write_;
// uv_connect_t connect_;
// uv_udp_send_t udp_send_;
// uv_getaddrinfo_t get_addr_info_;
// uv_getnameinfo_t get_name_info_;
// uv_work_t work_;
// uv_fs_t fs_;
// uv_random_t random_;

//}
// RequestStorage;

}  // namespace uvcc

#endif  // TYPES_H
