/*
 * pngwriter.cpp
 *
 *  Created on: 2016Äê5ÔÂ1ÈÕ
 *      Author: loushuai
 */

#include "pngwriter.h"
#include "log.h"

using namespace std;

namespace ffthumbnailer
{

PngWriter::PngWriter(const std::string& outputFile)
: mOutputFile(outputFile)
, mCodecCtxPtr(nullptr)
, mCodecPtr(nullptr)
{
	av_init_packet(&mEncpkt);
}

PngWriter::~PngWriter()
{
	if (mCodecCtxPtr) {
		avcodec_free_context(&mCodecCtxPtr);
	}
}

int PngWriter::init(AVFrame *frame)
{
	int ret = -1;

	mEncpkt.data = nullptr;
	mEncpkt.size = 0;

	mCodecCtxPtr = avcodec_alloc_context3(NULL);
	if (!mCodecCtxPtr) {
		LOGE("Not enough memory");
		goto end;
	}

	mCodecCtxPtr->width      = frame->width;
	mCodecCtxPtr->height     = frame->height;
	mCodecCtxPtr->pix_fmt    = AV_PIX_FMT_RGB24;
	mCodecCtxPtr->codec_id   = AV_CODEC_ID_PNG;
	mCodecCtxPtr->codec_type = AVMEDIA_TYPE_VIDEO;
	mCodecCtxPtr->time_base.num = 1;
	mCodecCtxPtr->time_base.den = 1;

	mCodecPtr = avcodec_find_encoder(mCodecCtxPtr->codec_id);
	if (!mCodecPtr) {
		LOGE("Can't find codec");
		goto end;
	}

	if (avcodec_open2(mCodecCtxPtr, mCodecPtr, nullptr) < 0) {
		LOGE("Can't open codec");
		goto end;
	}

	ret = 0;
end:
	return ret;
}

int PngWriter::writeFrame(AVFrame *frame, int quality)
{
	int ret = -1;
	int gotframe = 0;

	if (!frame) {
		LOGD("frame is null");
		goto end;
	}

	if (mOutputFile.length() == 0) {
		LOGD("No output file path");
		goto end;
	}

	LOGD("frame pix fmt %d", frame->format);

	ret = init(frame);
	if (ret < 0) {
		LOGD("Init failed");
		goto end;
	}

	ret = avcodec_encode_video2(mCodecCtxPtr, &mEncpkt, frame, &gotframe);
	if (ret < 0) {
		LOGD("Encode frame failed");
		goto end;
	}

	ret = save();
	if (ret < 0) {
		LOGD("Save file failed");
		goto end;
	}

end:
	return ret;
}

int PngWriter::save()
{
	int ret = -1;
	FILE *file = nullptr;

	if (!mEncpkt.data) {
		LOGD("No data to write");
		goto end;
	}

	file = fopen(mOutputFile.c_str(), "wb");
	if (!file) {
		LOGE("Open file %s failed, %s", mOutputFile.c_str(), strerror(errno));
		goto end;
	}

	if (fwrite(mEncpkt.data, mEncpkt.size, 1, file) != 1) {
		LOGE("Write file failed, %s", strerror(errno));
		goto end;
	}

	ret = 0;
end:
	if (file) {
		fclose(file);
	}

	av_packet_unref(&mEncpkt);

	return ret;
}


}
