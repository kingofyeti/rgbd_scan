#include "RGBDSensor.h"

RGBDSensor::RGBDSensor()
{
	m_colorWidth = m_colorHeight = 0;
	m_depthWidth = m_depthHeight = 0;
	m_flagInitSuccessful = m_flagScan = false;
	m_frameIndex = 0;
}

RGBDSensor::~RGBDSensor()
{

}