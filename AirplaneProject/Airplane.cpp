#include "Airplane.h"
#define redColor 0xFF0000


Airplane::Airplane(int id): Airplane(id,Point(),0,0)
{
	

}

Airplane::Airplane(int id, Point position) : Airplane(id, position,0,0)
{
}

Airplane::Airplane(int id, Point position, float velocity, float angle) : id(id), color(redColor), position(position), velocity(velocity), angle(angle)
{
	start = system_clock::now();
}


Airplane::~Airplane()
{
}

void Airplane::update(float delta)
{
	velocity = maxSpeed; 
	rand() % 2 == 0 ? angle++ : angle--;
}

void Airplane::destroy()
{
	end = system_clock::now();
}
