# Copyright (C) 2011 ArkCORE <http://www.arkania.net/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB sources *.cpp)

set(huffman_STAT_SRCS
  ${sources}
)

include_directories(
  ${CMAKE_SOURCE_DIR}/dep/StormLib
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(huffman STATIC ${huffman_STAT_SRCS})
