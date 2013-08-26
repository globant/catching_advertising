//OpenCV
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>

//OpenGL
#include <gl\freeglut.h>


//Own libraries
#include "Person.h"

//Project
#include <process.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <ctime>
#include <cmath>

//Functions signatures
IplImage* GetThresholdedImage(IplImage* imgHSV);
void trackObject(IplImage* imgThresh);
void createGrid(IplImage *frame);
void sectorDetectedObject(IplImage *img, int posX, int posY);
int distanceP2P(int posX, int posY, int lastX, int lastY);
unsigned __stdcall myTimeThread(void* a);
void calculateTime();
void InitOpenGL( int w, int h );
void ViewTransform( float w , float h );
void OnDraw();

//Globals
IplImage* imgTracking;
int lastX = -1;
int lastY = -1;
std::list<Person>personas;
Person p;
int cont = 0;
int idCont = 0;
int id1=11;
bool timeBool = true;
bool a1 = true,a2 = true,a3 = true,b1 = true,b2 = true,b3 =true,c1 = true,c2 = true,c3 = true;
float WIDTH = 600;
float HEIGHT = 600;


using namespace std;
using namespace cv;

int main(int argc, char* argv[]){
	CvCapture* capture =0;       
    capture = cvCaptureFromCAM(0);
    if(!capture){
		cout<<"Capture failure"<<endl;
		return -1;
    }
      
    IplImage* frame = 0;
	frame = cvQueryFrame(capture);           
    if(!frame) return -1;
	imgTracking=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U, 3);
    cvZero(imgTracking); //convert the image, 'imgTracking' to black
	cvNamedWindow("Video",CV_WINDOW_FULLSCREEN);     
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
		int c = cvWaitKey(1);
        if((char)c == 27 ) break;      
	}

    cvDestroyAllWindows() ;
    cvReleaseCapture(&capture);
    return 0;
}

IplImage* GetThresholdedImage(IplImage* imgHSV){       
    IplImage* imgThresh = cvCreateImage(cvGetSize(imgHSV),IPL_DEPTH_8U, 1);
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
		if(lastX>=0 && lastY>=0 && posX>=0 && posY>=0){
			cvLine(imgTracking, cvPoint(posX, posY), cvPoint(lastX, lastY), cvScalar(0,0,255), 4);
			int distance = distanceP2P(posX,posY,lastX,lastY);
			
			if(distance > 100 || personas.size() < 1){
				p.setId(idCont);
				idCont++;
				personas.push_back(p);
				HANDLE t2 = (HANDLE) _beginthreadex(NULL, 0,  myTimeThread,  &id1, 0, 0);
				personas.front().setSeconds(cont);
				sectorDetectedObject(imgTracking,posX,posY);
				
			}else {
				sectorDetectedObject(imgTracking,posX,posY);
			}
			timeBool = true;
		}
		lastX = posX;
		lastY = posY;
	}else{
		//personas.front().setSeconds(cont);
		cout<<".";
		cont = 0;
		timeBool = false;
	}
	free(moments);
}

