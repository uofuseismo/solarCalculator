#include <iostream>
#include "solarCalculator/sun.hpp"
#include "solarCalculator/location.hpp"
#include <gtest/gtest.h>

namespace
{

using namespace SolarCalculator;

TEST(Sun, Time)
{
    Sun sun;
    sun.setTime(1622042345); 
    EXPECT_TRUE(sun.haveTime());
    EXPECT_EQ(sun.getTime(), 1622042345);
}

TEST(Sun, Location)
{
    Sun sun;
    Location location(40.77, -111.89);
    EXPECT_NO_THROW(sun.setLocation(location));
    EXPECT_TRUE(sun.haveLocation());
    EXPECT_NEAR(sun.getLocation().getLatitude(),  40.77, 1.e-10);
    EXPECT_NEAR(sun.getLocation().getLongitude(), -111.89 + 360, 1.e-10);
}

TEST(Sun, Calculate)
{
    Sun sun;
    // May 26, 2021 at 9:19:05 local time
    Location location(40.77, -111.89);
    EXPECT_NO_THROW(sun.setLocation(location));
    EXPECT_NO_THROW(sun.setTime(1622042345));
    EXPECT_TRUE(sun.haveTimeAndLocation());
    EXPECT_NEAR(sun.getElevation(), 35.09, 0.01);
    EXPECT_NEAR(sun.getAzimuth(), 91.2, 0.1);
    EXPECT_NEAR(sun.getEquationOfTime(), 2.9, 0.1);
    EXPECT_NEAR(sun.getDeclination(), 21.24, 0.01);
    // Sunrise: Local time: 06:01
    // Sunset: Local time: 20:48

    sun.setTime(1600718786);
    EXPECT_NEAR(sun.getElevation(), 48.2, 0.1);
    EXPECT_NEAR(sun.getAzimuth(), 197.42, 0.01);
    EXPECT_NEAR(sun.getEquationOfTime(), 7.19, 0.01);
    EXPECT_NEAR(sun.getDeclination(), 0.28, 0.01);
    // Sunrise: Local time: 7:15
    // Sunset: Local time: 19:25

    location.setLatitude(39.77);
    location.setLongitude(-109.89);
    sun.setLocation(location);
    sun.setTime(1575507986);
    EXPECT_NEAR(sun.getElevation(), -13.4, 0.1);
    EXPECT_NEAR(sun.getAzimuth(), 252, 0.1);
    EXPECT_NEAR(sun.getEquationOfTime(), 9.68, 0.01);
    EXPECT_NEAR(sun.getDeclination(), -22.3, 0.1);
    // Sunrise: Local time: 7:24
    // Solar Noon: 12:09:46
    // Sunset: Local time: 16:55

    location.setLatitude(44);
    location.setLongitude(-110 + 360);
    sun.setLocation(location);
    sun.setTime(1420378385);
    EXPECT_NEAR(sun.getElevation(), -14.12, 0.01);
    EXPECT_NEAR(sun.getAzimuth(), 108.07, 0.01);
    EXPECT_NEAR(sun.getEquationOfTime(), -4.85, 0.01);
    EXPECT_NEAR(sun.getDeclination(), -22.71, 0.01);
    // Sunrise: Local time: 7:55
    // Solar Noon: 12:24:58
    // Sunset: Local time: 16:55
}


}

