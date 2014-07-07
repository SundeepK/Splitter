cmake_minimum_required(VERSION 2.6)
project(Splitter)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Enable debug symbols by default
if(CMAKE_BUILD_TYPE STREQUAL "")
      set(CMAKE_BUILD_TYPE Debug)
  endif()

  include_directories("${PROJECT_BINARY_DIR}")
  include_directories("${PROJECT_SOURCE_DIR}/include")
  include_directories("${PROJECT_SOURCE_DIR}/src")
  include_directories("${PROJECT_SOURCE_DIR}/Box2d-tools/Box2d\ tools")
  include_directories("${PROJECT_SOURCE_DIR}/SFML-tools/SFML-tools")

  set(SHAD "${PROJECT_SOURCE_DIR}/shaders/")

  set(SRC "${PROJECT_SOURCE_DIR}/src/")
  set(B2DT_SRC "${PROJECT_SOURCE_DIR}/Box2d-tools/Box2d\ tools/src")
  set(B2DT_HEAD "${PROJECT_SOURCE_DIR}/Box2d-tools/Box2d\ tools/header")
  set(SFMLT_SRC "${PROJECT_SOURCE_DIR}/SFML-tools/SFML-tools/src")
  set(SFMLT_HEAD "${PROJECT_SOURCE_DIR}/SFML-tools/SFML-tools/header")

  # Define sources and executable
  set(EXECUTABLE_NAME Splitter)
  
  set(SPLITTER_SRC
      "${SRC}/Splitter.cpp" 
      "${SRC}/TextureMapper.cpp")

  set(BOX2D_TOOLS_HEAD
       "${B2DT_HEAD}/B2BoxBuilder.cpp"
       "${B2DT_HEAD}/B2DBuilder.cpp"
       "${B2DT_HEAD}/Box2DConstants.cpp"
       "${B2DT_HEAD}/SFMLB2dDebugDraw.cpp"
       "${B2DT_HEAD}/Vec.cpp"
     )


  set(BOX2D_TOOLS_SRC
      "${B2DT_SRC}/B2BoxBuilder.cpp"
      "${B2DT_SRC}/B2DBuilder.cpp"
      "${B2DT_SRC}/Box2DConstants.cpp"
      "${B2DT_SRC}/SFMLB2dDebugDraw.cpp"
      "${B2DT_SRC}/Vec.cpp"
      ) 

  set(SFML_TOOLS_HEAD
      "${SFMLT_HEAD}/ActionController.h"
      "${SFMLT_HEAD}/Action.h"
      "${SFMLT_HEAD}/ActionNode.h"
      "${SFMLT_HEAD}/EventNode.h"   
      "${SFMLT_HEAD}/InputEventNode.h"
      "${SFMLT_HEAD}/InputNodes.h"
      "${SFMLT_HEAD}/TemplateHasher.h"
      )
  
  set(SFML_TOOLS_SRC
      "${SFMLT_SRC}/ActionController.cpp"
      "${SFMLT_SRC}/Action.cpp"
      "${SFMLT_SRC}/ActionNode.cpp"
      "${SFMLT_SRC}/EventNode.cpp"   
      "${SFMLT_SRC}/InputEventNode.cpp"
      "${SFMLT_SRC}/InputNodes.cpp"
      "${SFMLT_SRC}/TemplateHasher.cpp"
      )
  
  #file(GLOB DAYLIGHT_SOURCES *.cpp)
  add_executable(${EXECUTABLE_NAME} ${DAYLIGHT_SOURCES})

  file(COPY ${SHAD} DESTINATION "shaders")

  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
  #Find any version 2.X of SFML
  #See the FindSFML.cmake file for additional details and instructions
  find_package(SFML 2 REQUIRED system window graphics network audio)
  if(SFML_FOUND)
        include_directories(${SFML_INCLUDE_DIR})
          target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
      endif()


      # Install target
      install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)


      # CPack packaging
      include(InstallRequiredSystemLibraries)
      include(CPack)
