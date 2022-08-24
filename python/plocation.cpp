#include <solarCalculator/location.hpp>
#include "include/plocation.hpp"

using namespace PSolarCalculator;

/// C'tor
Location::Location() :
    mLocation(std::make_unique<SolarCalculator::Location> (0, 0))
{
}

Location::Location(const Location &location)
{
    *this = location;
}

Location::Location(Location &&location) noexcept
{
    *this = std::move(location);
}

Location::Location(const SolarCalculator::Location &location)
{
    *this = location;
}

/// Operators
Location& Location::operator=(const Location &location)
{
    if (&location == this){return *this;}
    mLocation
        = std::make_unique<SolarCalculator::Location> (*location.mLocation);
    return *this;
}

Location& Location::operator=(Location &&location) noexcept
{
    if (&location == this){return *this;}
    mLocation = std::move(location.mLocation);
    return *this;
}

Location& Location::operator=(const SolarCalculator::Location &location)
{
    mLocation = std::make_unique<SolarCalculator::Location> (location);
    return *this;
}

SolarCalculator::Location Location::getNativeClass() const noexcept
{
    return *mLocation;
}

/// Latitude
void Location::setLatitude(const double latitude)
{
    mLocation->setLatitude(latitude);
}

double Location::getLatitude() const
{
    return mLocation->getLatitude();
}

/// Longitude
void Location::setLongitude(const double longitude)
{
    mLocation->setLongitude(longitude);
}

double Location::getLongitude() const
{
    return mLocation->getLongitude();
}

/// Destructor
Location::~Location() = default;

/// Clear
void Location::clear() noexcept
{
    mLocation->clear();
    setLatitude(0);
    setLongitude(0);
}

/// Creates the class
void PSolarCalculator::initializeLocation(pybind11::module &m) 
{
    pybind11::class_<PSolarCalculator::Location> location(m, "Location");
    location.def(pybind11::init<> ());
    location.def("__copy__", [](const PSolarCalculator::Location &self)
    {
        return PSolarCalculator::Location(self);
    });

    location.doc() = "This module defines a latitude and longitude at which to perform solar calculations.\n\nProperties :\n\n  latitude : The latitude in degrees.  This must be in the range [-90,90].\n  longitude : The longitude in degrees.  This is measured in degrees and must be in the range [-540,540).";

    location.def_property("latitude",
                           &PSolarCalculator::Location::getLatitude,
                           &PSolarCalculator::Location::setLatitude);
    location.def_property("longitude",
                           &PSolarCalculator::Location::getLongitude,
                           &PSolarCalculator::Location::setLongitude);

    location.def("clear", &PSolarCalculator::Location::clear, "Resets the class.");

}
