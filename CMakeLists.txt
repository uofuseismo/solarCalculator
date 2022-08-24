cmake_minimum_required(VERSION 3.16)
project(solarCalculator VERSION 1.0.0 LANGUAGES CXX)
enable_testing()

include(CheckCXXCompilerFlag)
find_package(GTest REQUIRED)
set(FindGeographicLib_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(FindTime_DIR ${CMAKE_SOURCE_DIR}/cmake)
#find_package(FindGeographicLib REQUIRED)
find_package(FindTime REQUIRED)

set(PUBLIC_HEADER_DIRECTORIES
    ${CMAKE_SOURCE_DIR}/include)

set(SRC
    src/location.cpp
    src/sun.cpp)
add_library(solarCalculator SHARED ${SRC})
target_link_libraries(solarCalculator ${TIME_LIBRARY})
target_include_directories(solarCalculator
                           PUBLIC
                              $<BUILD_INTERFACE:${PUBLIC_HEADER_DIRECTORIES}>
                              $<INSTALL_INTERFACE:${PUBLIC_HEADER_DIRECTORIES}>
                           PRIVATE
                              $<BUILD_INTERFACE:${TIME_INCLUDE_DIR}>)
set_source_files_properties(src/sun.cpp PROPERTIES COMPILE_FLAGS -fno-fast-math)
set_target_properties(solarCalculator PROPERTIES
                      CXX_STANDARD 20
                      CXX_STANDARD_REQUIRED YES
                      CXX_EXTENSIONS NO)

# Python bindings
option(WRAP_PYTHON "WRAP_PYTHON" OFF)
if (WRAP_PYTHON)
   find_package(pybind11 REQUIRED)
   add_library(pysolarCalculator MODULE
               python/psolarCalculator.cpp
               python/plocation.cpp
               python/psun.cpp)
   target_link_libraries(pysolarCalculator PRIVATE  pybind11::module solarCalculator)
   target_include_directories(pysolarCalculator
                              PRIVATE
                                 $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/python>
                                 $<BUILD_INTERFACE:${PUBLIC_HEADER_DIRECTORIES}>)
   set_target_properties(pysolarCalculator PROPERTIES
                         PREFIX ""
                         CXX_STANDARD 20
                         CXX_STANDARD_REQUIRED YES
                         CXX_EXTENSIONS NO)
   configure_file(python/unit_tests.py unit_tests.py COPYONLY)
endif()

set(TEST_SRC
    testing/main.cpp
    testing/location.cpp
    testing/sun.cpp)

add_executable(unitTests ${TEST_SRC})
set_target_properties(unitTests PROPERTIES
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED YES
                      CXX_EXTENSIONS NO)
target_link_libraries(unitTests PRIVATE solarCalculator ${GTEST_BOTH_LIBRARIES})
target_include_directories(unitTests
                           PRIVATE
                              $<BUILD_INTERFACE:${PUBLIC_HEADER_DIRECTORIES}>
                              $<BUILD_INTERFACE:${GTEST_INCLUDE_DIRS}>)
add_test(NAME unitsTests
         COMMAND unitTests)

#========================================================================================#
#                                      Installation                                      #
#========================================================================================#
include(GNUInstallDirs)
if (WRAP_PYTHON)
   install(TARGETS solarCalculator pysolarCalculator
           RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
           LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
           ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
           PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
else()
   install(TARGETS solarCalculator
           RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
           LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
           ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
           PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()
install(DIRECTORY ${PUBLIC_HEADER_DIRECTORIES}/solarCalculator
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
##########################################################################################
#                                     CPACK Packaging                                    #
##########################################################################################
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "UUSS")
set(CPACK_PACKAGE_CONTACT "ben.baker@utah.edu")
set(CPACK_PACKAGE_LICENSE "MIT")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A utility for computing solar angle.  This is based on https://gml.noaa.gov/grad/solcalc/")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_COMPONENTS_ALL libraries headers)
if (WIN32)
    set(CPACK_GENERATOR ZIP WIX)
elseif (APPLE)
    set(CPACK_GENERATOR TGZ productbuild)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CPACK_GENERATOR TGZ RPM)
else()
    set(CPACK_GENERATOR TGZ)
endif()
set(CPACK_SOURCE_IGNORE_FILES
  /\\.git/
  \\.swp
  \\.orig
  /CMakeLists\\.txt\\.user
  /private/
)
include(CPack) # Put this last!
