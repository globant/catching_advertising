﻿//OpenCV
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>

//Own libraries
#include "Person.h"
#include "MyTime.h"

//Project
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <ctime>
#include <cmath>


IplImage* GetThresholdedImage(IplImage* imgHSV);
void trackObject(IplImage* imgThresh);
void createGrid(IplImage *frame);
void sectorDetectedObject(IplImage *img, int posX, int posY);
int distanceP2P(int posX, int posY, int lastX, int lastY);

IplImage* imgTracking;
IplImage *imgCopy;
int lastX = -1;
int lastY = -1;

std::list<Person>personas;
Person p;

using namespace std;
using namespace cv;

int main(int argc, char* argv[]){

	CvCapture* capture =0;       
    capture = cvCaptureFromCAM(0);
    if(!capture){
		cout<<"Capture failure"<<endl;
		return -1;
    }
      
    IplImage* frame=0;
	frame = cvQueryFrame(capture);           
    if(!frame) return -1;
	imgTracking=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U, 3);
    cvZero(imgTracking); //convert the image, 'imgTracking' to black
    cvNamedWindow("Video");     
    cvNamedWindow("Ball");    
    while(true){
		frame = cvQueryFrame(capture);           
        if(!frame) break;
		frame=cvCloneImage(frame); 
        cvSmooth(frame, frame, CV_GAUSSIAN,9,9); //smooth the original image using Gaussian kernel
        IplImage* imgHSV = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
        cvCvtColor(frame, imgHSV, CV_BGR2HSV); //Change the color format from BGR to HSV
        IplImage* imgThresh = GetThresholdedImage(imgHSV);
        cvSmooth(imgThresh, imgThresh, CV_GAUSSIAN,3,3); //smooth the binary image using Gaussian kernel
		trackObject(imgThresh);
		createGrid(frame);
        cvAdd(frame, imgTracking, frame);
        cvShowImage("Ball", imgThresh);           
        cvShowImage("Video", frame);
        cvReleaseImage(&imgHSV);
        cvReleaseImage(&imgThresh);            
        cvReleaseImage(&frame);
		int c = cvWaitKey(10);
        if((char)c == 27 ) break;      
	}

    cvDestroyAllWindows() ;
    cvReleaseCapture(&capture);     
    return 0;
}

IplImage* GetThresholdedImage(IplImage* imgHSV){       
    IplImage* imgThresh=cvCreateImage(cvGetSize(imgHSV),IPL_DEPTH_8U, 1);
    cvInRangeS(imgHSV, cvScalar(170,160,60), cvScalar(180,256,256), imgThresh);
    return imgThresh;
}

void trackObject(IplImage* imgThresh){
	CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
	cvMoments(imgThresh, moments, 1);
	double moment10 = cvGetSpatialMoment(moments, 1, 0);
	double moment01 = cvGetSpatialMoment(moments, 0, 1);
	double area = cvGetCentralMoment(moments, 0, 0);
	
	if(area > 1000){
		int posX = moment10/area;
		int posY = moment01/area;
		cout<<"posX: "<<posX<<" posY: "<<posY<<endl;
		if(lastX>=0 && lastY>=0 && posX>=0 && posY>=0){
			cvLine(imgTracking, cvPoint(posX, posY), cvPoint(lastX, lastY), cvScalar(0,0,255), 4);
			int distance = distanceP2P(posX,posY,lastX,lastY);
			//if(distance > 100){
				MyTime t;
				Person p;
				p.setId(1);
				p.setMyTime(t);
				personas.push_back(p);
				sectorDetectedObject(imgTracking,posX,posY);
				cout<<"TIME: "<<t.getMiliseconds()/1000<<endl;
			//}else{
				
			//}
			
		}
		lastX = posX;
		lastY = posY;
	}

	free(moments);
}
void createGrid(IplImage *frame){
	int yDivision = (int)frame->height/3;
	int xDivision = (int)frame->width/3;
	
	CvScalar lineColor = cvScalar(255, 255, 0);	
	int thickness = 2;

	//horizontal lines
	cvLine(frame, cvPoint(0,yDivision), cvPoint(frame->width, yDivision), lineColor, thickness);
	cvLine(frame, cvPoint(0,2*yDivision), cvPoint(frame->width,2*yDivision), lineColor, thickness);

	//vertical lines
	cvLine(frame, cvPoint(xDivision,0), cvPoint(xDivision,frame->height), lineColor, thickness);
	cvLine(frame, cvPoint(2*xDivision,0), cvPoint(2*xDivision,frame->height), lineColor, thickness);
}

void sectorDetectedObject(IplImage *img, int posX, int posY){
	int yDivision = (int)img->height/3;
	int xDivision = (int)img->width/3;
	CvFont font;
	font = cvFont(2,1);	 

	//A1 Sector
	if(posX > 0 && posX < xDivision
		&& posY > 0 && posY < yDivision){
			if(img != imgCopy){
				imgCopy = img;
				cvPutText(imgCopy,"A1:Nivel ALTO de interes.",cvPoint(10,25),&font,cvScalar(0,255,0));
			}
	}else{
		//img = imgCopy;
	}
	//A2 Sector
	if(posX > 0 && posX < xDivision
		&& posY > yDivision && posY < 2*yDivision){
		cvPutText(img,"A2:Nivel ALTO de interes.",cvPoint(10,25),&font,cvScalar(0,255,0));
	}
	//A3 Sector
	if(posX > 0 && posX < xDivision
		&& posY > 2*yDivision && posY < 3*yDivision){
		cvPutText(img,"A3:Nivel ALTO de interes.",cvPoint(10,25),&font,cvScalar(0,255,0));
	}

	//B1 Sector
	if(posX > xDivision && posX < 2*xDivision
		&& posY > 0 && posY < yDivision){
		cvPutText(img,"B1:Nivel MEDIO de interes.",cvPoint(10,25),&font,cvScalar(0,255,255));
	}

	//B2 Sector
	if(posX > xDivision && posX < 2*xDivision
		&& posY > yDivision && posY < 2*yDivision){
		cvPutText(img,"B2:Nivel MEDIO de interes.",cvPoint(10,25),&font,cvScalar(0,255,255));
	}

	//B3 Sector
	if(posX > xDivision && posX < 2*xDivision
		&& posY > 2*yDivision && posY < 3*yDivision){
		cvPutText(img,"B3:Nivel MEDIO de interes.",cvPoint(10,25),&font,cvScalar(0,255,255));
	}

	//C1 Sector
	if(posX > 2*xDivision && posX < 3*xDivision
		&& posY > 0 && posY < yDivision){
		cvPutText(img,"C1:Nivel BAJO de interes.",cvPoint(10,25),&font,cvScalar(0,0,255));
	}

	//C2 Sector
	if(posX > 2*xDivision && posX < 3*xDivision
		&& posY > yDivision && posY < 2*yDivision){
		cvPutText(img,"C2:Nivel BAJO de interes.",cvPoint(10,25),&font,cvScalar(0,0,255));
	}

	//C3 Sector
	if(posX > 2*xDivision && posX < 3*xDivision
		&& posY > 2*yDivision && posY < 3*yDivision){
		cvPutText(img,"C3:Nivel BAJO de interes.",cvPoint(10,25),&font,cvScalar(0,0,255));
	}
}

//Distance between 2 points and returns the distance in pixels
int distanceP2P(int posX, int posY, int lastX, int lastY){
	int res = sqrt(pow(double(lastX - posX),2) + pow(double(lastY - posY),2));
	return res;
}