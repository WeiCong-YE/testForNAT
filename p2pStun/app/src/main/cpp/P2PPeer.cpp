//
// Created by ouyangjie on 2018/7/27.
//

#include <unistd.h>
#include <netdb.h>
#include "appLog.h"
#include "P2PPeer.h"
#include "bind.h"

P2PPeer* P2PPeer::m_instance = nullptr;
P2PPeer::P2PPeer():m_fd(-1), m_oppositeNet{0}
{}

P2PPeer::~P2PPeer()
{
    if (m_fd > 0)
    {
        close(m_fd);
    }
}

int P2PPeer::convertLocalNet(const struct sockaddr *addr, socklen_t addrlen)
{
    char hostbuf[NI_MAXHOST], servbuf[NI_MAXSERV];

    int val = getnameinfo (addr, addrlen, hostbuf, sizeof (hostbuf),
                           servbuf, sizeof (servbuf),
                           NI_NUMERICHOST | NI_NUMERICSERV);
    if (val)
    {
        LOGE ("getnameinfo failed, ret:%d, %s", val, gai_strerror (val));
        return val;
    }
    else
    {
        m_localIp.assign(hostbuf);
        m_localPort.assign(servbuf);
        LOGD ("%s port %s", hostbuf, servbuf);
        return 0;
    }
}

int P2PPeer::getLocatIpAndPort(const char* serverIp, const char* port,
                                       std::string& localIp, std::string& localPort)
{
    if (m_fd > 0)
    {
        close(m_fd);
        m_fd = -1;
    }

    struct addrinfo hints, *res;
    const struct addrinfo *ptr;
    int ret = -1;

    memset (&hints, 0, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = 0;
    if (port == NULL)
        port = "3478";
    LOGW("STUN 服务器ip:%s, 端口:%s", serverIp, port);
    ret = getaddrinfo (serverIp, port, &hints, &res);
    if (ret)
    {
        LOGE("%s (port %s): %s", serverIp, port, gai_strerror (ret));
        return ret;
    }

    for (ptr = res; ptr != NULL; ptr = ptr->ai_next)
    {
        union {
            struct sockaddr_storage storage;
            struct sockaddr addr;
        } addr;
        socklen_t addrlen = sizeof (addr);
        StunUsageBindReturn val;

        val = MG_stun_usage_bind_run (ptr->ai_addr, ptr->ai_addrlen, &addr.storage,
                                      &addrlen, &m_fd);
        if (val)
            LOGE ("stun_usage_bind_run ret:%d", val);
        else
        {
            convertLocalNet(&addr.addr, addrlen);
            ret = 0;
        }
    }

    freeaddrinfo (res);
    return ret;
}

int P2PPeer::setOppositeNet(const char* ip, const char* port)
{
    struct addrinfo hints = {0}, *res;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = 0;

    LOGW("STUN 服务器ip:%s, 端口:%s", ip, port);
    int ret = getaddrinfo (ip, port, &hints, &res);
    if (ret)
    {
        LOGE("%s (port %s): %s", ip, port,
             gai_strerror (ret));
        return -1;
    }

    m_oppositeNet = *res->ai_addr;
    freeaddrinfo (res);
    return ret;
}

int P2PPeer::sendDataToOpposite(std::string data)
{
    if (m_fd < 0)
    {
        LOGE("未打开socket");
    }
    return sendto (m_fd, (void *)data.c_str(), data.length(),
                   MSG_DONTWAIT | MSG_NOSIGNAL,
                   &m_oppositeNet, sizeof(m_oppositeNet));
}