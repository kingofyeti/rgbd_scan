#include "imagecompression.h"
#include <Windows.h>

void decompressLog(string filename)
{
	string strFilename(filename);
	strFilename = strFilename.substr(0, strFilename.length() - 4);
	string currentFolder("./");
	currentFolder += strFilename + "/";
	wstring wstrCurrent(currentFolder.begin(), currentFolder.end());
	if (CreateDirectory(wstrCurrent.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
	{
		string outputFolderColor = currentFolder + "rgb/";
		wstring wstrColor(outputFolderColor.begin(), outputFolderColor.end());
		if (CreateDirectory(wstrColor.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
		{
			string outputFolderDepth = currentFolder + "depth/";
			wstring wstrDepth(outputFolderDepth.begin(), outputFolderDepth.end());
			if (CreateDirectory(wstrDepth.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
			{
				string strDepthWindowName("Depth"), strColorWindowName("Color");
				int depthWidth = 0, depthHeight = 0, colorWidth = 0, colorHeight = 0;
				int numFrames = 0, numDevices = 0, frameIdx = 0, depthSize = 0, rgbSize = 0;
				FILE* logFile = fopen(filename.c_str(), "rb");
				if (logFile)
				{
					fread(&numFrames, sizeof(int), 1, logFile);
					fread(&numDevices, sizeof(int), 1, logFile);
					fread(&depthWidth, sizeof(int), 1, logFile);
					fread(&depthHeight, sizeof(int), 1, logFile);
					fread(&colorWidth, sizeof(int), 1, logFile);
					fread(&colorHeight, sizeof(int), 1, logFile);
					unsigned char *rgbData = new unsigned char[colorWidth * colorHeight];
					unsigned char *depthData = new unsigned char[depthWidth * depthHeight * 2];
					int numDigits = getNumberOfDigits(numFrames);
					for (int i = 0; i < numFrames; ++i)
					{
						for (int j = 0; j < numDevices; ++j)
						{
							cout << "Frame " << i << ", Device " << j << "..." << endl;

							fread(&frameIdx, sizeof(int), 1, logFile);
							fread(&depthSize, sizeof(int), 1, logFile);
							fread(&rgbSize, sizeof(int), 1, logFile);

							memset(depthData, 0, depthWidth * depthHeight * 2);

							// Decompress depth image
							unsigned char *depthDataBinary = new unsigned char[depthSize];
							fread(depthDataBinary, depthSize, 1, logFile);
							unsigned long len = (unsigned long)(depthWidth * depthHeight * 2);
							int res = uncompress(depthData, &len, depthDataBinary, (unsigned long)depthSize);
							delete[]depthDataBinary;

							// Scale depth image
							depthDataBinary = NULL;
							cv::Mat mImageDepth(depthHeight, depthWidth, CV_16UC1, (void *)depthData);
							cv::Mat mScaledDepth;
							mImageDepth.convertTo(mScaledDepth, CV_16UC1, c_depthScaleFactor);
							//cv::flip(mScaledDepth, mScaledDepth, 1);

							// Get filename of depth image
							string depthImageName(outputFolderDepth);
							string strName;
							for (int k = 0; k != numDigits; ++k)
							{
								strName += "0";
							}
							string temp = to_string((long long)i);
							size_t val = temp.length();
							strName.replace(numDigits - val, val, temp);
							strName = to_string((long long)j) + "_" + strName + ".png";
							depthImageName += strName;
							
							// Write depth image
							cv::imwrite(depthImageName.c_str(), mScaledDepth);
							
							// Decompress color image
							fread(rgbData, rgbSize, 1, logFile);
							CvMat mat = cvMat(colorHeight, colorWidth, CV_8UC3, rgbData);
							//IplImage *p = cvDecodeImage( &mat, 1 );
							CvMat *p = cvDecodeImageM(&mat, 1);
							cv::Mat m(p);
							cv::cvtColor(m, m, CV_BGR2RGB);

							// Write color image
							//cv::flip(m, m, 1);
							string rgbImageName(outputFolderColor);
							rgbImageName += strName;
							imwrite(rgbImageName.c_str(), m);
						}
					}
					fclose(logFile);
					delete[]rgbData;
					delete[]depthData;
				}
				else
				{
					cerr << "WARNING: cannot open the file " << filename << endl;
				}
			}
		}
		else
		{
			// Failed to create directory.
			cout << "WARNING: cannot create the directory for saving decompressed images." << endl;
			return;
		}
	}
	else
	{
		// Failed to create directory.
		cout << "WARNING: cannot create the directory for saving decompressed images." << endl;
		return;
	}
}