/*
 * imagewriter.h
 *
 *  Created on: 2016Äê4ÔÂ17ÈÕ
 *      Author: loushuai
 */

#ifndef JNI_LIBFFTHUMBNAILER_IMAGEWRITER_H_
#define JNI_LIBFFTHUMBNAILER_IMAGEWRITER_H_

#include <string>

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
#include "libavutil/avutil.h"
}

struct AVFrame;

namespace ffthumbnailer
{

class ImageWriter {
public:
    ImageWriter() {}
    virtual ~ImageWriter() {}

    virtual int writeFrame(AVFrame *frame, int quality) = 0;
};


}

#endif /* JNI_LIBFFTHUMBNAILER_IMAGEWRITER_H_ */
