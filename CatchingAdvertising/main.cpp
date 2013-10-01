//OpenCV
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>

//OpenGL
#include <gl\freeglut.h>

//Own libraries
#include "Person.h"
#include "Sector.h"

//Project
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <cmath>

//Person Functions signatures
IplImage* GetThresholdedImage(IplImage* imgHSV);
void trackObject(IplImage* imgThresh);
void createGrid(IplImage *frame);
void sectorDetectedObject(IplImage *img, double posX, double posY);
double distanceP2P(double posX, double posY, double lastX, double lastY);
unsigned __stdcall myTimeThread(void* a);
void calculateTime();
void createSectors();

//Gesture Functions signatures
void detectAndDisplay( IplImage* img);

//Person recognition Globals
IplImage* imgTracking;
double lastX = -1;
double lastY = -1;
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

//Gesture recognition globals for Mouth
CvRect *r;
String haarcascade_mouth = "haarcascade_mcs_mouth.xml";
CvHaarClassifierCascade* cascadeMouth;
CvMemStorage* storageMouth;
bool boolMouth = false;

int main(int argc, char* argv[]){
	CascadeClassifier cascade_mouth;
	cascadeMouth = (CvHaarClassifierCascade*)cvLoad("haarcascade_mcs_mouth.xml");

	CvCapture* capture = 0;
	CvCapture* captureFace = 0;
    capture     = cvCaptureFromCAM(0);
	captureFace = cvCaptureFromCAM(1);

	//SectorTest
	createSectors();

    if(!capture || !captureFace){
		cout<<"Capture failure"<<endl;
		return -1;
    }

    IplImage* frame = 0;
	IplImage* frameFace = 0;
	frame = cvQueryFrame(capture);           
	frameFace = cvQueryFrame(captureFace);
    if(!frame || !frameFace) return -1;
	imgTracking=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U, 3);
    cvZero(imgTracking); //convert the image, 'imgTracking' to black
	cvNamedWindow("Video",CV_WINDOW_FULLSCREEN);     
    cvNamedWindow("Ball");    
    while(true){
		frame = cvQueryFrame(capture);
		frameFace = cvQueryFrame(captureFace);
        if(!frame || !frameFace) break;
		frame=cvCloneImage(frame); 
		//---------Mouth-----------
		storageMouth = cvCreateMemStorage(0);
		detectAndDisplay(frameFace);
		cvReleaseMemStorage(&storageMouth);
		//---------Mouth-----------
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
		boolMouth = false;
        cvReleaseImage(&imgHSV);
        cvReleaseImage(&imgThresh);    
        cvReleaseImage(&frame);
		int c = cvWaitKey(1);
        if((char)c == 27 ) break;      
	}
    cvDestroyAllWindows() ;
    cvReleaseCapture(&capture);
	cvReleaseCapture(&captureFace);
    return 0;
}

void createSectors(){
	Sector sector11;sector11.setName("A1");sector11.setLevelOfInterest("HIGH");
	Sector sector12;sector12.setName("A2");sector12.setLevelOfInterest("HIGH");
	Sector sector13;sector13.setName("A3");sector13.setLevelOfInterest("HIGH");
	Sector sector21;sector21.setName("B1");sector21.setLevelOfInterest("MEDIUM");
	Sector sector22;sector22.setName("B2");sector22.setLevelOfInterest("MEDIUM");
	Sector sector23;sector23.setName("B3");sector23.setLevelOfInterest("MEDIUM");
	Sector sector31;sector31.setName("C1");sector31.setLevelOfInterest("LOW");
	Sector sector32;sector32.setName("C2");sector32.setLevelOfInterest("LOW");
	Sector sector33;sector33.setName("C3");sector33.setLevelOfInterest("LOW");
	//cout<<"SECTOR: "<<sector32.getName()<<" "<<sector32.getLevelOfInterest()<<endl;
}

void detectAndDisplay(IplImage* img){
    int i;
	CvSeq* mouth = cvHaarDetectObjects(img, cascadeMouth, storageMouth, 1.2, 3, CV_HAAR_DO_CANNY_PRUNING, cvSize (100, 100));
    for(i = 0; i<(mouth ? mouth->total:0); i++){
         r=(CvRect*)cvGetSeqElem(mouth,i);
         cvRectangle(img, cvPoint(r->x, r->y),cvPoint(r->x + r->width, r->y + r->height),CV_RGB(0,0,255), 2, 8, 0);
		 boolMouth = true;
    }
	cvShowImage("Capture - Face Detection", img);
}

