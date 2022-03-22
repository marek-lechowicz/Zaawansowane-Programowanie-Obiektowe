cmake_minimum_required(VERSION 3.13)
project(net_simulation)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors -Werror=switch)

include_directories(include netsim_tests/include)

# FIXME: Uzupełnij.

set(SOURCE_FILES
        src/helpers.cpp
        src/storage_types.cpp
        src/package.cpp
        src/nodes.cpp
        src/factory.cpp
#        src/reports.cpp
#        src/simulation.cpp
        )

add_executable(net_simulation_debug ${SOURCE_FILES} main.cpp)

set(SOURCE_FILES_TESTS

        netsim_tests/test/test_Factory.cpp
#        netsim_tests/test/test_factory_io.cpp
        netsim_tests/test/test_nodes.cpp
        netsim_tests/test/test_package.cpp
#        netsim_tests/test/test_reports.cpp
#        netsim_tests/test/test_simulate.cpp
        netsim_tests/test/test_storage_types.cpp
#        src/simulation.cpp
        )



add_executable(net_simulation_test ${SOURCE_FILES} ${SOURCE_FILES_TESTS} netsim_tests/test/main_gtest.cpp)


target_include_directories(net_simulation_test PUBLIC
        ../googletest-master/googlemock/include
        ../googletest-master/googlemock/include/mocks
        )

add_subdirectory(../googletest-master build)

target_link_libraries(net_simulation_test gmock)

#target_compile_definitions(net_simulation_test PUBLIC EXERCISE_ID=EXERCISE_ID_PACKAGES)
#target_compile_definitions(net_simulation_test PUBLIC EXERCISE_ID=EXERCISE_ID_NODES)
target_compile_definitions(net_simulation_test PUBLIC EXERCISE_ID=EXERCISE_ID_FACTORY)
#target_compile_definitions(net_simulation_test PUBLIC EXERCISE_ID=REPORTING)
#target_compile_definitions(net_simulation_test PUBLIC EXERCISE_ID=SIMULATION)



