#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "highgui.h"
#include <iostream>

using namespace cv;
using namespace std;

int main()
{

	VideoCapture cap(0);

	if(!cap.isOpened())
	{
		cout << "Cannot load the image\n";
		return -1;
	}

	namedWindow("original + Tracking", CV_WINDOW_NORMAL);
	namedWindow("Binary_Image",CV_WINDOW_NORMAL);

	//for blue color
	int lowH = 75;
	int highH = 160;

	int lowS = 50;
	int highS = 200;

	int lowV = 50;
	int highV = 220;

	//for red color
	int lowH1 = 170;
	int highH1 = 179;

	int lowS1 = 111;
	int highS1 = 192;

	int lowV1 = 70;
	int highV1 = 199;

	int x_blue = -1;
	int y_blue = -1;

	Mat temp_img;
	cap.read(temp_img);

	Mat lineimg = Mat::zeros(temp_img.size(), CV_8UC3);

	while(1)
	{
		Mat frame;
		bool can = cap.read(frame);

		if(!can)
		{
			cout << "cannot load the frame\n";
			break;
		}
		Mat img_HSV;
		cvtColor(frame, img_HSV, CV_BGR2HSV);
		Mat threshhold_img_blue, threshhold_img_red;

		inRange(img_HSV, Scalar(lowH, lowS, lowV), Scalar(highH, highS, highV), threshhold_img_blue);
		inRange(img_HSV, Scalar(lowH1, lowS1, lowV1), Scalar(highH1, highS1, highV1), threshhold_img_red);

		//Morphological opening and closing for blue object
		erode(threshhold_img_blue, threshhold_img_blue, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(threshhold_img_blue, threshhold_img_blue, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		dilate(threshhold_img_blue, threshhold_img_blue, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(threshhold_img_blue, threshhold_img_blue, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//Morphological opening and closing for red object
		erode(threshhold_img_red, threshhold_img_red, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(threshhold_img_red, threshhold_img_red, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		dilate(threshhold_img_red, threshhold_img_red, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(threshhold_img_red, threshhold_img_red, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//to move the cursor
		Moments om = moments(threshhold_img_blue);
		double m01_blue = om.m01;
		double m10_blue = om.m10;
		double area_blue = om.m00;

		if(area_blue > 400)
		{
			int posx = m10_blue/area_blue;
			int posy = m01_blue/area_blue;

			system(("xdotool mousemove " + to_string(posx) + " " + to_string(posy)).c_str());

			if(x_blue >= 0 and y_blue >= 0 and posx >= 0 and posy >= 0)
			{
				line(lineimg, Point(posx, posy), Point(x_blue, y_blue), Scalar(200,1,0), 7, 3);
			}

			x_blue = posx;
			y_blue = posy;

		}

		//to click the right button of mouse
		Moments om_red = moments(threshhold_img_red);
		double area_red = om_red.m00;

		if(area_red > 400)
		{

			system(("xdotool mousemove " + to_string(x_blue) + " " + to_string(y_blue) + " " + "click " + to_string(1)).c_str());
		}

		frame = frame + lineimg;

		Mat threshhold_img = threshhold_img_red + threshhold_img_blue;
		imshow("original + Tracking", frame);
		imshow("Binary_Image", threshhold_img);

		if(waitKey(30) == 27)
		{
			cout << "Pressed esc\n";
			break;
		}
	}
	return 0;
}