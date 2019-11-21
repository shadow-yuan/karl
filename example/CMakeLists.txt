cmake_minimum_required(VERSION 3.8)

aux_source_directory(. KARL_SOURCES)

include_directories(
  ../include
)

add_executable(example ${KARL_SOURCES}
	../include/karl/json.hxx
    ../src/cbor.cc
    ../src/cbor.h
    ../src/cJSON.c
    ../src/cJSON.h
    ../src/karl.cc
    ../src/karl.h
)

if (WIN32)
target_link_libraries(example)
else (WIN32)
target_link_libraries(example -lm -lstdc++ )
endif (WIN32)
