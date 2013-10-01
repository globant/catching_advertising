#ifndef _PERSON_H_
#define _PERSON_H_

class Person{
	private:
		int id;
		float posX;
		float posY;
		int seconds;
		int totalTime;
	public:
		Person();
		Person(int id, float posX, float posY, int seconds);
		~Person();

		//Analyzers
		int getId();
		float getPosX();
		float getPosY();
		int getSeconds();
		int getTotalTime();

		//Modifiers
		void setId(int id);
		void setPosX(float px);
		void setPosY(float py);
		void setSeconds(int seconds);
		void setTotalTime(int tt);
};

#endif