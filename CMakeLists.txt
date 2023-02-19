cmake_minimum_required(VERSION 3.22)
project(kafkaStudyProject)

file(GLOB COMMON_SRC
     ${PROJECT_SOURCE_DIR}/common/*.c
     ${PROJECT_SOURCE_DIR}/common/*.cpp)

file(GLOB SERVICE_NEW_ORDER_SRC
     ${PROJECT_SOURCE_DIR}/service-new-order/*.cpp)


add_executable(newOrder     
    ${COMMON_SRC}
    ${SERVICE_NEW_ORDER_SRC})

target_link_libraries(newOrder -lrdkafka)

target_include_directories(newOrder
    PUBLIC
    common/
    service-new-order/)

