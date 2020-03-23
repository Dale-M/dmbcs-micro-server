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



bool  Tcp_Server::tick  (const chrono::system_clock::duration&  wait,
                         fd_set&  read_socket_set,
                         std::function<void(const fd_set&)>  fallout)
  {
    append_to_fdset (*this, &read_socket_set);

    timeval  t  {chrono::duration_cast<chrono::seconds> (wait).count (),
                 chrono::duration_cast<chrono::microseconds> (wait).count ()
                     % 1'000'000};

    auto const test
      =  select (FD_SETSIZE, &read_socket_set, nullptr, nullptr, &t);

    if (-1 == test)
       {
           if (EINTR  ==  errno)
                throw runtime_error {"program interrupted"};
           else
                throw runtime_error {"TCP server socket problem"};
       }

    if (0 == test)
      return 0;

    if (FD_ISSET (listen_socket, &read_socket_set))
       {
           sockaddr_in  addr;
           socklen_t size  =  sizeof (addr);
           int  sock  =  accept (listen_socket, (sockaddr*) &addr, &size);
           client_sockets [sock] = addr;
           return  1;
       }

    for (auto const c : client_sockets)
      if (FD_ISSET (c.first, &read_socket_set))
        {
          /* !!  Hardwire, hard limit. */
          auto const len  =  read (c.first,
                                   message_buffer.data (),
                                   message_buffer.size ());

          if  (len  <  0)
               {
                    std::cerr  <<  strerror (errno);
                    close (c.first);
                    client_departed  (c.first);
                    client_sockets.erase  (c.first);
               }

          else if  (! len)
               {
                    close (c.first);
                    client_departed  (c.first);
                    client_sockets.erase  (c.first);
               }

          else
              process_message  ({begin (message_buffer),
                                 begin (message_buffer) + len},
                                c.first);

          return 1;
        }

    fallout (read_socket_set);
    return 1;
  }



bool  Aggregate_Ticker::operator()
                          (const  chrono::system_clock::duration&  wait)
  {
    fd_set  fds;
    FD_ZERO (&fds);
    int top {0};

    for  (auto &T  :  servers)
      top  =  std::max (top, append_to_fdset  (*T, &fds));
    
    timeval  t  { chrono::duration_cast<chrono::seconds> (wait).count (),
                  chrono::duration_cast<chrono::microseconds> (wait).count ()
                      % 1'000'000};

    auto const test  =  select (FD_SETSIZE, &fds, nullptr, nullptr, &t);

    if (-1 == test)
      {
        if (EINTR  ==  errno)   throw  runtime_error {"program interrupted"};
        else
          throw  runtime_error {"server socket problem, errno="
                                                   + std::to_string (errno)};
      }

    if (0 == test)    return 0;

    for  (auto  &T   :  servers)
        if  (T->tick (chrono::microseconds {0}))
            return 1;

    return 0;
  }



  int  append_to_fdset  (Tcp_Server const &tcp,
                         fd_set *const set)
  {
    FD_SET (tcp.listen_socket, set);
    for  (auto const &S  :  tcp.client_sockets)  FD_SET (S.first, set);
    return std::max (tcp.listen_socket, begin (tcp.client_sockets)->first);
  }



  fd_set  provide_fdset  (Tcp_Server const &tcp)
  {
    fd_set  ret;
    FD_ZERO (&ret);
    append_to_fdset (tcp, &ret);
    return  ret;
  }



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
