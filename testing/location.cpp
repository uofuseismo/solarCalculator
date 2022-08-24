#include "solarCalculator/location.hpp"
#include <gtest/gtest.h>
namespace
{
using namespace SolarCalculator;
TEST(Location, location)
{
    Location loc1;
    EXPECT_NO_THROW(loc1.setLatitude(40)); 
    EXPECT_NEAR(loc1.getLatitude(), 40, 1.e-10);
    EXPECT_NO_THROW(loc1.setLongitude(-120));
    EXPECT_NEAR(loc1.getLongitude(), -120 + 360, 1.e-10);
    // Copy c'tor 
    Location locCopy(loc1);
    EXPECT_NEAR(loc1.getLatitude(),  locCopy.getLatitude(),  1.e-14);
    EXPECT_NEAR(loc1.getLongitude(), locCopy.getLongitude(), 1.e-14);
}
}
