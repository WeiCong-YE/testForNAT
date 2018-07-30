//
// Created by ouyangjie on 2018/7/30.
//

#ifndef P2PSTUN_CPPTOJAVAIMP_H
#define P2PSTUN_CPPTOJAVAIMP_H

#include <jni.h>

class CppCallJava
{
public:
    CppCallJava();
    void recvData(const char* data, int dataLen);

    void Init(JNIEnv* env, jobject thiz);
    void DeInit(JNIEnv* env);
private:
    JavaVM *m_jvm;
    jobject m_javaObj;

public:
    static CppCallJava* Instance()
    {
        if (m_instance == nullptr)
        {
            m_instance = new CppCallJava();
        }

        return m_instance;
    }

private:
    static CppCallJava* m_instance;
};

#endif //P2PSTUN_CPPTOJAVAIMP_H
