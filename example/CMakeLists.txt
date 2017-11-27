project(prometheus-cpp-it)

find_package(Threads)

add_executable(${PROJECT_NAME}
        src/server_test.cpp)

target_link_libraries(${PROJECT_NAME} prometheus-cpp cpp-httplib ${CMAKE_THREAD_LIBS_INIT})
