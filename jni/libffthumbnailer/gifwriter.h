/*
 * gifwriter.h
 *
 *  Created on: 2016Äê5ÔÂ2ÈÕ
 *      Author: loushuai
 */

#ifndef JNI_LIBFFTHUMBNAILER_GIFWRITER_H_
#define JNI_LIBFFTHUMBNAILER_GIFWRITER_H_

#include <string>
#include "imagewriter.h"

namespace ffthumbnailer
{

class GifWriter : public ImageWriter
{
public:
	GifWriter(const std::string& outputFile);
	~GifWriter();

	int writeFrame(AVFrame *frame, int quality);

private:
	int init(AVFrame *frame);
	int save();
	void finish();

private:
	std::string       mOutputFile;
	AVFormatContext*  mFmtCtxPtr;
	AVStream*         mStreamPtr;
	AVCodecContext*   mCodecCtxPtr;
	AVCodec*          mCodecPtr;
	AVPacket          mEncpkt;
};

}

#endif /* JNI_LIBFFTHUMBNAILER_GIFWRITER_H_ */
