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


#include  <DMBCS/micro-server/tcp-server.h>
#include  <sys/socket.h>
#include  <iostream>
#include  <netinet/in.h>
#include  <string.h>
#include  <sstream>
#include  <vector>
#include  <unistd.h>


namespace DMBCS { namespace Micro_Server {


template <typename Tick,  typename  Duration>
inline  Tick::rep  interval_count  (const Duration&  wait)
       {   return  std::chrono::duration_cast<Tick> (wait).count ();   }


/*  Used also by Udp_Server.  */

int  Tcp_Server::bind_socket  (uint16_t const port,  int const socket_type)
  {
    const int  listen_socket  =  socket (AF_INET, socket_type, 0);

    int option = 1;
    setsockopt (listen_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&option,
                sizeof (option));

    sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons (port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (::bind (listen_socket, (sockaddr const *) &addr, sizeof (addr)))
       throw  runtime_error {"cannot bind to port " + std::to_string (port)};
    
    return   listen_socket;
  }



Tcp_Server::Tcp_Server (uint16_t const port)
  {
    listen_socket  =  bind_socket  (port,  SOCK_STREAM);
    listen  (listen_socket,  5);
  }



bool  Tcp_Server::tick  (const std::chrono::system_clock::duration&  wait,
                         std::vector<pollfd>&&  read_socket_set,
                         std::function<void(std::vector<pollfd>&&)>  fallout)
  {
    std::vector<pollfd>  socket_set
                    =  append_to_pollset  (*this,  std::move (read_socket_set));

    const int  test  =  poll (socket_set.data (),  socket_set.size (),
                              interval_count<std::chrono::milliseconds> (wait));

    if (-1 == test)
       {
           if (EINTR  ==  errno)
                throw runtime_error {"program interrupted"};
           else
                throw runtime_error {"TCP server socket problem"};
       }

    if (0 == test)
      return 0;

    if (socket_set.back ().revents & POLLIN)
       {
           sockaddr_in  addr;
           socklen_t size  =  sizeof (addr);
           int  sock  =  accept (listen_socket, (sockaddr*) &addr, &size);
           client_sockets [sock] = addr;
           return  1;
       }

    for (const auto&  c  :  socket_set)
      if (c.revents & POLLIN)
        {
          /* !!  Hardwire, hard limit. */
          const auto  len  =  read (c.fd,
                                    message_buffer.data (),
                                    message_buffer.size ());

          if  (len  <  0)
               {
                    std::cerr  <<  strerror (errno);
                    close (c.fd);
                    client_departed  (c.fd);
                    client_sockets.erase  (c.fd);
               }

          else if  (! len)
               {
                    close (c.fd);
                    client_departed  (c.fd);
                    client_sockets.erase  (c.fd);
               }

          else
              process_message  ({begin (message_buffer),
                                 begin (message_buffer) + len},
                                c.fd);

          return 1;
        }

    fallout  (std::move  (read_socket_set));
    return 1;
  }



bool  Aggregate_Ticker::operator()
                          (const  chrono::system_clock::duration&  wait)
  {
    std::vector<pollfd>  pollset;

    for  (auto&  T  :  servers)
      pollset  =  append_to_pollset  (*T,  std::move (pollset));

    const int  test  =  poll  (pollset.data (),  pollset.size (),
                               interval_count<std::chrono::milliseconds> (wait));

    if (-1 == test)
      {
        if (EINTR  ==  errno)   throw  runtime_error {"program interrupted"};
        else
          throw  runtime_error {"server socket problem, errno="
                                                   + std::to_string (errno)};
      }

    if (0 == test)    return 0;

    for  (auto&  T   :  servers)
        if  (T->tick (std::chrono::seconds (0)))
            return 1;

    return 0;
  }



std::vector<pollfd>  append_to_pollset  (const Tcp_Server&  tcp,
                                         std::vector<pollfd>&&  p)
  {
      for  (const auto&  S  :  tcp.client_sockets)
              p.push_back ({  .fd = S.first,
                              .events = POLLIN,
                              .revents = 0        });
      p.push_back  ({   .fd = tcp.listen_socket,
                        .events = POLLIN,
                        .revents = 0         });
      return p;
  }



std::vector<pollfd>  provide_pollset  (const Tcp_Server&  tcp)
  {   return  append_to_pollset  (tcp,  {});   }



uint16_t  listener_port  (const Tcp_Server&  T)
  {
    sockaddr_in  addr;
    socklen_t    len  =  sizeof (addr);
    if   (0  !=  getsockname (T.listen_socket,  (sockaddr*) &addr,  &len))
       throw  std::runtime_error 
                        {"cannot get port of Tcp_Serverʼs listener socket"};
    return  ntohs (addr.sin_port);
  }

    

} }  /* End of namespace DMBCS::Micro_Server. */
