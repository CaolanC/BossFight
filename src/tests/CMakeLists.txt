add_executable(UnitTests
        JSONHelperTests.cpp
        SceneSnapshotTests.cpp
        SceneSerializerTests.cpp
        ModelManagerTests.cpp
        SessionTests.cpp
        AssetHelperTests.cpp
        SpawnTests.cpp
        UserControlTests.cpp
        TransformSystemTests.cpp
)

target_link_libraries(UnitTests PRIVATE GTest::gtest_main SharedLib ClientLib ServerLib)
target_compile_definitions(UnitTests PRIVATE SLJA_ASSETS_DIR="${ASSETS_DIR}")
target_compile_definitions(UnitTests PRIVATE SLJA_SAVES_DIR="${SAVES_DIR}")
target_include_directories(UnitTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    add_custom_command(TARGET UnitTests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/bin/libhv.dll"
            $<TARGET_FILE_DIR:UnitTests>
    )
endif()

include(GoogleTest)
gtest_discover_tests(UnitTests)