#include <jni.h>
#include <string>
#include "ffthumbnailer.h"

using std::string;
using namespace ffthumbnailer;

#ifdef __cplusplus
extern "C" {
#endif

jint
Java_com_example_loushuai_thumbnailerdemo_MainActivity_thumbnail(JNIEnv* env,
        											   jobject thiz, jstring inFile, jstring outFile, jint type) {
	const char *in_file = env->GetStringUTFChars(inFile, NULL);
	if (in_file == NULL) {
        return -1;
	}

	const char *out_file = env->GetStringUTFChars(outFile, NULL);
	if (out_file == NULL) {
        return -1;
	}

	ThumbInfo thumbInfo;
	thumbInfo.width = 0;
	thumbInfo.height = 0;
	thumbInfo.time = 0;
	thumbInfo.duration = 0;

	FFThumbnailer thumbnailer(string(in_file), type);
	thumbnailer.generateThumbnail(string(out_file), thumbInfo);

	return 0;
}

#ifdef __cplusplus
}
#endif

