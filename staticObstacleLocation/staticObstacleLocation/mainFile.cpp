#include "staticObstacleLocation.h"
#include "CapturePicture.h"
#include <conio.h>
#define ESC	0x1b		// scan code for ESC character
#define GetOnePicture	0x6f//o
#define SingleTest	0x73//s
#define BatchTest	0x62//b
#define GetPictures	0x67//g
int main()
{
	string NamePicture;
	int keypress = 0;
	staticObstacleLocation laboratory;
	CapturePicture capturepicture;
	string str1,str2;
	string picture108Path = "..\\images108";
	string resultsPath = "..\\results";
	string picture109Path = "..\\images109";
	string picture108 = "108.bmp";
	string picture109 = "109.bmp";
	cout<<"        === ORDER HELP ===      "<<endl;
	cout<<"o    - Catching A Pair Pictures Named <108.bmp><109.bmp>!"<<endl;
	cout<<"g    - Catching A Pair Pictures Named Your Enter!"<<endl;
	cout<<"s    - Test The Single Pair Pictures Named <108.bmp><109.bmp>!"<<endl;
	cout<<"b    - Test The Batch Pictures In The Folder!"<<endl;
	do
	{
		cout<<"Name Of Order:"<<endl;
		keypress = getch();
		switch(keypress)
		{
		case GetOnePicture:
			cout<<"A Pair Named <108.bmp><109.bmp>! Is Catching!:"<<endl;
			str1="108.bmp";
			capturepicture.InitNetSDK();
			capturepicture.pFileName=str1.data();
			capturepicture.Login("192.168.1.108");
			capturepicture.OnRadioSnaptime();
			capturepicture.OnBtnStartsnap();
			_sleep(0.3*1000);
			capturepicture.Logout();
			str2="109.bmp";
			capturepicture.InitNetSDK();
			capturepicture.pFileName=str2.data();
			capturepicture.Login("192.168.1.109");
			capturepicture.OnRadioSnaptime();
			capturepicture.OnBtnStartsnap();
			_sleep(0.3*1000);
			capturepicture.Logout();
			break;
		case SingleTest:
			cout<<"A Single Test Is Going On¡­¡­"<<endl;
			laboratory.singleTest(picture108.data(),picture109.data());
			break;
		case GetPictures:
			cout<<"Please Enter Name Of Picture!:"<<endl;
			cin>>NamePicture;
			str1="..\\images108\\"+NamePicture+".bmp";
			capturepicture.InitNetSDK();
			capturepicture.pFileName=str1.data();
			capturepicture.Login("192.168.1.108");
			capturepicture.OnRadioSnaptime();
			capturepicture.OnBtnStartsnap();
			_sleep(0.3*1000);
			capturepicture.Logout();
			str2="..\\images109\\"+NamePicture+".bmp";
			capturepicture.InitNetSDK();
			capturepicture.pFileName=str2.data();
			capturepicture.Login("192.168.1.109");
			capturepicture.OnRadioSnaptime();
			capturepicture.OnBtnStartsnap();
			_sleep(0.3*1000);
			capturepicture.Logout();
			break;
		case BatchTest:
			cout<<"A Batch Tests Is Going On¡­¡­"<<endl;
			laboratory.batchTesting(picture108Path,picture109Path,resultsPath);
			break;
		default:
			cout<<"Input Error!"<<endl;
			break;
		}
		cout<<endl;
	}while(keypress!=ESC );

	return 0;
}