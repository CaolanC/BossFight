add_executable(Tests
        JSONHelperTests.cpp
        SceneSnapshotTests.cpp
        SceneSerializerTests.cpp
        ModelManagerTests.cpp
        SessionTests.cpp
        AssetHelperTests.cpp
        SpawnTests.cpp
        UserControlTests.cpp
        TransformSystemTests.cpp
        IntegrationTests.cpp
)

target_link_libraries(Tests PRIVATE GTest::gtest_main SharedLib ClientLib ServerLib)
target_compile_definitions(Tests PRIVATE SLJA_ASSETS_DIR="${ASSETS_DIR}")
target_compile_definitions(Tests PRIVATE SLJA_SAVES_DIR="${SAVES_DIR}")
target_include_directories(Tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    add_custom_command(TARGET Tests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/bin/libhv.dll"
            $<TARGET_FILE_DIR:Tests>
    )
endif()

include(GoogleTest)
gtest_discover_tests(Tests)