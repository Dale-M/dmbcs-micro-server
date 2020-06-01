cmake_minimum_required  (VERSION 3.10)
project  (dmbcs-micro-server  VERSION 0.4  LANGUAGES CXX)
find_package (PkgConfig  REQUIRED)
pkg_check_modules  (third-party REQUIRED IMPORTED_TARGET curlpp)

add_subdirectory  (DMBCS/micro-server)

configure_file  (dmbcs-micro-server.pc.in  dmbcs-micro-server.pc  @ONLY)
install (FILES ${PROJECT_BINARY_DIR}/dmbcs-micro-server.pc
         DESTINATION  lib/pkgconfig)



include (InstallRequiredSystemLibraries REQUIRED)

set     (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set     (CPACK_PACKAGE_VERSION_MAJOR "${dmbcs-micro-server_VERSION_MAJOR}")
set     (CPACK_PACKAGE_VERSION_MINOR "${dmbcs-micro-server_VERSION_MINOR}")

include (CPack)



find_program  (MAKEINFO makeinfo)

set (info_in ${CMAKE_CURRENT_SOURCE_DIR}/dmbcs-micro-server.texi)
set (info_out ${CMAKE_CURRENT_BINARY_DIR}/dmbcs-micro-server.info)

configure_file  (version.texi.in  version.texi  @ONLY)
add_custom_command (OUTPUT ${info_out}
                    COMMAND  ${MAKEINFO} -o ${info_out} ${info_in}
                    DEPENDS ${info_in} ${CMAKE_CURRENT_BINARY_DIR}/version.texi
                    COMMENT "Creating info file ${info_out}"
                    VERBATIM)

add_custom_target  (info ALL DEPENDS ${info_out})

install            (FILES  ${info_out}  DESTINATION  share/info)
