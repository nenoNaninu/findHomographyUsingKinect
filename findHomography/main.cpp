#include"kinectAPI.h"
#include<opencv2\opencv.hpp>
#include<iostream>
#include<fstream>
#include<vector>
#include<string>

struct MouseParam
{
	unsigned int x;
	unsigned int y;
	int event;
	int flags;
};

void mFunc(int event, int x, int y, int flags, void *param)
{
	MouseParam* mouseParamPtr = static_cast<MouseParam*>(param);
	(*mouseParamPtr).x = x;
	(*mouseParamPtr).y = y;
	(*mouseParamPtr).event = event;
	(*mouseParamPtr).flags = flags;
}


std::vector<std::string> split(const std::string& line, char delimiter)
{
	std::istringstream stream(line);
	std::string filed;
	std::vector<std::string> result;
	while (std::getline(stream,filed,delimiter))
	{
		result.push_back(filed);
	}

	return result;
}


using namespace cv;

int main()
{
	KinectAPI kinect;
	kinect.initialize();

	int imageWidth = kinect.getWidth();
	int imageHight = kinect.getHight();

	cv::Mat depthImage(imageHight, imageWidth, CV_32FC1);
	char* windowName = "kinectImageData";
	cv::namedWindow(windowName);

	MouseParam mouseEvent;
	MouseParam prevMouseEvent;
	cv::setMouseCallback(windowName, mFunc, &mouseEvent);
	std::cout << "click 4 position" << std::endl;
	std::cout << "if you want to select again , press c" << std::endl;
	std::cout << "if you are finish , press f" << std::endl;
	
	int pointCount = 0;
	//Point calibPoint[4];
	std::vector<Point> calibPoint(4);
	while (1)
	{
		int flag = 0;
		pointCount = 0;
		std::cout << "click 4 position" << std::endl;
		while (1)
		{

			char key = cv::waitKey(1);
			kinect.upDateImage();
			kinect.queryImage(depthImage);
			
			if (mouseEvent.event == CV_EVENT_LBUTTONDOWN && prevMouseEvent.event != CV_EVENT_LBUTTONDOWN)
			{
				calibPoint[pointCount] = Point(mouseEvent.x, mouseEvent.y);
				pointCount++;

			}

			for (int i = 0; i < pointCount; i++)
			{
				circle(depthImage, calibPoint[i], 2, Scalar(0), -1);
			}

			for (int i = 1; i <pointCount; i++)
			{
				cv::line(depthImage, calibPoint[i - 1], calibPoint[i], Scalar(0), 1);
				if (i == 3)
				{
					cv::line(depthImage, calibPoint[0], calibPoint[3], Scalar(0), 1);
				}
			}

			if (key == 'f')
			{
				flag = 1;
				break;
			}

			if (key == 'c')
			{
				break;
			}
			prevMouseEvent = mouseEvent;
			imshow(windowName,depthImage);

		}

		if (pointCount == 4 && flag == 1)
		{
			break;
		}

		std::cout << "pointCount" << pointCount << std::endl;
		std::cout << "you have to select again" << std::endl;
	}

	std::cout << "caliculate homography" << std::endl;

	std::ifstream ifs("position.csv");

	std::string line;

	std::vector<Point> unityPositionArray(4);

	int i = 0;
	while (std::getline(ifs,line))
	{
		std::vector<std::string> temp = split(line, ',');
 		
		unityPositionArray[i].x = std::stod(temp[0]);
		unityPositionArray[i].y = std::stod(temp[1]);

		i++;

		if (i == 4)
		{
			break;
		}
	}

	Mat homographyMat =  findHomography(calibPoint, unityPositionArray, 0);

	std::cout << homographyMat << std::endl;
	Mat homography32F;
	homographyMat.convertTo(homography32F, CV_32FC1);

	float* homographyPtr = (float*)homography32F.data;
	
	std::ofstream ofs("homography.csv");
	
	ofs << homographyPtr[0] << "," << homographyPtr[1] << "," << homographyPtr[2] << std::endl;
	ofs << homographyPtr[3] << "," << homographyPtr[4] << "," << homographyPtr[5] << std::endl;
	ofs << homographyPtr[6] << "," << homographyPtr[7] << "," << homographyPtr[8] << std::endl;


	waitKey(0);

	return 0;
}

