#ifndef LOCATION_UTIL_H
#define LOCATION_UTIL_H


class Point
{
public:
	Point();
	Point(const double longitude, const double latitude);

	double GetLongitude() const;
	double GetLatitude() const;

private:
	double GetLoop(double v, double a, double b);
	double GetRange(double v, double a, double b);

	double m_longitude;
	double m_latitude;
};


class LocationUtil
{
public:
	static double GetDistance(const Point& point1, const Point& point2);
	static double GetDistanceSimplify(const Point& point1, const Point& point2);
private:
	static double DegreeToRad(const double degree);
};

#endif