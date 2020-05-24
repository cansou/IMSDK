#include "LocationUtil.h"
#include<math.h>
#include <cstdlib>


#define EARTH_RADIUS 6370996.81
#define MAX_LNG 180
#define MAX_LAT 80.0
#define PRECISION 7
#ifdef WIN32
#define M_PI 3.14159265358979323846
#endif


Point::Point()
{
	m_longitude = 0;
	m_latitude = 0;
}

Point::Point(const double longitude, const double latitude)
{
	m_longitude = GetLoop(longitude, -MAX_LNG, MAX_LNG);


    if (fabs(fabs(m_longitude) - MAX_LNG) < 0.0000001)	// 如果经度线逼近MAX_LNG(180度线),等同为MAX_LNG
	{
		m_longitude = MAX_LNG;
	}

	m_latitude = GetRange(latitude, -MAX_LAT, MAX_LAT);

	// 舍弃多余位数,防止后续计算溢出
	m_longitude = ((long)(m_longitude * pow(10.0, PRECISION) + 0.5)) / pow(10.0, PRECISION);
	m_latitude = ((long)(m_latitude * pow(10.0, PRECISION) + 0.5)) / pow(10.0, PRECISION);
}

double Point::GetLongitude() const
{
	return m_longitude;
}

double Point::GetLatitude() const
{
	return m_latitude;
}

double Point::GetLoop(double v, double a, double b)
{
	while (v > b)
	{
		v -= b - a;
	}
	while (v < a)
	{
		v += b - a;
	}
	return v;
}

double Point::GetRange(double v, double a, double b)
{
	v = (v > a) ? v : a;
	v = (v < b) ? v : b;
	return v;
}



double LocationUtil::GetDistance(const Point& point1, const Point& point2)
{
	double x1, x2, y1, y2;
	x1 = DegreeToRad(point1.GetLongitude());
	y1 = DegreeToRad(point1.GetLatitude());
	x2 = DegreeToRad(point2.GetLongitude());
	y2 = DegreeToRad(point2.GetLatitude());

	return EARTH_RADIUS * acos((sin(y1) * sin(y2) + cos(y1) * cos(y2) * cos(x2 - x1)));
}

double LocationUtil::GetDistanceSimplify(const Point& point1, const Point& point2)
{
	double dx = point1.GetLongitude() - point2.GetLongitude();			// 经度差值
	double dy = point1.GetLatitude() - point2.GetLatitude();			// 纬度差值
	double b = (point1.GetLatitude() + point2.GetLatitude()) / 2.0;		// 平均纬度
	double Lx = EARTH_RADIUS * DegreeToRad(dx) * cos(DegreeToRad(b));	// 东西距离
	double Ly = EARTH_RADIUS * DegreeToRad(dy);							// 南北距离
	return sqrt(Lx * Lx + Ly * Ly);										// 用平面的矩形对角距离公式计算总距离
}

double LocationUtil::DegreeToRad(const double degree)
{
	return M_PI * degree / 180;
}
