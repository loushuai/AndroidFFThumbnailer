/*
 * jpegwriter.h
 *
 *  Created on: 2016Äê5ÔÂ1ÈÕ
 *      Author: loushuai
 */

#ifndef JNI_LIBFFTHUMBNAILER_JPEGWRITER_H_
#define JNI_LIBFFTHUMBNAILER_JPEGWRITER_H_

#include <string>
#include "imagewriter.h"

namespace ffthumbnailer
{

class JpegWriter : public ImageWriter
{
public:
	JpegWriter(const std::string& outputFile);
	~JpegWriter();

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

#endif /* JNI_LIBFFTHUMBNAILER_JPEGWRITER_H_ */
