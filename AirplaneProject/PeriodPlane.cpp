
#include "PeriodPlane.h"
#define BLUE 0x0000FF


PeriodPlane::PeriodPlane(int id) : PeriodPlane(id, Point(200,200), 0,0)
{
}

PeriodPlane::PeriodPlane(int id, Point position) : PeriodPlane(id,position,0,0)
{
}

PeriodPlane::PeriodPlane(int id, Point position, float velocity, float angle) : Airplane(id,position,velocity,angle) 
{
	color = BLUE;
	speedUp = true;
	acceleration = 0.05f;
	maxSpeed = 10;
	minSpeed = 2;
}


PeriodPlane::~PeriodPlane()
{
}

void PeriodPlane::update(float delta) 
{
	
	if (speedUp) {
		velocity += acceleration;
		angle += 1;
		if (velocity>=maxSpeed)speedUp = false;
	}
	else
	{
		velocity -= acceleration;
		angle -= 1;
		if (velocity <= minSpeed)speedUp = true;
	}
}
