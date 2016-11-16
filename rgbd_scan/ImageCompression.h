#ifndef _IMAGECOMPRESSION_H
#define _IMAGECOMPRESSION_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <zlib\zlib.h>
#include "global.h"

using namespace cv;
using namespace std;

// Decompress the .klg log file
void decompressLog(string filename);

template <class T>
int getNumberOfDigits(T number)
{
	int digits = 0;
	if (number < 0)
		digits = 1; // remove this line if '-' counts as a digit
	while (number) 
	{
		number /= 10;
		digits++;
	}
	return digits;
}


class ImageCompression
{
public:
	ImageCompression(int inWidth, int inHeight){
		//depth_compress_buf_size = inWidth * inHeight * sizeof(int16_t) * 4;
		//depth_compress_buf = (uint8_t*)malloc(depth_compress_buf_size);
		depth_compress_buf_size = inWidth * inHeight * sizeof(unsigned char) * 2;
		depth_compress_buf = new unsigned char[depth_compress_buf_size];
		encodedImage = 0;
	}

	~ImageCompression(){
		delete []depth_compress_buf;
		//free(depth_compress_buf);
		if(encodedImage != 0)
			cvReleaseMat(&encodedImage);
	}

	void encodeJpeg(cv::Vec<unsigned char, 3> * rgb_data, int width, int height){
		cv::Mat3b rgb(height, width, rgb_data, width * 3);
		IplImage * img = new IplImage(rgb);
		int jpeg_params[] = {CV_IMWRITE_JPEG_QUALITY, 90, 0};
		if(encodedImage != 0)
			cvReleaseMat(&encodedImage);
		encodedImage = cvEncodeImage(".jpg", img, jpeg_params);
		delete img;
	}

	void compressDepth(unsigned char* dataptr, int width, int height, unsigned long &depthSize){
		int res = compress2(depth_compress_buf, &depthSize, dataptr, width * height * sizeof(short), Z_BEST_SPEED);
		//int res = compress(depth_compress_buf, &depthSize, (const Bytef*)dataptr, width * height * sizeof(short));
		if (res != 0)
			printf ("WARNING: Compression Error !\n");
	}


public:
	//uint8_t *rgbBuffer;
	//uint8_t *depthBuffer;
	int depth_compress_buf_size;
	unsigned char * depth_compress_buf;
	CvMat * encodedImage;
};



#endif