cmake_minimum_required(VERSION 3.8)

aux_source_directory(. KARL_SRCS)

set(SOURCES
    ${KARL_SRCS}
)

set(KARL_INC
    PUBLIC
    ../include
)

add_library(karl-static STATIC ${SOURCES})
target_include_directories(karl-static ${KARL_INC})

if (WIN32)
target_link_libraries(karl-static)
else (WIN32)
target_link_libraries(karl-static -lm -lstdc++ )
endif (WIN32)
set_target_properties(karl-static PROPERTIES OUTPUT_NAME karl CLEAN_DIRECT_OUTPUT 1)
