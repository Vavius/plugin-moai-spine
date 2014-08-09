cmake_minimum_required ( VERSION 2.8.5 )
project ( moai-spine )


  set ( PLUGIN_ROOT ${CMAKE_CURRENT_SOURCE_DIR} )

  set ( MOAI_SPINE_INCLUDES 
        ${PLUGIN_ROOT}/../
  )

  # Use all cpp files in directory as source code
  file ( GLOB MOAI_SPINE_SRC 
    "${PLUGIN_ROOT}/*.cpp" 
    "${PLUGIN_ROOT}/*.h" 
  )


  add_library ( moai-spine STATIC ${MOAI_SPINE_SRC} )
  target_include_directories( moai-spine PUBLIC ${MOAI_SPINE_INCLUDES})
  target_link_libraries (moai-spine spine moai-core moai-util moai-sim zlcore)
