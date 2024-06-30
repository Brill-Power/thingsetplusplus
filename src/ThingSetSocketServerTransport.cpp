#include "ThingSetSocketServerTransport.hpp"
#include <sys/socket.h>

#define SOCKET_TRANSPORT_MAX_CONNECTIONS 10

ThingSet::ThingSetSocketServerTransport::ThingSetSocketServerTransport()
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);

    _address = (sockaddr_in){
        .sin_family = AF_INET,
        .sin_port = htons(9001),
        .sin_addr.s_addr = INADDR_ANY,
    };
    bind(_socket, (struct sockaddr *)&_address, sizeof(_address));
}

bool ThingSet::ThingSetSocketServerTransport::listen(
    std::function<std::pair<uint8_t *, size_t>(uint8_t *, size_t)> callback)
{
    return ::listen(_socket, SOCKET_TRANSPORT_MAX_CONNECTIONS) == 0;
}
