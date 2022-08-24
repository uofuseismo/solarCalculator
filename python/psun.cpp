#include <solarCalculator/location.hpp>
#include <solarCalculator/sun.hpp>
#include "include/plocation.hpp"
#include "include/psun.hpp"

using namespace PSolarCalculator;

/// C'tor
Sun::Sun() :
    mSun(std::make_unique<SolarCalculator::Sun> ()) 
{
    mSun->setTime(0);
    mSun->setLocation(SolarCalculator::Location(0, 0));
}

Sun::Sun(const Sun &sun)
{
    *this = sun;
}

Sun::Sun(Sun &&sun) noexcept
{
    *this = std::move(sun);
}

/// Operators
Sun& Sun::operator=(const Sun &sun)
{
    if (&sun == this){return *this;}
    mSun = std::make_unique<SolarCalculator::Sun> (*sun.mSun);
    return *this;
}

Sun& Sun::operator=(Sun &&sun) noexcept
{
    if (&sun == this){return *this;}
    mSun = std::move(sun.mSun);
    return *this;
}

/// Destructor
Sun::~Sun() = default;

/// Clear
void Sun::clear() noexcept
{
    mSun->clear();
}

/// Set time
void Sun::setTime(const int64_t time)
{
    mSun->setTime(time);
}

int64_t Sun::getTime() const
{
    return mSun->getTime();
}

/// Set location
void Sun::setLocation(const Location &location)
{
    auto loc = location.getNativeClass();
    mSun->setLocation(loc);
}

Location Sun::getLocation() const
{
    PSolarCalculator::Location location(mSun->getLocation());
    return location;
}
    

double Sun::getElevation() const
{
    return mSun->getElevation();
}

double Sun::getAzimuth() const
{
    return mSun->getAzimuth();
}

double Sun::getDeclination() const
{
    return mSun->getDeclination();
}

double Sun::getEquationOfTime() const
{
    return mSun->getEquationOfTime();
}

/// Initialize
void PSolarCalculator::initializeSun(pybind11::module &m)
{
    pybind11::class_<PSolarCalculator::Sun> sun(m, "Sun");
    sun.def(pybind11::init<> ());
    sun.def("__copy__", [](const PSolarCalculator::Sun &self)
    {   
        return PSolarCalculator::Sun(self);
    }); 

    sun.doc() = "This modules performs solar calculations.\n\nProperties:\n\n  location : The location at which to compute the sun's properties.\n  time : The time in UTC measured as seconds from the epoch (January 1 1970) at which to compute the sun's properties.\n\nResults:\n\n elevation : The angle between the sun and the horizon in degrees.\n azimuth : The azimuth of the sun in degrees measured positive clockwise from true north.\n declination : The declination of the sun in degrees.  This varies from -23.44 degrees in the northern hemisphere during the winter solstice to 0 degrees at the vernal equinox to +23.44 degrees at the summer solstice.\n equation_of_time : The equation of time in minutes.  This is an astronomical term accounting for the changes in time of solar noon for a given location over the course of the year."; 
    sun.def_property("location",
                     &PSolarCalculator::Sun::getLocation,
                     &PSolarCalculator::Sun::setLocation);
    sun.def_property("time",
                     &PSolarCalculator::Sun::getTime,
                     &PSolarCalculator::Sun::setTime);

    sun.def_property_readonly("elevation",
                              &PSolarCalculator::Sun::getElevation);
    sun.def_property_readonly("azimuth",
                              &PSolarCalculator::Sun::getAzimuth);
    sun.def_property_readonly("declination",
                              &PSolarCalculator::Sun::getDeclination);
    sun.def_property_readonly("equation_of_time",
                              &PSolarCalculator::Sun::getEquationOfTime);
}
