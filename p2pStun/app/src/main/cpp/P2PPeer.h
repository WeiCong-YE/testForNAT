//
// Created by ouyangjie on 2018/7/27.
//

#ifndef P2PSTUN_P2PPEER_H
#define P2PSTUN_P2PPEER_H

#include <sys/socket.h>
#include <string>
#include <thread>
#include <memory>

enum class ThreadStatus:unsigned char{
    NOT_SART = 1,
    RUNNING = 2,
    EXIT = 3
};

class P2PPeer{
public:
    P2PPeer();
    ~P2PPeer();

    static P2PPeer* Instance(){
        if (m_instance == nullptr)
        {
            m_instance = new P2PPeer();
        }
        return m_instance;
    }
    /**
     * 获取本地外网IP和端口号
     * @param serverIp STUN服务器IP
     * @param port STUN服务器端口号
     * @param localIp 获取到的本地外网Ip
     * @param localPort 获取到的本地外网port
     * @return 本地外网IP和端口号对，型如XX.XX.XX.XX:PORT
     */
    int getLocatIpAndPort(const char* serverIp, const char* port,
                          std::string& localIp, std::string& localPort);

    /**
     * 设置对端IP和端口
     * @param ip 对端IP
     * @param port  对端端口号
     * @return 0：设置成功， 其他值：设置失败
     */
    int setOppositeNet(const char* ip, const char* port);

    int sendDataToOpposite(std::string data);

private:
    int convertLocalNet(const struct sockaddr *addr, socklen_t addrlen);

    /**
     * 循环接收UDP数据
     * @return
     */
    int recvUdpDataLoop();

    /**
     * 处理接收到的UDP数据
     * @return
     */
    int proccesRecvedData(const char* data, int dataLen);

    /**
     * 开启接收UDP数据的线程
     * @return
     */
    int startRecvUdpData();

    /**
     * 停止接收UDP数据的线程
     * @return
     */
    int stopRecvUdpData();

private:
    int m_fd;
    struct sockaddr m_oppositeNet;
    std::string m_localIp;
    std::string m_localPort;

    std::unique_ptr<std::thread> m_thraed;
    ThreadStatus m_threadStatu;

    static P2PPeer* m_instance;
};
#endif //P2PSTUN_P2PPEER_H
