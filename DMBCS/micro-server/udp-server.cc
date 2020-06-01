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

    std::vector<pollfd>  s  =  provide_pollset  (*this);

    const int  test
                =  poll  (s.data (),  s.size (),
                          C::duration_cast<C::milliseconds> (linger).count ());

    if (-1 == test)
      {
        if (EINTR  ==  errno)
          throw runtime_error {"program interrupted"};
      }

    else if (0 == test)
      return 0;

    else if (s [0].revents & POLLIN)
      {
        static  char buffer [1500];
        auto  const  size
                       =  recv  (listen_socket,  buffer,  sizeof (buffer),  0);

        process_message  (std::vector<uint8_t> 
                               ((uint8_t*) buffer, (uint8_t*) (buffer + size)));
      }

    return 1;
  }



  std::vector<pollfd>  append_to_pollset  (const Udp_Server&  udp,
                                           std::vector<pollfd>&&  p)
  {
    p.push_back  ({  .fd  =  udp.listen_socket,
                     .events  =  POLLIN,
                     .revents =  0             });
    return  p;
  }



  std::vector<pollfd>  provide_pollset  (const Udp_Server&  udp)
  {
    return  {{  .fd  =  udp.listen_socket,
                .events  =  POLLIN,
                .revents =  0              }};
  }


} }  /* End of namespace DMBCS::Micro_Server. */
