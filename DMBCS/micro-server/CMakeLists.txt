add_library  (dmbcs-micro-server  SHARED
              http-server.cc hyper-tags.cc query-string.cc
              tcp-server.cc udp-server.cc  utils.cc)

target_compile_features  (dmbcs-micro-server  PUBLIC  cxx_std_20)

target_compile_options (dmbcs-micro-server
                                 PUBLIC  -fconcepts
                                 PRIVATE -Wall -Wextra -DMAC="1235"
                                         -I${CMAKE_CURRENT_SOURCE_DIR}/../..)

install  (TARGETS  dmbcs-micro-server  DESTINATION lib)
install  (FILES  micro-server.h  DESTINATION  include/DMBCS)
install  (FILES  http-server.h http-server--impl.h hyper-tags.h
                 micro-server.h  query-string.h
                 tcp-server.h udp-server.h utils.h
          DESTINATION  include/DMBCS/micro-server)
