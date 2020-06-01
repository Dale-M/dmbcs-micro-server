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


#ifndef DMBCS__MICRO_SERVER__TCP_SERVER__H
#define DMBCS__MICRO_SERVER__TCP_SERVER__H


#include  <arpa/inet.h>
#include  <chrono>
#include  <functional>
#include  <map>
#include  <poll.h>
#include  <set>
#include  <unistd.h>
#include  <vector>


namespace DMBCS { namespace Micro_Server {


  using namespace  std;


  /* A Tcp_Server is an object which listens to a TCP port, accepts
   * connections from multiple clients (and disconnections), and takes any
   * messages these clients may send to us and hand them off to a
   * specialized message despatcher.  Real servers will inherit this class
   * and provide their own specialized message processor. */

  struct Tcp_Server
  {
    /* The socket on which we take new connections. */
    int listen_socket;

    /* All of the connected sockets, kept in _descending_ order so that
     * the highest file descriptor of the sockets is immediately to
     * hand. */
    map<int, sockaddr_in, greater<int>> client_sockets;

    vector<char>  message_buffer  {vector<char> (10000)};


    static  int  bind_socket  (const uint16_t  port,  const int  socket_type);


    /** The sole object constructor.  Creates a ready-to-use server object
     *  which operates the TCP protocol on the local \a port, and will
     *  call out to the process_message method when any messages come in.
     *  The only action which can be performed on this object is to call
     *  the \c tick method, allowing the server to perform a small unit of
     *  work.
     *
     *  Will throw a runtime_error if it is not possible to establish the
     *  listening socket. */
    explicit  Tcp_Server   (const uint16_t  port);


    /* Boilerplate requirement of destructor of virtual class. */
    virtual ~Tcp_Server () noexcept   {  close  (listen_socket);  }
    


    /* A function which must take the message which has come off of the
     * wire, do whatever work is required to service the message, and then
     * send an appropriate response back to the client, via the given
     * socket. */
    virtual  void  process_message  (vector<char>&&  message,
                                     const int  socket)  =  0;


    /* Will be called when a client disconnects.  */
    virtual  void  client_departed  (const int  /* socket */)   {}


    /** Cause the server to perform some small unit of work, or wait up to
     *  /wait/ time for some work requirement to appear.
     *
     *  The return value indicates if any work was done.  Will throw a
     *  runtime_error if the program receives a signal during
     *  operations. */
    bool  tick  (const std::chrono::system_clock::duration&  wait,
                 std::vector<pollfd>&&  read_socket_set,
                 std::function<void(std::vector<pollfd>&&)>  fallout);

    bool   tick  (const  chrono::system_clock::duration&  wait)
    {
      return  tick (wait, {}, nullptr);
    }


    /** Surrender our interest in this socket.  The socket itself will
     *  continue to remain active, but we will no longer be polling it for
     *  data and returning messages from it.  */
    int  disown  (const int  socket)
    {   client_sockets.erase (socket);    return  socket;  }


  } ;  /* End of class Tcp_Server. */



/* Find out the TCP port number of the listener socket, to which clients
 * can connect.  This is necessary if the class was set up with a zero
 * port, in which case the operating system would have quietly assigned
 * a random port.
 *
 * Will throw a runtime_error if the operation fails. */
uint16_t  listener_port  (const Tcp_Server&);


/* Append all listening and connected sockets in the Tcp_Server to the
 * fd_set. */
std::vector<pollfd>  append_to_pollset  (const Tcp_Server&,
                                         std::vector<pollfd>&&);

inline  std::vector<pollfd>  append_to_pollset  (std::vector<pollfd>&&  p,
                                                 const Tcp_Server&  T)
{   return append_to_pollset  (T, std::move (p));   }


/* Manufacture an fdset containing the listening socket and all of the
 * connected client sockets in the Tcp_Server. */
std::vector<pollfd>  provide_pollset  (const Tcp_Server&);



  /* This class proxies a set of separate Tcp_Server's, presenting them to
   * the application as if they were one.  In particular, it allows each
   * of the servers equal opportunity to service an incoming message.
   *
   * NOTE that it is left to the application to destroy the individual
   * servers, once an Aggregate_Ticker is destroyed. */

  struct Aggregate_Ticker
  {
    vector <Tcp_Server*>   servers;
    
    Aggregate_Ticker  (vector <Tcp_Server*>&&  s_)  :  servers {move (s_)}
    {}
    
    bool operator() (const  chrono::system_clock::duration&  wait);
  };
  

  /* Manufacture an Aggregate_Ticker. */
  inline  Aggregate_Ticker  aggregate_ticker  (vector <Tcp_Server*>&&  S)
  {   return  Aggregate_Ticker {move (S)};   }


} }  /* End of namespace DMBCS::Micro_Server. */


#endif  /* Undefined DMBCS__MICRO_SERVER__TCP_SERVER__H. */
