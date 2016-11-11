#ifndef RGBDSENSOR_H
#define RGBDSENSOR_H

#include <iostream>
#include <string>

using namespace std;

class RGBDSensor{
public:
	RGBDSensor();

	virtual ~RGBDSensor();

	virtual void init(int colorWidth, int colorHeight, int depthWidth, int depthHeight, int fps, bool flagShowImage = false) = 0;

	virtual void runScanning() = 0;

protected:
	int m_colorWidth, m_colorHeight;
	int m_depthWidth, m_depthHeight;
	int m_fps;
	int m_numFrames;
	int m_numDevices;
	int m_frameIndex;
	string m_logFilename;
	bool m_flagInitSuccessful;
	bool m_flagScan;
	bool m_flagShowImage;
	float m_depthScaleFactor;
};

#endif