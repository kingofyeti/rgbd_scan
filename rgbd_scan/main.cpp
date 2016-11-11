#include <iostream>
#include "IntelRSSensor.h"
#include "global.h"

void printUsage(){
	cout << "Usage: " << endl
		<< "   rgbd_scan -option [-camera] [filename] [flagShowImage]" << endl << endl;
	cout << "-option:" << endl
		<< "   -c: use depth devices to scan and compress RGBD data into a log file in .klg format" << endl
		<< "   -d: decompress the klg file into png images and save them into local folder" << endl;
	cout << "-camera:" << endl
		<< "   -asus: Asus XTion" << endl
		<< "   -kinect: Microsoft Kinect" << endl
		<< "   -intel: Intel RealScene R200" << endl;
	cout << "filename: " << endl
		<< "   klg filename to be compressed or decompressed. Default filename is \"saved.klg\"" << endl;
	cout << "flagShowImage: " << endl
		<< "   flag for showing images (true by default) " << endl << endl;
}

int main(int argc, char** argv)
{
	if (argc < 2 || argc > 5){
		printUsage();
		return EXIT_FAILURE;
	}
	string option(argv[1]), logFilename("saved.klg");
	if (option == "-c")
	{
		string cameraType(argv[2]);
		bool flagShowImage = false;
		if (argc == 4)
			logFilename = string(argv[3]);
		if (argc == 5)
		{
			string flag(argv[4]);
			if (flag == "true")
				flagShowImage = true;
			else if (flag != "false")
			{
				printUsage();
				return EXIT_FAILURE;
			}
		}

		RGBDSensor *rgbdsensor = nullptr;
		if (cameraType == "-asus")
		{

		}
		else if (cameraType == "-kinect")
		{

		}
		else if (cameraType == "-intel")
			rgbdsensor = new IntelRSSensor(logFilename);
		else{
			printUsage();
			return EXIT_FAILURE;
		}

		if (c_colorWidth > 640)
			flagShowImage = false;
		rgbdsensor->init(c_colorWidth, c_colorHeight, c_depthWidth, c_depthheight, c_fps, flagShowImage);
		rgbdsensor->runScanning();
		delete rgbdsensor;
	}
	else if (option == "-d")
	{
		if (argc == 3)
			logFilename = string(argv[2]);
		decompressLog(logFilename);
	}
	else
	{
		printUsage();
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}