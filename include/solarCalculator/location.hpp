#ifndef SOLARCALCULATOR_LOCATION_HPP
#define SOLARCALCULATOR_LOCATION_HPP
#include <memory>
namespace SolarCalculator
{
/// @class Location "location.hpp" "solarCalculator/location.hpp"
/// @brief Defines a computation location by a latitude and longitude.
/// @copyright Ben Baker (University of Utah) distributed under the MIT license.
class Location
{
public:
    /// @name Constructors
    /// @{
    /// @brief Constructor.
    Location();
    /// @brief Creates a location from the given latitude and longitude.
    /// @param[in] latitude    The location's latitude in degrees.
    /// @param[in] longitude   The location's longitude in degrees.
    /// @sa \c setLatitude(), \c setLongitude().
    Location(double latitude, double longitude);
    /// @brief Copy constructor.
    /// @param[in] location   The location class from which to initialize
    ///                       this class.
    Location(const Location &location);
    /// @brief Move constructor.
    /// @param[in,out] location  The location class from which to initialize
    ///                          this class.  On exit, location's behavior
    ///                          is undefined.
    Location(Location &&location) noexcept;
    /// @}

    /// @name Operators
    /// @{
    /// @brief Copy assignment operator.
    /// @param[in] location  The location class to copy to this.
    /// @result A deep copy of location.
    Location& operator=(const Location &location);
    /// @brief Move assignment operator.
    /// @param[in,out] location  The location class whose memory will be moved
    ///                          to this.  On exit, location's behavior is
    ///                          undefined.
    /// @result The memory from location moved to this.
    Location& operator=(Location &&location) noexcept;
    /// @}

    /// @name Latitude
    /// @{
    /// @brief Sets the latitude.
    /// @param[in] latitude  The latitude in degrees where positive is north
    ///                      from the equator.  This must be in the
    ///                      range [-90,90].
    void setLatitude(double latitude);
    /// @result The latitude in degrees.
    /// @throws std::runtime_error if \c haveLatitude() is false.
    [[nodiscard]] double getLatitude() const;
    /// @result True indicates that the latitude was set.
    [[nodiscard]] bool haveLatitude() const noexcept;
    /// @}

    /// @name Longitude
    /// @{
    /// @brief Sets the longitude.
    /// @param[in] longitude  The longitude in degrees where positive is east.
    ///                       This must be in the range [-540,540).
    void setLongitude(double longitude);
    /// @result The longitude in degrees.
    /// @throws std::runtime_error if \c haveLongitude() is false.
    [[nodiscard]] double getLongitude() const;
    /// @result True indicates that the longitude was set.
    [[nodiscard]] bool haveLongitude() const noexcept;
    /// @}

    /// @name Destructors
    /// @{
    /// @brief Destructor.
    ~Location();
    /// @brief Resets the class.
    void clear() noexcept;
    /// @}
private:
    class LocationImpl;
    std::unique_ptr<LocationImpl> pImpl;
};
}
#endif
