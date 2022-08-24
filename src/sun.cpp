#include <iostream>
#include <iomanip>
#include <cassert>
#include <cmath>
#include <time/utc.hpp>
#include "solarCalculator/sun.hpp"
#include "solarCalculator/location.hpp"

using namespace SolarCalculator;

namespace
{

///--------------------------------------------------------------------------///
///                              Angle Conversions                           ///
///--------------------------------------------------------------------------///

double radToDeg(const double angleRad)
{
    return (180.0*angleRad)/M_PI;
}

double degToRad(const double angleDeg)
{
    return (M_PI*angleDeg)/180.0;
}

///--------------------------------------------------------------------------///
///                                Julian day                                ///
///--------------------------------------------------------------------------///
bool isLeapYear(const int yr)
{
    return ((yr%4 == 0 && yr%100 != 0) || yr%400 == 0);
}

double calcTimeJulianCent(const double jd)
{
    return (jd - 2451545.0)/36525.0;
}

double getJD(int year, int month, int day)
{
    if (month <= 2)
    {
        year -= 1;
        month += 12;
    }
    auto A = static_cast<int> (std::floor(year/100.));
    auto B = 2 - A + static_cast<int> (std::floor(A/4.));
    auto JD = std::floor(365.25*(year + 4716))
            + std::floor(30.6001*(month+1))
            + day + B - 1524.5;
    return JD;
}


std::tuple<int, int, double> calcDateFromJD(const double jd)
{
    auto z = static_cast<int> (std::floor(jd + 0.5));
    double f = (jd + 0.5) - z;
    int A = 0;
    if (z < 2299161)
    {
        A = z;
    }
    else
    {
       auto alpha = static_cast<int> (std::floor((z - 1867216.25)/36524.25));
       A = z + 1 + alpha - static_cast<int> (std::floor(alpha/4.));
    }
    auto B = A + 1524;
    auto C = static_cast<int> (std::floor((B - 122.1)/365.25));
    auto D = static_cast<int> (std::floor(365.25*C));
    auto E = static_cast<int> (std::floor((B - D)/30.6001));
    auto day = B - D - static_cast<int> (std::floor(30.6001 * E)) + f;
    auto month = (E < 14) ? E - 1 : E - 13;
    auto year = (month > 2) ? C - 4716 : C - 4715;
    return std::tuple<int, int, double> (year, month, day);
    //{"year": year, "month": month, "day": day}
}


double calcDoyFromJD(const double jd)
{
    auto date = calcDateFromJD(jd);
    auto year = std::get<0> (date);
    auto month = std::get<1> (date);
    auto day = std::get<2> (date);
    auto k = isLeapYear(year) ? 1 : 2;
    auto doy = std::floor((275*month)/9)
             - k*std::floor((month + 9)/12) + day - 30;
    return doy;
}

///--------------------------------------------------------------------------///
///                                Earth's tilt                              ///
///--------------------------------------------------------------------------///
double calcEccentricityEarthOrbit(const double t)
{
    return 0.016708634 - t*(0.000042037 + 0.0000001267*t); // Unitless
}

double calcMeanObliquityOfEcliptic(const double t)
{
    double seconds = 21.448 - t*(46.8150 + t*(0.00059 - t*(0.001813)));
    double e0 = 23.0 + (26.0 + (seconds/60.0))/60.0;
    return e0; // in degrees
}

double calcObliquityCorrection(const double t)
{
    auto e0 = calcMeanObliquityOfEcliptic(t);
    auto omega = 125.04 - 1934.136 * t;
    auto e = e0 + 0.00256 * std::cos(degToRad(omega));
    return e; // in degrees
}

double calcRefraction(const double elev)
{
    double correction = 0;
    if (elev > 85.0)
    {
        correction = 0.0;
    }
    else
    {
        auto te = std::tan(degToRad(elev));
        if (elev > 5.0)
        {
            correction = 58.1/te - 0.07/(te*te*te) + 0.000086/(te*te*te*te*te);
        }
        else if (elev > -0.575)
        {
            correction = 1735.0
                 + elev*(-518.2 + elev*(103.4 + elev*(-12.79 + elev*0.711)));
        }
        else
        {
            correction = -20.774/te;
        }
        correction = correction/3600.0;
    }
    return correction;
}

///--------------------------------------------------------------------------///
///                                Sun's geometry                            ///
///--------------------------------------------------------------------------///
double calcGeomMeanLongSun(const double t)
{
    auto L0 = 280.46646 + t*(36000.76983 + t*(0.0003032));
    while (L0 > 360.0)
    {
        L0 -= 360.0;
    }
    while (L0 < 0.0)
    {
        L0 += 360.0;
    }
    return L0; // in degrees
}


double calcGeomMeanAnomalySun(const double t)
{
    return 357.52911 + t*(35999.05029 - 0.0001537*t); // In degrees
}

double calcSunEqOfCenter(const double t)
{
    auto m = calcGeomMeanAnomalySun(t);
    auto mrad = degToRad(m);
    auto sinm = std::sin(mrad);
    auto sin2m = std::sin(2*mrad); //mrad + mrad);
    auto sin3m = std::sin(3*mrad); //mrad + mrad + mrad);
    auto C = sinm*(1.914602 - t*(0.004817 + 0.000014*t))
           + sin2m*(0.019993 - 0.000101*t) + sin3m*0.000289;
    return C; // in degrees
}

double calcSunTrueLong(const double t)
{
    auto l0 = calcGeomMeanLongSun(t);
    auto c = calcSunEqOfCenter(t);
    auto O = l0 + c;
    return O; // in degrees
}

double calcSunApparentLong(const double t)
{
    auto o = calcSunTrueLong(t);
    auto omega = 125.04 - 1934.136*t;
    auto lambda = o - 0.00569 - 0.00478*std::sin(degToRad(omega));
    return lambda; // in degrees
}

double calcSunDeclination(const double t)
{
   auto e = calcObliquityCorrection(t);
   auto lambda = calcSunApparentLong(t);
   auto sint = std::sin(degToRad(e))*std::sin(degToRad(lambda));
   auto theta = radToDeg(std::asin(sint));
   return theta; // in degrees
}

/// Equation of time
double calcEquationOfTime(const double t)
{
    auto epsilon = calcObliquityCorrection(t);
    auto l0 = calcGeomMeanLongSun(t);
    auto e = calcEccentricityEarthOrbit(t);
    auto m = calcGeomMeanAnomalySun(t);

    auto y = std::tan(degToRad(epsilon)/2.0);
    y *= y;

    auto sin2l0 = std::sin(2.0*degToRad(l0));
    auto sinm   = std::sin(degToRad(m));
    auto cos2l0 = std::cos(2.0*degToRad(l0));
    auto sin4l0 = std::sin(4.0*degToRad(l0));
    auto sin2m  = std::sin(2.0*degToRad(m));

    auto Etime = y*sin2l0 - 2.0*e*sinm + 4.0*e*y*sinm*cos2l0
               - 0.5*y*y*sin4l0 - 1.25*e*e*sin2m;
    return radToDeg(Etime)*4.0;	// in minutes of time
}

///--------------------------------------------------------------------------///
///                            Sunrise/Sunset                                ///
///--------------------------------------------------------------------------///

double calcHourAngleSunrise(const double lat, const double solarDec)
{
    auto latRad = degToRad(lat);
    auto sdRad  = degToRad(solarDec);
    auto HAarg = (std::cos(degToRad(90.833))/(std::cos(latRad)*std::cos(sdRad))
                - std::tan(latRad)*std::tan(sdRad));
    auto HA = std::acos(HAarg);
    return HA; // in radians (for sunset, use -HA)
}

double calcSunriseSetUTC(const bool rise, const double JD,
                         const double latitude, const double longitude)
{
    auto t = calcTimeJulianCent(JD);
    auto eqTime = calcEquationOfTime(t);
    auto solarDec = calcSunDeclination(t);
    auto hourAngle = calcHourAngleSunrise(latitude, solarDec);
    if (!rise) hourAngle = -hourAngle;
    auto delta = longitude + radToDeg(hourAngle);
    auto timeUTC = 720 - (4.0*delta) - eqTime; // in minutes
    return timeUTC;
}

std::pair<double, double> 
    calcAzEl(const double T, const double localtime,
             const double latitude, const double longitude, const int zone,
             const double eqTime, const double theta)
{
    auto solarTimeFix = eqTime + 4.0*longitude - 60.0*zone;
    //auto earthRadVec = calcSunRadVector(T);
    auto trueSolarTime = localtime + solarTimeFix;
    while (trueSolarTime > 1440)
    {
        trueSolarTime -= 1440;
    }
    auto hourAngle = trueSolarTime/4.0 - 180.0;
    if (hourAngle < -180)
    {
        hourAngle += 360.0;
    }
    auto haRad = degToRad(hourAngle);
    auto csz = std::sin(degToRad(latitude))*std::sin(degToRad(theta))
             + std::cos(degToRad(latitude))
              *std::cos(degToRad(theta))*std::cos(haRad);
    if (csz > 1.0)
    {
        csz = 1.0;
    }
    else if (csz < -1.0)
    { 
       csz = -1.0;
    }
    auto zenith = radToDeg(std::acos(csz));
    auto azDenom = std::cos(degToRad(latitude))*std::sin(degToRad(zenith));
    double azimuth = 0;
    if (std::abs(azDenom) > 0.001)
    {
        auto azRad = ((std::sin(degToRad(latitude))
                      *std::cos(degToRad(zenith))) - std::sin(degToRad(theta)))
                     /azDenom;
        if (std::abs(azRad) > 1.0)
        {
            if (azRad < 0)
            {
                azRad = -1.0;
            }
            else
            {
                azRad = 1.0;
            }
        }
        azimuth = 180.0 - radToDeg(std::acos(azRad));
        if (hourAngle > 0.0)
        {
            azimuth = -azimuth;
        }
    }
    else
    {
        if (latitude > 0.0)
        {
            azimuth = 180.0;
	}
        else
        { 
            azimuth = 0.0;
	}
    }
    if (azimuth < 0.0)
    {
        azimuth += 360.0;
    }
    auto exoatmElevation = 90.0 - zenith;
    // Atmospheric Refraction correction
    auto refractionCorrection = calcRefraction(exoatmElevation);
    auto solarZen = zenith - refractionCorrection;
    auto elevation = 90.0 - solarZen;
    return std::pair<double, double> (azimuth, elevation);
}

double calcJDofNextPrevRiseSet(const int next, const bool rise,
                               const double JD,
                               const double latitude, const double longitude,
                               const double tz)
{
    auto julianday = JD;
    auto increment = ((next) ? 1.0 : -1.0);
    auto time = calcSunriseSetUTC(rise, julianday, latitude, longitude);
    while (std::isnan(time)) //!isNumber(time))
    {
        julianday += increment;
        time = calcSunriseSetUTC(rise, julianday, latitude, longitude);
    }
    auto timeLocal = time + tz*60.0;
    while ((timeLocal < 0.0) || (timeLocal >= 1440.0))
    {
       auto incr = (timeLocal < 0) ? 1 : -1;
       timeLocal += (incr * 1440.0);
       julianday -= incr;
    }
    return julianday;
}

// rise = 1 for sunrise, 0 for sunset
std::tuple<double, double, double>
    calcSunriseSet(const bool rise, const double JD,
                   const double latitude,
                   const double longitude, const double timezone)
{
    auto timeUTC = calcSunriseSetUTC(rise, JD, latitude, longitude);
    auto newTimeUTC = calcSunriseSetUTC(rise, JD + timeUTC/1440.0,
                                        latitude, longitude);
    double timeLocal = 0;
    double azimuth = 0;
    double jday = 0;
    if (!std::isnan(newTimeUTC))
    {
        timeLocal = newTimeUTC + (timezone*60.0);
        auto riseT = calcTimeJulianCent(JD + newTimeUTC/1440.0);
        auto eqnOfTime = calcEquationOfTime(riseT);
        auto theta = calcSunDeclination(riseT);
        auto riseAzEl = calcAzEl(riseT, timeLocal, latitude,
                                 longitude, timezone,
                                 eqnOfTime, theta);
        azimuth = riseAzEl.first;
        jday = JD;
        if ( (timeLocal < 0.0) || (timeLocal >= 1440.0) )
        {
             auto increment = ((timeLocal < 0) ? 1 : -1);
             while ((timeLocal < 0.0)||(timeLocal >= 1440.0))
             {
                 timeLocal += increment * 1440.0;
                 jday -= increment;
             }
        }
    }
    else // no sunrise/set found
    {
        std::cerr << "Untested portion of code" << std::endl; 
        azimuth = -1.0;
        timeLocal = 0.0;
        auto doy = calcDoyFromJD(JD);
        if ( ((latitude > 66.4) && (doy > 79) && (doy < 267)) ||
             ((latitude < -66.4) && ((doy < 83) || (doy > 263))) )
        {
            //previous sunrise/next sunset
            jday = calcJDofNextPrevRiseSet(!rise, rise, JD, latitude, longitude,
                                           timezone);
        }
        else
        {
            // previous sunset/next sunrise
            jday = calcJDofNextPrevRiseSet(rise, rise, JD, latitude, longitude,
                                           timezone);
        }
    }
    return std::tuple<double, double, double> (jday, timeLocal, azimuth);
    ///{"jday": jday, "timelocal": timeLocal, "azimuth": azimuth}
}

double calcSolNoon(const double jd, const double longitude,
                   const double timezone)
{
    auto tnoon = calcTimeJulianCent(jd - longitude/360.0);
    auto eqTime = calcEquationOfTime(tnoon);
    auto solNoonOffset = 720.0 - longitude*4 - eqTime; // in minutes
    auto newt = calcTimeJulianCent(jd + solNoonOffset/1440.0);
    eqTime = calcEquationOfTime(newt);
    auto solNoonLocal = 720 - longitude*4 - eqTime + timezone*60.0; // in minutes
    while (solNoonLocal < 0.0)
    {
        solNoonLocal += 1440.0;
    }
    while (solNoonLocal >= 1440.0)
    {
        solNoonLocal -= 1440.0;
    }
    return solNoonLocal;
}

}

