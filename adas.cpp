//#include "opencv2/imgcodecs.hpp"
//#include "opencv2/highgui.hpp"
//#include "opencv2/imgproc.hpp"
//#include <opencv2/core/core.hpp>
//
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <iostream>
#include <unistd.h>
#include <time.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <syslog.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include "ped-detect.h"
#include "follow.h"
#include "lanedetect.h"
#include "traffic.h"

#define ESCAPE_KEY (27)
#define MY_SCHEDULER SCHED_FIFO

using namespace cv;
using namespace std;

Detector PD;
follow FD;
lanedetect LD;
TrafficLight TL;
Mat src;
int OnFeatures[4]; //1 - Ped 2- Follow 3- Lane 4- Traf ligh
typedef struct
{
    int threadIdx;
	int frameCount;
} threadParams_t;



void *workerThread(void *threadp)
{
    threadParams_t *threadParams = (threadParams_t *)threadp;
	if(OnFeatures[threadParams->threadIdx] == 1 && threadParams->frameCount%6 == 0 )
	{
		PD.detectPeople(src);
		
	}
	else if(OnFeatures[threadParams->threadIdx] == 2 && threadParams->frameCount%2 == 0)
	{
		FD.detect(src);
	}
	else if(OnFeatures[threadParams->threadIdx] == 3 && threadParams->frameCount%1 == 0)
	{
		LD.detect(src);
	}
	else if(OnFeatures[threadParams->threadIdx] == 4 && threadParams->frameCount%6 == 0)
	{
		TL.detect(src);
	}
}

