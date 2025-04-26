
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} act_lib)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib)