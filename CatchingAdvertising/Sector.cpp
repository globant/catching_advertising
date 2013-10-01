#include "Sector.h"

Sector::Sector(){
	this->name = "";
	this->levelOfInterest = "";
	this->hasElementInside = false;
}

Sector::Sector(char* name, char* loi, bool hei){
	this->name = name;
	this->levelOfInterest = loi;
	this->hasElementInside = hei;
}

Sector::~Sector(){}

char* Sector::getName(){
	return (this->name);
}

char* Sector::getLevelOfInterest(){
	return (this->levelOfInterest);
}

bool Sector::getHasElement(){
	return (this->hasElementInside);
}

void Sector::setName(char* name){
	this->name = name;
}

void Sector::setLevelOfInterest(char* loi){
	this->levelOfInterest = loi;
}

void Sector::setHasElementInside(bool hei){
	this->hasElementInside = hei;
}