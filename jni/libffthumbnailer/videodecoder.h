/*
 * videodecoder.h
 *
 *  Created on: 2016Äê4ÔÂ17ÈÕ
 *      Author: loushuai
 */

#ifndef JNI_LIBFFTHUMBNAILER_VIDEODECODER_H_
#define JNI_LIBFFTHUMBNAILER_VIDEODECODER_H_

#include <string>
#include "log.h"

struct AVFormatContext;
struct AVCodecContext;
struct AVCodec;
struct AVStream;
struct AVPacket;
struct AVFrame;
struct AVFilterGraph;
struct AVFilterContext;

namespace ffthumbnailer
{

class VideoDecoder;
class Filters{
public:
	Filters() : mFilterGraphPtr(nullptr)
              , mBufSrcCtxPtr(nullptr)
			  , mBufSinkCtxPtr(nullptr) {}
	~Filters();
	void setPixFmt(int pixfmt);
	void setScale(int w, int h);
	int init(VideoDecoder *decoder);
	bool hasInited() const {
		return mFilterGraphPtr != nullptr;
	}

	AVFilterGraph*          mFilterGraphPtr;
	AVFilterContext*        mBufSrcCtxPtr;
	AVFilterContext*        mBufSinkCtxPtr;
	std::string             mDesc;
};

class VideoDecoder {
	friend class Filters;
public:
	VideoDecoder();
	VideoDecoder(int format);
	~VideoDecoder();

public:
	int init(const std::string& filename);
	void destroy();
	int decodeVideoFrame();
	int seek(int timeInSec);
	AVFrame* getFrame(int w = 0, int h = 0);

private:
	int initializeVideo();
	int getVideoPacket();
	int decodeVideoPacket();

private:
	int					    mVideoIndex;
	AVFormatContext*        mFmtCtxPtr;
	AVCodecContext*         mVideoCodecCtxPtr;
	AVCodec*                mVideoCodecPtr;
	AVStream*               mVideoStreamPtr;
	AVPacket*               mPacketPtr;
	AVFrame*                mFramePtr;
	AVFrame*                mFilterFramePtr;
	Filters                 mFilters;
	int                     mPixFmt;
};

}



#endif /* JNI_LIBFFTHUMBNAILER_VIDEODECODER_H_ */
