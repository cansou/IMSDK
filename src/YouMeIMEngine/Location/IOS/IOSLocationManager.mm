#include "IOSLocationManager.h"
#include <YouMeIMEngine/Location/IOS/GeographyLocation.h>


void IOSLocationManager::SetLocationListen(ILocationListen* listen)
{
    [[GeographyLocation Instance] SetLocationListen:listen];
}

LocationErrorcode IOSLocationManager::GetCurrentLocation()
{
	return [[GeographyLocation Instance] GetLocation];
}
