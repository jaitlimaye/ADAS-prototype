//#include "opencv2/imgcodecs.hpp"
//#include "opencv2/highgui.hpp"
//#include "opencv2/imgproc.hpp"
//#include <opencv2/core/core.hpp>
//
//
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "follow.h"
#include <valarray>
#include <string>
#include <math.h>

using namespace cv;
using namespace std;

follow::follow()
{
	
	
}

void	follow::init(Mat src)
{
	/*
	1280 x 720 
	*/
	framecounter = 0;
	H = src.rows;
	W = src.cols;
	left_pt_x =530; //280
	right_pt_x = 750; //880
	bot_pt_y = H-200; //H-160
	top_pt_y = 150; //400
	initdetects();
	
}

void follow::detect(Mat &src)
{
	
	Mat gry,can,bif,crop_gry;
	cvtColor(src, gry, COLOR_BGR2GRAY);
	GaussianBlur(gry,bif,Size(5,5),0);
	//bilateralFilter(gry,bif,5,50,100);
	Canny(bif, can, 50, 80, 3);//20 60
	getRoi(can);
	contour(src);
	PutRect(src);

}


void follow::getRoi(Mat src)
{
	
	Point ROI[1][4];
	ROI[0][0] = Point(left_pt_x,bot_pt_y);
	ROI[0][1] = Point(left_pt_x,top_pt_y);
	ROI[0][2] = Point(right_pt_x ,top_pt_y);
	ROI[0][3] = Point(right_pt_x ,bot_pt_y);
	
	const Point* ptlist[1] = {ROI[0]};
	
	Mat mask(src.size(),CV_8UC1,Scalar(0));
	
	int num_pts = 4;
	fillPoly(mask,ptlist,&num_pts,1,Scalar(255),8);
	bitwise_and(src,mask,Crop);
}

void follow::contour(Mat src)
{
	
		RNG rng(12345);
	 vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( Crop, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE  );
    Mat drawing(src.size(),CV_8UC1,Scalar(0));
	if(framecounter == 0)
{
	initdetects();
	int reccount = 0;
    for( size_t i = 0; i< contours.size(); i++ )
    {
		if(contourArea(contours[i]) > 300)
	{
		Rect R = boundingRect(contours[i]);
        
		int Xpt = R.x + (R.width/2);
		int Ypt = R.y + (R.height/2);
		if(Xpt > 500 && Xpt < 700 && Ypt >300)
		{
			if(reccount <9)
			{
				detects[reccount++] =  R;
			}
		}
        //drawContours( src, contours, (int)i, color, 2, LINE_8, hierarchy, 0 );
		//if(R.x>600 && (R.x + R.width) < 700)
		//{
		
		//}
	}
    }
	RectClean(reccount);
	
}
framecounter++;
if(framecounter == 10) 
{
	framecounter = 0;
}
    
}
// 
void follow::RectClean(int count)
{
	Rect Emp(0,0,0,0);
	int newcount = count;
	if(count>1)
	{
		for(unsigned i = 0; i<count; i++)
		{
			for(unsigned j = 1; i<count; i++)
			{
				if(i != j && detects[j] != Emp && detects[i] != Emp)
				{
					Rect A = detects[i];
					Rect B = detects[j];
					Rect C = A | B;
					Rect D = A & B;
					if( C.area() > D.area()/2)
					{
						detects[i] = D;
						detects[j] = Emp;
					}			
				}
			}
			
		}
	}
}
void follow::PutRect(Mat &src)
{
	Rect Emp(0,0,0,0);
	for(unsigned i =0; i<10 ; i++)
	{
		if(detects[i] != Emp)
		{
			
	Rect R = detects[i] ;
	int Xpt = R.x + (R.width/2);
		int Ypt = R.y + (R.height/2);
	Scalar color = Scalar(255, 0, 0 );
			Point Ctr(Xpt,Ypt);
			int dist =abs(Ypt - bot_pt_y)/10; 
			string txt = to_string(dist) + " FT";
			rectangle(src,R,color);
			putText(src,txt,Ctr,FONT_HERSHEY_SIMPLEX,2,color);
			
	}
	}
}

void follow::initdetects()
{
	Rect R(0,0,0,0);
	for(unsigned i = 0; i<10 ; i++)
	{
		
		detects[i] = R;
	}
}

