#ifndef PSOLARCALCULATOR_SUN_HPP
#define PSOLARCALCULATOR_SUN_HPP
#include <memory>
#include <pybind11/pybind11.h>
namespace SolarCalculator
{
class Sun;
}

namespace PSolarCalculator
{
class Location;
class Sun
{
public:
    /// Constructor
    Sun();
    Sun(const Sun &sun);
    Sun(Sun &&sun) noexcept;

    Sun& operator=(const Sun &sun);
    Sun& operator=(Sun &&sun) noexcept;

    void setLocation(const Location &location);
    [[nodiscard]] Location getLocation() const;
    void setTime(int64_t time);
    [[nodiscard]] int64_t getTime() const;
    //[[nodiscard]] bool haveTime() const noexcept;
    //[[nodiscard]] bool haveTimeAndLocation() const noexcept;

    [[nodiscard]] double getElevation() const;
    [[nodiscard]] double getAzimuth() const;
    [[nodiscard]] double getDeclination() const;
    [[nodiscard]] double getEquationOfTime() const;

    /// Destructors
    ~Sun();
    void clear() noexcept;
private:
    std::unique_ptr<SolarCalculator::Sun> mSun;
};
void initializeSun(pybind11::module &m);
}
#endif
