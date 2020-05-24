#import <CoreLocation/CoreLocation.h>
#import <string>
#include <YouMeIMEngine/Location/LocationInterface.h>


struct LocationInfo2
{
    double longtitude;
    double latitude;
    std::string country;
    std::string province;
    std::string city;
    std::string detailAddress;
    
    LocationInfo2() : longtitude(0), latitude(0){}
};


@protocol GeographyLocatonDelegate <NSObject>

@required

-(void) updateLocation:(LocationErrorcode)errorcode locationInfo:(LocationInfo2)locationInfo;

@end



@interface GeographyLocation : NSObject

+(instancetype _Nullable) Instance;
-(void) SetLocationListen:(ILocationListen* _Nonnull)listener;
-(void) SetAccuracy:(CLLocationAccuracy)accuracy distanceFilter:(CLLocationDistance)distance;
-(LocationErrorcode) GetLocation;

@property(assign, nonatomic, nullable) id<GeographyLocatonDelegate> delegate;

@end
