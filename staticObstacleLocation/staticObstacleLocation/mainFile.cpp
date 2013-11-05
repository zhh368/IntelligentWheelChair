#include "staticObstacleLocation.h"
#include "CapturePicture.h"

int main()
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

	string picture108Path = "..\\images108";
	string resultsPath = "..\\results";
	string picture109Path = "..\\images109";
	string picture108 = "108.bmp";
	string picture109 = "109.bmp";
	staticObstacleLocation laboratory;
	laboratory.singleTest(picture108.data(),picture109.data());
	//laboratory.batchTesting(picture108Path,picture109Path,resultsPath);
	return 0;
}