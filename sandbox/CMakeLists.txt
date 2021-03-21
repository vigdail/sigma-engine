file(GLOB_RECURSE GAME_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
list(REMOVE_ITEM GAME_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/pch.h)

file(GLOB_RECURSE GAME_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

SET(GAME_SOURCES ${GAME_SOURCES} ${GAME_INCLUDES})

add_executable(sandbox ${GAME_SOURCES})

target_include_directories(sandbox PUBLIC include)
target_include_directories(sandbox PRIVATE ../sigma/include)

set_target_properties(sandbox PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

target_link_libraries(sandbox sigma)

