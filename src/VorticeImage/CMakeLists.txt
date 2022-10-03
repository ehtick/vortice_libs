# Define target name
set (TARGET_NAME VorticeImage)

add_library(${TARGET_NAME}
    include/VorticeImage.h
    src/VorticeImage.c
    src/VorticeImagePrivate.h
)

target_include_directories(${TARGET_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_definitions(${TARGET_NAME} PRIVATE VIMAGE_SHARED_LIBRARY_BUILD=1)

target_link_libraries(${TARGET_NAME}
    ktx
)

# Build flags
if(NOT MSVC)
    set_property(TARGET ${TARGET_NAME} PROPERTY COMPILE_FLAGS "-std=gnu99 -Wall -Wno-strict-aliasing -pedantic")
endif()

# MinGW builds should statically link libgcc
if(MINGW)
    target_link_libraries(${TARGET_NAME} PRIVATE -static-libgcc)
endif()

if(BUILD_SHARED_LIBS)
	set_property(TARGET ${TARGET_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

# Soname
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "${TARGET_NAME}"
	VERSION ${LIB_VERSION}
	SOVERSION ${LIB_MAJOR_VERSION}
)
