
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <iomanip>
using namespace cv;
using namespace std;

class Detector{

private:
      enum Mode
    {
        Default,
        Daimler
    } m;
    HOGDescriptor hog, hog_d;
    string write_frames;
	vector<Rect> found;
	int framecount;
	void toggleMode();
	int height;
	int width;
  string modeName() const;
  vector<Rect> detect(InputArray img);
  void adjustRect(Rect &r) const;

public:

  Detector();
  
 void detectPeople(Mat &current_frame);

};
