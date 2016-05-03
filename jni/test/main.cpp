/*
 * main.cpp
 *
 *  Created on: 2016Äê4ÔÂ28ÈÕ
 *      Author: loushuai
 */

#include <iostream>
#include "ffthumbnailer.h"

using namespace std;
using namespace ffthumbnailer;

int main(int argc, char *argv[])
{
	string input, output;
	int format = 0 , width = -1, height = -1, time = -1, duration = -1;

	if (argc % 2 == 0) {
		cout << "Usage: test -i input -o output [-f format] [-w width] [-h height] [-t time] [-d duration]" << endl;
		return -1;
	}

	for (int i = 1; i < argc;) {
		if (!strcmp(argv[i], "-i")) {
			input = string(argv[i+1]);
		} else if (!strcmp(argv[i], "-o")) {
			output = string(argv[i+1]);
		} else if (!strcmp(argv[i], "-f")) {
			format = atoi(argv[i+1]);
		} else if (!strcmp(argv[i], "-w")) {
			width = atoi(argv[i+1]);
		} else if (!strcmp(argv[i], "-h")) {
			height = atoi(argv[i+1]);
		} else if (!strcmp(argv[i], "-t")) {
			time = atoi(argv[i+1]);
		} else if (!strcmp(argv[i], "-d")) {
			duration = atoi(argv[i+1]);
		} else {
			cout << "Invalid parameter" << endl;
			return -1;
		}

		i += 2;
	}

	if (input.empty() || output.empty()) {
		cout << "No input or output" << endl;
		return -1;
	}

	ThumbInfo thumbInfo;
	thumbInfo.width    = width;
	thumbInfo.height   = height;
	thumbInfo.time     = time;
	thumbInfo.duration = duration;

	FFThumbnailer thumbnailer(input, format);
	thumbnailer.generateThumbnail(output, thumbInfo);

	return 0;
}


