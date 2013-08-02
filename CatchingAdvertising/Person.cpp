#include "Person.h"

Person::Person(){
	this -> id = 0;
	this -> posX = 0;
	this -> posY = 0;
	this -> seconds = 0;
}

Person::Person(int id, float posX, float posY, float seconds,MyTime t){
	this -> id = id;
	this -> posX = posX;
	this -> posY = posY;
	this -> seconds = seconds;
	this -> time = t;
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

float Person::getSeconds(){
	return this -> seconds;
}

MyTime Person::getMyTime(){
	return this->time;
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

void Person::setSeconds(float seconds){
	this->seconds = seconds;
}

void Person::setMyTime(MyTime t){
	this->time = t;
}
		
void Person::setTotalTime(int tt){
	this->totalTime = tt;
}