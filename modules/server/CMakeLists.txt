file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

add_library(ServerLib ${SERVER_SOURCES})
target_include_directories(ServerLib
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(ServerLib
    PRIVATE
        SharedLib
        crossguid
        concurrentqueue
        hv
        EnTT::EnTT
)