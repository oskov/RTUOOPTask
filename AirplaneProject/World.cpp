#include "World.h"

#define FPS 60
#define TICK 1.0/FPS
#define MAXPLANES 10
#define CONNECTION_STRING "Provider='Microsoft.ACE.OLEDB.12.0';Data Source='airplaneDB.accdb';"


World::World(): sessionStart(chrono::system_clock::now())
{	
	factory = NULL;
	id = 0;
	run = true;
}


World::~World()
{

	DiscardGraphicsResources();
	SafeRelease(&factory);
	SafeRelease(&airplaneGeometry);
	SafeRelease(&transformed);
	for (Airplane *currentPlane : *airplanes.get()) {
		delete currentPlane;
	}
	for (Airplane *currentPlane : *destroyedAirplanes.get()) {
		delete currentPlane;
	}
	
}

HRESULT World::Init(HWND window, int width, int height)
{
	mainWindow = window;
	this->width = width;
	this->height = height;
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
	if (SUCCEEDED(hr)) {
		hr= factory->CreatePathGeometry(&airplaneGeometry);
		if (SUCCEEDED(hr)) {
			ID2D1GeometrySink *pSink = NULL;
			hr =airplaneGeometry->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
				pSink->BeginFigure(
					D2D1::Point2F(0, 0),
					D2D1_FIGURE_BEGIN_FILLED
				);
				D2D1_POINT_2F points[10] = {
				   D2D1::Point2F(0, 20),
				   D2D1::Point2F(25, 20),
				   D2D1::Point2F(25, 40),
				   D2D1::Point2F(40, 20),
				   D2D1::Point2F(60, 20),
				   D2D1::Point2F(70, 10),
				   D2D1::Point2F(60, 0),
				   D2D1::Point2F(40, 0),
				   D2D1::Point2F(25, -20),
				   D2D1::Point2F(25, 0),
				};
				pSink->AddLines(points, ARRAYSIZE(points));
				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
				hr = pSink->Close();
			}
			SafeRelease(&pSink);
		}

	}
	timer.SetFixedTimeStep(true);
	timer.SetTargetElapsedSeconds(TICK);

	airplanes = make_unique<vector<Airplane*>>();
	destroyedAirplanes = make_unique<vector<Airplane*>>();
	database = make_unique<Database>(CONNECTION_STRING);
	return hr;

}

void World::OnWindowSizeChanged(int width, int height)
{
	this->width = width;
	this->height = height;
	DiscardGraphicsResources();
}

void World::Tick()
{
	if (run) {
	timer.Tick([&]()
	{
		Update(timer);
	});


	Render();
	}
	
}

void World::RemovePlane(int x, int y)
{
	for (Airplane *currentPlane : *airplanes.get()) {
		const D2D1::Matrix3x2F rot = D2D1::Matrix3x2F::Rotation(currentPlane->getAngle());
		const D2D1::Matrix3x2F trans = D2D1::Matrix3x2F::Translation(currentPlane->getPosition().x, currentPlane->getPosition().y);
		BOOL contains;
		airplaneGeometry->FillContainsPoint(D2D1::Point2F(x,y),rot*trans, &contains);
		if (contains) {
			vector<Airplane*>::iterator position = find(airplanes.get()->begin(), airplanes.get()->end(), currentPlane);
			if (position != airplanes.get()->end()) {
			airplanes.get()->erase(position);
			currentPlane->destroy();
			destroyedAirplanes.get()->push_back(currentPlane);
			} 
				
		}
	}

}

void World::Exit()
{	
	database->SaveData(sessionStart, airplanes, destroyedAirplanes);
	cout << "EXIT"<<endl;
	run = false;
}

vector<_bstr_t> World::GetSessionData()
{	

	auto output = database->GetData(sessionStart);
	return output;

	
}

HRESULT World::CreateGraphicsResources()
{
	HRESULT hr = S_OK;
	if (renderTarget == NULL)
	{
		RECT rc;
		GetClientRect(mainWindow, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)),D2D1::HwndRenderTargetProperties(mainWindow, size),&renderTarget);

		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 0);
			hr = renderTarget->CreateSolidColorBrush(color, &brush);
			

		}
	}
	return hr;
}



void World::DiscardGraphicsResources()
{
	SafeRelease(&renderTarget);
	SafeRelease(&brush);
}



void World::Render()
{
	if (timer.GetFrameCount() == 0)
	{
		return;
	}
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(mainWindow, &ps);

		renderTarget->BeginDraw();

		Clear();
		for (Airplane *currentPlane : *airplanes.get()) {
			const D2D1::Matrix3x2F rot = D2D1::Matrix3x2F::Rotation(currentPlane->getAngle());
			const D2D1::Matrix3x2F trans = D2D1::Matrix3x2F::Translation(currentPlane->getPosition().x, currentPlane->getPosition().y);
			factory->CreateTransformedGeometry(airplaneGeometry, rot*trans, &transformed);
			
			brush->SetColor(D2D1::ColorF(currentPlane->getColor()));
			renderTarget->FillGeometry(transformed, brush);
		}

		hr = renderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}
		EndPaint(mainWindow, &ps);
	}
}

inline void World::Clear()
{
	renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::LightBlue));
}

void World::Update(StepTimer const & timer)
{
	float elapsedTime = float(timer.GetElapsedSeconds());
	if (airplanes->size()!=MAXPLANES) {
		if (airplanes->empty()) 
		{ 
			CreatePlane();
		}
		else if(rand()%100==0) {
			CreatePlane();
	
		}
	}
	
	
	MovePlanes(elapsedTime);

}

void World::MovePlanes(float delta)
{
	for (Airplane *currentPlane : *airplanes.get()) {
		currentPlane->update(delta);
		Point pos = currentPlane->getPosition();
		pos.x += currentPlane->getVelocity()*cos(currentPlane->getAngle()*PI / 180);
		pos.y += currentPlane->getVelocity()*sin(currentPlane->getAngle()*PI / 180);

		if (pos.x > width || pos.x < 0) {
			currentPlane->setAngle(currentPlane->getAngle() + 180);

		}
		if (pos.y > height || pos.y < 0) {
			currentPlane->setAngle(currentPlane->getAngle() + 180);

		}
		currentPlane->setPosition(pos);
	}
}

void World::CreatePlane()
{
	Airplane *plane;
	Point position;
	position.x = width / 3 + rand() % (width / 3);
	position.y = height / 3 + rand() % (height / 3);
	switch(rand() % 2) {
		case 0:
			plane = new PeriodPlane(id,position);
			break;
		case 1:
			plane = new Airplane(id, position);
			break;
	}
	airplanes->push_back(plane);
	id++;
}

