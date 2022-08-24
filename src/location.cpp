#include <string>
#include "solarCalculator/location.hpp"

using namespace SolarCalculator;

class Location::LocationImpl
{
public:
    double mLatitude = 0;
    double mLongitude = 0;
    bool mHaveLatitude = false;
    bool mHaveLongitude = false;
};

/// C'tor
Location::Location() :
    pImpl(std::make_unique<LocationImpl> ())
{
}

/// C'tor
Location::Location(const double latitude, const double longitude) :
    pImpl(std::make_unique<LocationImpl> ())
{
    setLatitude(latitude);
    setLongitude(longitude);
}

/// Copy c'tor
Location::Location(const Location &location)
{
    *this = location;
}

/// Move c'tor
[[maybe_unused]]
Location::Location(Location &&location) noexcept
{
    *this = std::move(location);
}

/// Copy assignment operator
Location& Location::operator=(const Location &location)
{
    if (&location == this){return *this;}
    pImpl = std::make_unique<LocationImpl> (*location.pImpl);
    return *this;
}

/// Move assignment operator
Location& Location::operator=(Location &&location) noexcept
{
    if (&location == this){return *this;}
    pImpl = std::move(location.pImpl);
    return *this;
}

/// Destructor
Location::~Location() = default;

/// Clears class
void Location::clear() noexcept
{
    pImpl->mLatitude = 0;
    pImpl->mLongitude = 0;
    pImpl->mHaveLatitude = false;
    pImpl->mHaveLongitude = false;
}

/// Latitude
void Location::setLatitude(const double latitude)
{
    if (latitude < -90 || latitude > 90)
    {
        throw std::invalid_argument("Latitude = " + std::to_string(latitude)
                                  + " must be in range [-90,90]");
    }
    pImpl->mLatitude = latitude;
    pImpl->mHaveLatitude = true;
}

double Location::getLatitude() const
{
    if (!haveLatitude()){throw std::invalid_argument("Latitude not set");}
    return pImpl->mLatitude;
}

bool Location::haveLatitude() const noexcept
{
    return pImpl->mHaveLatitude;
}

/// Longitude
void Location::setLongitude(const double longitude)
{
    if (longitude < -540 || longitude >=540)
    {
        throw std::invalid_argument("Longitude = " + std::to_string(longitude)
                                  + " must be in range [-540,540)");
    }
    auto lonWork = longitude;
    while (lonWork < 0){lonWork = lonWork + 360;}
    while (lonWork > 360){lonWork = lonWork - 360;}
    pImpl->mLongitude = lonWork;
    pImpl->mHaveLongitude = true;
}

double Location::getLongitude() const
{
    if (!haveLongitude()){throw std::invalid_argument("Longitude not set");}
    return pImpl->mLongitude;
}

bool Location::haveLongitude() const noexcept
{
    return pImpl->mHaveLongitude;
}