class Sun::SunImpl
{
public:
    void update()
    {
        constexpr double tz = 0;
        //calculate(mTime, mLocation);
        auto jday = getJD(mTime.getYear(), mTime.getMonth(),
                          mTime.getDayOfMonth());
        auto timeLocal = mTime.getHour()*60
                       + mTime.getMinute()
                       + mTime.getSecond()/60.0;
        auto total = jday + timeLocal/1440.0 - tz/24.0;
        auto T = calcTimeJulianCent(total);
        mEquationOfTime = calcEquationOfTime(T);
        mSolarDeclination = calcSunDeclination(T);
        auto azel = calcAzEl(T, timeLocal, mLocation.getLatitude(),
                             mLocation.getLongitude(), tz,
                             mEquationOfTime, mSolarDeclination);
        auto solnoon = calcSolNoon(jday, mLocation.getLongitude(), tz);
	auto rise = calcSunriseSet(true,  jday, mLocation.getLatitude(),
                                   mLocation.getLongitude(), tz);
	auto set  = calcSunriseSet(false, jday, mLocation.getLatitude(), 
                                   mLocation.getLongitude(), tz);
        auto sunrise = calcDateFromJD(std::get<0> (rise));
        auto sunset  = calcDateFromJD(std::get<0> (set));
       
        mAzimuth = azel.first;
        mElevation = azel.second;
        mSolarNoon = solnoon;
 
        try
        {
            Time::UTC t;
            t.setYear(std::get<0> (sunrise));
            auto iday = static_cast<int> (std::round(std::get<2> (sunrise)));
            t.setMonthAndDay({std::get<1> (sunrise), iday});
            auto hour = std::get<1> (rise)/60.;
            auto ihour = static_cast<int> (hour);
            auto minute = (hour - ihour)*60.;
            auto iminute = static_cast<int> (minute);
            auto second = (minute - iminute)*60.;
            auto isecond = static_cast<int> (second);
            t.setHour(hour);
            t.setMinute(iminute);
            t.setSecond(isecond);
            mSunrise = t;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Failed to set sunrise: " << e.what() << std::endl;
            mSunrise.clear();
        }
        try
        {
            Time::UTC t;
            t.setYear(std::get<0> (sunset));
            auto iday = static_cast<int> (std::round(std::get<2> (sunset)));
            t.setMonthAndDay({std::get<1> (sunset), iday});
            auto hour = std::get<1> (set)/60.;
            auto ihour = static_cast<int> (hour);
            auto minute = (hour - ihour)*60.;
            auto iminute = static_cast<int> (minute);
            auto second = (minute - iminute)*60.;
            auto isecond = static_cast<int> (second);
            t.setHour(hour);
            t.setMinute(iminute);
            t.setSecond(isecond);
            mSunset = t;
        }
        catch (const std::exception &e) 
        {
            std::cerr << "Failed to set sunset: " << e.what() << std::endl;
            mSunset.clear();
        }
/*
        std::cout << std::setprecision(16) << jday << " " << T << std::endl;
        std::cout << mEquationOfTime << " " << mSolarDeclination << " " << mAzimuth << " " << mElevation << std::endl;
std::cout << solnoon/60. << std::endl;
std::cout << std::get<0> (sunrise) << " " << std::get<1> (sunrise) << " " << std::get<2> (sunrise) << " " << std::get<1> (rise)/60. << " " << mSunrise << std::endl;
std::cout << std::get<0> (sunset)  << " " << std::get<1> (sunset)  << " " << std::get<2> (sunset)  << " " << std::get<1> (set)/60. <<  " " << mSunset << std::endl;
*/
    }

