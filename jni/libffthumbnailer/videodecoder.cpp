/*
 * videodecoder.cpp
 *
 *  Created on: 2016Äê4ÔÂ17ÈÕ
 *      Author: loushuai
 */

#include <stdexcept>
#include "videodecoder.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
}

using namespace std;

namespace ffthumbnailer
{

Filters::~Filters()
{
    if (mFilterGraphPtr) {
    	avfilter_graph_free(&mFilterGraphPtr);
    	mFilterGraphPtr = nullptr;
    }
}

void Filters::setPixFmt(int pixfmt)
{
	if (!mDesc.empty()) {
		mDesc += ",";
	}

	switch (pixfmt) {
	case AV_PIX_FMT_YUV420P:
		mDesc += "format=pix_fmts=yuv420p";
		break;
	case AV_PIX_FMT_RGB24:
		mDesc += "format=pix_fmts=rgb24";
		break;
	case AV_PIX_FMT_RGB8:
		mDesc += "format=pix_fmts=rgb8";
		break;
	default:
		break;
	}
}

void Filters::setScale(int w, int h)
{
	if (!mDesc.empty()) {
		mDesc += ",";
	}
	mDesc += string("scale=") + to_string(w) + string(":") + to_string(h);
}

int Filters::init(VideoDecoder *decoder)
{
	char args[512];
    int ret = 0;
    AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_RGB24, AV_PIX_FMT_RGB8, AV_PIX_FMT_NONE };

    mFilterGraphPtr = avfilter_graph_alloc();
    if (!inputs || !outputs || !mFilterGraphPtr) {
    	LOGE("Alloc filter graph failed");
    	goto end;
    }

    snprintf(args, sizeof(args),
                "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
				decoder->mVideoCodecCtxPtr->width,
				decoder->mVideoCodecCtxPtr->height,
				decoder->mVideoCodecCtxPtr->pix_fmt,
				decoder->mVideoCodecCtxPtr->time_base.num,
				decoder->mVideoCodecCtxPtr->time_base.den,
				decoder->mVideoCodecCtxPtr->sample_aspect_ratio.num,
				decoder->mVideoCodecCtxPtr->sample_aspect_ratio.den);
    LOGD("Buffer args: %s", args);

    ret = avfilter_graph_create_filter(&(mBufSrcCtxPtr), buffersrc, "in",
                                           args, NULL, mFilterGraphPtr);
    if (ret < 0) {
    	LOGE("Create buffer src context failed");
    	goto end;
    }

    ret = avfilter_graph_create_filter(&(mBufSinkCtxPtr), buffersink, "out",
                                           NULL, NULL, mFilterGraphPtr);
    if (ret < 0) {
    	LOGE("Create buffer sink context failed");
    	goto end;
    }

    ret = av_opt_set_int_list(mBufSinkCtxPtr, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
    	LOGE("Cannot set output pixel format");
        goto end;
    }

    outputs->name       = av_strdup("in");
    outputs->filter_ctx = mBufSrcCtxPtr;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = mBufSinkCtxPtr;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    setPixFmt(decoder->mPixFmt);
    LOGD("Filter desc %s", mDesc.c_str());
    if ((ret = avfilter_graph_parse_ptr(mFilterGraphPtr, mDesc.c_str(),
                                        &inputs, &outputs, NULL)) < 0) {
    	LOGE("Parse filter graph failed");
        goto end;
    }

    if ((ret = avfilter_graph_config(mFilterGraphPtr, NULL)) < 0) {
    	LOGE("Config filter graph failed");
        goto end;
    }

end:
	avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
	return ret;
}

VideoDecoder::VideoDecoder()
: mVideoIndex(-1)
, mFmtCtxPtr(nullptr)
, mPacketPtr(nullptr)
, mFramePtr(nullptr)
, mFilterFramePtr(nullptr)
, mPixFmt(AV_PIX_FMT_YUV420P)
{

}

VideoDecoder::VideoDecoder(int format)
: mVideoIndex(-1)
, mFmtCtxPtr(nullptr)
, mPacketPtr(nullptr)
, mFramePtr(nullptr)
, mFilterFramePtr(nullptr)
, mPixFmt(format)
{

}

VideoDecoder::~VideoDecoder()
{
	destroy();
}

int VideoDecoder::init(const string& filename)
{
	av_register_all();
    avcodec_register_all();
    avformat_network_init();
    avfilter_register_all();

    if (avformat_open_input(&mFmtCtxPtr, filename.c_str(), nullptr, nullptr) != 0) {
    	LOGE("Could not open input file: %s", filename.c_str());
    	goto failed;
    }

    if (avformat_find_stream_info(mFmtCtxPtr, nullptr) < 0) {
        LOGE("Could not find stream information");
        goto failed;
    }

    if (initializeVideo() < 0) {
    	LOGE("Init video failed");
    	goto failed;
    }

    mFramePtr = av_frame_alloc();
    if (!mFramePtr) {
    	LOGE("Not enough memory");
    	goto failed;
    }

    mFilterFramePtr = av_frame_alloc();
    if (!mFilterFramePtr) {
    	LOGE("Not enough memory");
    	goto failed;
    }

    return 0;
failed:
	destroy();
	return -1;
}

