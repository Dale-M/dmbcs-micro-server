/*
 *  dmbcs-micro-server    A C++ library providing CGI or built-in
 *                        web server functions
 *
 *  Copyright (C) 2018  DM Bespoke Computer Solutions Ltd
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <DMBCS/micro-server/udp-server.h>
#include <DMBCS/micro-server/tcp-server.h>   /*  For bind_socket.  */
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <unistd.h>


namespace DMBCS { namespace Micro_Server {


  Udp_Server::Udp_Server  (uint16_t const port)
  {
    listen_socket  =  Tcp_Server::bind_socket  (port,  SOCK_DGRAM);
  }



  bool  Udp_Server::tick  (std::chrono::system_clock::duration  linger)
  {
    namespace  C  =  std::chrono;

    auto  s  =  provide_fdset  (*this);

    timeval  t  { C::duration_cast<C::seconds> (linger).count (),
                  C::duration_cast<C::microseconds> (linger).count ()
                      % 1'000'000};

    auto const test  =  select (listen_socket + 1, &s, nullptr, nullptr, &t);

    if (-1 == test)
      {
        if (EINTR  ==  errno)
          throw runtime_error {"program interrupted"};
      }

    else if (0 == test)
      return 0;

    else if (FD_ISSET (listen_socket, &s))
      {
        static  char buffer [1500];
        auto  const  size
                       =  recv  (listen_socket,  buffer,  sizeof (buffer),  0);

        process_message  (std::vector<uint8_t> 
                               ((uint8_t*) buffer, (uint8_t*) (buffer + size)));
      }

    return 1;
  }



  int  append_to_fdset  (Udp_Server const &udp,
                         fd_set *const set)
  {
    FD_SET  (udp.listen_socket,  set);
    return  udp.listen_socket;
  }



  fd_set  provide_fdset  (Udp_Server const &udp)
  {
    fd_set  ret;
    FD_ZERO  (&ret);
    append_to_fdset  (udp,  &ret);
    return  ret;
  }


} }  /* End of namespace DMBCS::Micro_Server. */
