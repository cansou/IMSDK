#include "AndroidLocationManager.h"


extern int Youme_GetCurrentLocation();
extern void YouMe_SetAndroidLocationListen(ILocationListen* listen);

void AndroidLocationManager::SetLocationListen(ILocationListen* listen)
{
	YouMe_SetAndroidLocationListen(listen);
}

LocationErrorcode AndroidLocationManager::GetCurrentLocation()
{
	int errorcode = Youme_GetCurrentLocation();
	return (LocationErrorcode)errorcode;
}