    Location mLocation;
    Time::UTC mTime;
    /// Date of sunrise (TODO debugging needed)
    Time::UTC mSunrise;
    /// Date of sunset (TODO debbugging needed)
    Time::UTC mSunset;
    /// Solar azimuth (degrees)
    double mAzimuth = 0;
    /// Solar elevation (degrees)
    double mElevation = 0;
    /// Equation of time (minutes)
    double mEquationOfTime = 0;
    /// Solar declination (degrees)
    double mSolarDeclination = 0;
    /// Solar noon (TODO debugging needed)
    double mSolarNoon = 0;
    bool mHaveLocation = false;
    bool mHaveTime = false;
};

/// C'tor
Sun::Sun() :
    pImpl(std::make_unique<SunImpl> ())
{
}

/// Copy c'tor
Sun::Sun(const Sun &sun)
{
    *this = sun;
}

/// Move c'tor
Sun::Sun(Sun &&sun) noexcept
{
    *this = std::move(sun);
}

/// Copy assignment
Sun& Sun::operator=(const Sun &sun)
{
    if (&sun == this){return *this;}
    pImpl = std::make_unique<SunImpl> (*sun.pImpl);
    return *this;
}

/// Move assignment
Sun& Sun::operator=(Sun &&sun) noexcept
{
    if (&sun == this){return *this;}
    pImpl = std::move(sun.pImpl);
    return *this;
}

