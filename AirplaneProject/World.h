#pragma once
#include "Headers.h"
#include "StepTimer.h"
#include "Airplane.h"
#include "PeriodPlane.h"
#include <algorithm>
#include "Database.h"


#define PI 3.14159265f
using namespace std;
class World
{
public:

	World();
	~World();

	HRESULT Init(HWND window, int width, int height);
	void OnWindowSizeChanged(int width, int height);
	void Tick();
	void RemovePlane(int x, int y);
	void Exit();
	vector<_bstr_t> GetSessionData();
private:

	HRESULT CreateGraphicsResources();
	void DiscardGraphicsResources();	
	void Render();
	inline void Clear();
	void Update(StepTimer const& timer);
	void MovePlanes(float delta);
	void CreatePlane();

	bool run;

	//Database
	unique_ptr<Database> database;
	//Session time
	const time_point<system_clock>  sessionStart;
	//Render
	HWND							mainWindow;
	int								height;
	int								width;
	//Aircrafts
	unique_ptr<vector<Airplane*>>	airplanes;
	unique_ptr<vector<Airplane*>>	destroyedAirplanes;
	int								id;
	//Tick
	StepTimer						timer;
	//Direct2d
	ID2D1Factory					*factory;
	ID2D1HwndRenderTarget			*renderTarget;
	ID2D1SolidColorBrush			*brush;
	ID2D1PathGeometry				*airplaneGeometry; //Template
	ID2D1TransformedGeometry		*transformed; // Drawing geometry

};

