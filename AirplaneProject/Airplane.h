#pragma once
#include "Headers.h"
#include "Point.h"
#include <ole2.h>  
#include <stdio.h>  
#include "conio.h"  
#include "icrsint.h"
using namespace std::chrono;
class Airplane
{
public:
	Airplane(int id);
	Airplane(int id, Point position);
	Airplane(int id, Point position, float velocity, float angle);
	~Airplane();
	virtual void update(float delta);
	void destroy();
	inline Point getPosition() { return position; };
	inline void setPosition(Point p) { position = p; };
	inline bool getDirectionX() { return angle; };
	inline float getVelocity() {  return velocity; };
	inline float getAngle() { return angle; };
	inline void setAngle(float a) { angle = a; };
	inline int getId() { return id; };
	inline int getColor() { return color; };
	inline auto getStartTime() { return start; };
	inline auto getEndTime() { return end; };
protected:
	int color;
	int id;
	Point position;
	float velocity;
	float angle;
	float maxSpeed=5;

	time_point<system_clock> start;
	time_point<system_clock> end;

};


