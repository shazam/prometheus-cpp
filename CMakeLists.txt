cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

option(ENABLE_TESTS "Compile source code for tests." ON)
option(ENABLE_EXAMPLE "Compile source code for example project." ON)
option(ENABLE_PACKAGE "Allow package of lib and headers" ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
endif ()

# main library
add_subdirectory(lib)

if (ENABLE_EXAMPLE)
    include(cmake/external/cpp-httplib.cmake)
    add_subdirectory(example)
endif ()

if (ENABLE_TESTS)
    include(CTest)
    include(cmake/external/google_mock.cmake)  # This comes bundled with GoogleTest
    add_subdirectory(tests)
endif ()

if (ENABLE_PACKAGE)
    set(CPACK_PACKAGE_NAME "prometheus-cpp")
    set(CPACK_PACKAGE_VERSION_MAJOR "0")
    set(CPACK_PACKAGE_VERSION_MINOR "1")
    set(CPACK_PACKAGE_VERSION_PATCH "0")
    set(CPACK_PACKAGE_VENDOR "Shazam Entertainment Ltd")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
    set(CPACK_GENERATOR "ZIP")
    set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
    include(CPack)
endif ()