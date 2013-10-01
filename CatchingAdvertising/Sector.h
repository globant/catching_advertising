#ifndef _SECTOR_H_
#define _SECTOR_H_

using namespace std;

class Sector{
	private:
		char* name;
		char* levelOfInterest;
		bool hasElementInside;
	public:
		Sector();
		Sector(char* name, char* loi, bool hei);
		~Sector();

		//Analyzers
		char* getName();
		char* getLevelOfInterest();
		bool getHasElement();

		//Modifiers
		void setName(char* name);
		void setLevelOfInterest(char* loi);
		void setHasElementInside(bool hei);
};
#endif