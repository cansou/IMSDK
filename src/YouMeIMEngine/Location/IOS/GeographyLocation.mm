#import "GeographyLocation.h"


#define LOCATION_TIMEOUT 10.0f

typedef enum LocatonStatus : NSUInteger {
    LOCATIONSTATUS_IDLE,
    LOCATIONSTATUS_RUNNING,
    LOCATIONSTATUS_DONE
} LocatonStatus;


@interface GeographyLocation()<CLLocationManagerDelegate>

@property (nonatomic, strong) CLLocationManager *locationManager;
@property (nonatomic, assign) ILocationListen *locationListener;
@property (nonatomic, strong) CLLocation *currentLocation;
@property (nonatomic) CLLocationAccuracy desiredAccuracy;
@property (nonatomic) CLLocationDistance distanceFilter;
@property (nonatomic) LocatonStatus locationStatus;
@property (nonatomic, strong) NSTimer *timer;

@end


@implementation GeographyLocation


+(instancetype) Instance
{
    static GeographyLocation *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[self alloc] initPrivate];
    });
    return instance;
}

-(instancetype) init
{
    @throw [NSException exceptionWithName:@"Singleton" reason:@"Use +(instancetype) instance" userInfo:nil];
    return nil;
}

-(instancetype) initPrivate
{
    self = [super init];
    if (!self) {
        return nil;
    }
    
    _desiredAccuracy = kCLLocationAccuracyKilometer;
    _distanceFilter = 100.0f;
    _locationListener = NULL;
    _locationStatus = LOCATIONSTATUS_IDLE;
    
    return self;
}

-(void) SetLocationListen:(ILocationListen* _Nonnull)listener
{
    _locationListener = listener;
}

- (void)SetAccuracy:(CLLocationAccuracy)accuracy distanceFilter:(CLLocationDistance)distance
{
    _desiredAccuracy = accuracy;
    _distanceFilter = distance;
}

- (LocationErrorcode)GetLocation
{
    if (_locationStatus == LOCATIONSTATUS_RUNNING) {
        return LOCATIONERROR_SUCCESS;
    }
    
    if ([CLLocationManager authorizationStatus] == kCLAuthorizationStatusDenied ||
        [CLLocationManager authorizationStatus] == kCLAuthorizationStatusRestricted )
    {
        NSLog(@"has notlocation authorization");
        return LOCATIONERROR_AUTHORIZE;
    }
    if (![CLLocationManager locationServicesEnabled]) {
        NSLog(@"locatio service is unable");
        return LOCATIONERROR_AUTHORIZE;
    }
    
    _locationStatus = LOCATIONSTATUS_RUNNING;
    
    if (_locationManager == nil)
    {
        _locationManager = [[CLLocationManager alloc] init];
        _locationManager.delegate = self;
        _locationManager.desiredAccuracy = _desiredAccuracy;
        _locationManager.distanceFilter = _distanceFilter;
    }
    
    // for iOS 8 and later, specific user level permission is required,
    // "when-in-use" authorization grants access to the user's location
    // important: be sure to include NSLocationWhenInUseUsageDescription along with its
    // explanation string in your Info.plist or startUpdatingLocation will not work.
    if ([_locationManager respondsToSelector:@selector(requestWhenInUseAuthorization)]) {
        [_locationManager requestWhenInUseAuthorization];
    }
    
    [_locationManager startUpdatingLocation];
    
    _timer = [NSTimer timerWithTimeInterval:LOCATION_TIMEOUT target:self selector:@selector(OnTimer) userInfo:nil repeats:false];
    if (_timer != nil) {
        [[NSRunLoop currentRunLoop] addTimer:_timer forMode:NSDefaultRunLoopMode];
    }
    
    return LOCATIONERROR_SUCCESS;
}

- (void)stopUpdatingCurrentLocation
{
    [_locationManager stopUpdatingLocation];
}

- (void) OnTimer
{
    NSLog(@"ontimer status:%d", (int)_locationStatus);
    
    [_timer invalidate];
    
    _locationStatus = LOCATIONSTATUS_IDLE;
    
    if (_delegate && [_delegate respondsToSelector:@selector(updateLocation:locationInfo:)]) {
        LocationInfo2 locationInfo;
        [_delegate updateLocation:LOCATIONERROR_FAILED locationInfo:locationInfo];
    }
    
    /*if (NULL == _locationListener) {
        NSLog(@"location listener is null");
        return;
    }
    
    if (_currentLocation != nil) {
        _locationListener->OnUpdateLocation(LOCATIONERROR_TIMEOUT, _currentLocation.coordinate.longitude, _currentLocation.coordinate.latitude);
    } else {
        _locationListener->OnUpdateLocation(LOCATIONERROR_TIMEOUT, -1, -1);
    }*/
}

- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray<CLLocation *> *)locations
{
    NSLog(@"didUpdateLocations");
    
    if ([locations count] > 0) {
        [self stopUpdatingCurrentLocation];
        
        _currentLocation = [locations lastObject];
        
        if (LOCATIONSTATUS_DONE == _locationStatus) {
            return;
        }
        
        /*double distance = 99999;
        if (_currentLocation != nil) {
            distance = [_currentLocation distanceFromLocation:newLocation];
        }
        CLLocation *newLocation = [locations lastObject];
        _currentLocation = newLocation;
        if (distance < 20) {
            return;
        }*/
        
        
        _locationStatus = LOCATIONSTATUS_DONE;
        
        /*static int n = 0;
        if (++n > 1) {
            return;
        }*/
        
        if (_timer != nil) {
            [_timer invalidate];
        }
        
        NSLog(@"didUpdateLocations longitude:%f latitude:%f", _currentLocation.coordinate.longitude, _currentLocation.coordinate.latitude);
        
        if (_locationListener != NULL) {
            _locationListener->OnUpdateLocation(LOCATIONERROR_SUCCESS, _currentLocation.coordinate.longitude, _currentLocation.coordinate.latitude);
        }
        
        
        
        //[self getAddressInfo:_currentLocation];
    }
    
    [self stopUpdatingCurrentLocation];
}

/*- (void)locationManager:(CLLocationManager *)manager didUpdateToLocation:(CLLocation *)newLocation fromLocation:(CLLocation *)oldLocation
{
    NSLog(@"didUpdateToLocation longitude:%f latitude:%f", newLocation.coordinate.longitude, newLocation.coordinate.latitude);
    
    // if the location is older than 30s ignore
    if (fabs([newLocation.timestamp timeIntervalSinceDate:[NSDate date]]) > 30)
    {
        NSLog(@"didUpdateToLocation location old");
        return;
    }
    
    // after recieving a location, stop updating
    [self stopUpdatingCurrentLocation];
    
    //[self getAddressInfo:newLocation];
}*/

- (void)locationManager:(CLLocationManager *)manager didFailWithError:(NSError *)error
{
    NSLog(@"didFailWithError %ld", (long)[error code]);
    
    [self stopUpdatingCurrentLocation];
    
    _locationStatus = LOCATIONSTATUS_IDLE;
    
    if (_timer != nil) {
        [_timer invalidate];
    }
    
    if (_locationListener != NULL) {
        _locationListener->OnUpdateLocation(LOCATIONERROR_FAILED, _currentLocation.coordinate.longitude, _currentLocation.coordinate.latitude);
    }
    
    if (_delegate && [_delegate respondsToSelector:@selector(updateLocation:locationInfo:)]) {
        LocationInfo2 locationInfo;
        [_delegate updateLocation:LOCATIONERROR_FAILED locationInfo:locationInfo];
    }
}

/*- (void)getAddressInfo:(CLLocation*)location
{
    CLGeocoder *geocoder = [[CLGeocoder alloc] init];
    [geocoder reverseGeocodeLocation:location completionHandler:^(NSArray *placemarks, NSError *error) {
        LocationErrorcode errorcode = LOCATIONERROR_SUCCESS;
        LocationInfo locationInfo;
        do {
            if (error){
                NSLog(@"Geocode failed with error: %@", error);
                errorcode = LOCATIONERROR_RESOLVE_FAILED;
                break;
            }
            
            //NSLog(@"Received placemarks: %@", placemarks);
            if (placemarks == nil || placemarks.count == 0) {
                errorcode = LOCATIONERROR_RESOLVE_FAILED;
                break;
            }
            locationInfo = [self getLocationInfo:placemarks[0]];
        } while(0);
        
        if (_delegate && [_delegate respondsToSelector:@selector(updateLocation:locationInfo:)]) {
            [_delegate updateLocation:errorcode locationInfo:locationInfo];
        }
        //NSLog(@"longitude:%f latitude:%f country:%s province:%s city:%s detailAddress:%s", locationInfo.longtitude, locationInfo.latitude, locationInfo.country.c_str(), locationInfo.province.c_str(), locationInfo.city.c_str(), locationInfo.detailAddress.c_str());
        
    }];
}

- (LocationInfo)getLocationInfo:(CLPlacemark*)placemark
{
    LocationInfo locationInfo;
    locationInfo.longtitude = placemark.location.coordinate.longitude;
    locationInfo.latitude = placemark.location.coordinate.latitude;
    NSMutableString *detailAddress = [[NSMutableString alloc] init];
    if (placemark.country) {
        locationInfo.country = [placemark.country UTF8String];
        [detailAddress appendString:placemark.country];
    }
    if (placemark.administrativeArea) {
        locationInfo.province = [placemark.administrativeArea UTF8String];
        [detailAddress appendString:placemark.administrativeArea];
    }
    if (placemark.locality) {
        locationInfo.city = [placemark.locality UTF8String];
        [detailAddress appendString:placemark.locality];
    }
    if (placemark.thoroughfare) {
        [detailAddress appendString:placemark.thoroughfare];
        if (placemark.subThoroughfare) {
            [detailAddress appendString:placemark.subThoroughfare];
        }
    }
    locationInfo.detailAddress = [detailAddress UTF8String];
    
    return locationInfo;
}*/


@end
