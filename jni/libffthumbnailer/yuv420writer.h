/*
 * rawwriter.h
 *
 *  Created on: 2016Äê4ÔÂ28ÈÕ
 *      Author: loushuai
 */

#ifndef JNI_LIBFFTHUMBNAILER_YUV420WRITER_H_
#define JNI_LIBFFTHUMBNAILER_YUV420WRITER_H_

#include <string>
#include "imagewriter.h"

namespace ffthumbnailer
{

class YUV420Writer : public ImageWriter
{
public:
	YUV420Writer(const std::string& outputFile);
	~YUV420Writer();

	int writeFrame(AVFrame *frame, int quality);

private:

private:
	std::string  mOutputFile;

};

}

#endif /* JNI_LIBFFTHUMBNAILER_YUV420WRITER_H_ */
