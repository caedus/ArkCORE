# Copyright (C) 2011 ArkCORE <http://www.arkania.net/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB_RECURSE sources *.cpp *.h)

# uncomment next line to disable debug mode
add_definitions("-DIOMAP_DEBUG")

# build setup currently only supports libmpq 0.4.x
add_definitions("-DUSE_LIBMPQ04")
add_definitions("-Wall")
add_definitions("-ggdb")
add_definitions("-O3")

if( UNIX )
  include_directories(
    ${CMAKE_SOURCE_DIR}/dep/libmpq
  )
elseif( WIN32 )
  include_directories(
    ${CMAKE_SOURCE_DIR}/dep/libmpq
    ${CMAKE_SOURCE_DIR}/dep/libmpq/win
  )
endif()

add_executable(vmap3extractor ${sources})

target_link_libraries(vmap3extractor
  ${BZIP2_LIBRARIES}
  ${ZLIB_LIBRARIES}
  mpq
)

add_dependencies(vmap3extractor mpq)

if( UNIX )
  install(TARGETS vmap3extractor DESTINATION bin)
elseif( WIN32 )
  install(TARGETS vmap3extractor DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()
