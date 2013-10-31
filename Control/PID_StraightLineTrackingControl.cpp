// PID_StraightLineTrackingControl.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
#include<string>
#include<vector>
#include <math.h>
#include<windows.h>
#include <Mmsystem.h>
#include <fstream>
#pragma comment(lib, "winmm.lib")

using namespace std;

typedef struct
{
	double x;
	double y;
}point;

typedef struct
{
	double T_coordinate[2][1];
	double angle;
}Estimatedwheelchairpos;

ofstream fout("data.txt");
Estimatedwheelchairpos InitialPose;
Estimatedwheelchairpos CurrentPose;
vector<point> TrajectoryPoint;
double AngularVelocity=0;
double LinearVelocity=0;
double Derrj=0;
double Derrt=0;
double Derrk=0;
double DistanceDot2lineendpt=0;
double Ts,Kp,Ki,Kd;
vector<point>::size_type i=1;

double DistanceDot2Line(point& pt,point& linestartpt,point& lineendpt)
{
	double x0,y0,x1,y1,x2,y2;
	x0=pt.x;
	y0=pt.y;
	x1=linestartpt.x;
	y1=linestartpt.y;
	x2=lineendpt.x;
	y2=lineendpt.y;
	return ((y2-y1)*(x0-x1)-(x2-x1)*(y0-y1))/sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
}

double PID_AngularVelocity(Estimatedwheelchairpos& CurrentPose,point& linestartpt,point& lineendpt,double& Ts,double& Kp,double& Ki,double& Kd)
{
	double AngularVelocityK;
	point PreviewPoint;

	PreviewPoint.x=CurrentPose.T_coordinate[0][0]+0.5*cos(CurrentPose.angle);
	PreviewPoint.y=CurrentPose.T_coordinate[1][0]+0.5*sin(CurrentPose.angle);
	Derrk=DistanceDot2Line(PreviewPoint,linestartpt,lineendpt);

	AngularVelocityK=Kp*Derrk+Ki*Ts*Derrt+Kd/Ts*(Derrk-Derrj);
	Derrj=Derrk;
	if(AngularVelocityK>0.8)
		AngularVelocityK=0.8;
	if(AngularVelocityK<-0.8)
		AngularVelocityK=-0.8;
	return AngularVelocityK;
}
void GetCurrentPose()
{
	double step=0.5;
	CurrentPose.angle=CurrentPose.angle+AngularVelocity*step;
	CurrentPose.T_coordinate[0][0]=CurrentPose.T_coordinate[0][0]+LinearVelocity*cos(CurrentPose.angle)*step;
	CurrentPose.T_coordinate[1][0]=CurrentPose.T_coordinate[1][0]+LinearVelocity*sin(CurrentPose.angle)*step;
//	fout.precision(10);
	fout<<CurrentPose.T_coordinate[0][0]<<',';
	fout<<CurrentPose.T_coordinate[1][0]<<';';
}
void SendVA()
{
	cout<<"AngularVelocity:"<<AngularVelocity<<endl;
}
void CALLBACK StraightLineTrackingControl( UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2) 
{
	GetCurrentPose();
	DistanceDot2lineendpt=sqrt((TrajectoryPoint[i].x-CurrentPose.T_coordinate[0][0])*(TrajectoryPoint[i].x-CurrentPose.T_coordinate[0][0])
		+(TrajectoryPoint[i].y-CurrentPose.T_coordinate[1][0])*(TrajectoryPoint[i].y-CurrentPose.T_coordinate[1][0]));
	if(i!=TrajectoryPoint.size()-1)
	{
		if(DistanceDot2lineendpt<0.3)
			++i;
	}
	else
	{
		if(DistanceDot2lineendpt<0.05)
		{
			fout.close();
			exit(0);
		}
	}
	cout<<i;
	Ts=0.5;
	Kp=5;
	Ki=0.001;
	Kd=0.011;
	LinearVelocity=0.2;
	AngularVelocity=PID_AngularVelocity(CurrentPose,TrajectoryPoint[i-1],TrajectoryPoint[i],Ts,Kp,Ki,Kd);
	SendVA();
}
int main()
{
	//跟踪轨迹
	point point0,point1,point2,point3,point4,point5;
	point0.x=0; point0.y=0;
	point1.x=2; point1.y=2;
	point2.x=2; point2.y=4;
	point3.x=4; point3.y=4;
	point4.x=4; point4.y=2;
	point5.x=2; point5.y=2;
	TrajectoryPoint.push_back(point0);
	TrajectoryPoint.push_back(point1);
	TrajectoryPoint.push_back(point2);
	TrajectoryPoint.push_back(point3);
	TrajectoryPoint.push_back(point4);
	TrajectoryPoint.push_back(point5);
	//初始状态
	CurrentPose.T_coordinate[0][0]=1;
	CurrentPose.T_coordinate[1][0]=0;
	CurrentPose.angle=0;
	//500ms执行一次
	timeSetEvent( 50, 0, StraightLineTrackingControl, 0, (UINT)TIME_PERIODIC);
	getchar();
	return 0;
}

