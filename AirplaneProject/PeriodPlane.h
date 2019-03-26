#pragma once
#include "Airplane.h"
class PeriodPlane :
	public Airplane
{
public:
	PeriodPlane(int id);
	PeriodPlane(int id, Point position);
	PeriodPlane(int id, Point position, float velocity, float angle);
	~PeriodPlane();
	virtual void update(float delta) override;
protected:
	bool speedUp;
	float acceleration;
	float minSpeed;
};

