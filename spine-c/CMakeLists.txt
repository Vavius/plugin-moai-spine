cmake_minimum_required ( VERSION 2.8.5 )
project ( spine )

    set ( SPINE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )

    set ( SPINE_INCLUDES "${SPINE_SOURCE_DIR}/include")

    file ( GLOB SPINE_SOURCES 
        "${SPINE_SOURCE_DIR}/src/spine/*.c" 
        "${SPINE_SOURCE_DIR}/src/spine/*.h" 
        "${SPINE_SOURCE_DIR}/include/spine/*.h" 
    )

add_library(spine STATIC ${SPINE_SOURCES})
target_include_directories(spine PUBLIC ${SPINE_INCLUDES})