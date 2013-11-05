#include "staticObstacleLocation.h"
Mat src,dst;
staticObstacleLocation::staticObstacleLocation()
{

}
staticObstacleLocation::~staticObstacleLocation()
{

}
//������������
int staticObstacleLocation::batchTesting(string picture108Path,string picture109Path,string resultsPath)
{	
	// ������������
	string fileExtension = "bmp";
	string picture108 = picture108Path + "\\*." + fileExtension;
	string picture109 = picture109Path + "\\*." + fileExtension;
	// �����ļ���

	struct _finddata_t fileInfo108;    // �ļ���Ϣ�ṹ��
	struct _finddata_t fileInfo109;    // �ļ���Ϣ�ṹ��
	long findNext108,findNext109;
	// 1. ��һ�β���
	long findResult108 = _findfirst(picture108.c_str(), &fileInfo108);  
	long findResult109 = _findfirst(picture109.c_str(), &fileInfo109);
	if (findResult108 == -1 || findResult109 == -1)
	{
		_findclose(findResult108); 
		_findclose(findResult109); 
		return -1;
	}
	// 2. ѭ������
	do 
	{
		sprintf(fileName108, "%s\\%s", picture108Path.c_str(), fileInfo108.name);
		IplImage* imgSrc108 = cvLoadImage(fileName108);
		calibration(imgSrc108);
		Mat matSrc(imgSrc108);
		matSrc.copyTo(src108);
		sprintf(fileName109, "%s\\%s", picture109Path.c_str(), fileInfo109.name);
		IplImage* imgSrc109 = cvLoadImage(fileName109);
		calibration(imgSrc109);
		Mat matSrc1(imgSrc109);
		Mat src109;
		matSrc1.copyTo(src109);
		sprintf(resultsName, "%s\\%s", resultsPath.c_str(), fileInfo108.name);
		cvContours contours108,contours109;
		contours108=getContours(src108,"contours108");
		contours109=getContours(src109,"contours109");
		coordinateTransform(contours108,108,"transformedContours108");
		coordinateTransform(contours109,109,"transformedContours109");
		getContourIntersection(contours108,contours109,0);
		getRealAndTest(resultsName);
		//waitKey(0);
		findNext108 = _findnext(findResult108, &fileInfo108);  
		findNext109 = _findnext(findResult109, &fileInfo109);

	} while (!findNext108||!findNext109);  
	_findclose(findResult108); 
	_findclose(findResult109); 
	cout<<"This batch test is complete!"<<endl;
	return 0;
}
//�������
void staticObstacleLocation::singleTest(const char * picture108,const char * picture109)
{	
	IplImage* imgSrc108 = cvLoadImage(picture108);
	calibration(imgSrc108);
	Mat matSrc(imgSrc108);
	matSrc.copyTo(src108);
#if 0
	src=src108;
	getXY();
#endif
	IplImage* imgSrc109 = cvLoadImage(picture109);
	calibration(imgSrc109);
	Mat matSrc1(imgSrc109);
	Mat src109;
	matSrc1.copyTo(src109);
#if 0
	src=src109;
	getXY();
#endif
	cvContours contours108,contours109;
	contours108=getContours(src108,"contours108");
	contours109=getContours(src109,"contours109");
	coordinateTransform(contours108,108,"transformedContours108");
	coordinateTransform(contours109,109,"transformedContours109");
	getContourIntersection(contours108,contours109,1);
	getRealAndTest("realContours");
	cout<<"This single test is complete!"<<endl;
}
//У��
void staticObstacleLocation::calibration(IplImage* imgSrc)
{
	CvSize sizePic = cvSize(WIDTHPIC, HEIGHTPIC);
	CvMat* intrinsic = (CvMat*)cvLoad("..\\Intrinsics.xml");
	CvMat* distortion = (CvMat*)cvLoad("..\\Distortion.xml");
	IplImage* mapx = cvCreateImage(sizePic, DEPTH, CHANNELS);
	IplImage* mapy = cvCreateImage(sizePic, DEPTH, CHANNELS);
	cvInitUndistortMap(intrinsic, distortion, mapx, mapy);
	if(!imgSrc){
		printf("Couldn't open the first picture.Please check it out.\n");
		return;
	}
	IplImage* temp = cvCloneImage(imgSrc);
	cvRemap(temp, imgSrc, mapx, mapy);
}
//����û������
int staticObstacleLocation::uniformLightness(Mat &matSrc)
{
	if( matSrc.empty() ) {
		CV_Error(CV_StsBadArg, "the picture is empty.Please check it out.\n");
		return -1;
	}
	Mat matGaussian;
	matSrc.copyTo(matGaussian);
	int kernel = 31;
	CvSize ksize = cvSize(kernel, kernel);
	double sigma = 0.3*((kernel-1)*0.5 - 1) + 0.8;
	GaussianBlur(matSrc, matGaussian, ksize, sigma);
	Scalar meanSrc = mean(matSrc);
	matSrc = matSrc - matGaussian + meanSrc[0];
	return 0;
}
//����任ʱȡ��Ӧ���ص�
void staticObstacleLocation::getXY()
{
	src.copyTo(dst);
	cvNamedWindow("src",CV_WINDOW_AUTOSIZE );
	cvSetMouseCallback( "src", on_mouse, 0 );
	imshow("src",src);
	cvWaitKey(0);
}
//����任ʱȡ��Ӧ���ص���Ӧ����
void on_mouse( int event, int x, int y, int flags, void* ustc)
{
	static Point pre_pt = (-1,-1);
	static Point cur_pt = (-1,-1);
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, CV_AA);
	char temp[50];

	Vec3b intensity = src.at<Vec3b>(Point(x, y));
	if( event == CV_EVENT_LBUTTONDOWN )
	{
		dst.copyTo(src);
		sprintf(temp,"(%d,%d,%d,%d,%d)",x,y,intensity.val[0],intensity.val[1],intensity.val[2]);
		cout<<x<<";"<<y<<endl;
		pre_pt = cvPoint(x,y);
		putText(src,temp, pre_pt, FONT_HERSHEY_SIMPLEX, 0.5,cvScalar(0,0, 0, 255),1,8);
		circle( src, pre_pt, 3,cvScalar(255,0,0,0) ,CV_FILLED, CV_AA, 0 );
		imshow( "src", src );
		src.copyTo(dst);
	}
	else if( event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))
	{
		dst.copyTo(src);
		sprintf(temp,"(%d,%d,%d,%d,%d)",x,y,intensity.val[0],intensity.val[1],intensity.val[2]);
		cur_pt = cvPoint(x,y);		
		putText(src,temp, cur_pt,FONT_HERSHEY_SIMPLEX, 0.5,cvScalar(0,0, 0, 255),1,8);
		line(src, pre_pt, cur_pt, cvScalar(0,255,0,0), 1, CV_AA, 0 );
		imshow( "src", src );
	}
	else if( event == CV_EVENT_LBUTTONUP )
	{
		dst.copyTo(src);
		sprintf(temp,"(%d,%d,%d,%d,%d)",x,y,intensity.val[0],intensity.val[1],intensity.val[2]);
		cur_pt = Point(x,y);		
		putText(src,temp, cur_pt, FONT_HERSHEY_SIMPLEX, 0.5,cvScalar(0,0, 0, 255),1,8);
		circle( src, cur_pt, 3,cvScalar(255,0,0,0) ,CV_FILLED, CV_AA, 0 );
		line( src, pre_pt, cur_pt, cvScalar(0,255,0,0), 1, CV_AA, 0 );
		imshow( "src", src );
		src.copyTo(dst);
	}
}
//��ȡ͹��
cvContours& staticObstacleLocation::getContours(Mat matCut_S,string saveName)
{
	Mat src_gray,src_binary;
	cvtColor(matCut_S, src_gray,CV_BGR2GRAY);//�ҶȻ�
	//��ֵ��
	threshold(src_gray,src_binary,65,255,THRESH_BINARY_INV);
	//���ö������ϴ���
	ConnectedComponents(src_binary, 0, 8, 1, Rect(), Point(-1, -1));
	/// Find contours
	cvContours contours;
	vector<Vec4i> hierarchy;
	findContours( src_binary, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE , Point(0, 0) );
	contours1=contours;
	for( size_t i = 0; i < contours.size(); i++ )
		approxPolyDP( Mat(contours[i]), contours1[i], 3, true );
	// Draw contours
	Scalar color(255, 0, 0 );
	Mat drawing = Mat::zeros( src_binary.size(), CV_8UC3 );
	ofstream fout5(saveName+".txt");
	for( size_t i = 0; i< contours1.size(); i++ )
	{
		drawContours( matCut_S, contours1, (int)i, color, 2, 8, hierarchy, 0, Point() );
		for( size_t j = 0; j< contours1.at(i).size();j++ )
		{
			fout5<<i<<'.'<<j<<":(";
			fout5<<contours1.at(i)[j].x<<','<<contours1.at(i)[j].y<<")"<<endl;
		}
	}
	fout5.close();
	imwrite(saveName+".bmp", matCut_S);
	return contours1;
}
//��ͨ����ѧϰopencv��P319
void staticObstacleLocation::ConnectedComponents(Mat &mask_process, int poly1_hull0, float perimScale, int number,Rect &bounding_box, Point &contour_centers)
{
	/*����4�������Ϊ�˼���ԭ�����ӿڣ����ڲ�ʹ�õ���c��񣬵�����ӿ���c++����*/
	IplImage *mask1 = &mask_process.operator IplImage();
	int *num = &number;
	CvRect *bbs = &bounding_box.operator CvRect();
	CvPoint *centers = &contour_centers.operator CvPoint();
	static CvMemStorage*    mem_storage    = NULL;
	static CvSeq*            contours    = NULL;
	//CLEAN UP RAW MASK
	//���������ã�ƽ��������ȥ��ϸ��,�Ͽ�ȱ��
	cvMorphologyEx( mask1, mask1, NULL, NULL, CV_MOP_OPEN, 1 );//������mask1���п�������CVCLOSE_ITRΪ�������Ĵ��������Ϊmask1ͼ��
	//���������ã�ƽ������������ȱ��
	cvMorphologyEx( mask1, mask1, NULL, NULL, CV_MOP_CLOSE, 1 );//������mask1���бղ�����CVCLOSE_ITRΪ�ղ����Ĵ��������Ϊmask1ͼ��
	//FIND CONTOURS AROUND ONLY BIGGER REGIONS
	if( mem_storage==NULL ) mem_storage = cvCreateMemStorage(0);
	else cvClearMemStorage(mem_storage);
	//CV_RETR_EXTERNAL=0����types_c.h�ж���ģ�CV_CHAIN_APPROX_SIMPLE=2Ҳ���ڸ��ļ��ж����
	CvContourScanner scanner = cvStartFindContours(mask1,mem_storage,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
	CvSeq* c;
	int numCont = 0;
	//��while�ڲ�ֻ��ԱȽϴ���������߽����滻����
	while( (c = cvFindNextContour( scanner )) != NULL )
	{
		double arear = fabs(cvContourArea(c));
		double len = cvContourPerimeter( c );
		double q = (mask1->height + mask1->width) /perimScale;   //calculate perimeter len threshold
		if( len < q ) //Get rid of blob if it's perimeter is too small
		{
			cvSubstituteContour( scanner, NULL );    //��NULL����ԭ�����Ǹ�����
		}
		else //Smooth it's edges if it's large enough
		{
			CvSeq* c_new;
			if(poly1_hull0)
			{//Polygonal approximation of the segmentation
				c_new = cvApproxPoly(c,sizeof(CvContour),mem_storage,CV_POLY_APPROX_DP, 2,0);
			}
			else //Convex Hull of the segmentation
			{
				c_new = cvConvexHull2(c,mem_storage,CV_CLOCKWISE,1);
				double arear1 = fabs(cvContourArea(c_new));
				if(arear1>5*arear)//2.2
					c_new = cvApproxPoly(c,sizeof(CvContour),mem_storage,CV_POLY_APPROX_DP, 2,0);
			}
			cvSubstituteContour( scanner, c_new ); //�ʼ��������͹�����߶���ʽ��������滻
			numCont++;
		}
	}
	contours = cvEndFindContours( &scanner );    //�����������Ҳ���
	// PAINT THE FOUND REGIONS BACK INTO THE IMAGE
	cvZero( mask1 );
	IplImage *maskTemp;
	//CALC CENTER OF MASS AND OR BOUNDING RECTANGLES
	if(*num != 0)
	{
		int N = *num, numFilled = 0, i=0;
		CvMoments moments;
		double M00, M01, M10;
		maskTemp = cvCloneImage(mask1);
		for(i=0, c=contours; c != NULL; c = c->h_next,i++ )        //h_nextΪ���������е���һ������
		{
			if(i < N) //Only process up to *num of them
			{
				//CV_CVX_WHITE�ڱ��������ǰ�ɫ����˼
				cvDrawContours(maskTemp,c,CV_CVX_WHITE, CV_CVX_WHITE,-1,CV_FILLED,8);
				//Find the center of each contour
				if(centers != &cvPoint(-1, -1))
				{
					cvMoments(maskTemp,&moments,1);    //����mask1ͼ�����ߴ�3�׵ľ�
					M00 = cvGetSpatialMoment(&moments,0,0); //��ȡx��0�κ�y��0�ξ�
					M10 = cvGetSpatialMoment(&moments,1,0); //��ȡx��1�κ�y��0�ξ�
					M01 = cvGetSpatialMoment(&moments,0,1); //��ȡx��0�κ�y��1�ξ�
					centers[i].x = (int)(M10/M00);    //���þصĽ��������������ĵ�����
					centers[i].y = (int)(M01/M00);
				}
				//Bounding rectangles around blobs
				if(bbs != &CvRect())
				{
					bbs[i] = cvBoundingRect(c); //�������c����Ӿ���
				}
				cvZero(maskTemp);
				numFilled++;
			}
			//Draw filled contours into mask1
			cvDrawContours(mask1,c,CV_CVX_WHITE,CV_CVX_WHITE,-1,CV_FILLED,8); //draw to central mask1
		} //end looping over contours
		*num = numFilled;
		cvReleaseImage( &maskTemp);
	}
	//ELSE JUST DRAW PROCESSED CONTOURS INTO THE MASK
	else
	{
		for( c=contours; c != NULL; c = c->h_next )
		{
			cvDrawContours(mask1,c,CV_CVX_WHITE, CV_CVX_BLACK,-1,CV_FILLED,8);
		}
	}
}
//����ת��
void staticObstacleLocation::coordinateTransform(cvContours& contours_figure,int num_cam,string saveName)
{	
	vector<cv::Point2f> corners;
	vector<cv::Point2f> quad_pts;
	if (num_cam==108)
	{
		corners.push_back(cv::Point2f(472, 273));
		corners.push_back(cv::Point2f(732, 280));
		corners.push_back(cv::Point2f(422, 516));
		corners.push_back(cv::Point2f(773, 522));

		quad_pts.push_back(cv::Point2f(1733, 3085));
		quad_pts.push_back(cv::Point2f(2933,3085 ));
		quad_pts.push_back(cv::Point2f(1733,4680 ));
		quad_pts.push_back(cv::Point2f(2933,4680 ));
	}
	else if (num_cam==109)
	{
		corners.push_back(cv::Point2f(826, 442));
		corners.push_back(cv::Point2f(494, 442));
		corners.push_back(cv::Point2f(783, 218));
		corners.push_back(cv::Point2f(530, 220));

		quad_pts.push_back(cv::Point2f(1733, 3085));
		quad_pts.push_back(cv::Point2f(2933,3085 ));
		quad_pts.push_back(cv::Point2f(1733,4680 ));
		quad_pts.push_back(cv::Point2f(2933,4680 ));
	}
	else
		cout<<"�������������"<<endl;
	Mat transmtx = getPerspectiveTransform(corners, quad_pts);
	pointTransform(contours_figure,transmtx,saveName);
}
//����任ʦ�ܳ�����
void staticObstacleLocation::pointTransform(cvContours& contours, Mat warpMat,string saveName)
{
	double temp1 = 0;//x'*temp
	double temp2 = 0;//y'*temp
	double temp = 0;
	double x=0;
	double y=0;
	//����Ϊ����ת���Ĺ�ʽ���ɴ˵ó������
	// t*x'                  x
	// t*y'   =   warpMat *  y
	//  t                    1
	//printf("transform matrix\n");
	ofstream fout5(saveName+".txt");
	for( size_t i = 0; i< contours.size(); i++ )
	{
		for( size_t j = 0; j< contours.at(i).size();j++ )
		{
			temp1 = warpMat.at<double>(0,0)*contours.at(i)[j].x + warpMat.at<double>(0,1)*contours.at(i)[j].y + warpMat.at<double>(0,2);
			temp2 = warpMat.at<double>(1,0)*contours.at(i)[j].x + warpMat.at<double>(1,1)*contours.at(i)[j].y + warpMat.at<double>(1,2);
			temp  = warpMat.at<double>(2,0)*contours.at(i)[j].x + warpMat.at<double>(2,1)*contours.at(i)[j].y + warpMat.at<double>(2,2);
			x=temp1/temp;
			y=temp2/temp;
			contours.at(i)[j].x = x>WIDTHROOM ? WIDTHROOM : x;
			contours.at(i)[j].x = contours.at(i)[j].x<0 ? 0 : contours.at(i)[j].x;
			contours.at(i)[j].y = y>LENGTHROOM ? LENGTHROOM : y;
			contours.at(i)[j].y = contours.at(i)[j].y<0 ? 0 : contours.at(i)[j].y;
			contours.at(i)[j].x = contours.at(i)[j].x/10;
			contours.at(i)[j].y = contours.at(i)[j].y/10;
			fout5<<contours.at(i)[j].x<<','<<contours.at(i)[j].y<<";";
		}
		fout5<<endl;
	}
	fout5.close();

}
//��ȡ����contours�Ĺ�������
void staticObstacleLocation::getContourIntersection(cvContours cont108,cvContours cont109,int singleTestOrNot)
{
	vector<Point> deadZone108,deadZone109;
	vector<matchIndex> matchedContourPairs;
	contourRange cont108Index;
	contourRange cont109Index;
	int test=0;
	//deadZone108.push_back(cv::Point(0, 0));
	//deadZone108.push_back(cv::Point(477, 0));
	//deadZone108.push_back(cv::Point(477, 200));
	//deadZone108.push_back(cv::Point(420, 55));
	//deadZone108.push_back(cv::Point(70, 40));
	//deadZone108.push_back(cv::Point(0, 120));
	//deadZone109.push_back(cv::Point(0, 747));
	//deadZone109.push_back(cv::Point(477, 747));
	//deadZone109.push_back(cv::Point(477, 480));
	//deadZone109.push_back(cv::Point(410, 545));
	//deadZone109.push_back(cv::Point(60, 560));
	//deadZone109.push_back(cv::Point(0, 440));
	//cont108.push_back(deadZone109);
	//cont109.push_back(deadZone108);
	int sizeCont108=cont108.size();
	int sizeCont109=cont109.size();
	for(unsigned int j = 0; j < cont108.size(); j++)
	{
		cont108Index=getContourRange(cont108[j]);
		for (unsigned int i = 0; i < cont109.size(); i++)
		{
			cont109Index=getContourRange(cont109[i]);
			test =matchContours(cont108Index,cont109Index,j,i,sizeCont108,sizeCont109);
			if (test)
			{
				matchedContourPairs.push_back(matchedContourPair);
			}
		}
	}
	Mat drawing = Mat::zeros( Size(500,750), CV_8UC3 );
	for (unsigned int l=0;l<matchedContourPairs.size();l++)
	{
		for(int j = matchedContourPairs[l].validMinY; j < matchedContourPairs[l].validMaxY; j++)
		{
			for (int i = matchedContourPairs[l].validMinX; i < matchedContourPairs[l].validMaxX; i++)
			{
				if((pointPolygonTest(cont108[matchedContourPairs[l].contourIndex1],Point2f(i,j),false)>0)&&(pointPolygonTest(cont109[matchedContourPairs[l].contourIndex2],Point2f(i,j),false)>0))
				{
					drawing.at<Vec3b>(j,i)[0] = 255;
				}
			}
		}
	}
	imwrite("result.bmp",drawing);
	if (singleTestOrNot!=1)
	{
		imwrite(resultsName,drawing);
	}
	
}
//�õ�ʵ�ʵķ�Χ��ӳ�䵽ԭͼƬ
void staticObstacleLocation::getRealAndTest(string saveName)
{
	Mat resultImg=imread("result.bmp",0);
	cvContours resultConts;
	vector<Vec4i> hierarchy;

	findContours( resultImg, resultConts, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE , Point(0, 0) );
	/// Find the convex hull object for each contour
	vector<vector<Point> >hull( resultConts.size() );
	for( size_t i = 0; i < resultConts.size(); i++ )
	{   
		approxPolyDP( Mat(resultConts[i]), resultConts[i], 3, true );
		convexHull( Mat(resultConts[i]), hull[i], true );
	}
	resultConts=hull;
	vector<cv::Point2f> corners;
	vector<cv::Point2f> quad_pts;

	corners.push_back(cv::Point2f(472, 273));
	corners.push_back(cv::Point2f(732, 280));
	corners.push_back(cv::Point2f(422, 516));
	corners.push_back(cv::Point2f(773, 522));

	quad_pts.push_back(cv::Point2f(1733, 3085));
	quad_pts.push_back(cv::Point2f(2933,3085 ));
	quad_pts.push_back(cv::Point2f(1733,4680 ));
	quad_pts.push_back(cv::Point2f(2933,4680 ));

	Mat transmtx = getPerspectiveTransform(quad_pts, corners);
	double temp1 = 0;//x'*temp
	double temp2 = 0;//y'*temp
	double temp = 0;
	double x=0;
	double y=0;
	ofstream fout5(saveName+".txt");
	for( size_t i = 0; i< resultConts.size(); i++ )
	{
		for( size_t j = 0; j< resultConts.at(i).size();j++ )
		{	
			resultConts.at(i)[j].x = resultConts.at(i)[j].x*10;
			resultConts.at(i)[j].y = resultConts.at(i)[j].y*10;
			fout5<<resultConts.at(i)[j].y<<','<<resultConts.at(i)[j].x<<";";
			temp1 = transmtx.at<double>(0,0)*resultConts.at(i)[j].x + transmtx.at<double>(0,1)*resultConts.at(i)[j].y + transmtx.at<double>(0,2);
			temp2 = transmtx.at<double>(1,0)*resultConts.at(i)[j].x + transmtx.at<double>(1,1)*resultConts.at(i)[j].y + transmtx.at<double>(1,2);
			temp  = transmtx.at<double>(2,0)*resultConts.at(i)[j].x + transmtx.at<double>(2,1)*resultConts.at(i)[j].y + transmtx.at<double>(2,2);
			x=temp1/temp;
			y=temp2/temp;
			resultConts.at(i)[j].x = x;
			resultConts.at(i)[j].y = y;
		}
		fout5<<endl;
	}
	fout5.close();
	for( size_t i = 0; i< resultConts.size(); i++ )
	{
		Scalar color = Scalar(0,255,0);
		drawContours( src108, resultConts, (int)i, color, 2, 8, hierarchy, 0, Point() );
	}
	/// Show in a window
	imshow( "Contours", src108 );
	imwrite(saveName+".bmp", src108);
	cvWaitKey();
}
//��ȡcontours��xy��Χ
contourRange staticObstacleLocation::getContourRange(vector<Point> Contour)
{
	contourRange ContourIndex;
	int MinX=Contour[0].x;
	int MaxX=Contour[0].x;
	int MinY=Contour[0].y;
	int MaxY=Contour[0].y;
	for (unsigned int i=1;i<Contour.size();i++)
	{
		if (Contour[i].x<MinX)
		{
			MinX=Contour[i].x;
		}
		if (Contour[i].x>MaxX)
		{
			MaxX=Contour[i].x;
		}
		if (Contour[i].y<MinY)
		{
			MinY=Contour[i].y;
		}
		if (Contour[i].y>MaxY)
		{
			MaxY=Contour[i].y;
		}
	}
	ContourIndex.MaxX=MaxX;
	ContourIndex.MinX=MinX;
	ContourIndex.MaxY=MaxY;
	ContourIndex.MinY=MinY;
	return ContourIndex;
}
//����contours��xy��Χ�ҳ�contours�п����ཻ��ƥ���
bool staticObstacleLocation::matchContours(contourRange Contour108,contourRange Contour109,int i,int j,int sizeCont108,int sizeCont109)
{
	/*if (i!=(sizeCont108-1)&&j!=(sizeCont109-1))
	{*/
		if ((Contour108.MinX>=Contour109.MaxX)||(Contour109.MinX>=Contour108.MaxX)||(Contour108.MinY>=Contour109.MaxY)||(Contour108.MinY>=Contour109.MaxY))
			return false;
		else if ((Contour109.MinY>=Contour108.MinY)&&(Contour109.MaxY>=Contour108.MaxY))
		{
			matchedContourPair.contourIndex1=i;
			matchedContourPair.contourIndex2=j;
			matchedContourPair.validMaxX=min(Contour108.MaxX,   Contour109.MaxX);
			matchedContourPair.validMaxY=min(Contour108.MaxY,   Contour109.MaxY);
			matchedContourPair.validMinX=max(Contour108.MinX,   Contour109.MinX);
			matchedContourPair.validMinY=max(Contour108.MinY,   Contour109.MinY);
			return true;
		}
		else
			return false;
	//} 
	//else if(i==(sizeCont108-1))
	//{
	//	if ((Contour108.MinX>=Contour109.MaxX)||(Contour109.MinX>=Contour108.MaxX)||(Contour108.MinY>=Contour109.MaxY)||(Contour108.MinY>=Contour109.MaxY))
	//		return false;
	//	else if(abs(747-Contour109.MaxY)<20)
	//	{
	//		matchedContourPair.contourIndex1=i;
	//		matchedContourPair.contourIndex2=j;
	//		matchedContourPair.validMaxX=min(Contour108.MaxX,   Contour109.MaxX);
	//		matchedContourPair.validMaxY=min(Contour108.MaxY,   Contour109.MaxY);
	//		matchedContourPair.validMinX=max(Contour108.MinX,   Contour109.MinX);
	//		matchedContourPair.validMinY=max(Contour108.MinY,   Contour109.MinY);
	//		return true;
	//	}
	//}
	//else
	//{
	//	if ((Contour108.MinX>=Contour109.MaxX)||(Contour109.MinX>=Contour108.MaxX)||(Contour108.MinY>=Contour109.MaxY)||(Contour108.MinY>=Contour109.MaxY))
	//		return false;
	//	else if(abs(Contour109.MinY)<20)
	//	{
	//		matchedContourPair.contourIndex1=i;
	//		matchedContourPair.contourIndex2=j;
	//		matchedContourPair.validMaxX=min(Contour108.MaxX,   Contour109.MaxX);
	//		matchedContourPair.validMaxY=min(Contour108.MaxY,   Contour109.MaxY);
	//		matchedContourPair.validMinX=max(Contour108.MinX,   Contour109.MinX);
	//		matchedContourPair.validMinY=max(Contour108.MinY,   Contour109.MinY);
	//		return true;
	//	}
	//}
}