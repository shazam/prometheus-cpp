include(cmake/external_lib_utils.cmake)

find_package(Threads)

# Google Mock dependency
include(ExternalProject)
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/third_party)

# Add google Mock
ExternalProject_Add(
        googleMock
        EXCLUDE_FROM_ALL 1
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.8.0
        CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        BUILD_BYPRODUCTS "${CMAKE_BINARY_DIR}/third_party/src/googleMock-build/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}"
        BUILD_BYPRODUCTS "${CMAKE_BINARY_DIR}/third_party/src/googleMock-build/googlemock/${CMAKE_FIND_LIBRARY_PREFIXES}gmock${CMAKE_STATIC_LIBRARY_SUFFIX}"
        BUILD_BYPRODUCTS "${CMAKE_BINARY_DIR}/third_party/src/googleMock-build/googlemock/${CMAKE_FIND_LIBRARY_PREFIXES}gmock_main${CMAKE_STATIC_LIBRARY_SUFFIX}"
        # Disable install step
        INSTALL_COMMAND ""
        UPDATE_COMMAND ""
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON
        LOG_BUILD ON)

# Specify library output dirs
ExternalProject_Get_Property(googleMock source_dir)
ExternalProject_Get_Property(googleMock binary_dir)

#GTest
set(GTEST_INCLUDE_DIR ${source_dir}/googletest/include)
file(MAKE_DIRECTORY ${GTEST_INCLUDE_DIR})

set(GTEST_LIBRARY_PATH ${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_STATIC_LIBRARY_SUFFIX})
set(GTEST_LIBRARY gtest)
add_library(${GTEST_LIBRARY} IMPORTED STATIC GLOBAL)
set_target_properties(${GTEST_LIBRARY} PROPERTIES
        IMPORTED_LOCATION "${GTEST_LIBRARY_PATH}"
        INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
        IMPORTED_LINK_INTERFACE_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}"
        )
SET_XCODE_MSVC_EXT_LIB(gtest)


set(GMOCK_INCLUDE_DIR ${source_dir}/googlemock/include)
file(MAKE_DIRECTORY ${GMOCK_INCLUDE_DIR})

set(GMOCK_LIBRARY_PATH ${binary_dir}/googlemock/${CMAKE_FIND_LIBRARY_PREFIXES}gmock${CMAKE_STATIC_LIBRARY_SUFFIX})

# Library
set(GMOCK_LIBRARY gmock)
add_library(${GMOCK_LIBRARY} IMPORTED STATIC GLOBAL)
set_target_properties(${GMOCK_LIBRARY} PROPERTIES
        IMPORTED_LOCATION "${GMOCK_LIBRARY_PATH}"
        INTERFACE_INCLUDE_DIRECTORIES "${GMOCK_INCLUDE_DIR}"
        IMPORTED_LINK_INTERFACE_LIBRARIES "${GTEST_LIBRARY};${CMAKE_THREAD_LIBS_INIT}"
        )
SET_XCODE_MSVC_EXT_LIB(gmock)

#GMock main
set(GMOCK_MAIN_LIBRARY_PATH ${binary_dir}/googlemock/${CMAKE_FIND_LIBRARY_PREFIXES}gmock_main${CMAKE_STATIC_LIBRARY_SUFFIX})

add_library(gmock_main IMPORTED STATIC GLOBAL)
set_target_properties(gmock_main PROPERTIES
        IMPORTED_LOCATION "${GMOCK_MAIN_LIBRARY_PATH}"
        IMPORTED_LINK_INTERFACE_LIBRARIES "${GMOCK_LIBRARY}"
        )
SET_XCODE_MSVC_EXT_LIB(gmock_main)

add_dependencies(${GMOCK_LIBRARY} googleMock)
add_dependencies(gmock_main ${GMOCK_LIBRARY})
