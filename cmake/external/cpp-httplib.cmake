include(cmake/external_lib_utils.cmake)

find_package(Threads)

include(ExternalProject)
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/third_party)

ExternalProject_Add(
        cpp-httplibExternal
        EXCLUDE_FROM_ALL 1
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
        # Aug 29, 2017
        GIT_TAG 5f1ff58ed2b76c9e915de58d29451663026095e3
        INSTALL_COMMAND ""
        CONFIGURE_COMMAND ""
        UPDATE_COMMAND ""
        BUILD_COMMAND ""
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON
        LOG_BUILD ON)

# Specify library output dirs
ExternalProject_Get_Property(cpp-httplibExternal source_dir)

set(CPPHTTPLIB_INCLUDE_DIR ${source_dir})
file(MAKE_DIRECTORY ${CPPHTTPLIB_INCLUDE_DIR})

set(CPPHTTPLIB_LIBRARY cpp-httplib)
add_library(${CPPHTTPLIB_LIBRARY} INTERFACE IMPORTED GLOBAL)
set_target_properties(${CPPHTTPLIB_LIBRARY} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CPPHTTPLIB_INCLUDE_DIR}")

add_dependencies(${CPPHTTPLIB_LIBRARY} cpp-httplibExternal)
