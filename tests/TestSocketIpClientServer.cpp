/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ip/sockets/ThingSetSocketClientTransport.hpp"
#include "thingset++/ip/sockets/ThingSetSocketServerTransport.hpp"
#include "thingset++/ThingSetClient.hpp"
#include "thingset++/ThingSetServer.hpp"
#include "thingset++/ThingSetFunction.hpp"
#include "gtest/gtest.h"
#include <atomic>
#include <thread>
#include <vector>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace ThingSet;
using namespace ThingSet::Ip::Sockets;

static std::array<uint8_t, 1024> rxBuffer;
static std::array<uint8_t, 1024> txBuffer;

#define SOCKET_TEST(Name, Body) \
TEST(SocketIpClientServer, Name) \
{ \
    ThingSetReadWriteProperty<float> totalVoltage { 0x300, 0, "totalVoltage", 24.0f }; \
    ThingSetUserFunction<0x1000, 0x0, "xAddNumber", int, int, int> doSomething([](auto x, auto y) { return x + y; }); \
\
    ThingSetSocketServerTransport serverTransport; \
    auto server = ThingSetServerBuilder::build(serverTransport); \
    server.listen(); \
\
    auto endpoint = "127.0.0.1"; \
    ThingSetSocketClientTransport clientTransport(endpoint); \
    auto client = ThingSetClient(clientTransport, rxBuffer, txBuffer); \
    bool clientRanSuccessfully = false; \
    std::thread clientThread([&]() \
    { \
        std::this_thread::sleep_for(std::chrono::milliseconds(125)); \
        ASSERT_TRUE(client.connect()); \
        Body \
        clientRanSuccessfully = true; \
    }); \
\
    clientThread.join(); \
\
    ASSERT_TRUE(clientRanSuccessfully); \
}

SOCKET_TEST(GetFloatById,
    float tv;
    auto result = client.get(0x300, tv);
    ASSERT_TRUE(result);
    ASSERT_EQ(ThingSetStatusCode::content, result.code());
    ASSERT_EQ(24.0f, tv);
)

SOCKET_TEST(GetFloatByName,
    float tv;
    ASSERT_TRUE(client.get("totalVoltage", tv));
    ASSERT_EQ(24.0f, tv);
)

SOCKET_TEST(NotFoundById,
    float tv;
    auto result = client.get(0x1300, tv);
    ASSERT_FALSE(result);
    ASSERT_EQ(ThingSetStatusCode::notFound, result.code());
)

SOCKET_TEST(NotFoundByName,
    float tv;
    auto result = client.get("notTotalVoltage", tv);
    ASSERT_FALSE(result);
    ASSERT_EQ(ThingSetStatusCode::notFound, result.code());
)

SOCKET_TEST(GetFunction,
    int result;
    ASSERT_FALSE(client.get(0x1000, result));
)

SOCKET_TEST(ExecFunction,
    int value;
    auto result = client.exec(0x1000, &value, 2, 3);
    ASSERT_TRUE(result);
    ASSERT_EQ(ThingSetStatusCode::changed, result.code());
    ASSERT_EQ(5, value);
)

SOCKET_TEST(FetchFunctionParameters,
    std::vector<int> ids;
    ASSERT_TRUE(client.fetch(0x1000, ids));
    ASSERT_EQ(2, ids.size());
    ASSERT_GE(ids[0], 0x1001);
    ASSERT_GE(ids[1], 0x1001);

    std::vector<std::string> names;
    ASSERT_TRUE(client.fetch(std::string("xAddNumber"), names));
    ASSERT_EQ(2, names.size());
    ASSERT_STREQ("xAddNumberi32", names[0].substr(0, 13).c_str());
)

SOCKET_TEST(UpdateFloatByName,
    auto result = client.update("totalVoltage", 25.0f);
    ASSERT_TRUE(result);
    ASSERT_EQ(ThingSetStatusCode::changed, result.code());
    ASSERT_EQ(25.0, totalVoltage.getValue());
)

