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
#include "lanedetect.h"
#include <valarray>
#include <vector>

using namespace cv;
using namespace std;

lanedetect::lanedetect()
{
	
	
}

void	lanedetect::init(Mat src)
{
	/*
	1280 x 720 
	*/
	framecount = 0;
	H = src.rows;
	W = src.cols;
	botlleft_pt_x =350; //280
	botlright_pt_x = 900; //880
	mid_x = (botlleft_pt_x+botlright_pt_x)/2;
	bot_pt_y = H-210; //H-160
	top_pt_x = (W/2)+20 ;//(W/2)-30
	top_pt_y = 350; //400
	for(unsigned i = 0; i<8; i++)
	{
		Lcount[i] = 0;
	}
	lastL[0] = (((top_pt_x+botlleft_pt_x)/2) - 90 + top_pt_x)/2;
	lastL[1] = (bot_pt_y + top_pt_y )/2 ;
	lastL[2] = (((top_pt_x+botlright_pt_x)/2) + 90 + top_pt_x)/2;
	lastL[3] = (bot_pt_y + top_pt_y )/2 ;
	lastL[4] = (top_pt_x+botlleft_pt_x)/2 - 90;
	lastL[5] = bot_pt_y ;
	lastL[6] = (top_pt_x+botlright_pt_x)/2 + 90 ;
	lastL[7] = bot_pt_y ;
	set = false;
}

void lanedetect::detect(Mat &src)
{
	
	Mat gry,can,Gau,crop_gry;
	cvtColor(src, gry, COLOR_BGR2GRAY);
	
	 GaussianBlur(gry,Gau,Size(5,5),0);
	Canny(Gau, can, 20, 60, 3);//20 60
	getRoi(can);
	//src = Crop;
	SetHoughLines();
	drawPoly(src);
}

void lanedetect::SetHoughLines(){
	
     vector<Vec4i> lines; // will hold the results of the detection
    HoughLinesP(Crop, lines, 2, CV_PI/180, 50, 50, 150 ); // runs the actual detection //OG: 2, CV_PI/180, 100, 40, 5 )150

	double m;
	
	//int x = 0;
    // Draw the lines

	framecount++;
	if(framecount == 3)
	{
    for( size_t i = 0; i < lines.size(); i++ )
    {	
		Vec4i l = lines[i];
		m = (atan2((l[1] - l[3]),(l[0] - l[2])) * (180/CV_PI));
		
		//x1 = 0,y1 =1 || x2 = 2, y2 = 3
		if((m > 130 && m < 150)  || (m>=-150 && m<= -130))
		{
			
			if((l[0] >  mid_x)&&(l[2] >  mid_x)) //2,3 6,7
			{	
				
				if(l[1] > l[3])
				{
					lastL[2] = l[2];
					lastL[3] = l[3];
					if(lastL[7] < l[1] )
					{
						lastL[6] = getAvgP(l[0],6);
						//lastL[7] = l[1];
					}
					
				}
				else
				{
					lastL[2] = l[0];
					lastL[3] = getAvgP(l[1],3);
					if(lastL[7] < l[3] )
					{
						
						lastL[6] = getAvgP(l[2],6);
						//lastL[7] = l[3];
					}
					
				}
			}
          	else if((l[0] <  mid_x)&&(l[2] <  mid_x))//0,1 4,5
			{
				
				if(l[1] < l[3])
				{
					lastL[0] = l[0];
					lastL[1] = l[1];
					if(lastL[5] < l[3] )
					{
						lastL[4] = getAvgP(l[2],4);
						//lastL[5] = l[3];
					}
				
				}
				else
				{
					lastL[0] = l[2];
					lastL[1] = l[3];
					if(lastL[5] < l[1] )
					{
						lastL[4] = getAvgP(l[0],4);
						//lastL[5] = l[1];
					}
					
				}
			}
		}
		
    }
	framecount = 0;
	}
	//cout<<endl;
  	//cout<<x<<endl;
    
}

void  lanedetect::getRoi(Mat src)
{
	int hgt = src.rows;
	int wdt = src.cols;
	
	Point ROI[1][3];
	ROI[0][0] = Point(botlleft_pt_x,bot_pt_y);
	ROI[0][1] = Point(top_pt_x,top_pt_y);
	ROI[0][2] = Point(botlright_pt_x ,bot_pt_y);
	
	const Point* ptlist[1] = {ROI[0]};
	
	Mat mask(src.size(),CV_8UC1,Scalar(0));
	
	int num_pts = 3;
	fillPoly(mask,ptlist,&num_pts,1,Scalar(255),8);
	bitwise_and(src,mask,Crop);
}

bool lanedetect::polyset()
{
	if(set) return true;
	for(unsigned i = 0; i<7;i = i+2)
	{
		if(lastL[i] == W+1) return false;
		if(lastL[i+1] == H+1) return false;
	}
	set = true;
	return true;
}

void lanedetect::drawPoly(Mat &src)
{
	vector<Point> pts = {Point(lastL[0], lastL[1]), Point(lastL[2], lastL[3]),Point(lastL[6], lastL[7]),Point(lastL[4], lastL[5])};
	vector<vector<Point> > poly = {pts};
	
	if(polyset())
	{

	Mat layer = Mat::zeros(src.size(), CV_8UC3);
	fillPoly(layer, poly, Scalar(0,0,255));
	//drawContours(src, poly, 0, Scalar(0,255,0),-1);
	addWeighted(src, 1, layer, 0.2, 0, src);
	}
	/*
	line(src,Point(top_pt_x,top_pt_y), Point(botlright_pt_x, bot_pt_y), Scalar(230,0,0), 3, LINE_AA);
	line(src, Point(top_pt_x,top_pt_y),Point(botlleft_pt_x, bot_pt_y), Scalar(230,0,0), 3, LINE_AA);*/
}

int lanedetect::getAvgP(int newval,int index)
{
	int temp;
	if(Lcount[index] == 0)
	{	
		Lcount[index]++;
		//cerr<<index<<" "<<newval<<" "<<Lcount[index]<<endl;
		return newval;
	}
	else
	{
		temp = (lastL[index] * Lcount[index]) + newval;
		Lcount[index]++;
		//cerr<<index<<" "<< (lastL[index] + newval)/Lcount[index]<<" "<<Lcount[index]<<endl;
		return (temp )/Lcount[index];
	}
}
	

