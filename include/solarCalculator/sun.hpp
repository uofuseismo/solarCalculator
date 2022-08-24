#ifndef SOLARCALCULATOR_SUN_HPP
#define SOLARCALCULATOR_SUN_HPP
#include <memory>
namespace SolarCalculator
{
class Location;
/// @class Sun "sun.hpp" "solarCalculator/sun.hpp"
/// @brief This class performs the solar calculator computations and will,
///        for example, compute the sun's azimuth, elevation, etc. at a
///        a position for a given time.
/// @copyright Ben Baker (University of Utah) distributed under the MIT license.
class Sun
{
public:
    /// @name Constructors
    /// @{
    /// @brief Constructors.
    Sun();
    /// @brief Copy constructor.
    /// @param[in] sun  The class from which to initialize this class.
    Sun(const Sun &sun);
    /// @brief Move Constructor.
    /// @param[in,out] sun  The class from which to initialize this class.
    ///                     On exit, sun's behavior is undefined.
    Sun(Sun &&sun) noexcept;
    /// @}

    /// @name Operators
    /// @{
    /// @brief Copy assignment operator.
    /// @param[in] sun  The sun class to copy to this.
    /// @result A deep copy of sun.
    Sun& operator=(const Sun &sun);
    /// @brief Move assignment operator.
    /// @param[in,out] sun  The sun class whose memory will be moved to this.
    ///                     On exit, sun's behavior is undefined.
    /// @result The memory from sun moved to this.
    Sun& operator=(Sun &&sun) noexcept;
    /// @}

    /// @name Parameters
    /// @{
    /// @brief Sets the location at which to compute the solar calculations.
    /// @param[in] location  The location (latitude and longitude) at which
    ///                      to perform the solar calculations.
    /// @throws std::invalid_argument if location.haveLatitude() or 
    ///         location.haveLongitude() is false.
    void setLocation(const Location &location);
    /// @result The location at which to perform the calculation.
    /// @throws std::runtime_error if \c haveLocation() is false.
    [[nodiscard]] Location getLocation() const;
    /// @result True indicates that the location was set.
    [[nodiscard]] bool haveLocation() const noexcept;

    /// @brief Sets the time at which to perform the solar calculations.
    /// @param[in] time  The UTC time in seconds from the epoch
    ///                  (e.g., Jan 1 1970).
    /// @throws std::invalid_argument if the time stamp is earlier than the year
    ///         -1000 or greater than the year 2999.
    void setTime(int64_t time);
    /// @result The time at which to perform the solar calculations.
    /// @throws std::runtime_error if \c haveTime() is false.
    [[nodiscard]] int64_t getTime() const;
    /// @result True indicates that the time was set.
    [[nodiscard]] bool haveTime() const noexcept;
 
    /// @result Convenience function to determine if both \c haveTime()
    ///         and \c haveLocation() are true.
    [[nodiscard]] bool haveTimeAndLocation() const noexcept;
    /// @}

    /// @name Results
    /// @{
    /// @result The angle between the sun and the horizon in degrees.
    /// @throws std::runtime_error if \c haveTimeAndLocation() is false.
    [[nodiscard]] double getElevation() const;
    /// @result The azimuth of the sun in degrees measured positive clockwise
    ///         from true north.
    /// @throws std::runtime_error if \c haveTimeAndLocation() is false. 
    [[nodiscard]] double getAzimuth() const;
    /// @result The declination of the sun in degrees.  This varies
    ///         from -23.44 degrees in the northern hemisphere during
    ///         the winter solstice to 0 degrees at the vernal equinox to
    ///         +23.44 degrees at the summer solstice.
    /// @throws std::runtime_error if \c haveTimeAndLocation() is false.
    [[nodiscard]] double getDeclination() const;
    /// @result The equation of time in minutes.  This is an astronomical
    ///         term accounting for the changes in time of solar noon for
    ///         a given location over the course of the year.
    /// @throws std::runtime_error if \c haveTimeAndLocation() is false.
    [[nodiscard]] double getEquationOfTime() const;
    /// @}

    /// @name Destructors
    /// @{
    /// @result Resets the class.
    void clear() noexcept;
    /// @brief Destructor.
    ~Sun();
    /// @}
private:
    class SunImpl;
    std::unique_ptr<SunImpl> pImpl;
};
}
#endif