/// Destructor
Sun::~Sun() = default;

/// Clear
void Sun::clear() noexcept
{
    pImpl = std::make_unique<SunImpl> ();
}

/// Location
void Sun::setLocation(const Location &location)
{
    if (!location.haveLatitude()){throw std::invalid_argument("Latitude not set");}
    if (!location.haveLongitude()){throw std::invalid_argument("Longitude not set");}
    pImpl->mLocation = location;
    pImpl->mHaveLocation = true;
    // Update?
    if (haveTime()){pImpl->update();}
}

Location Sun::getLocation() const
{
    if (!haveLocation()){throw std::runtime_error("Location not yet set");}
    return pImpl->mLocation;
}

bool Sun::haveLocation() const noexcept
{
    return pImpl->mHaveLocation;
} 

/// Time 
void Sun::setTime(const int64_t epoch)
{
    Time::UTC time(epoch);
    if (time.getYear() > 2999)
    {   
        throw std::invalid_argument("Year must be less than 2999");
    }   
    if (time.getYear() < -1000)
    {   
        throw std::invalid_argument("Year must be greater than -1000");
    }
    pImpl->mTime = time;
    pImpl->mHaveTime = true;
    // Update?
    if (haveLocation()){pImpl->update();}
}

int64_t Sun::getTime() const
{ 
    if (!haveTime()){throw std::runtime_error("Time not yet set");}
    return static_cast<int64_t> (std::round(pImpl->mTime.getEpoch()));
}

