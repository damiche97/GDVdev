#Author: Roman Getto                          
#Roman.Getto@gris.informatik.tu-darmstadt.de                   

cmake_minimum_required(VERSION 2.8)
project(PRAK2)

# GROUP SOURCES AND CREATE PROJECT
SET(HEADER_FILES
  "main.h"
  "TriangleMesh.h"
  "shader.h"
  "Vec3.h"
  "Task.h"
  "GridTask.h"
  "SceneTask.h"
  "Matrix.h"
  "Timer.h"
)
SET(SOURCE_FILES  
  "main.cpp"
  "TriangleMesh.cpp"
  "Task.cpp"
  "GridTask.cpp"
  "SceneTask.cpp"
  "Matrix.cpp"
  "Timer.cpp"
)
SET(SHADER_FILES
  "Shader/FlatGreyShader.vert"
  "Shader/FlatGreyShader.frag"
)
SOURCE_GROUP(Header FILES ${HEADER_FILES})
SOURCE_GROUP(Source FILES ${SOURCE_FILES})
SOURCE_GROUP(Shader FILES ${SHADER_FILES})
add_executable(main ${HEADER_FILES} ${SOURCE_FILES} ${SHADER_FILES})

option(AUTO_SEARCH_AND_INCLUDE_OpenGL "You can activate this option or include OpenGL by yourself" ON)
option(AUTO_SEARCH_AND_INCLUDE_Glut "You can activate this option or include GLUT by yourself" ON)
option(AUTO_SEARCH_AND_INCLUDE_Glew "You can activate this option or include GLEW by yourself" ON)

# FIND AND INCLUDE OPENGL				
if(AUTO_SEARCH_AND_INCLUDE_OpenGL)
find_package(OpenGL REQUIRED)
include_directories( ${OpenGL_INCLUDE_DIR})
target_link_libraries(main ${OPENGL_LIBRARIES})
endif(AUTO_SEARCH_AND_INCLUDE_OpenGL)

# FIND AND INCLUDE GLUT 				 
if(AUTO_SEARCH_AND_INCLUDE_Glut)
find_package(GLUT REQUIRED)
include_directories( ${GLUT_INCLUDE_DIR})
target_link_libraries(main ${GLUT_LIBRARIES})        
endif(AUTO_SEARCH_AND_INCLUDE_Glut)

# FIND AND INCLUDE GLEW                                                   
if(AUTO_SEARCH_AND_INCLUDE_Glew)
find_package(GLEW REQUIRED)
include_directories( ${GLEW_INCLUDE_DIRS})
target_link_libraries(main ${GLEW_LIBRARIES})
else(AUTO_SEARCH_AND_INCLUDE_Glew)
# At least define the glew library if the package is not searched       
    IF (WIN32)
    set(GLEW_LIBRARY GLEW32)
    ELSE (WIN32)
    set(GLEW_LIBRARY GLEW)
    ENDIF(WIN32)
    target_link_libraries(main ${GLEW_LIBRARY})
endif(AUTO_SEARCH_AND_INCLUDE_Glew)

#include source                                               
include_directories( ${PROJECT_SOURCE_DIR})


# generate compile_commands.json for ccls language server
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# enable C++17
set_property(TARGET main PROPERTY CXX_STANDARD 17)

if(MSVC)
  # Uncomment to enable more aggressive warnings for msvc
  #target_compile_options(main PRIVATE /W4 /WX)
else()
  target_compile_options(main PRIVATE -Wall -pedantic -Werror)
endif()