void VideoDecoder::destroy()
{
    if (mVideoCodecCtxPtr) {
        avcodec_close(mVideoCodecCtxPtr);
        mVideoCodecCtxPtr = nullptr;
    }

	if (mFmtCtxPtr) {
		avformat_close_input(&mFmtCtxPtr);
	}

	if (mPacketPtr) {
		av_packet_unref(mPacketPtr);
		delete mPacketPtr;
		mPacketPtr = nullptr;
	}

    if (mFramePtr) {
        av_frame_free(&mFramePtr);
        mFramePtr = nullptr;
    }

    if (mFilterFramePtr) {
        av_frame_free(&mFramePtr);
        mFramePtr = nullptr;
    }

	mVideoIndex = -1;
}

int VideoDecoder::initializeVideo()
{
	for (int i = 0; i < mFmtCtxPtr->nb_streams; ++i) {
		if (mFmtCtxPtr->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			mVideoStreamPtr = mFmtCtxPtr->streams[i];
			mVideoIndex = i;
			break;
		}
	}

	if (mVideoIndex == -1) {
        LOGE("Could not find video stream");
        goto failed;
	}

	mVideoCodecCtxPtr = mFmtCtxPtr->streams[mVideoIndex]->codec;
	mVideoCodecPtr = avcodec_find_decoder(mVideoCodecCtxPtr->codec_id);

	if (mVideoCodecPtr == nullptr) {
		mVideoCodecCtxPtr = nullptr;
		LOGE("Video Codec not found");
		goto failed;
	}

    if (avcodec_open2(mVideoCodecCtxPtr, mVideoCodecPtr, nullptr) < 0) {
        LOGE("Could not open video codec");
        goto failed;
    }

    return 0;
failed:
	destroy();
	return -1;
}

int VideoDecoder::getVideoPacket()
{
	int attempts = 0;
	bool pktgot = false;

	if (mPacketPtr) {
		av_packet_unref(mPacketPtr);
		delete mPacketPtr;
	}

	mPacketPtr = new AVPacket();
	if (!mPacketPtr) {
		LOGE("Not enough memory");
		goto out;
	}

	while (!pktgot && attempts++ < 1000) {
		if (av_read_frame(mFmtCtxPtr, mPacketPtr) < 0) {
			LOGE("Can't read packet");
			goto out;
		}

		pktgot = (mPacketPtr->stream_index == mVideoIndex);

		if (!pktgot) {
			av_packet_unref(mPacketPtr);
		}
	}

out:
	return pktgot ? 0 : -1;
}

int VideoDecoder::decodeVideoPacket() {
	int frameFinished;

	if (mPacketPtr->stream_index != mVideoIndex) {
		return -1;
	}

	if (mFramePtr) {
		av_frame_unref(mFramePtr);
	}

	int bytesDecoded = avcodec_decode_video2(mVideoCodecCtxPtr, mFramePtr, &frameFinished, mPacketPtr);
	if (bytesDecoded < 0) {
		LOGE("Decode failed");
		return -1;
	}

	return frameFinished == 0 ? 0 : 1;
}

int VideoDecoder::decodeVideoFrame() {
	int gotpkt =  -1, decpkt = -1;

	while (((gotpkt = getVideoPacket()) == 0)
		&& ((decpkt = decodeVideoPacket()) == 0)) {
		//empty
	}

	if (gotpkt < 0 || decpkt < 0) {
		return -1;
	}

	return 0;
}

int VideoDecoder::seek(int timeInSec)
{
	int ret = -1;
	int64_t timestamp = AV_TIME_BASE * static_cast<int64_t>(timeInSec);
    if (timestamp < 0) {
        timestamp = 0;
    }

    ret = av_seek_frame(mFmtCtxPtr, -1, timestamp, 0);
    if (ret < 0) {
    	LOGE("av_seek_frame failed");
    	return -1;
    }
    avcodec_flush_buffers(mVideoCodecCtxPtr);

    return 0;
}

AVFrame* VideoDecoder::getFrame(int w, int h)
{
	int ret = -1;

	if (w > 0 && h > 0) {
		mFilters.setScale(w, h);
	}

	if (!mFilters.hasInited() && mFilters.init(this) < 0) {
		LOGE("Init filters failed");
		goto failed;
	}

	if (av_buffersrc_add_frame_flags(mFilters.mBufSrcCtxPtr, mFramePtr, AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
		LOGE("Buffer src add frame failed");
		goto failed;
	}

	ret = av_buffersink_get_frame(mFilters.mBufSinkCtxPtr, mFilterFramePtr);
	if (ret < 0) {
		LOGE("Get frame failed");
		goto failed;
	}

	return mFilterFramePtr;
failed:

	return nullptr;
}

} // end namespace ffthumbnailer