void createGrid(IplImage *frame){
	int yDivision = (int)frame->height/3;
	int xDivision = (int)frame->width/3;
	
	CvScalar lineColor = cvScalar(255, 255, 0);	
	int thickness = 1;

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
	IplImage *imgCopy = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U, 3);
	char resc[255];
	sprintf(resc, "Tiempo: %d", (int)personas.front().getSeconds());
	//A1 Sector
	if(posX > 0 && posX < xDivision
		&& posY > 0 && posY < yDivision){
			if(a1){
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"A1:Nivel ALTO de interes.",cvPoint(10,25),&font,cvScalar(0,255,0));
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,255,0));
				cvShowImage("TEXT",imgCopy);
				cout<<endl<<"SECTOR: A1 - Nivel ALTO de interes";
				a1 = false;
				a2 = true,a3 = true,b1 = true,b2 = true,b3 =true,c1 = true,c2 = true,c3 = true;
			}
	}
	//A2 Sector
	if(posX > 0 && posX < xDivision
		&& posY > yDivision && posY < 2*yDivision){
			if(img != imgCopy && a2){
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"A2:Nivel ALTO de interes.",cvPoint(10,25),&font,cvScalar(0,255,0));
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,255,0));
				cvShowImage("TEXT",imgCopy);
				cout<<endl<<"SECTOR: A2 - Nivel ALTO de interes";
				a2 = false;
				a1 = true,a3 = true,b1 = true,b2 = true,b3 =true,c1 = true,c2 = true,c3 = true;
			}
	}
	//A3 Sector
	if(posX > 0 && posX < xDivision
		&& posY > 2*yDivision && posY < 3*yDivision){
			if(img != imgCopy && a3){
				//imgCopy = img;
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"A3:Nivel ALTO de interes.",cvPoint(10,25),&font,cvScalar(0,255,0));	
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,255,0));
				cvShowImage("TEXT",imgCopy);
				cout<<endl<<"SECTOR: A3 - Nivel ALTO de interes";
				a3 = false;
				a2 = true,a1 = true,b1 = true,b2 = true,b3 =true,c1 = true,c2 = true,c3 = true;
			}
	}

	//B1 Sector
	if(posX > xDivision && posX < 2*xDivision
		&& posY > 0 && posY < yDivision){
			if(img != imgCopy && b1){
				//imgCopy = img;
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"B1:Nivel MEDIO de interes.",cvPoint(10,25),&font,cvScalar(0,255,255));	
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,255,255));
				cvShowImage("TEXT",imgCopy);
				cout<<endl<<"SECTOR: B1 - Nivel MEDIO de interes";
				b1 = false;
				a2 = true,a3 = true,a1 = true,b2 = true,b3 =true,c1 = true,c2 = true,c3 = true;
			}
	}

	//B2 Sector
	if(posX > xDivision && posX < 2*xDivision
		&& posY > yDivision && posY < 2*yDivision){
			if(img != imgCopy && b2){
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"B2:Nivel MEDIO de interes.",cvPoint(10,25),&font,cvScalar(0,255,255));	
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,255,255));
				cvShowImage("TEXT",imgCopy);
				cout<<endl<<"SECTOR: B2 - Nivel MEDIO de interes";
				b2 = false;
				a2 = true,a3 = true,b1 = true,a1 = true,b3 =true,c1 = true,c2 = true,c3 = true;
			}
	}

	//B3 Sector
	if(posX > xDivision && posX < 2*xDivision
		&& posY > 2*yDivision && posY < 3*yDivision){
			if(img != imgCopy && b3){
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"B3:Nivel MEDIO de interes.",cvPoint(10,25),&font,cvScalar(0,255,255));	
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,255,255));
				cvShowImage("TEXT",imgCopy);
				cout<<endl<<"SECTOR: B3 - Nivel MEDIO de interes";
				b3 = false;
				a2 = true,a3 = true,b1 = true,b2 = true,a1 =true,c1 = true,c2 = true,c3 = true;
			}
	}

	//C1 Sector
	if(posX > 2*xDivision && posX < 3*xDivision
		&& posY > 0 && posY < yDivision){
			if(c1){
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"C1:Nivel BAJO de interes.",cvPoint(10,25),&font,cvScalar(0,0,255));	
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,0,255));
				cvShowImage("TEXT",imgCopy);
				cout<<endl<<"SECTOR: C1 - Nivel BAJO de interes";
				c1 = false;
				a2 = true,a3 = true,b1 = true,b2 = true,b3 =true,a1 = true,c2 = true,c3 = true;
			}
	}

	//C2 Sector
	if(posX > 2*xDivision && posX < 3*xDivision
		&& posY > yDivision && posY < 2*yDivision){
			if(c2){
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"C2:Nivel BAJO de interes.",cvPoint(10,25),&font,cvScalar(0,0,255));	
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,0,255));
				cvShowImage("TEXT",imgCopy);
				cout<<endl<<"SECTOR: C2 - Nivel BAJO de interes";
				c2 = false;
				a2 = true,a3 = true,b1 = true,b2 = true,b3 =true,c1 = true,a1 = true,c3 = true;
			}
	}

	//C3 Sector
	if(posX > 2*xDivision && posX < 3*xDivision
		&& posY > 2*yDivision && posY < 3*yDivision){
			if(c3){
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"C3:Nivel BAJO de interes.",cvPoint(10,25),&font,cvScalar(0,0,255));	
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,0,255));
				cvShowImage("TEXT",imgCopy);
				cout<<endl<<"SECTOR: C3 - Nivel BAJO de interes";
				c3 = false;
				a2 = true,a3 = true,b1 = true,b2 = true,b3 =true,c1 = true,c2 = true,a1 = true;
			}
	}
}

//Distance between 2 points and returns the distance in pixels
int distanceP2P(int posX, int posY, int lastX, int lastY){
	int res = sqrt(pow(double(lastX - posX),2) + pow(double(lastY - posY),2));
	return res;
}

//Calculate the time that a person stays in the web cam
void calculateTime(){
	Sleep(1000);
	personas.front().setSeconds(cont);
	if(cont > 0){
		cout<<endl<<"La persona ha estado: "<<personas.front().getSeconds()<<" SEGUNDOS.";
	}
	cont++;
}

//Time Thread function
unsigned __stdcall myTimeThread(void* a) {
	int child = *(int*) a;
	while(true){
		if(timeBool){
			calculateTime();
		}
	}
	return 0;
}