bool Sun::haveTime() const noexcept
{
    return pImpl->mHaveTime;
}

/// Have location and time?
bool Sun::haveTimeAndLocation() const noexcept
{
    return haveLocation() & haveTime();
}

/// Elevation
double Sun::getElevation() const
{
    if (!haveTimeAndLocation())
    {
        if (!haveLocation()){throw std::runtime_error("Location not set");}
        if (!haveTime()){throw std::runtime_error("Time not set");}
    }
    return pImpl->mElevation;
}

/// Azimuth
double Sun::getAzimuth() const
{
    if (!haveTimeAndLocation())
    {
        if (!haveLocation()){throw std::runtime_error("Location not set");}
        if (!haveTime()){throw std::runtime_error("Time not set");}
    }
    return pImpl->mAzimuth;
}

/// Eqn of Time
double Sun::getEquationOfTime() const
{
    if (!haveTimeAndLocation())
    {   
        if (!haveLocation()){throw std::runtime_error("Location not set");}
        if (!haveTime()){throw std::runtime_error("Time not set");}
    }
    return pImpl->mEquationOfTime;
}

/// Declination
double Sun::getDeclination() const
{
    if (!haveTimeAndLocation())
    {
        if (!haveLocation()){throw std::runtime_error("Location not set");}
        if (!haveTime()){throw std::runtime_error("Time not set");}
    }
    return pImpl->mSolarDeclination;
}