IplImage* GetThresholdedImage(IplImage* imgHSV){       
    IplImage* imgThresh = cvCreateImage(cvGetSize(imgHSV),IPL_DEPTH_8U, 1);
    cvInRangeS(imgHSV, cvScalar(160,120,50), cvScalar(180,256,256), imgThresh);
    return imgThresh;
}

void trackObject(IplImage* imgThresh){
	CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
	cvMoments(imgThresh, moments, 1);
	double moment10 = cvGetSpatialMoment(moments, 1, 0);
	double moment01 = cvGetSpatialMoment(moments, 0, 1);
	double area = cvGetCentralMoment(moments, 0, 0);
	double posX = moment10/area;
	double posY = moment01/area;
	if(area > 1000){
		
		if(lastX>=0 && lastY>=0 && posX>=0 && posY>=0){
			cvLine(imgTracking, cvPoint((int)posX, (int)posY), cvPoint((int)lastX, (int)lastY), cvScalar(0,0,255), 4);
			double distance = distanceP2P(posX,posY,lastX,lastY);
			
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
		cout<<".";
		/*if(!personas.empty() && personas.front().getSeconds() > 10 && (a1 == true || a2 == true || a3 == true)){
			cout<<endl<<"La persona Realmente esta interesada, ha pasado mas de 10 seg en area de ALTO interes"<<endl;
		}
		if(!personas.empty() && personas.front().getSeconds() > 10 && (b1 == true || b2 == true || b3 == true)){
			cout<<endl<<"La persona esta interesada, ha pasado mas de 10 seg en area de MEDIO interes"<<endl;
		}
		if(!personas.empty() && personas.front().getSeconds() > 10 && (c1 == true || c2 == true || c3 == true)){
			cout<<endl<<"La persona NO esta interesada, ha pasado mas de 10 seg en area de BAJO interes"<<endl;
		}*/
		lastX = moment10/area;
		lastY = moment01/area;
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

void sectorDetectedObject(IplImage *img, double posX, double posY){
	int yDivision = (int)img->height/3;
	int xDivision = (int)img->width/3;
	CvFont font;
	font = cvFont(2,1);
	IplImage *imgCopy = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U, 3);
	char resc[255];

	//A1 Sector
	if(posX > 0 && posX < xDivision
		&& posY > 0 && posY < yDivision){
			if(a1 || true){
				sprintf(resc, "Real Time: %d  Seconds", (int)personas.front().getSeconds());
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"A1:High level of interest.",cvPoint(10,25),&font,cvScalar(0,255,0));
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,255,0));
				cvShowImage("TEXT",imgCopy);
				a1 = false;
				a2 = true,a3 = true,b1 = true,b2 = true,b3 =true,c1 = true,c2 = true,c3 = true;
				if(boolMouth){
					cvPutText(imgCopy,"MOUTH DETECTED",cvPoint(10,85),&font,cvScalar(0,255,0));
					cvShowImage("TEXT",imgCopy);
				}
			}
	}
	//A2 Sector
	if(posX > 0 && posX < xDivision
		&& posY > yDivision && posY < 2*yDivision){
			if(a2 || true){
				sprintf(resc, "Real Time: %d  Seconds", (int)personas.front().getSeconds());
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"A2:High level of interest.",cvPoint(10,25),&font,cvScalar(0,255,0));
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,255,0));
				cvShowImage("TEXT",imgCopy);
				a2 = false;
				a1 = true,a3 = true,b1 = true,b2 = true,b3 =true,c1 = true,c2 = true,c3 = true;
			}
	}
	//A3 Sector
	if(posX > 0 && posX < xDivision
		&& posY > 2*yDivision && posY < 3*yDivision){
			if(a3 || true ){
				sprintf(resc, "Real Time: %d  Seconds", (int)personas.front().getSeconds());
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"A3:High level of interest.",cvPoint(10,25),&font,cvScalar(0,255,0));	
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,255,0));
				cvShowImage("TEXT",imgCopy);
				a3 = false;
				a2 = true,a1 = true,b1 = true,b2 = true,b3 =true,c1 = true,c2 = true,c3 = true;
			}
	}

	//B1 Sector
	if(posX > xDivision && posX < 2*xDivision
		&& posY > 0 && posY < yDivision){
			if(b1 || true){
				sprintf(resc, "Real Time: %d  Seconds", (int)personas.front().getSeconds());
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"B1:Medium level of interest.",cvPoint(10,25),&font,cvScalar(0,255,255));	
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,255,255));
				cvShowImage("TEXT",imgCopy);
				b1 = false;
				a2 = true,a3 = true,a1 = true,b2 = true,b3 =true,c1 = true,c2 = true,c3 = true;
			}
	}

	//B2 Sector
	if(posX > xDivision && posX < 2*xDivision
		&& posY > yDivision && posY < 2*yDivision){
			if(b2 || true){
				sprintf(resc, "Real Time: %d  Seconds", (int)personas.front().getSeconds());
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"B2:Medium level of interest.",cvPoint(10,25),&font,cvScalar(0,255,255));	
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,255,255));
				cvShowImage("TEXT",imgCopy);
				b2 = false;
				a2 = true,a3 = true,b1 = true,a1 = true,b3 =true,c1 = true,c2 = true,c3 = true;
			}
	}

	//B3 Sector
	if(posX > xDivision && posX < 2*xDivision
		&& posY > 2*yDivision && posY < 3*yDivision){
			if(b3 || true){
				sprintf(resc, "Real Time: %d  Seconds", (int)personas.front().getSeconds());
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"B3:Medium level of interest.",cvPoint(10,25),&font,cvScalar(0,255,255));	
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,255,255));
				cvShowImage("TEXT",imgCopy);
				b3 = false;
				a2 = true,a3 = true,b1 = true,b2 = true,a1 =true,c1 = true,c2 = true,c3 = true;
			}
	}

	//C1 Sector
	if(posX > 2*xDivision && posX < 3*xDivision
		&& posY > 0 && posY < yDivision){
			if(c1 || true){
				sprintf(resc, "Real Time: %d  Seconds", (int)personas.front().getSeconds());
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"C1:Low level of interest.",cvPoint(10,25),&font,cvScalar(0,0,255));	
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,0,255));
				cvShowImage("TEXT",imgCopy);
				c1 = false;
				a2 = true,a3 = true,b1 = true,b2 = true,b3 =true,a1 = true,c2 = true,c3 = true;
			}
	}

	//C2 Sector
	if(posX > 2*xDivision && posX < 3*xDivision
		&& posY > yDivision && posY < 2*yDivision){
			if(c2 || true){
				sprintf(resc, "Real Time: %d  Seconds", (int)personas.front().getSeconds());
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"C2:Low level of interest.",cvPoint(10,25),&font,cvScalar(0,0,255));	
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,0,255));
				cvShowImage("TEXT",imgCopy);
				c2 = false;
				a2 = true,a3 = true,b1 = true,b2 = true,b3 =true,c1 = true,a1 = true,c3 = true;
			}
	}

	//C3 Sector
	if(posX > 2*xDivision && posX < 3*xDivision
		&& posY > 2*yDivision && posY < 3*yDivision){
			if(c3 || true){
				sprintf(resc, "Real Time: %d  Seconds", (int)personas.front().getSeconds());
				cvNamedWindow("TEXT");
				cvPutText(imgCopy,"C3:Low level of interest.",cvPoint(10,25),&font,cvScalar(0,0,255));	
				cvPutText(imgCopy,resc,cvPoint(10,55),&font,cvScalar(0,0,255));
				cvShowImage("TEXT",imgCopy);
				c3 = false;
				a2 = true,a3 = true,b1 = true,b2 = true,b3 =true,c1 = true,c2 = true,a1 = true;
			}
	}
}

//Distance between 2 points and returns the distance in pixels
double distanceP2P(double posX, double posY, double lastX, double lastY){
	double res = sqrt(pow(double(lastX - posX),2) + pow(double(lastY - posY),2));
	return res;
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

//Calculate the time that a person stays in the web cam
void calculateTime(){
	Sleep(1000);
	personas.front().setSeconds(cont);
	cont++;
}