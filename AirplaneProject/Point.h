class Point
{
public:
	float x;
	float y;
	Point() : x(0.0), y(0.0) {};
	Point(float x, float y) : x(x), y(y) {};
	Point(float val) : x(val), y(val) {};
	Point operator+(Point p) {
		this->x += p.x;
		this->y += p.y;
		return *this;
	}
	void invertX() { this->x *= -1; }
	void invertY() { this->y *= -1; }
};

