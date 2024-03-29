# Copyright (C) 2011 ArkCORE <http://www.arkania.net/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

include_directories(
  ${CMAKE_SOURCE_DIR}/dep/g3dlite/include
  ${CMAKE_SOURCE_DIR}/src/server/shared
  ${CMAKE_SOURCE_DIR}/src/server/shared/Debugging
  ${CMAKE_SOURCE_DIR}/src/server/collision/Maps
  ${CMAKE_SOURCE_DIR}/src/server/collision/Models
  ${ACE_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIR}
)

add_definitions(-DNO_CORE_FUNCS)
add_executable(vmap3assembler VMapAssembler.cpp)
add_dependencies(vmap3assembler mpq)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set_target_properties(vmap3assembler PROPERTIES LINK_FLAGS "-framework Carbon")
endif()

target_link_libraries(vmap3assembler
  collision
  g3dlib
  ${ZLIB_LIBRARIES}
)

if( UNIX )
  install(TARGETS vmap3assembler DESTINATION bin)
elseif( WIN32 )
  install(TARGETS vmap3assembler DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()
