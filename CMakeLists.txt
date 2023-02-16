cmake_minimum_required(VERSION 3.22)
project(kafkaStudyProject)

add_executable(newOrder service-new-order/newOrder.cpp)

target_link_libraries(newOrder -lrdkafka)

target_include_directories(newOrder
 PUBLIC
  common/
  service-new-order/)