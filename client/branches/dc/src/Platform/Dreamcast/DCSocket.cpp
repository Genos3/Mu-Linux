// DCSocket.cpp - ISocket implementation for KOS's native network stack.
//
// Direct port of the non-Windows half of Platform/SDL3/SDLSocket.cpp:
//   * BSD sockets, O_NONBLOCK via fcntl()
//   * select()-based readiness mask fed to the SocketEventSink, producing the
//     same FD_READ/FD_WRITE/FD_CLOSE/FD_CONNECT codes the Win32 path delivers
//   * errno -> 100xx table preserved verbatim (MU's retry policy depends on it)
// KOS API used: sys/socket.h, sys/select.h, sys/ioctl.h, netdb.h (getaddrinfo),
// arpa/inet.h (htons). Init with KOS_INIT_FLAGS(... | INIT_NET) (DCInit.cpp) and
// a connected BBA / LAN / W5500 adapter; otherwise CreateTcp fails with
// SocketWouldBlock and the client stays offline.

#include "Platform/ISocket.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>

#ifndef FD_SETSIZE
#define FD_SETSIZE 64   // newlib default; KOS does not redefine it
#endif

namespace Platform {
namespace {
using NativeSocket = int;
using Length = socklen_t;
int Error() {
    switch (errno) {
    case EAGAIN: return SocketWouldBlock;
#if EWOULDBLOCK != EAGAIN
    case EWOULDBLOCK: return SocketWouldBlock;
#endif
    case EINPROGRESS: case EALREADY: return SocketWouldBlock;
    case EINTR: return 10004;
    case ECONNREFUSED: return 10061;
    case ETIMEDOUT: return 10060;
    case ECONNRESET: case EPIPE: return 10054;
    case ENOTCONN: return 10057;
    case EBADF: return 10038;
    default: return 10022;
    }
}
int CloseNative(NativeSocket s) { return ::close(s); }
bool NonBlocking(NativeSocket s) { const int flags = fcntl(s, F_GETFL, 0); return flags >= 0 && fcntl(s, F_SETFL, flags | O_NONBLOCK) == 0; }

class DCSocket final : public ISocket {
    enum class State { Idle, Connecting, Connected, Failed, Closed };
    SocketHandle active = InvalidSocket;
    State state = State::Idle;
    SocketEventSink sink = nullptr;
    uint64_t generation = 0; // the redirect may close and reuse the same descriptor inside READ
    int error = 0, failure = 0, mask = 0;
    bool writeWanted = false;
    NativeSocket Native() const { return static_cast<NativeSocket>(active); }
    void Fail(int reason) { failure = reason; state = State::Failed; }
public:
    bool IsOpen(SocketHandle socket) const override { return socket != InvalidSocket; }
    void BindEvents(SocketEventSink value) override { sink = value; }
    bool IsConnected(SocketHandle s) const override { return s == active && state == State::Connected; }
    int Startup(uint16_t requested, uint16_t& actual) override { actual = requested; return 0; }
    int Cleanup() override { return 0; }
    SocketHandle CreateTcp(int protocol) override {
        const NativeSocket s = socket(AF_INET, SOCK_STREAM, protocol);
        if (s == -1) { error = Error(); return InvalidSocket; }
        if (!NonBlocking(s)) { error = Error(); CloseNative(s); return InvalidSocket; }
        if (s >= FD_SETSIZE) { CloseNative(s); error = 10024; return InvalidSocket; }
        active = static_cast<SocketHandle>(s); ++generation;
        state = State::Idle; failure = 0; mask = 0; writeWanted = true;
        return active;
    }
    bool ResolveIPv4(const char* name, uint32_t& address) override {
        addrinfo hints{}; hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM;
        addrinfo* result = nullptr;
        if (getaddrinfo(name, nullptr, &hints, &result) != 0 || !result) { error = 11001; return false; }
        address = reinterpret_cast<sockaddr_in*>(result->ai_addr)->sin_addr.s_addr;
        freeaddrinfo(result); return true;
    }
    int ConnectIPv4(SocketHandle s, uint32_t address, uint16_t port) override {
        if (s != active) { error = 10038; return -1; }
        sockaddr_in target{}; target.sin_family = AF_INET;
        target.sin_addr.s_addr = address; target.sin_port = htons(port);
        state = State::Connecting;
        const int result = connect(Native(), reinterpret_cast<sockaddr*>(&target), sizeof(target));
        if (result < 0) { const int code = Error(); if (code != SocketWouldBlock) Fail(code); }
        // A synchronous failure is still reported through CLOSE in Poll, before any send.
        error = SocketWouldBlock;
        return -1;
    }
    int NotifyWindow(SocketHandle s, void*, uint32_t, int32_t flags) override {
        if (s != active) { error = 10038; return -1; }
        mask = flags; return 0;
    }
    int Send(SocketHandle s, const char* data, int size, int flags) override {
        if (!IsConnected(s)) { error = SocketWouldBlock; writeWanted = true; return -1; }
        const int result = static_cast<int>(::send(Native(), data, size, flags));
        if (result < 0) {
            error = Error();
            if (error == SocketWouldBlock) writeWanted = true;
        }
        return result;
    }
    int Receive(SocketHandle s, char* data, int size, int flags) override {
        if (!IsConnected(s)) { error = SocketWouldBlock; return -1; }
        const int result = static_cast<int>(recv(Native(), data, size, flags));
        if (result < 0) error = Error();
        return result;
    }
    int Close(SocketHandle s) override {
        if (s == active) { active = InvalidSocket; state = State::Idle; ++generation; }
        const int result = CloseNative(static_cast<NativeSocket>(s));
        if (result < 0) error = Error();
        return result;
    }
    int LastError() override { return error; }
    void Poll() override {
        if (active == InvalidSocket || !sink || state == State::Idle || state == State::Closed) return;
        const auto stamp = generation;
        bool closing = state == State::Failed;
        fd_set reads, writes, errors;
        FD_ZERO(&reads); FD_ZERO(&writes); FD_ZERO(&errors);
        FD_SET(Native(), &reads); FD_SET(Native(), &writes); FD_SET(Native(), &errors);
        timeval timeout{};
        const int ready = closing ? 0 : select(static_cast<int>(Native()) + 1, &reads, &writes, &errors, &timeout);
        if (ready < 0) { const int code = Error(); if (code == 10004) return; Fail(code); closing = true; }
        if (state == State::Connecting && ready > 0 && (FD_ISSET(Native(), &writes) || FD_ISSET(Native(), &errors))) {
            int code = 0; Length length = sizeof(code);
            if (getsockopt(Native(), SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&code), &length) != 0) code = Error();
            else if (code) { errno = code; code = Error(); }
            if (code) { Fail(code); closing = true; }
            else if (FD_ISSET(Native(), &writes)) state = State::Connected;
        }
        if (state == State::Connected && ready > 0 && FD_ISSET(Native(), &reads)) {
            char byte;
            const int available = static_cast<int>(recv(Native(), &byte, 1, MSG_PEEK));
            if (available > 0 && (mask & 1)) sink(1, 0);
            else if (available == 0) closing = true;
            else if (available < 0 && Error() != SocketWouldBlock) { Fail(Error()); closing = true; }
            if (generation != stamp) return;
        }
        // Original dispatcher order: READ, WRITE, CLOSE. Do not group events:
        // READ may open another socket during a redirect or re-enter OpenWorld.
        if (!closing && state == State::Connected && ready > 0 && FD_ISSET(Native(), &writes) && writeWanted && (mask & 2)) {
            writeWanted = false; sink(2, 0);
            if (generation != stamp) return;
        }
        if (closing && (mask & 32)) { state = State::Closed; sink(32, static_cast<uint16_t>(failure)); }
    }
};
}
ISocket& GetSocket() { static auto* socket = new DCSocket; return *socket; }
}