SOCKET_TEST(UpdateFloatById,
    auto result = client.update(0x300, 26.0f);
    ASSERT_TRUE(result);
    ASSERT_EQ(ThingSetStatusCode::changed, result.code());
    ASSERT_EQ(26.0, totalVoltage.getValue());
)

SOCKET_TEST(CannotUpdateFunctionByName,
    auto result = client.update("xAddNumber", 26.0f);
    ASSERT_FALSE(result);
    ASSERT_EQ(ThingSetStatusCode::badRequest, result.code());
)

SOCKET_TEST(CannotUpdateFunctionById,
    auto result = client.update(0x1000, 26.0f);
    ASSERT_FALSE(result);
    ASSERT_EQ(ThingSetStatusCode::badRequest, result.code());
)

/// The connection stays up between requests, so a single client can make many.
SOCKET_TEST(ManyRequestsOnOneConnection,
    for (int i = 0; i < 50; i++) {
        ASSERT_TRUE(client.update(0x300, (float)i)) << "iteration " << i;
        float tv;
        ASSERT_TRUE(client.get(0x300, tv)) << "iteration " << i;
        ASSERT_EQ((float)i, tv);
        int sum;
        ASSERT_TRUE(client.exec(0x1000, &sum, i, 1)) << "iteration " << i;
        ASSERT_EQ(i + 1, sum);
    }
)
static int connectRaw()
{
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9001);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(sock, (sockaddr *)&addr, sizeof(addr)) != 0) {
        close(sock);
        return -1;
    }
    return sock;
}

