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

class TrafficLight
{

		private:
    			void detectTrafficLight(Mat &res, Mat &orig)	;
			
		public:
			TrafficLight();
			void detect(Mat &src);
			
};
