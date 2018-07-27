//
// Created by ouyangjie on 2018/7/26.
//

#ifndef MYAPPLICATION2_APPLOG_H
#define MYAPPLICATION2_APPLOG_H

#ifdef __cplusplus
extern "C" {
#endif
#include<string.h>
#include <android/log.h>
#ifndef LOG_TAG
#define LOG_TAG (NULL == strrchr(__FILE__, '\\') ? __FILE__ : (char *)(strrchr(__FILE__, '\\') + 1))
#endif

#define HW_LOG_UNKNOWN 0   //    ANDROID_LOG_UNKNOWN = 0,
#define HW_LOG_DEFAULT 1   //    ANDROID_LOG_DEFAULT,    /* only for SetMinPriority() */
#define HW_LOG_VERBOSE 2   //    ANDROID_LOG_VERBOSE,
#define HW_LOG_DEBUG   3   //    ANDROID_LOG_DEBUG,
#define HW_LOG_INFO    4   //    ANDROID_LOG_INFO,
#define HW_LOG_WARN    5   //    ANDROID_LOG_WARN,
#define HW_LOG_ERROR   6   //    ANDROID_LOG_ERROR,
#define HW_LOG_FATAL   7   //    ANDROID_LOG_FATAL,
#define HW_LOG_SILENT  8   //    ANDROID_LOG_SILENT,     /* only for SetMinPriority(); must be last */

/*
 * 日志等级调节开关，  等级越高， 打印的日志越少
 * 调到  HW_LOG_UNKNOWN 则打开所有日志
 * 调到  HW_LOG_SILENT 则关闭所有的日志
 */
#define LOG_LEVEL HW_LOG_UNKNOWN

#if (LOG_LEVEL <= HW_LOG_DEBUG)
#define LOGD(fmt, ...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define LOGD(fmt...)
#endif

#if  (LOG_LEVEL <= HW_LOG_INFO)
#define LOGI(fmt, ...)    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define LOGI(fmt...)
#endif

#if  (LOG_LEVEL <= HW_LOG_WARN)
#define LOGW(fmt, ...)  __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define LOGW(fmt...)
#endif

#if  (LOG_LEVEL <= HW_LOG_ERROR)
#define LOGE(fmt, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define LOGE(fmt...)
#endif

#if  (LOG_LEVEL <= HW_LOG_FATAL)
#define LOGV(fmt, ...)  __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define LOGV(fmt...)
#endif

#ifdef __cplusplus
};
#endif


#endif //MYAPPLICATION2_APPLOG_H
