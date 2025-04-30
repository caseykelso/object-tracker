cmake_minimum_required (VERSION 3.20)
project(tracker-rbr-tests)
add_executable(tracker_rbr_tests tests.cpp)
include_directories(BEFORE ${CMAKE_PREFIX_PATH}/include ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tracker_rbr_tests pthread rt)
install(TARGETS tracker_rbr_tests DESTINATION bin)

