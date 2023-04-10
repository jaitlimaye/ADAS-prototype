
#include "ped-detect.h"


    Detector::Detector() : m(Default), hog(), hog_d(Size(48, 96), Size(16, 16), Size(8, 8), Size(8, 8), 9), write_frames("false"), framecount(0)
    {
        hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
        hog_d.setSVMDetector(HOGDescriptor::getDaimlerPeopleDetector());
    }

    void Detector::toggleMode()
    {
        m = (m == Default ? Daimler : Default);
    }

    string Detector::modeName() const
    {
        return (m == Default ? "Default" : "Daimler");
    }

    vector<Rect> Detector::detect(InputArray img)
    {
        
        m=Daimler;
        
        vector<Rect> found;
        if (m == Default){
            
            hog.detectMultiScale(img, found, 0, Size(4, 4), Size(16, 16), 1.07, 2, false);
           
        }
        else if (m == Daimler)
            hog_d.detectMultiScale(img, found, 0.5, Size(4, 4), Size(16, 16), 1.05, 2, true);
        return found;
    }

    void Detector::adjustRect(Rect &r) const
    {
       
        r.x += cvRound(r.width * 0.1);
        r.width = cvRound(r.width * 0.8);
        r.y += cvRound(r.height * 0.07);
        r.height = cvRound(r.height * 0.8);
    }

    void Detector::detectPeople(Mat &current_frame)
    {
		
        char buff[100];
        Mat img;
      
        cv::cvtColor(current_frame, img, COLOR_BGR2GRAY);
        height = img.rows;
        width = img.cols;
        
        //sprintf(buff, "%04d", count);
        //string imageName = std::string(buff) + ".jpg";
        
        img=img(Range(0.35*height,0.56*height),Range(0.25*width,0.75*width));
        
        //int64 t = getTickCount();
	
	
        found = this->detect(img);
        //t=getTickCount()-t;

        //ostringstream buf;
        //buf << "FPS: " << fixed << setprecision(1) << (getTickFrequency() / (double)t);
        //putText(current_frame, buf.str(), Point(10, 30), FONT_HERSHEY_PLAIN, 2.0, Scalar(0, 0, 255), 2, LINE_AA);
		
        for (vector<Rect>::iterator i = found.begin(); i != found.end(); ++i)
        {
            Rect &r = *i;
            this->adjustRect(r);
           
            Point tl= r.tl();
            Point br= r.br();
           
            Point ntl2(round(tl.x)+round(0.25*width),tl.y+round(0.35*height));
            Point nbr2(round(br.x)+round(0.25*width),br.y+round(0.35*height));
            rectangle(current_frame, ntl2, nbr2, cv::Scalar(0, 255, 0), 2);
            
        }
		
      /* 
	if(write=="true"){
		cout<<"writing current frame"<<endl;
	  bool isSuccess = imwrite( imageName, current_frame);
         }
        */
        
    }


