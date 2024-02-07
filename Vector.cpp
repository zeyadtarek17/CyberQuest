#include "Vector.h"


Vector::Vector() {}
Vector::Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}


void Vector:: operator +=(float value)
{
	x += value;
	y += value;
	z += value;
}

void Vector:: operator +=(Vector b)
{
	x += b.x;
	y += b.y;
	z += b.z;
}


void Vector:: operator -=(Vector b)
{
	x -= b.x;
	y -= b.y;
	z -= b.z;
}

Vector Vector:: operator+ (Vector b) {
	Vector res(this->x + b.x, this->y + b.y, this->z + b.z);
	return res;
}


Vector Vector:: operator- (Vector b) {
	Vector res(this->x - b.x, this->y - b.y, this->z - b.z);
	return res;
}

Vector Vector:: operator* (double val) {
	Vector res(this->x * val, this->y * val, this->z * val);
	return res;
}

Vector Vector::crossProd(Vector b) {
	Vector res(0, 0, 0);
	res.x = y * b.z - z * b.y;
	res.y = -x * b.z + z * b.x;
	res.z = x * b.y - y * b.x;
	return res;
}

Vector Vector::unit() {
	double mag = x * x + y * y + z * z;
	mag = sqrt(mag);
	Vector u(x / mag, y / mag, z / mag);
	return u;
}

void Vector::display() {
	std::cout << x << " " << y << " " << z << "\n";
}

double Vector::dist(Vector v) {
	double dx = this->x - v.x;
	double dz = this->z - v.z;
	double dy = this->y - v.y;

	double d = dx * dx + dy * dy + dz * dz;
	return sqrt(d);
}