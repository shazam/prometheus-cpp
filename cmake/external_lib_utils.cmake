# Handle XCode build type folders
macro(SET_XCODE_MSVC_EXT_LIB lib)

    # get the original libary location, and append the build varient name to the path
    get_target_property(ORIGIN_LOCATION ${lib} IMPORTED_LOCATION)
    get_filename_component(LIBRARY_BASE_DIR ${ORIGIN_LOCATION} DIRECTORY)

    if(CMAKE_GENERATOR STREQUAL Xcode OR MSVC)
        foreach(BTYPE ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER ${BTYPE} UPPER_BTYPE)

            set_target_properties(${lib} PROPERTIES
               IMPORTED_LOCATION_${UPPER_BTYPE} "${LIBRARY_BASE_DIR}/${BTYPE}/${CMAKE_FIND_LIBRARY_PREFIXES}${lib}${CMAKE_STATIC_LIBRARY_SUFFIX}"
            )
        endforeach()
   endif()

endmacro()
