/*
 * imagewriterfactory.h
 *
 *  Created on: 2016Äê4ÔÂ28ÈÕ
 *      Author: loushuai
 */

#ifndef JNI_LIBFFTHUMBNAILER_IMAGEWRITERFACTORY_H_
#define JNI_LIBFFTHUMBNAILER_IMAGEWRITERFACTORY_H_

#include "pngwriter.h"
#include "jpegwriter.h"
#include "gifwriter.h"
#include "yuv420writer.h"

namespace ffthumbnailer
{

template <typename T>
class ImageWriterFactory
{
public:
	static ImageWriter* createImageWriter(ThumbnailerImageType imageType, T output)
	{
		switch (imageType) {
		case Png:
			return new PngWriter(output);
			break;
		case Jpeg:
			return new JpegWriter(output);
			break;
		case Gif:
			return new GifWriter(output);
			break;
		case Raw:
			return new YUV420Writer(output);
			break;
		default:
			break;
		}

		return nullptr;
	}
};

}

#endif /* JNI_LIBFFTHUMBNAILER_IMAGEWRITERFACTORY_H_ */
