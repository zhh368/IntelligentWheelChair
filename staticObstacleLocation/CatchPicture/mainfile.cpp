#include <iostream>
#include "CapturePicture.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <string.h>
using namespace std;
using namespace cv;
void main(void)
{
	CapturePicture capturepicture;
	capturepicture.InitNetSDK();
	string str1="D:\\video\\108.bmp";
	capturepicture.pFileName=str1.data();
	capturepicture.Login("192.168.1.108");
	capturepicture.OnRadioSnaptime();
	capturepicture.OnBtnStartsnap();
	_sleep(0.3*1000);
	string str2="D:\\video\\109.bmp";
	capturepicture.pFileName=str2.data();
	capturepicture.Login("192.168.1.109");
	capturepicture.OnRadioSnaptime();
	capturepicture.OnBtnStartsnap();
	_sleep(0.3*1000);
}