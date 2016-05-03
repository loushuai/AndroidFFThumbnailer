/*
 * rawwriter.cpp
 *
 *  Created on: 2016Äê4ÔÂ28ÈÕ
 *      Author: loushuai
 */

#include "yuv420writer.h"

#include <errno.h>
#include "log.h"

using namespace std;

namespace ffthumbnailer
{

YUV420Writer::YUV420Writer(const string& outputFile)
: mOutputFile(outputFile)
{

}

YUV420Writer::~YUV420Writer()
{

}

int YUV420Writer::writeFrame(AVFrame *frame, int quality)
{
	FILE *filePtr = nullptr;
	uint8_t *dataPtr = nullptr;

	if (!frame) {
		LOGD("frame is null");
		goto failed;
	}

	if (mOutputFile.length() == 0) {
		LOGD("No output file path");
		goto failed;
	}

	if (frame->format != AV_PIX_FMT_YUV420P) {
		LOGD("Format not supported");
		goto failed;
	}

	LOGD("Frame width %d, height %d, linesize %d",
			frame->width, frame->height, frame->linesize[0]);
	LOGD("data[1] %x, linesize[1] %d, data[2] %x, linesize[2] %d, data[3] %x, linesize[3] %d",
			frame->data[1], frame->linesize[1], frame->data[2], frame->linesize[2], frame->data[3], frame->linesize[3]);

	filePtr = fopen(mOutputFile.c_str(), "wb");
	if (!filePtr) {
		LOGE("Open file %s failed, %s", mOutputFile.c_str(), strerror(errno));
		goto failed;
	}

	dataPtr = frame->data[0];
	for (int i = 0; i < frame->height; ++i) {
		if (fwrite(dataPtr, frame->width, 1, filePtr) != 1) {
			LOGE("Write file failed %s", strerror(errno));
			goto failed;
		}
		dataPtr += frame->linesize[0];
	}

	dataPtr = frame->data[1];
	for (int i = 0; i < frame->height/2; ++i) {
		if (fwrite(dataPtr, frame->width/2, 1, filePtr) != 1) {
			LOGE("Write file failed %s", strerror(errno));
			goto failed;
		}
		dataPtr += frame->linesize[1];
	}

	dataPtr = frame->data[2];
	for (int i = 0; i < frame->height/2; ++i) {
		if (fwrite(dataPtr, frame->width/2, 1, filePtr) != 1) {
			LOGE("Write file failed %s", strerror(errno));
			goto failed;
		}
		dataPtr += frame->linesize[2];
	}

	fclose(filePtr);

	return 0;
failed:
	if (filePtr) {
		fclose(filePtr);
	}
	return -1;
}

}
