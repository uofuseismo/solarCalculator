#!/usr/bin/env python3
import pysolarCalculator

def test_location():
    location = pysolarCalculator.Location()
    location.latitude = 40.77
    location.longitude = -111.89;

    assert abs(location.latitude - 40.77) < 1.e-14, 'latitude is wrong'
    assert abs(location.longitude - (-111.89 + 360)) < 1.e-14, 'longitude is wrong'

def test_sun():
    location = pysolarCalculator.Location()
    location.latitude = 40.77
    location.longitude = -111.89;

    sun = pysolarCalculator.Sun()
    sun.location = location
    sun.time = 1622042345
    
    assert sun.time == 1622042345, 'time failed'
    assert abs(sun.elevation - 35.09) < 0.01, 'elevation wrong'
    assert abs(sun.azimuth - 91.2) < 0.1, 'azimuth wrong'
    assert abs(sun.equation_of_time - 2.9) < 0.1, 'equation of time wrong'
    assert abs(sun.declination - 21.24) < 0.01, 'declination wrong'


if __name__ == "__main__":
    test_location()
    print("Passed location test")
    test_sun()
    print("Passed sun test")
