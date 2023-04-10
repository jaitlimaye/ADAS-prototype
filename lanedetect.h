//#include "opencv2/imgcodecs.hpp"
//#include "opencv2/highgui.hpp"
//#include "opencv2/imgproc.hpp"
//#include <opencv2/core/core.hpp>
//
//
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include<time.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;
using namespace std;

class lanedetect
{
		private:
			Mat Crop ;
			int framecount;
			int botlleft_pt_x;
			int botlright_pt_x;
			int mid_x;
			int bot_pt_y;
			int top_pt_x;
			int top_pt_y;
			int H;
			int W;
			int lastL[8];
			int Lcount[8];
			bool set;
			void SetHoughLines();
			void  getRoi(Mat src);
			
			bool polyset();
			void drawPoly(Mat &src);
			int getAvgP(int newval,int index);
		public:
			lanedetect();
			
			void	init(Mat src);
			void detect(Mat &src);
			
};
