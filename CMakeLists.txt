cmake_minimum_required(VERSION  3.26.4)
project(particle)


add_executable(${PROJECT_NAME} src/main.cpp)
add_subdirectory(opengl-wrapper/)
target_link_libraries(${PROJECT_NAME} glm imgui glwrap)