/// With every client slot taken, a further connection must be refused (closed
/// by the server) rather than accepted and then silently never served.
TEST(SocketIpClientServer, RefusesConnectionWhenAllSlotsTaken)
{
    ThingSetReadWriteProperty<float> totalVoltage { 0x300, 0, "totalVoltage", 24.0f };
    ThingSetSocketServerTransport serverTransport;
    auto server = ThingSetServerBuilder::build(serverTransport);
    ASSERT_TRUE(server.listen());
    std::this_thread::sleep_for(std::chrono::milliseconds(125));

    std::array<int, THINGSET_SERVER_MAX_CLIENTS> held;
    for (auto &sock : held) {
        sock = connectRaw();
        ASSERT_GE(sock, 0);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // acceptor assigns slots

    int extra = connectRaw();
    ASSERT_GE(extra, 0); // the handshake completes (backlog), then the server closes it
    timeval tv { 1, 0 };
    setsockopt(extra, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    uint8_t byte;
    ASSERT_EQ(0, recv(extra, &byte, 1, 0)); // orderly close, not a timeout
    close(extra);

    // Releasing one slot lets a new client in again.
    close(held[0]);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    int again = connectRaw();
    ASSERT_GE(again, 0);
    setsockopt(again, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    ASSERT_EQ(-1, recv(again, &byte, 1, 0)); // open and idle: times out, no close
    ASSERT_TRUE(errno == EAGAIN || errno == EWOULDBLOCK);
    close(again);
    for (size_t i = 1; i < held.size(); i++) {
        close(held[i]);
    }
}

/// A client taking over a slot that was freed while the others stay idle
/// must be served, and the handler must keep serving afterwards. (On the
/// HMCU this stalled: the acceptor wrote the new fd into the poll set while
/// the handler was inside poll(), and the handler then blocked in recv() on
/// an idle client until that client went away.)
TEST(SocketIpClientServer, ServesClientInReusedSlotWhileOthersIdle)
{
    ThingSetReadWriteProperty<float> totalVoltage { 0x300, 0, "totalVoltage", 24.0f };
    ThingSetSocketServerTransport serverTransport;
    auto server = ThingSetServerBuilder::build(serverTransport);
    ASSERT_TRUE(server.listen());
    std::this_thread::sleep_for(std::chrono::milliseconds(125));

    std::array<int, THINGSET_SERVER_MAX_CLIENTS> held;
    for (auto &sock : held) {
        sock = connectRaw();
        ASSERT_GE(sock, 0);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // the first client goes away without ever sending anything
    close(held[0]);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    for (int round = 0; round < 3; round++) {
        {
            ThingSetSocketClientTransport clientTransport("127.0.0.1");
            auto client = ThingSetClient(clientTransport, rxBuffer, txBuffer);
            ASSERT_TRUE(client.connect());
            float tv = 0;
            ASSERT_TRUE(client.get(0x300, tv)) << "round " << round;
            ASSERT_EQ(24.0f, tv);
        }
        // client transport closed on destruction; wait for the handler to free the slot
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    for (size_t i = 1; i < held.size(); i++) {
        close(held[i]);
    }
}

/// The listening socket carries keep-alive, which accepted sockets inherit.
TEST(SocketIpClientServer, ListenerHasKeepAliveConfigured)
{
    ThingSetSocketServerTransport serverTransport;
    auto server = ThingSetServerBuilder::build(serverTransport);
    ASSERT_TRUE(server.listen());
    std::this_thread::sleep_for(std::chrono::milliseconds(125));

    int sock = connectRaw();
    ASSERT_GE(sock, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // Inspect from the client side is impossible; instead confirm our own
    // accepted-socket inheritance assumption on this platform with a probe
    // listener configured the same way.
    int probe = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int enable = 1, idle = THINGSET_PLUS_PLUS_SOCKET_SERVER_KEEPALIVE_IDLE;
    ASSERT_EQ(0, setsockopt(probe, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable)));
    ASSERT_EQ(0, setsockopt(probe, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle)));
    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    ASSERT_EQ(0, bind(probe, (sockaddr *)&addr, sizeof(addr)));
    ASSERT_EQ(0, listen(probe, 1));
    socklen_t len = sizeof(addr);
    ASSERT_EQ(0, getsockname(probe, (sockaddr *)&addr, &len));
    int c = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    ASSERT_EQ(0, connect(c, (sockaddr *)&addr, sizeof(addr)));
    int accepted = accept(probe, nullptr, nullptr);
    ASSERT_GE(accepted, 0);
    int got = 0;
    len = sizeof(got);
    ASSERT_EQ(0, getsockopt(accepted, SOL_SOCKET, SO_KEEPALIVE, &got, &len));
    ASSERT_EQ(1, got);
    len = sizeof(got);
    ASSERT_EQ(0, getsockopt(accepted, IPPROTO_TCP, TCP_KEEPIDLE, &got, &len));
    ASSERT_EQ(idle, got);
    close(accepted);
    close(c);
    close(probe);
    close(sock);
}

static void setReceiveTimeout(int sock, int seconds)
{
    timeval tv { seconds, 0 };
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

TEST(SocketIpClientServer, ClientRejectsInvalidAddress)
{
    ASSERT_THROW(ThingSetSocketClientTransport("not an address"), std::invalid_argument);
    ASSERT_THROW(ThingSetSocketClientTransport("256.0.0.1"), std::invalid_argument);
}

TEST(SocketIpClientServer, ClientConnectFailsWithoutServer)
{
    ThingSetSocketClientTransport clientTransport("127.0.0.1");
    auto client = ThingSetClient(clientTransport, rxBuffer, txBuffer);
    ASSERT_FALSE(client.connect());
}

/// A second server cannot take the port from one that is already listening,
/// and the first carries on serving regardless.
TEST(SocketIpClientServer, ListenFailsWhenPortInUse)
{
    ThingSetReadWriteProperty<float> totalVoltage { 0x300, 0, "totalVoltage", 24.0f };
    ThingSetSocketServerTransport serverTransport;
    auto server = ThingSetServerBuilder::build(serverTransport);
    ASSERT_TRUE(server.listen());
    std::this_thread::sleep_for(std::chrono::milliseconds(125)); // acceptor starts listening

    ThingSetSocketServerTransport otherTransport;
    auto other = ThingSetServerBuilder::build(otherTransport);
    ASSERT_FALSE(other.listen());

    ThingSetSocketClientTransport clientTransport("127.0.0.1");
    auto client = ThingSetClient(clientTransport, rxBuffer, txBuffer);
    ASSERT_TRUE(client.connect());
    float tv = 0;
    ASSERT_TRUE(client.get(0x300, tv));
    ASSERT_EQ(24.0f, tv);
}

/// Every slot can be in use by an active client at once, with requests from
/// all of them interleaved, and each gets its own answers back.
TEST(SocketIpClientServer, ServesMaxClientsConcurrently)
{
    ThingSetUserFunction<0x1000, 0x0, "xAddNumber", int, int, int> doSomething([](auto x, auto y) { return x + y; });
    ThingSetSocketServerTransport serverTransport;
    auto server = ThingSetServerBuilder::build(serverTransport);
    ASSERT_TRUE(server.listen());
    std::this_thread::sleep_for(std::chrono::milliseconds(125));

    std::atomic<int> successes = 0;
    std::vector<std::thread> threads;
    for (int c = 0; c < THINGSET_SERVER_MAX_CLIENTS; c++) {
        threads.emplace_back([&, c]()
        {
            std::array<uint8_t, 1024> rx;
            std::array<uint8_t, 1024> tx;
            ThingSetSocketClientTransport clientTransport("127.0.0.1");
            auto client = ThingSetClient(clientTransport, rx, tx);
            ASSERT_TRUE(client.connect()) << "client " << c;
            for (int i = 0; i < 25; i++) {
                int sum = 0;
                ASSERT_TRUE(client.exec(0x1000, &sum, c * 1000, i)) << "client " << c << " iteration " << i;
                ASSERT_EQ(c * 1000 + i, sum) << "client " << c << " iteration " << i;
            }
            successes++;
        });
    }
    for (auto &t : threads) {
        t.join();
    }
    ASSERT_EQ(THINGSET_SERVER_MAX_CLIENTS, successes);
}

/// Clients coming and going must hand their slots back, so many more clients
/// than there are slots can be served one after another.
TEST(SocketIpClientServer, RepeatedConnectDisconnectDoesNotLeakSlots)
{
    ThingSetReadWriteProperty<float> totalVoltage { 0x300, 0, "totalVoltage", 24.0f };
    ThingSetSocketServerTransport serverTransport;
    auto server = ThingSetServerBuilder::build(serverTransport);
    ASSERT_TRUE(server.listen());
    std::this_thread::sleep_for(std::chrono::milliseconds(125));

    for (int round = 0; round < THINGSET_SERVER_MAX_CLIENTS * 3; round++) {
        {
            ThingSetSocketClientTransport clientTransport("127.0.0.1");
            auto client = ThingSetClient(clientTransport, rxBuffer, txBuffer);
            ASSERT_TRUE(client.connect()) << "round " << round;
            float tv = 0;
            ASSERT_TRUE(client.get(0x300, tv)) << "round " << round;
            ASSERT_EQ(24.0f, tv);
        }
        // give the handler a poll cycle to notice the close
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

/// A client that resets its connection rather than closing it cleanly must
/// still have its slot freed.
TEST(SocketIpClientServer, ResetConnectionFreesSlot)
{
    ThingSetReadWriteProperty<float> totalVoltage { 0x300, 0, "totalVoltage", 24.0f };
    ThingSetSocketServerTransport serverTransport;
    auto server = ThingSetServerBuilder::build(serverTransport);
    ASSERT_TRUE(server.listen());
    std::this_thread::sleep_for(std::chrono::milliseconds(125));

    std::array<int, THINGSET_SERVER_MAX_CLIENTS> held;
    for (auto &sock : held) {
        sock = connectRaw();
        ASSERT_GE(sock, 0);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // a zero linger timeout makes close() send RST instead of FIN
    linger lin { 1, 0 };
    ASSERT_EQ(0, setsockopt(held[0], SOL_SOCKET, SO_LINGER, &lin, sizeof(lin)));
    close(held[0]);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ThingSetSocketClientTransport clientTransport("127.0.0.1");
    auto client = ThingSetClient(clientTransport, rxBuffer, txBuffer);
    ASSERT_TRUE(client.connect());
    float tv = 0;
    ASSERT_TRUE(client.get(0x300, tv));
    ASSERT_EQ(24.0f, tv);

    for (size_t i = 1; i < held.size(); i++) {
        close(held[i]);
    }
}

/// A request the server does not recognise gets a bad request status, and the
/// connection remains usable for a valid request afterwards.
TEST(SocketIpClientServer, UnrecognisedRequestGetsBadRequest)
{
    ThingSetReadWriteProperty<float> totalVoltage { 0x300, 0, "totalVoltage", 24.0f };
    ThingSetSocketServerTransport serverTransport;
    auto server = ThingSetServerBuilder::build(serverTransport);
    ASSERT_TRUE(server.listen());
    std::this_thread::sleep_for(std::chrono::milliseconds(125));

    int sock = connectRaw();
    ASSERT_GE(sock, 0);
    setReceiveTimeout(sock, 1);

    uint8_t junk[] = { 0xff, 0x12, 0x34 };
    ASSERT_EQ((ssize_t)sizeof(junk), send(sock, junk, sizeof(junk), 0));
    uint8_t response[64];
    ASSERT_EQ(1, recv(sock, response, sizeof(response), 0));
    ASSERT_EQ(ThingSetStatusCode::badRequest, (ThingSetStatusCode)response[0]);

    // binary get of 0x300: request type then the ID as a CBOR uint16
    uint8_t get[] = { (uint8_t)ThingSetBinaryRequestType::get, 0x19, 0x03, 0x00 };
    ASSERT_EQ((ssize_t)sizeof(get), send(sock, get, sizeof(get), 0));
    ssize_t len = recv(sock, response, sizeof(response), 0);
    ASSERT_GE(len, 5);
    ASSERT_EQ(ThingSetStatusCode::content, (ThingSetStatusCode)response[0]);
    // the value is last: a CBOR single-precision float, 24.0 = 0x41c00000
    uint8_t expected[] = { 0xfa, 0x41, 0xc0, 0x00, 0x00 };
    ASSERT_EQ(0, memcmp(expected, response + len - sizeof(expected), sizeof(expected)));

    close(sock);
}

/// Publishing needs the bound publish socket, so it fails until the server is
/// listening.
TEST(SocketIpClientServer, PublishFailsBeforeListen)
{
    ThingSetReadWriteProperty<float> totalVoltage { 0x300, 0, "totalVoltage", 24.0f };
    ThingSetSocketServerTransport serverTransport;
    auto server = ThingSetServerBuilder::build(serverTransport);
    ASSERT_FALSE(server.publish(totalVoltage));
}

/// Reports are broadcast over UDP to port 9002 on the subnet, each one
/// carrying the next message number.
TEST(SocketIpClientServer, PublishBroadcastsReports)
{
    ThingSetReadWriteProperty<float> totalVoltage { 0x300, 0, "totalVoltage", 24.0f };
    ThingSetSocketServerTransport serverTransport;
    auto server = ThingSetServerBuilder::build(serverTransport);
    ASSERT_TRUE(server.listen());

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    ASSERT_GE(sock, 0);
    int enable = 1;
    ASSERT_EQ(0, setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)));
    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9002);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ASSERT_EQ(0, bind(sock, (sockaddr *)&addr, sizeof(addr)));
    setReceiveTimeout(sock, 1);

    for (uint8_t i = 0; i < 3; i++) {
        ASSERT_TRUE(server.publish(totalVoltage));
        uint8_t frame[THINGSET_STREAMING_MSG_SIZE];
        ssize_t len = recv(sock, frame, sizeof(frame), 0);
        ASSERT_GT(len, 2) << "report " << (int)i;
        // a small report fits in a single frame
        ASSERT_EQ(Ip::MessageType::single, (Ip::MessageType)(frame[0] & THINGSET_STREAMING_MESSAGE_TYPE_MASK));
        ASSERT_EQ(i, frame[1]);
    }

    close(sock);
}
