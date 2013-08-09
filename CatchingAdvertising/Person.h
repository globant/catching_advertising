#ifndef _PERSON_H_
#define _PERSON_H_

#include "MyTime.h"

class Person{
	private:
		int id;
		float posX;
		float posY;
		float seconds;
		int totalTime;
	public:
		Person();
		Person(int id, float posX, float posY, float seconds);
		~Person();

		//Analyzers
		int getId();
		float getPosX();
		float getPosY();
		float getSeconds();
		int getTotalTime();

		//Modifiers
		void setId(int id);
		void setPosX(float px);
		void setPosY(float py);
		void setSeconds(float seconds);
		void setTotalTime(int tt);
};

#endif