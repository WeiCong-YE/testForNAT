#include <jni.h>
#include <string>
#include "appLog.h"
#include "P2PPeer.h"
#include "CppCallJava.h"

extern "C" JNIEXPORT void
JNICALL
        Java_com_p2p_p2pstun_MainActivity_initEnv(JNIEnv *env,
                                                  jobject thiz){
    CppCallJava::Instance()->Init(env, thiz);
}

extern "C" JNIEXPORT jstring
JNICALL
Java_com_p2p_p2pstun_MainActivity_getLocatIpAndPort(
        JNIEnv *env,
        jobject /* this */,
        jstring serverIp,
        jstring serverPort) {
    const char* ip = env->GetStringUTFChars(serverIp, nullptr);
    const char* port = env->GetStringUTFChars(serverPort, nullptr);
    LOGD("serverIP:%s, serverPort:%s", ip, port);

    std::string localIp, localPort;
    int ret = P2PPeer::Instance()->getLocatIpAndPort(ip, port, localIp, localPort);

    env->ReleaseStringUTFChars(serverIp, ip);
    env->ReleaseStringUTFChars(serverPort, port);
    if (ret == 0){
        LOGD("获得外网IP与端口对：%s:%s", localIp.c_str(), localPort.c_str());
        return env->NewStringUTF((localIp + ":" + localPort).c_str());
    } else {
        LOGE("获取外网IP与端口对失败");
        return nullptr;
    }
}

extern "C" JNIEXPORT jint
JNICALL
Java_com_p2p_p2pstun_MainActivity_setOppositeNet(
        JNIEnv *env,
        jobject /* this */,
        jstring peerIp,
        jstring peerPort){
    const char* ip = env->GetStringUTFChars(peerIp, nullptr);
    const char* port = env->GetStringUTFChars(peerPort, nullptr);
    int ret = P2PPeer::Instance()->setOppositeNet(ip, port);

    env->ReleaseStringUTFChars(peerIp, ip);
    env->ReleaseStringUTFChars(peerPort, port);
    return ret;
}

extern "C" JNIEXPORT jint
JNICALL
Java_com_p2p_p2pstun_MainActivity_sendData(
        JNIEnv *env,
        jobject /* this */,
        jstring data){
    const char* pData = env->GetStringUTFChars(data, nullptr);
//    int dataLen = strlen
    LOGW("发送的数据：%s", pData);
    int ret = P2PPeer::Instance()->sendDataToOpposite(pData);

    env->ReleaseStringUTFChars(data, pData);
    LOGD("数据发送完毕， ret:%d", ret);
    return ret;
}
