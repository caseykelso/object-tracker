cmake_minimum_required (VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Use -std=c++20 instead of -std=gnu++20
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++")
project(tracker-rbr-tests)
add_executable(tracker_rbr_tests tests.cpp)
include_directories(BEFORE ${CMAKE_PREFIX_PATH}/include ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tracker_rbr_tests tracker gtest pthread rt)
target_link_directories(tracker_rbr_tests BEFORE PUBLIC ${CMAKE_PREFIX_PATH}/lib)
install(TARGETS tracker_rbr_tests DESTINATION bin)

