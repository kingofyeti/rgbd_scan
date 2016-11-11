#ifndef INTELRSSENSOR_H
#define INTELRSSENSOR_H

#include "RGBDSensor.h"
#include <librealsense/rs.hpp>
#include <zlib\zlib.h>
#include "ImageCompression.h"

template <class T>
int getNumberOfDigits(T number)
{
	int digits = 0;
	if (number < 0) 
		digits = 1; // remove this line if '-' counts as a digit
	while (number) {
		number /= 10;
		digits++;
	}
	return digits;
}

class IntelRSSensor : public RGBDSensor
{
public:
	IntelRSSensor(string logFilename);
	
	~IntelRSSensor();

	void init(int colorWidth, int colorHeight, int depthWidth, int depthHeight, int fps, bool flagShowImage = false);

	void runScanning();

private:
	void logData(FILE* logFile, int32_t * timestamp, int32_t * depthSize, int32_t * rgbSize, unsigned char * depthData, unsigned char * rgbData);

};

#endif