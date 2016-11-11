#include "IntelRSSensor.h"

IntelRSSensor::IntelRSSensor(string logFilename)
{
	m_logFilename = logFilename;
}

IntelRSSensor::~IntelRSSensor()
{
	
}

void IntelRSSensor::init(int colorWidth, int colorHeight, int depthWidth, int depthHeight, int fps, bool flagShowImage)
{
	m_colorWidth = colorWidth;
	m_colorHeight = colorHeight;
	m_depthWidth = depthWidth;
	m_depthHeight = depthHeight;
	m_fps = fps;

	m_flagInitSuccessful = true;
	m_flagScan = true;
	m_numFrames = 100;
	m_numDevices = 1; 
	m_frameIndex = 0;
	m_flagShowImage = flagShowImage;	
}

void IntelRSSensor::runScanning()
{
	// Create a context object. This object owns the handles to all connected realsense devices.
	rs::context ctx;
	printf("There are %d connected RealSense devices.\n", ctx.get_device_count());
	if (ctx.get_device_count() == 0) return;

	// This tutorial will access only a single device, but it is trivial to extend to multiple devices
	rs::device * dev = ctx.get_device(0);
	printf("\nUsing device 0, an %s\n", dev->get_name());
	printf("    Serial number: %s\n", dev->get_serial());
	printf("    Firmware version: %s\n", dev->get_firmware_version());

	// Configure depth and color to run with the device's preferred settings
	//dev->enable_stream(rs::stream::depth, rs::preset::best_quality);
	//dev->enable_stream(rs::stream::color, rs::preset::best_quality);
	dev->enable_stream(rs::stream::depth, m_depthWidth, m_depthHeight, rs::format::z16, m_fps);
	dev->enable_stream(rs::stream::color, m_colorWidth, m_colorHeight, rs::format::rgb8, m_fps);
	dev->start();

	// Create image compression object
	ImageCompression* imageComp = new ImageCompression(m_depthWidth, m_depthHeight);

	// Open log file to compress
	FILE* logFile = fopen(m_logFilename.c_str(), "wb+");
	fwrite(&m_frameIndex, sizeof(int32_t), 1, logFile); // write the number of frames as 0 in the head and update it later after scanning
	fwrite(&m_numDevices, sizeof(int32_t), 1, logFile); // write the number of devices
	fwrite(&m_depthWidth, sizeof(int32_t), 1, logFile);
	fwrite(&m_depthHeight, sizeof(int32_t), 1, logFile);
	fwrite(&m_colorWidth, sizeof(int32_t), 1, logFile);
	fwrite(&m_colorHeight, sizeof(int32_t), 1, logFile);

	// OpenCV variables to show images
	string strDepthWindowName("Depth"), strColorWindowName("Color");
	if (m_flagShowImage)
	{
		cv::namedWindow(strDepthWindowName, CV_WINDOW_AUTOSIZE);
		cv::namedWindow(strColorWindowName, CV_WINDOW_AUTOSIZE);
	}

	// Image retrieval and compression process
	unsigned long depthSize = 0;
	while (true){

		dev->wait_for_frames();

		// Retrieve color image
		const uint8_t * color_image = (const uint8_t *)dev->get_frame_data(rs::stream::color);		

		// Covert to opencv variable and show
		if (m_flagShowImage)
		{
			cv::Mat mImageRGB(m_colorHeight, m_colorWidth, CV_8UC3, (void*)color_image);
			cv::Mat cImageBGR;
			cv::cvtColor(mImageRGB, cImageBGR, CV_RGB2BGR);
			cv::imshow(strColorWindowName, cImageBGR);
		}
		
		// Compress color image 
		imageComp->encodeJpeg((cv::Vec<unsigned char, 3> *)color_image, m_colorWidth, m_colorHeight);

		// Retrieve depth image
		//const uint16_t * depth_image = (const uint16_t *)dev->get_frame_data(rs::stream::depth);
		const unsigned char *depth_image = (const unsigned char *)dev->get_frame_data(rs::stream::depth);


		// Convert to opencv variable and show
		if (m_flagShowImage)
		{
			cv::Mat mImageDepth(m_depthHeight, m_depthWidth, CV_16UC1, (unsigned char*)depth_image);
			cv::Mat mScaledDepth;
			mImageDepth.convertTo(mScaledDepth, CV_16UC1, c_depthScaleFactor); // with scale
			cv::imshow(strDepthWindowName, mScaledDepth);
		}

		// Compress depth image
		depthSize = imageComp->depth_compress_buf_size;
		imageComp->compressDepth((unsigned char*)depth_image, m_depthWidth, m_depthHeight, depthSize);

		int32_t rgbSize = imageComp->encodedImage->width;
		unsigned char * rgbData = (unsigned char *)imageComp->encodedImage->data.ptr;
		unsigned char * depthData = (unsigned char *)imageComp->depth_compress_buf;
		logData(logFile, (int32_t *)&m_frameIndex, (int32_t *)&depthSize, &rgbSize, depthData, rgbData); // write compressed depth and color data for each device
		cout << "Frame " << m_frameIndex << "..." << endl;

		// Check keyboard
		if (m_flagShowImage && cv::waitKey(1) == 27)
		{
			fseek(logFile, 0, SEEK_SET);
			fwrite(&m_frameIndex, sizeof(int32_t), 1, logFile);
			fflush(logFile);
			fclose(logFile);
			break;
		}
		else
		{
			if (m_frameIndex >= m_numFrames)
			{
				fseek(logFile, 0, SEEK_SET);
				fwrite(&m_frameIndex, sizeof(int32_t), 1, logFile);
				fflush(logFile);
				fclose(logFile);
				break;
			}			
		}
		m_frameIndex++;
	}

}

void IntelRSSensor::logData(FILE* logFile,
	int32_t * timestamp,
	int32_t * depthSize,
	int32_t * rgbSize,
	unsigned char * depthData,
	unsigned char * rgbData)
{
	fwrite(timestamp, sizeof(int32_t), 1, logFile);
	fwrite(depthSize, sizeof(int32_t), 1, logFile);
	fwrite(rgbSize, sizeof(int32_t), 1, logFile);
	fwrite(depthData, *depthSize, 1, logFile);
	fwrite(rgbData, *rgbSize, 1, logFile);
}
