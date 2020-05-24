#ifndef ANDROID_LOCATION_MANAGER_H
#define ANDROID_LOCATION_MANAGER_H

#include <YouMeIMEngine/Location/LocationInterface.h>

class AndroidLocationManager : public ILocationManager
{
public:
	AndroidLocationManager(){}

	void SetLocationListen(ILocationListen* listen) override;
	virtual LocationErrorcode GetCurrentLocation() override;
};

#endif

