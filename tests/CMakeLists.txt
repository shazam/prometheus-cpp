project(prometheus-cpp-test)

enable_testing()

add_executable(${PROJECT_NAME}
        check_names_test.cpp
        counter_test.cpp
        untyped_test.cpp
        gauge_test.cpp
        histogram_test.cpp
        text_searializer_untyped_test.cpp
        text_searializer_counter_test.cpp
        text_searializer_gauge_test.cpp
        text_searializer_histogram_test.cpp
        summary_test.cpp
        text_searializer_summary_test.cpp
        )

# bring in the private includes because we are going to test them :)
target_include_directories(${PROJECT_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/src>)

target_link_libraries(${PROJECT_NAME} gmock_main prometheus-cpp)
add_test(NAME ${PROJECT_NAME} COMMAND $<TARGET_FILE:${PROJECT_NAME}>)