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

class follow
{
		private:
			Mat Crop ;
			Rect detects[10];
			int framecounter;
			int left_pt_x;
			int right_pt_x;
			int bot_pt_y;
			int top_pt_y;
			int H;
			int W;
			bool set;
			void getRoi(Mat src);
			void contour(Mat src);
			void initdetects();
			void PutRect(Mat &src);
			void RectClean(int count);
			
		
		public:
			follow();
			
			void	init(Mat src);
			void detect(Mat &src);
			
};
