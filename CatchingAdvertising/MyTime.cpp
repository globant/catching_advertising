#include "MyTime.h"

MyTime::MyTime(){
	this->beg = 0;
	this->end = 0;
	this->state = 0;
}

MyTime::MyTime(int beg, int end, int state){
	this->beg = beg;
	this->end = end;
	this->state = state;
}

MyTime::~MyTime(){}

//Analyzers
int MyTime::getBeg(){
	return this->beg;
}

int MyTime::getEnd(){
	return this->end;
}

int MyTime::getState(){
	return this->state;
}

//Modifiers
void MyTime::setBeg(int b){
	this->beg = b;
}

void MyTime::setEnd(int e){
	this->end = e;
}

void MyTime::setState(int s){
	this->state = s;
}

void MyTime::start(){	
	if(this->state == 1){
		state = 0;
		this->beg = clock() - (end - beg);
	}else{
		beg=clock();
		end=0;
		state=0;
	}	
}

int MyTime::isStoped(){
	return this->state;
}

int MyTime::getMiliseconds(){
	if(!isStoped()){
		return (long)((clock()-beg)*1000.0/CLOCKS_PER_SEC);
	}else{
		return(long)((end-beg)*1000.0/CLOCKS_PER_SEC);
	}
}

void MyTime::stop(){
	if(!isStoped()){
		state = 1;
		end=clock();	
	}
}