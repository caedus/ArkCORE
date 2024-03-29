# Copyright (C) 2011 ArkCORE <http://www.arkania.net/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

if( CMAKE_COMPILER_IS_GNUCXX )
  add_definitions(--no-warnings)
elseif( MSVC )
  add_definitions(/W0)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  if(SERVERS)
    add_subdirectory(jemalloc)
  endif()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  if(SERVERS)
    add_subdirectory(acelite)
    if(USE_MYSQL_SOURCES)
      add_subdirectory(mysqllite)
    endif()
  endif()
  if(TOOLS)
    add_subdirectory(bzip2)
  endif()
  add_subdirectory(zlib)
endif()

add_subdirectory(g3dlite)

if(SERVERS)
  add_subdirectory(gsoap)
endif()

if(TOOLS)
  add_subdirectory(StormLib)
  add_subdirectory(jenkins)
  add_subdirectory(libtomcrypt)
  add_subdirectory(libtommath)
  add_subdirectory(adpcm)
  add_subdirectory(pklib)
  add_subdirectory(lzma)
  add_subdirectory(huffman)
  add_subdirectory(sparse)
  add_subdirectory(libmpq)
endif()
