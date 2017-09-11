
find_package(Boost 1.51.0 REQUIRED COMPONENTS graph random)

set(SOURCE_FILES TestMain.cpp GraphFuzzingTest.cpp)
set(HEADER_FILES include/catch.hpp)

add_executable(FringeSearchTest ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(FringeSearchTest PUBLIC FringeSearch PRIVATE ${Boost_LIBRARIES})

target_include_directories(FringeSearchTest PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE ${Boost_INCLUDE_DIRS})