set(OpenGL_GL_PREFERENCE "GLVND")

find_package(glfw3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
find_package(OpenGL REQUIRED)
add_definitions(${OPENGL_DEFINITIONS})
find_package(X11 REQUIRED)
find_package(Freetype REQUIRED)

set(LIBS glfw X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread freetype)

add_library(stb_image "../vendor/stb_image/stb_image.cpp")
target_include_directories(stb_image PRIVATE "../vendor/stb_image/")
set(LIBS ${LIBS} stb_image)

add_library(glad "../vendor/glad/glad.c")
target_include_directories(glad PUBLIC "../vendor/glad/")
set(LIBS ${LIBS} glad)

add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD)
file(GLOB IMGUI_SOURCE "../vendor/imgui/*.cpp")
add_library(imgui ${IMGUI_SOURCE})
target_include_directories(imgui PRIVATE "../vendor/imgui/" "../vendor/")
set(LIBS ${LIBS} imgui )
set(LIBS ${LIBS} CACHE INTERNAL "")
