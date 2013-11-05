#pragma once

#include <iostream>
#include <fstream>
#include <opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <io.h>
using namespace std;
using namespace cv;

#define		DEPTH				IPL_DEPTH_32F	//载入图像的图像深度
#define		CHANNELS			1				//载入图像的频道数
#define		WIDTHPIC			1280			//载入图像的像素宽度
#define		HEIGHTPIC			720				//载入图像的像素高度
const int WIDTHROOM=4770;
const int LENGTHROOM=7475;
//Just some convienience macros
#define CV_CVX_WHITE    CV_RGB(0xff,0xff,0xff)
#define CV_CVX_BLACK    CV_RGB(0x00,0x00,0x00)

typedef vector<vector<Point> > cvContours;
typedef struct
{
	int contourIndex1;
	int contourIndex2;
	int validMinX;
	int validMaxX;
	int validMinY;
	int validMaxY;
}matchIndex;

typedef struct
{
	int MinX;
	int MaxX;
	int MinY;
	int MaxY;
}contourRange;


//鼠标取点xy坐标/响应函数
void on_mouse( int event, int x, int y, int flags, void* ustc);

class staticObstacleLocation
{
public:
	matchIndex matchedContourPair;
	cvContours contours1;

	Mat src108;
	char fileName108[1000];
	char fileName109[1000];
	char resultsName[1000];

public:

	staticObstacleLocation();
	~staticObstacleLocation();
	//校正
	void calibration(IplImage* imgSrc);
	//均光
	int uniformLightness(Mat &matSrc);
	
	//坐标变换
	void coordinateTransform(cvContours& contours_figure,int num_cam,string saveName);
	void pointTransform(cvContours& contours, Mat warpMat,string saveName);
	void getContourIntersection(cvContours cont108,cvContours cont109,int singleTestOrNot);
	//鼠标取点xy坐标
	void getXY();
	//滑块拖动响应函数
	void thresh_callback(int, void* );
	void ConnectedComponents(Mat &mask_process, int poly1_hull0, float perimScale, int number,Rect &bounding_box, Point &contour_centers);
	cvContours& getContours(Mat matCut_S,string saveName);
	void getRealAndTest(string saveName);
	contourRange getContourRange(vector<Point> Contour);
	bool matchContours(contourRange Contour108,contourRange Contour109,int i,int j,int sizeCont108,int sizeCont109);
	int batchTesting(string picture108Path,string picture109Path,string resultsPath);
	void singleTest(const char * picture108,const char * picture109);
};