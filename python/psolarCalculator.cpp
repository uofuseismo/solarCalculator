#include "include/psun.hpp"
#include "include/plocation.hpp"
#include "include/plocation.hpp"
//#include <solarCalculator/version.hpp>
#include <pybind11/pybind11.h>

PYBIND11_MODULE(pysolarCalculator, m)
{
    //m.attr("__version__") = SOLARCALCULATOR_VERSION;
    m.attr("__doc__") = "Given a latitude, longitude, and time this estimates solar properties such as the solar elevation and azimuth.";

    PSolarCalculator::initializeLocation(m);
    PSolarCalculator::initializeSun(m);
}
