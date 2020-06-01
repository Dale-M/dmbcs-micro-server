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


#ifndef DMBCS__MICRO_SERVER__UDP_SERVER__H
#define DMBCS__MICRO_SERVER__UDP_SERVER__H


#include  <chrono>
#include  <functional>
#include  <map>
#include  <poll.h>
#include  <set>
#include  <string>
#include  <vector>


namespace DMBCS { namespace Micro_Server {


  using namespace  std;


  /* A Udp_Server is an object which listens to a UDP port, accepts
   * connections from multiple clients (and disconnections), and takes any
   * messages these clients may send to us and hand them off to a
   * specialized message despatcher.  Real servers will inherit this class
   * and provide their own specialized message processor. */

  struct Udp_Server
  {
    /* The socket on which we take new connections. */
    int  listen_socket;

    std::string  buffer;


    /** The sole object constructor.  Creates a ready-to-use server object
     *  which operates the TCP protocol on the local \a port, and will
     *  call out to the process_message method when any messages come in.
     *  The only action which can be performed on this object is to call
     *  the \c tick method, allowing the server to perform a small unit of
     *  work. */
    explicit  Udp_Server   (uint16_t const port);


    /* Boilerplate requirement of destructor of virtual class. */
    virtual ~Udp_Server ()  =  default;


    /* A function which must take the message which has come off of the
     * wire, do whatever work is required to service the message, and then
     * send an appropriate response back to the client, via the given
     * socket. */
    virtual  void  process_message  (std::vector<uint8_t> &&)  =  0;


    /** Cause the server to perform some small unit of work, or wait up to
     *  /wait/ time for some work requirement to appear.
     *
     *  The return value indicates if any work was done. */
    bool   tick  (std::chrono::system_clock::duration);


  } ;  /* End of class Udp_Server. */



  /* Append all listening and connected sockets in the Udp_Server to the
   * fd_set. */
  std::vector<pollfd>  append_to_pollset  (const Udp_Server&,
                                           std::vector<pollfd>&&);

  inline  std::vector<pollfd>  append_to_pollset  (std::vector<pollfd>&&  p,
                                                   const Udp_Server&  U)
  {   return  append_to_pollset  (U, std::move (p));   }


  /* Manufacture a pollset containing the listening socket and all of the
   * connected client sockets in the Udp_Server. */
  std::vector<pollfd>  provide_pollset  (const Udp_Server&);



} }  /* End of namespace DMBCS::Micro_Server. */


#endif  /* Undefined DMBCS__MICRO_SERVER__UDP_SERVER__H. */
