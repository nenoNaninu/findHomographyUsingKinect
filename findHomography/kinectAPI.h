#pragma once

#include<Kinect.h>
#include<vector>
#include<iostream>
#include<sstream>
#include<opencv2\core.hpp>

class KinectAPI
{

private:

	IKinectSensor* kinectSensor = nullptr;
	IDepthFrameReader* depthFrameReader = nullptr;

	std::vector<UINT16> depthBuffer;

	cv::Mat depthImage;

	const char* DepthWindowName = "Depth Image";

	int depthWidth;
	int depthHight;



public:
	void initialize();

	void queryImage(cv::Mat& mat);

	void upDateImage();

	int getWidth();
	int getHight();


};