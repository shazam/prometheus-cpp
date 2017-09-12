project(prometheus-cpp)

set(PUBLIC_HEADER_FILES
        include/prometheus/metric.h
        include/prometheus/type_metric.h
        include/prometheus/atomic_double.h
        include/prometheus/counter.h
        include/prometheus/gauge.h
        include/prometheus/histogram.h
        include/prometheus/metric_vector.h
        include/prometheus/metric_registry.h
        include/prometheus/summary.h
        include/prometheus/metric_family.h
        include/prometheus/metric_labels.h
        include/prometheus/spin_lock.h
        include/prometheus/timers.h
        include/prometheus/check.h
        )

add_library(${PROJECT_NAME}
        src/check_names.cpp
        src/text_serializer.cpp
        src/metric_registry.cpp
        src/histogram.cpp
        src/summary.cpp
        src/metric_family.cpp

        ${PUBLIC_HEADER_FILES}
        )

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/prometheus>)

target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(${PROJECT_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        PUBLIC_HEADER DESTINATION include
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)
