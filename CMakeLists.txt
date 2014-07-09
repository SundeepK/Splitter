project(Splitter)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Enable debug symbols by default
if(CMAKE_BUILD_TYPE STREQUAL "")
      set(CMAKE_BUILD_TYPE Debug)
  endif()

  include_directories("${PROJECT_BINARY_DIR}")
  include_directories("${PROJECT_SOURCE_DIR}/include")
  include_directories("${PROJECT_SOURCE_DIR}/example")
  include_directories("${PROJECT_SOURCE_DIR}/src")
  include_directories("${PROJECT_SOURCE_DIR}/Box2d-tools/Box2d\ tools")
  include_directories("${PROJECT_SOURCE_DIR}/Box2d-tools/Box2d\ tools/include")
  include_directories("${PROJECT_SOURCE_DIR}/SFML-tools/SFML-tools")
  include_directories("${PROJECT_SOURCE_DIR}/SFML-tools/SFML-tools/include")


  set(SRC "${PROJECT_SOURCE_DIR}/src")
  set(HEAD "${PROJECT_SOURCE_DIR}/include")
  set(EXAMPLE "${PROJECT_SOURCE_DIR}/example")
  set(B2DT_SRC "${PROJECT_SOURCE_DIR}/Box2d-tools/Box2d\ tools/src")
  set(B2DT_HEAD "${PROJECT_SOURCE_DIR}/Box2d-tools/Box2d\ tools/include")
  set(SFMLT_SRC "${PROJECT_SOURCE_DIR}/SFML-tools/SFML-tools/src")
  set(SFMLT_HEAD "${PROJECT_SOURCE_DIR}/SFML-tools/SFML-tools/include")

  # Define sources and executable
  set(PROJECT_NAME Splitter)
  set(TEXTURES "${PROJECT_SOURCE_DIR}/textures/")
  
  set(SPLITTER_HEAD
      "${HEAD}/Splitter.h" 
      "${HEAD}/TextureMapper.h"
      "${HEAD}/B2BodySplitCallback.h"   
      "${HEAD}/B2vecHasher.h" 
	  "${HEAD}/B2DWorld.h"
     )

  set(SPLITTER_SRC
      "${SRC}/Splitter.cpp" 
      "${SRC}/TextureMapper.cpp")


  set(SPLITTER_EXAMPLE
		"${EXAMPLE}/B2DWorld.cpp"
		"${EXAMPLE}/main.cpp"
	)

  set(BOX2D_TOOLS_HEAD
       "${B2DT_HEAD}/B2BoxBuilder.h"
       "${B2DT_HEAD}/B2Builder.h"
       "${B2DT_HEAD}/Box2DConstants.h"
       "${B2DT_HEAD}/B2DBuilderHelper.h"
       "${B2DT_HEAD}/SFMLB2dDebugDraw.h"
       "${B2DT_HEAD}/Vec.h"
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
      "${SFMLT_SRC}/Action.cpp"
      "${SFMLT_SRC}/AndEventNode.cpp"
      "${SFMLT_SRC}/InputEventNode.cpp"
      "${SFMLT_SRC}/OrEventNode.cpp"   
      "${SFMLT_SRC}/RealtimeAndNode.cpp"
      "${SFMLT_SRC}/RealTimeNode.cpp"
      "${SFMLT_SRC}/RealtimeOrNode.cpp"
      )
 
 file(COPY ${TEXTURES} DESTINATION "textures")

 add_library(box2d_tools SHARED ${BOX2D_TOOLS_HEAD} ${BOX2D_TOOLS_SRC})
 add_library(sfml_tools SHARED ${SFML_TOOLS_HEAD} ${SFML_TOOLS_SRC})
 add_library(splitter SHARED ${SPLITTER_HEAD} ${SPLITTER_SRC})
 add_library(example SHARED ${SPLITTER_EXAMPLE})

 set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
 #Find any version 2.X of SFML
 #See the FindSFML.cmake file for additional details and instructions
 find_package(SFML 2 REQUIRED system window graphics network audio)
 find_package(BOX2D REQUIRED)
 find_package(GLUT REQUIRED)
 find_package(OpenGL REQUIRED)

 


 set(EXECUTABLE_NAME Splitter)

 if(SFML_FOUND)
		add_executable(${EXECUTABLE_NAME} ${EXAMPLE}/main.cpp ${EXAMPLE}/B2DWorld.cpp ${BOX2D_TOOLS_SRC} ${SFML_TOOLS_SRC} ${SPLITTER_SRC})
        target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
        target_link_libraries(${EXECUTABLE_NAME} ${GLUT_LIBRARIES})
        target_link_libraries(${EXECUTABLE_NAME} ${OpenGL_LIBRARIES})
		target_link_libraries(${EXECUTABLE_NAME} GL GLU glut)

        target_link_libraries(${EXECUTABLE_NAME} ${BOX2D_LIBRARIES})
        target_link_libraries(${EXECUTABLE_NAME} ${box2d_tools})
        target_link_libraries(${EXECUTABLE_NAME} ${sfml_tools})
        target_link_libraries(${EXECUTABLE_NAME} ${example})
        target_link_libraries(${EXECUTABLE_NAME} ${splitter})
 endif()


 #Install target
 install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)


 #CPack packaging
 include(InstallRequiredSystemLibraries)
 include(CPack)
