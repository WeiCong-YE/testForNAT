//
// Created by ouyangjie on 2018/7/27.
//

#include <unistd.h>
#include <netdb.h>
#include <poll.h>

#include "bind.h"
#include "appLog.h"
#include "P2PPeer.h"
#include "CppCallJava.h"

P2PPeer* P2PPeer::m_instance = nullptr;
P2PPeer::P2PPeer():m_fd(-1), m_oppositeNet{0},
                   m_threadStatu(ThreadStatus::NOT_SART)
{}

P2PPeer::~P2PPeer()
{
    if (m_fd > 0)
    {
        close(m_fd);
    }

    stopRecvUdpData();
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
        return 0;
    }
}

int P2PPeer::getLocatIpAndPort(const char* serverIp, const char* port,
                                       std::string& localIp, std::string& localPort)
{
    stopRecvUdpData();
    struct addrinfo hints, *res;
    const struct addrinfo *ptr;
    int ret = -1;

    memset (&hints, 0, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = 0;
    if (port == NULL)
        port = "3478";
    ret = getaddrinfo (serverIp, port, &hints, &res);
    if (ret)
    {
        LOGE("%s (port %s): %s", serverIp, port, gai_strerror (ret));
        return ret;
    }

    if (m_fd < 0)
    {
    	m_fd = MG_stun_usage_bind_socket_create(res->ai_addr, res->ai_addrlen);
    }

    m_localIp.clear();
    m_localPort.clear();
    for (ptr = res; ptr != NULL; ptr = ptr->ai_next)
    {
        char hostbuf[NI_MAXHOST], servbuf[NI_MAXSERV];
        getnameinfo(ptr->ai_addr, sizeof(sockaddr), 
					hostbuf, sizeof(hostbuf),
                    servbuf, sizeof(servbuf),
                    NI_NUMERICHOST | NI_NUMERICSERV);

        union {
            struct sockaddr_storage storage;
            struct sockaddr addr;
        } addr;
        socklen_t addrlen = sizeof (addr);
        StunUsageBindReturn val;
        val = MG_stun_usage_bind_run (ptr->ai_addr, 
                                      ptr->ai_addrlen,
                                      &addr.storage,
                                      &addrlen, &m_fd);
        if (val)
            LOGE ("stun_usage_bind_run ret:%d", val);
        else
        {
            convertLocalNet(&addr.addr, addrlen);

            localIp = m_localIp;
            localPort = m_localPort;
            LOGD("本地IP与端口对(%s:%s), 服务器IP与端口对(%s:%s)",
                 localIp.c_str(), localPort.c_str(), hostbuf, servbuf);
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

    LOGW("p2p对端ip:%s, 端口:%s", ip, port);
    int ret = getaddrinfo (ip, port, &hints, &res);
    if (ret)
    {
        LOGE("%s (port %s): %s", ip, port,
             gai_strerror (ret));
        return -1;
    }

    m_oppositeNet = *res->ai_addr;
    freeaddrinfo (res);

    startRecvUdpData();//设置好了对端地址就开启线程准备接收对端发送过来的数据
    return ret;
}

int P2PPeer::sendDataToOpposite(std::string data)
{
    if (m_fd < 0)
    {
        LOGE("未打开socket");
        return -1;
    }
    LOGE("send data:%s", data.c_str());
    return sendto (m_fd, (void *)data.c_str(), data.length(),
                   MSG_DONTWAIT | MSG_NOSIGNAL,
                   &m_oppositeNet, sizeof(m_oppositeNet));
}

#define RECV_BUFF_LEN_MXX 1300 //接收缓冲大小，考虑网络上MTU，这里最好548字节以内
int P2PPeer::recvUdpDataLoop()
{
    char* recvBuf = new char[RECV_BUFF_LEN_MXX];
    int delayTime = 2000;//milliseconds
    struct sockaddr srcAddr = {0};
    socklen_t srcAddrLen = 0;
    pollfd ufd = {0};
    ufd.fd = m_fd;
    ufd.events |= POLLIN;
    memset(recvBuf, 0x0, RECV_BUFF_LEN_MXX);

    LOGD("接收数据的线程开始...");
    m_threadStatu = ThreadStatus::RUNNING;
    while(m_threadStatu == ThreadStatus::RUNNING)
    {
        int pollRet = poll (&ufd, 1, delayTime);
        if (pollRet == 0)//超时
        {
            continue;
        }
        else if (pollRet > 0)//有数据可读
        {
            int recv = recvfrom (m_fd, (void *)recvBuf, RECV_BUFF_LEN_MXX,
                                 MSG_DONTWAIT | MSG_NOSIGNAL, &srcAddr,
                                 &srcAddrLen);
            if (recv > 0)
            {
                {
                    char hostbuf[NI_MAXHOST], servbuf[NI_MAXSERV];
                    int val = getnameinfo(&srcAddr, sizeof(sockaddr), hostbuf, sizeof(hostbuf),
                                          servbuf, sizeof(servbuf),
                                          NI_NUMERICHOST | NI_NUMERICSERV);
                    LOGW("接收到对端数据，对端IP：%s, 对端端口：%s, 数据：%s", hostbuf, servbuf, recvBuf);
                }
                proccesRecvedData(recvBuf, recv);
            }
            else//出错了
            {
                LOGE("recvfrom 返回了 0");
                break;
            }
            memset(recvBuf, 0x0, RECV_BUFF_LEN_MXX);
        }
        else//出错了
        {
            LOGE("poll（） 返回了负值:%d", pollRet);
            break;
        }
    }
    
    delete[] recvBuf;
    m_threadStatu = ThreadStatus::EXIT;
    LOGD("接收数据的线程结束。。。。。。。。。。。。");
    return 0;
}

//处理接收到的数据
int P2PPeer::proccesRecvedData(const char* data, int dataLen)
{
    CppCallJava::Instance()->recvData(data, dataLen);
    return 0;
}

int P2PPeer::startRecvUdpData()
{
    stopRecvUdpData();//先停止掉原线程
    m_thraed.reset(new std::thread([this](){
        this->recvUdpDataLoop();
    }));

    return 0;
}

int P2PPeer::stopRecvUdpData()
{
    LOGD("停止接收数据的线程");
    if (m_threadStatu == ThreadStatus::RUNNING && m_thraed)
    {
        m_threadStatu = ThreadStatus ::EXIT;
        LOGD("开始等待接收数据线程退出...");
        m_thraed->join();
//        std::this_thread::sleep_for (std::chrono::seconds(2));
    }

    m_threadStatu = ThreadStatus ::EXIT;
    return 0;
}