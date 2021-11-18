project(The-Room)
add_executable(${PROJECT_NAME} camera.cpp ground.cpp main.cpp openglwindow.cpp roof.cpp left_wall.cpp right_wall.cpp back_wall.cpp front_wall.cpp model.cpp)
enable_abcg(${PROJECT_NAME})
