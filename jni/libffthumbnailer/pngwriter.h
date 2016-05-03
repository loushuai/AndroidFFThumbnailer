/*
 * pngwriter.h
 *
 *  Created on: 2016Äê5ÔÂ1ÈÕ
 *      Author: loushuai
 */

#ifndef JNI_LIBFFTHUMBNAILER_PNGWRITER_H_
#define JNI_LIBFFTHUMBNAILER_PNGWRITER_H_

#include <string>
#include "imagewriter.h"

namespace ffthumbnailer
{

class PngWriter : public ImageWriter
{
public:
	PngWriter(const std::string& outputFile);
	~PngWriter();

	int writeFrame(AVFrame *frame, int quality);

private:
	int init(AVFrame *frame);
	int save();

private:
	std::string       mOutputFile;
	AVCodecContext*   mCodecCtxPtr;
	AVCodec*          mCodecPtr;
	AVPacket          mEncpkt;
};

}

#endif /* JNI_LIBFFTHUMBNAILER_PNGWRITER_H_ */
