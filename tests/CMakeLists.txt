add_subdirectory(../external/catch2 catch2)

add_executable(sLink_Tests
        test_message.cpp
        test_timestamp.cpp
        test_safe_queue.cpp
)

target_link_libraries(sLink_Tests
        PRIVATE
        common_shared
        Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(sLink_Tests)