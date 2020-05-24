#ifndef IOS_LOCATION_MANAGER_H
#define IOS_LOCATION_MANAGER_H


#include <YouMeIMEngine/Location/LocationInterface.h>

class IOSLocationManager : public ILocationManager
{
public:
	IOSLocationManager(){}

	void SetLocationListen(ILocationListen* listen) override;
	virtual LocationErrorcode GetCurrentLocation() override;
};


#endif
