#ifndef _MYTIME_H_
#define _MYTIME_H_

#include <ctime>

class MyTime{
	private:
		int beg;
		int end;
		int state;
	public:	
		MyTime();
		MyTime(int beg, int end, int state);
		~MyTime();

		//Analyzers
		int getBeg();
		int getEnd();
		int getState();


		//Modifiers
		void setBeg(int b);
		void setEnd(int e);
		void setState(int s);

		void stop(); //Metodo que detiene el cronometro
		void start(); //Inicia el cronometro
		int getMiliseconds(); //Devuelve los milisegundos transcurridos
		int isStoped(); //Comprueba si el cronometro esta detenido
};

#endif