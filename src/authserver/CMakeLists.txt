# BlizzLikeCore integrates as part of this file: CREDITS.md and LICENSE.md

file(GLOB sources_localdir *.cpp *.h)

set(authserver_SRCS
  ${authserver_SRCS}
  ${sources_localdir}
)

if( WIN32 )
  set(authserver_SRCS
    ${authserver_SRCS}
    ${sources_Debugging}
    authserver.rc
  )
endif()

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_SOURCE_DIR}/src/shared/Database
  ${CMAKE_SOURCE_DIR}/src/framework
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
)

set(authserver_LINK_FLAGS "")

add_executable(authserver
  ${authserver_SRCS}
)

add_dependencies(authserver revision.h)

if( NOT WIN32 )
  add_definitions(-D_BLIZZLIKE_AUTH_CONFIG="${CONF_DIR}/authserver.conf")
endif()

if( UNIX )
  set(authserver_LINK_FLAGS "-pthread ${authserver_LINK_FLAGS}")
endif()

if( CMAKE_SYSTEM_NAME MATCHES "Darwin" )
  set(authserver_LINK_FLAGS "-framework Carbon ${authserver_LINK_FLAGS}")
endif()

set_target_properties(authserver PROPERTIES LINK_FLAGS "${authserver_LINK_FLAGS}")

target_link_libraries(authserver
  shared
  blizzlikeframework
  ${MYSQL_LIBRARY}
  ${OPENSSL_LIBRARIES}
  ${OPENSSL_EXTRA_LIBRARIES}
  ${OSX_LIBS}
)

if( WIN32 )
  add_custom_command(TARGET authserver
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/authserver.conf.dist ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
  )
endif()

if( UNIX )
  install(TARGETS authserver DESTINATION bin)
  install(FILES authserver.conf.dist DESTINATION etc)
elseif( WIN32 )
  install(TARGETS authserver DESTINATION "${CMAKE_INSTALL_PREFIX}")
  install(FILES authserver.conf.dist DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()
