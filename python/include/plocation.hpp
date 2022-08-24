#ifndef PSOLARCALCULATOR_LOCATION_HPP
#define PSOLARCALCULATOR_LOCATION_HPP
#include <memory>
#include <pybind11/pybind11.h>
namespace SolarCalculator
{
class Location;
}

namespace PSolarCalculator
{
class Location
{
public:
    /// Constructors
    Location();
    Location(const Location &location);
    Location(Location &&sun) noexcept;
    Location(const SolarCalculator::Location &location);

    /// Operators
    Location& operator=(const Location &location);
    Location& operator=(const SolarCalculator::Location &location);
    Location& operator=(Location &&location) noexcept;

    [[nodiscard]] SolarCalculator::Location getNativeClass() const noexcept;

    void setLatitude(double latitude);
    [[nodiscard]] double getLatitude() const;
    //[[nodiscard]] bool haveLatitude() const noexcept;

    void setLongitude(double longitude);
    [[nodiscard]] double getLongitude() const;
    //[[nodiscard]] bool haveLongitude() const noexcept;

    /// Destructors
    ~Location();
    void clear() noexcept;
private:
    std::unique_ptr<SolarCalculator::Location> mLocation;
};
void initializeLocation(pybind11::module &m);
}
#endif
