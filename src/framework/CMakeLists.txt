# BlizzLikeCore integrates as part of this file: CREDITS.md and LICENSE.md

SET(blizzlikeframework_STAT_SRCS
   Policies/ObjectLifeTime.cpp
   Utilities/EventProcessor.cpp
)

include_directories(
  ${ACE_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/src/framework
)

add_library(blizzlikeframework STATIC ${blizzlikeframework_STAT_SRCS})
