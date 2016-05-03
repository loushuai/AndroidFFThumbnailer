/*
 * thumbnailer.cpp
 *
 *  Created on: 2016Äê4ÔÂ17ÈÕ
 *      Author: loushuai
 */
#include <memory>
#include "ffthumbnailer.h"
#include "imagewriterfactory.h"

using namespace std;

namespace ffthumbnailer
{

FFThumbnailer::FFThumbnailer(const std::string &fileName, int type)
: mFileName(fileName)
, mType(type)
, mWidth(0)
, mHeight(0)
, mTime(0)
, mDuration(0)
, mVideoDecoder(getPixFmt(type))
{

}

int FFThumbnailer::getPixFmt(int type) {
	int ret = AV_PIX_FMT_YUV420P;

	switch(type) {
	case Png:
		ret = AV_PIX_FMT_RGB24;
		break;
	case Gif:
		ret = AV_PIX_FMT_RGB8;
		break;
	case Jpeg:
	case Raw:
		ret = AV_PIX_FMT_YUV420P;
		break;
	default:
		ret = AV_PIX_FMT_YUV420P;
		break;
	}

	return ret;
}

int FFThumbnailer::generateThumbnail(const string& outputFile, ThumbInfo &info)
{
	int ret = -1;
	LOGD("videoFile %s, outputFile %s, type %d, width %d, height %d, time %d, duration %d",
			mFileName.c_str(), outputFile.c_str(), mType, info.width, info.height, info.time, info.duration);
	if (info.duration > 0 && mType != Gif) {
		info.duration = 0;
	}

	unique_ptr<ImageWriter> imageWriter(ImageWriterFactory<const string&>::createImageWriter(static_cast<ThumbnailerImageType>(mType), outputFile));

	if (mVideoDecoder.init(mFileName) < 0) {
		LOGE("Init decoder failed");
		return -1;
	}

	std::function< AVFrame*(VideoDecoder &)> getFrame;
	int w = info.width, h = info.height;
	if (w > 0 && h > 0) {
		getFrame = [w, h](VideoDecoder &decoder)->AVFrame* {return decoder.getFrame(w, h);};
	} else {
		getFrame = [](VideoDecoder &decoder)->AVFrame* {return decoder.getFrame();};
	}

	if (info.time > 0) {
		ret = mVideoDecoder.seek(info.time);
		if (ret < 0) {
			return -1;
		}
	}

	if (info.duration <= 0) {
		ret = generateThumbnail(mVideoDecoder, *imageWriter, getFrame);
	} else {
		for (int i = 0; i < info.duration*25; ++i) {
			ret = generateThumbnail(mVideoDecoder, *imageWriter, getFrame);
			if (ret) {
				break;
			}
		}
	}

	return ret;
}

int FFThumbnailer::generateThumbnail(VideoDecoder& videoDecoder, ImageWriter& imageWriter, std::function< AVFrame*(VideoDecoder &)> getFrame)
{
	int ret = -1;

	if (videoDecoder.decodeVideoFrame() < 0) {
		LOGE("Video decode failed");
		return -1;
	}

	ret = imageWriter.writeFrame(getFrame(videoDecoder), 0);

	return 0;
}

} // end namespace ffthumbnailer
