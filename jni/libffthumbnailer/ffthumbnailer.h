/*
 * thumbnailer.h
 *
 *  Created on: 2016Äê4ÔÂ17ÈÕ
 *      Author: loushuai
 */

#ifndef JNI_LIBFFTHUMBNAILER_FFTHUMBNAILER_H_
#define JNI_LIBFFTHUMBNAILER_FFTHUMBNAILER_H_

#include <string>
#include <functional>
#include <vector>
#include "videodecoder.h"
#include "imagetypes.h"
#include "imagewriter.h"
#include "log.h"

namespace ffthumbnailer
{

typedef struct _ThumbInfo {
	int width;
	int height;
	int time;
	int duration;
} ThumbInfo;

class FFThumbnailer {
public:
	FFThumbnailer(const std::string &fileName, int type = 0);

	int generateThumbnail(const std::string& outputFile, ThumbInfo &info);

private:
	int generateThumbnail(VideoDecoder & videoDecoder, ImageWriter& imageWriter, std::function< AVFrame*(VideoDecoder &)> getFrame);
	int getPixFmt(int type);

private:
	std::string           mFileName;
	int                   mType;
	int                   mWidth;
	int                   mHeight;
	int 				  mTime;
	int                   mDuration;
	VideoDecoder          mVideoDecoder;
};

}

#endif /* JNI_LIBFFTHUMBNAILER_FFTHUMBNAILER_H_ */
