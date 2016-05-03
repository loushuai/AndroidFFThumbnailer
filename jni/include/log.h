/*
 * log.h
 *
 *  Created on: 2016��4��25��
 *      Author: loushuai
 */

#ifndef JNI_INCLUDE_LOG_H_
#define JNI_INCLUDE_LOG_H_

#include <android/log.h>

#define ANDROID_LOG 0

#define LOG_TAG    "FFThumbnailer"

#define LOGD(fmt, ...) PRINT_LOG(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) PRINT_LOG(ANDROID_LOG_INFO, LOG_TAG, fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) PRINT_LOG(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##__VA_ARGS__)


#if ANDROID_LOG
#define PRINT_LOG(level, tag, fmt, ...) \
		__android_log_print(level, tag, "%s(%s) %d: " fmt, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define PRINT_LOG(level, tag, fmt, ...) \
		printf("%s(%s) %d: " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif

#endif /* JNI_INCLUDE_LOG_H_ */