int main(int argc, char** argv)
{   
	int NUM_THREADS = 4;
	int numberOfProcessors=4;
	bool CanLane =true;
	bool CanFoll =true;
	bool CanTlig =true;
	bool CanPedn =true;
 
 setlogmask(LOG_UPTO (LOG_NOTICE));
 openlog("Log_tag", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
 syslog(LOG_NOTICE, "Logging");
	
	pthread_t threads[NUM_THREADS];
	threadParams_t threadParams[NUM_THREADS];
	pthread_attr_t rt_sched_attr[NUM_THREADS];
	pid_t mainpid;
	int rt_max_prio, rt_min_prio;
	int rc;
	int coreid;
	cpu_set_t threadcpu;
	struct sched_param rt_param[NUM_THREADS];
	struct sched_param main_param;
	char winInput;
	numberOfProcessors = get_nprocs_conf();
	mainpid=getpid();
	 main_param.sched_priority=rt_max_prio;
	rc=sched_setscheduler(getpid(), MY_SCHEDULER, &main_param) < 0;

   	rt_max_prio = sched_get_priority_max(SCHED_FIFO);
   	rt_min_prio = sched_get_priority_min(SCHED_FIFO);
	rc=sched_getparam(mainpid, &main_param);
  	
	

    	VideoCapture frame;
	if(argc>1)
	{
	    	const char *filename=argv[1];
		frame.open(filename);
		if (!frame.isOpened())
		{
			perror("video file cannot be opened");
             exit(-1);
       	}
    }
	else
	{

        perror("video file not provided");
      }
        
        vector<Rect> found;
        struct timespec frames;
   		struct timespec curr;
   clock_gettime(CLOCK_MONOTONIC, &frames);


		
      
	int f=0;
       frame>>src;
		FD.init(src);
		LD.init(src);

	while(!src.empty())
	{
		int feat = 0;
		for(unsigned i=0; i < 4; i++)
   		{
			OnFeatures[i] = 0;
		}
		if(CanPedn)
		{		
			OnFeatures[feat++] = 1;
		}
		if(CanFoll)
		{		
			OnFeatures[feat++] = 2;
		}
		if(CanLane)
		{		
			OnFeatures[feat++] = 3;
		}
		if(CanTlig)
		{		
			OnFeatures[feat++] = 4;
		}
		f++;
		for(unsigned i=0; i < NUM_THREADS; i++)
   		{
       		CPU_ZERO(&threadcpu);

       		coreid=i%numberOfProcessors;
       		
       		CPU_SET(coreid, &threadcpu);

       		rc=pthread_attr_init(&rt_sched_attr[i]);
       		rc=pthread_attr_setinheritsched(&rt_sched_attr[i], PTHREAD_EXPLICIT_SCHED);
       rc=pthread_attr_setschedpolicy(&rt_sched_attr[i], MY_SCHEDULER);
       rc=pthread_attr_setaffinity_np(&rt_sched_attr[i], sizeof(cpu_set_t), &threadcpu);

       rt_param[i].sched_priority=rt_max_prio-i-1;
       pthread_attr_setschedparam(&rt_sched_attr[i], &rt_param[i]);

       threadParams[i].threadIdx=i;
	threadParams[i].frameCount=f;

       pthread_create(&threads[i],&rt_sched_attr[i], workerThread,(void *)&(threadParams[i]));

   		}
	for(unsigned i=0;i<NUM_THREADS;i++)
		{
       		pthread_join(threads[i], NULL);
		}
          
		
		clock_gettime(CLOCK_MONOTONIC, &curr);
		auto diff = difftime(curr.tv_sec,frames.tv_sec);
		string ss="frame rate: ";
		ss=ss+to_string(f/diff);
    syslog(LOG_NOTICE, "Current FPS: %f\n", f/diff);

		putText(src,ss,cv::Point(50,50),FONT_HERSHEY_DUPLEX,1,Scalar(0,255,0),2,false);
		 imshow("Detection", src);
   		winInput=waitKey(1);

     	if (winInput==ESCAPE_KEY)
   		{
          	break;
     	}
		else if(winInput=='a')
		{
			if(CanPedn)
			{		
				CanPedn = false;
				NUM_THREADS --;
        syslog(LOG_NOTICE, "Pedestrian detector disabled: \n");
			}
			else
			{
				CanPedn = true;
				NUM_THREADS ++;
        syslog(LOG_NOTICE, "Pedestrian detector enabled: \n");
			}
      syslog(LOG_NOTICE," CanPedn: %d",CanPedn);
      syslog(LOG_NOTICE," CanTlig: %d",CanTlig);
      syslog(LOG_NOTICE," CanLane: %d",CanLane);
      syslog(LOG_NOTICE," CanFoll: %d",CanFoll);
      
		}
		else if(winInput=='s')
		{
			if(CanTlig)
			{
				CanTlig = false;
				NUM_THREADS --;
        syslog(LOG_NOTICE, "Traffic light detector disabled: \n");
			}
			else
			{
				CanTlig = true;
				NUM_THREADS ++;
        syslog(LOG_NOTICE, "Traffic light detector enabled: \n");
			}
      syslog(LOG_NOTICE," CanPedn: %d",CanPedn);
      syslog(LOG_NOTICE," CanTlig: %d",CanTlig);
      syslog(LOG_NOTICE," CanLane: %d",CanLane);
      syslog(LOG_NOTICE," CanFoll: %d",CanFoll);
		}
		else if(winInput=='d')
		{
			if(CanLane)
			{
				CanLane = false;
				NUM_THREADS --;
        syslog(LOG_NOTICE, "Lane detector disabled: \n");
			}
			else
			{
				CanLane = true;
				NUM_THREADS ++;
        syslog(LOG_NOTICE, "Lane detector enabled: \n");
			}
      syslog(LOG_NOTICE," CanPedn: %d",CanPedn);
      syslog(LOG_NOTICE," CanTlig: %d",CanTlig);
      syslog(LOG_NOTICE," CanLane: %d",CanLane);
      syslog(LOG_NOTICE," CanFoll: %d",CanFoll);
		}
		else if(winInput=='f')
		{
			if(CanFoll)
			{
				CanFoll = false;
				NUM_THREADS --;
        syslog(LOG_NOTICE, "Following distance detector disabled: \n");
			}
			else
			{
				CanFoll = true;
				NUM_THREADS ++;
        syslog(LOG_NOTICE, "Following distance detector enabled: \n");
			}
      syslog(LOG_NOTICE," CanPedn: %d",CanPedn);
      syslog(LOG_NOTICE," CanTlig: %d",CanTlig);
      syslog(LOG_NOTICE," CanLane: %d",CanLane);
      syslog(LOG_NOTICE," CanFoll: %d",CanFoll);
		}
         	frame>>src;
		
       }
       
  syslog(LOG_NOTICE, "Program Finished");
  closelog();
    
   
    return 0;
}
