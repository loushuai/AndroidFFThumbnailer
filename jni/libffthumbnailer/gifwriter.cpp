/*
 * gifwriter.cpp
 *
 *  Created on: 2016Äê5ÔÂ2ÈÕ
 *      Author: loushuai
 */

#include "gifwriter.h"
#include "log.h"

using namespace std;

namespace ffthumbnailer
{

GifWriter::GifWriter(const std::string& outputFile)
: mOutputFile(outputFile)
, mFmtCtxPtr(nullptr)
, mStreamPtr(nullptr)
, mCodecCtxPtr(nullptr)
, mCodecPtr(nullptr)
{

}

GifWriter::~GifWriter()
{
	finish();

	if (mStreamPtr->codec) {
		avcodec_close(mStreamPtr->codec);
	}

	if (mFmtCtxPtr && !(mFmtCtxPtr->oformat->flags & AVFMT_NOFILE)) {
		avio_closep(&mFmtCtxPtr->pb);
	}

	if (mFmtCtxPtr) {
		avformat_free_context(mFmtCtxPtr);
	}
}

int GifWriter::init(AVFrame *frame)
{
	int ret = -1;

	mEncpkt.data = nullptr;
	mEncpkt.size = 0;

	avformat_alloc_output_context2(&mFmtCtxPtr, NULL, "gif", mOutputFile.c_str());
	if (!mFmtCtxPtr) {
		LOGE("Alloc format Context failed");
		goto end;
	}

	LOGD("format: %s", mFmtCtxPtr->oformat->name);

	mStreamPtr = avformat_new_stream(mFmtCtxPtr, NULL);
	if (!mStreamPtr) {
		LOGE("Failed allocating output stream");
        goto end;
	}

	mCodecCtxPtr             = mStreamPtr->codec;
	mCodecCtxPtr->width      = frame->width;
	mCodecCtxPtr->height     = frame->height;
	mCodecCtxPtr->pix_fmt    = AV_PIX_FMT_RGB8;
	mCodecCtxPtr->codec_id   = AV_CODEC_ID_GIF;
	mCodecCtxPtr->codec_type = AVMEDIA_TYPE_VIDEO;
	mCodecCtxPtr->time_base  = (AVRational){1, 25};

	mCodecPtr = avcodec_find_encoder(mCodecCtxPtr->codec_id);
	if (!mCodecPtr) {
		LOGE("Can't find codec");
		goto end;
	}

	if (avcodec_open2(mCodecCtxPtr, mCodecPtr, nullptr) < 0) {
		LOGE("Can't open codec");
		goto end;
	}

	if (mFmtCtxPtr->oformat->flags & AVFMT_NOFILE) {
		LOGE("AVFMT_NOFILE");
		goto end;
	}

	ret = avio_open(&mFmtCtxPtr->pb, mOutputFile.c_str(), AVIO_FLAG_WRITE);
	if (ret < 0) {
		LOGE("avio_open failed");
		goto end;
	}

	ret = avformat_write_header(mFmtCtxPtr, NULL);
	if (ret < 0) {
		LOGE("Writer header failed");
		goto end;
	}

end:
	return ret;
}

int GifWriter::writeFrame(AVFrame *frame, int quality)
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

	if (!mFmtCtxPtr) {
		ret = init(frame);
		if (ret < 0) {
			LOGD("Init failed");
			goto end;
		}
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

int GifWriter::save()
{
	int ret = -1;

	ret = av_interleaved_write_frame(mFmtCtxPtr, &mEncpkt);

end:
	av_packet_unref(&mEncpkt);
	return ret;
}

void GifWriter::finish()
{
	LOGD("Gif finish");
	av_write_trailer(mFmtCtxPtr);
	LOGD("Gif finish exit");
}

}
