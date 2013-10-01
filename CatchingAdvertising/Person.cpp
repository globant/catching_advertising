#include "Person.h"


Person::Person(){
	this -> id = 0;
	this -> posX = 0;
	this -> posY = 0;
	this -> seconds = 0;
}

Person::Person(int id, float posX, float posY, int seconds){
	this -> id = id;
	this -> posX = posX;
	this -> posY = posY;
	this -> seconds = seconds;
}

Person::~Person(){}

//Analyzers
int Person::getId(){
	return this -> id;
}

float Person::getPosX(){
	return this -> posX;
}

float Person::getPosY(){
	return this -> posY;
}

int Person::getSeconds(){
	return this -> seconds;
}

int Person::getTotalTime(){
	return this->totalTime;
}

//Modifiers
void Person::setId(int id){
	this->id = id;
}

void Person::setPosX(float px){
	this->posX = px;
}

void Person::setPosY(float py){
	this->posY = py;
}

void Person::setSeconds(int seconds){
	this->seconds = seconds;
}
		
void Person::setTotalTime(int tt){
	this->totalTime = tt;
}
