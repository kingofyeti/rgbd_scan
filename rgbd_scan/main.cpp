#include <iostream>
#include "IntelRSSensor.h"


const int colorWidth = 1920;
const int colorHeight = 1080;
//const int colorWidth = 640;
//const int colorHeight = 480;
const int depthWidth = 640;
const int depthheight = 480;
const int cst_fps = 30;

void printUsage(){
	cout << "Usage: " << endl
		<< "   rgbd_scan -camera -option [filename] [flagShowImage]" << endl << endl;
	cout << "-camera:" << endl
		<< "   -asus: Asus XTion" << endl
		<< "   -kinect: Microsoft Kinect" << endl
		<< "   -intel: Intel RealScene R200" << endl;
	cout << "-option:" << endl
		<< "   -c: use depth devices to scan and compress RGBD data into a log file in .klg format" << endl
		<< "   -d: decompress the klg file into png images and save them into local folder" << endl;
	cout << "filename: " << endl
		<< "   klg filename to be compressed or decompressed. Default filename is \"saved.klg\"" << endl;
	cout << "flagShowImage: " << endl
		<< "   flag for showing images (true by default) " << endl << endl;
}

int main(int argc, char** argv)
{
	if (argc < 3 || argc > 5){
		printUsage();
		return EXIT_FAILURE;
	}
	string cameraType(argv[1]), option(argv[2]), logFilename("saved.klg");
	bool flagShowImage = false;
	if (argc == 4)
		logFilename = string(argv[3]);
	if (argc == 5){
		string flag(argv[4]);
		if (flag == "true")
			flagShowImage = true;
		else if (flag != "false"){
			printUsage();
			return EXIT_FAILURE;
		}
	}
	if (option == "-c"){
		RGBDSensor *rgbdsensor = nullptr;
		if (cameraType == "-asus"){

		}
		else if (cameraType == "-kinect"){

		}
		else if (cameraType == "-intel"){
			rgbdsensor = new IntelRSSensor(logFilename);
		}
		else{
			printUsage();
			return EXIT_FAILURE;
		}

		if (colorWidth > 640)
			flagShowImage = false;
		rgbdsensor->init(colorWidth, colorHeight, depthWidth, depthheight, cst_fps, flagShowImage);
		rgbdsensor->runScanning();
	}
	else if (option == "-d"){

	}
	else{
		printUsage();
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}