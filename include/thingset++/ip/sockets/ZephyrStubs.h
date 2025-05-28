/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

// Zephyr provides a config flag (CONFIG_POSIX_API) to provide POSIX-compatible
// method signatures for sockets, but this also imports other APIs (e.g. pthreads)
// which conflict with other definitions (at least when building for native_sim).
// For now, we create our own limited set of stubs here.

#if defined(__ZEPHYR__)

#include <zephyr/posix/poll.h>

#ifdef __cplusplus
extern "C" {
#endif

inline int accept(int sock, struct sockaddr *addr, socklen_t *addrlen) {
    return zsock_accept(sock, addr, addrlen);
}

inline int bind(int sock, const struct sockaddr *addr, socklen_t addrlen) {
    return zsock_bind(sock, addr, addrlen);
}

inline int connect(int sock, const struct sockaddr *addr, socklen_t addrlen) {
    return zsock_connect(sock, addr, addrlen);
}

inline int getsockname(int sock, struct sockaddr *addr, socklen_t *addrlen) {
    return zsock_getsockname(sock, addr, addrlen);
}

inline int getpeername(int sock, struct sockaddr *addr, socklen_t *addrlen) {
    return zsock_getpeername(sock, addr, addrlen);
}

inline int getsockopt(int sock, int level, int optname, void *optval, socklen_t *optlen) {
    return zsock_getsockopt(sock, level, optname, optval, optlen);
}

inline int listen(int sock, int backlog) {
    return zsock_listen(sock, backlog);
}

inline ssize_t recv(int sock, void *buf, size_t max_len, int flags) {
    return zsock_recv(sock, buf, max_len, flags);
}

inline ssize_t recvfrom(int sock, void *buf, size_t max_len, int flags, struct sockaddr *src_addr,
		                socklen_t *addrlen) {
    return zsock_recvfrom(sock, buf, max_len, flags, src_addr, addrlen);
}

inline ssize_t send(int sock, const void *buf, size_t len, int flags) {
    return zsock_send(sock, buf, len, flags);
}

inline ssize_t sendto(int sock, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr,
	                  socklen_t addrlen) {
    return zsock_sendto(sock, buf, len, flags, dest_addr, addrlen);
}

inline int setsockopt(int sock, int level, int optname, const void *optval, socklen_t optlen) {
    return zsock_setsockopt(sock, level, optname, optval, optlen);
}

inline int socket(int family, int type, int proto) {
    return zsock_socket(family, type, proto);
}

inline int poll(struct pollfd *fds, int nfds, int timeout) {
    return zsock_poll(fds, nfds, timeout);
}

inline int inet_pton(sa_family_t family, const char *src, void *dst)
{
    return zsock_inet_pton(family, src, dst);
}

inline char *inet_ntop(sa_family_t family, const void *src, char *dst, size_t size)
{
    return zsock_inet_ntop(family, src, dst, size);
}

inline int close(int file) {
    return zsock_close(file);
}

#ifdef __cplusplus
}
#endif

#endif