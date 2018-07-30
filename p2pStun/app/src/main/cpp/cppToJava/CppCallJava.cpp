//
// Created by ouyangjie on 2018/7/30.
//

#include "../appLog.h"
#include "CppCallJava.h"

CppCallJava* CppCallJava::m_instance = nullptr;

CppCallJava::CppCallJava():m_jvm(nullptr), m_javaObj(nullptr)
{ }

void CppCallJava::recvData(const char* data, int dataLen)
{
    JNIEnv* env = nullptr;
    bool bIsAttached = false;

    if (m_jvm == nullptr)
    {
        LOGE("JVM not init");
        return ;
    }

    int status = (m_jvm)->GetEnv((void**) &env, JNI_VERSION_1_2);
    if (status < 0)
    {
        status = (m_jvm)->AttachCurrentThread(&env, nullptr);
        if (status < 0)
            return;
        bIsAttached = true;
    }

    do
    {
        jclass cls = (env)->GetObjectClass(m_javaObj);
        if(!cls) break;

        jmethodID methodID = (env)->GetMethodID(cls, "recvData", "(Ljava/lang/String;)V");
        if (!methodID) break;

        jstring strData = env->NewStringUTF(data);
        env->CallVoidMethod(m_javaObj, methodID, strData);

        env->DeleteLocalRef(cls);
        env->DeleteLocalRef(strData);
    }while(0);

    if (bIsAttached)
        (m_jvm)->DetachCurrentThread();
}

void CppCallJava::Init(JNIEnv* env, jobject thiz)
{
    if (m_jvm == nullptr)
    {
        (env)->GetJavaVM(&m_jvm);
    }

    if (m_javaObj == nullptr)
    {
        m_javaObj = (env)->NewGlobalRef(thiz);
    }
};

void CppCallJava::DeInit(JNIEnv* env)
{
    if (m_javaObj != nullptr)
    {
        (env)->DeleteGlobalRef(m_javaObj);
        m_javaObj = nullptr;
    }

    if (m_jvm != nullptr)
    {
        m_jvm = nullptr;
    }
};