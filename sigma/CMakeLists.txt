file(GLOB_RECURSE ENGINE_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
list(REMOVE_ITEM ENGINE_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/pch.h)

file(GLOB_RECURSE ENGINE_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        )

SET(ENGINE_SOURCES ${ENGINE_SOURCES} ${ENGINE_INCLUDES} include/render/shader.cpp include/render/shader.h)

add_library(sigma STATIC ${ENGINE_SOURCES})

target_include_directories(sigma PUBLIC "include/")
target_include_directories(sigma PUBLIC "../vendor/")

target_link_libraries(sigma ${LIBS})

target_precompile_headers(sigma PUBLIC include/pch.h)

