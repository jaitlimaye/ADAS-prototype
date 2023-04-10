#include "traffic.h"


	TrafficLight::TrafficLight(){}

    void TrafficLight::detectTrafficLight(Mat &res, Mat &orig)
    {
        vector<Vec3f> circles;

        HoughCircles(res, circles, HOUGH_GRADIENT, 1,
                     50,          // change this value to detect circles with different distances to each other
                     50, 9, 4, 10 // change the last two parameters
                                  // (min_radius & max_radius) to detect larger circles
        );
        for (size_t i = 0; i < circles.size(); i++)
        {
            Vec3i c = circles[i];
            Point center = Point(c[0], c[1]);
            // circle center
            circle(orig, center, 1, Scalar(0, 100, 100), 3, LINE_AA);
            // circle outline
            int radius = c[2];
            circle(orig, center, radius, Scalar(255, 0, 255), 3, LINE_AA);
        }
    }

    void TrafficLight::detect(Mat &src)
    {

        Mat hsv;
        cvtColor(src, hsv, COLOR_BGR2HSV);
        GaussianBlur(hsv, hsv, Size(3, 3), 0);
        GaussianBlur(hsv, hsv, Size(5, 5), 0);
        int height = src.rows;
        int width = src.cols;
        Mat mask1;
        hsv = hsv(Range(0, 0.45 * height), Range(0, 0.85 * width));
        // inRange(hsv, Scalar(45, 140, 140), Scalar(255, 255, 255), mask1);
        inRange(hsv, Scalar(0, 100, 20), Scalar(179, 255, 255), mask1);
       
        detectTrafficLight(mask1,src);
    }

