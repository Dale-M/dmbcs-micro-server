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


#ifndef DMBCS__MICRO_SERVER__HTTP_SERVER__H
#define DMBCS__MICRO_SERVER__HTTP_SERVER__H


#include <DMBCS/micro-server/tcp-server.h>
#include <DMBCS/micro-server/query-string.h>


namespace DMBCS { namespace Micro_Server {


  /** We understand a HTTP server as a purely functional object: a
   *  method call with arguments, returning a value which is a function
   *  only of those arguments and having no other side-effects.  The
   *  prototype URL is
   *  {host}:{port}/{method}?{par1}={value1}&{par2}={value2}... where
   *  {host} is obviously us and {port} is supplied to the server
   *  constructor.  The constructor method (\c create) takes one other
   *  argument: a mapping of {method} names onto function objects; these
   *  are called with a set of parameters as distilled from the URL, and
   *  a socket to which the function is expected to send the
   *  corresponding reply.  Convenience methods are provided by this
   *  class to help with the composition of those replies, and all
   *  functions should conclude their work by calling one of these
   *  methods.
   *
   *  Thus there are two functions which the application is immediately
   *  concerned with: the \c create function which returns a
   *  fully-constructed \c Http_Server object given a map of method names
   *  to functions and a local port number on which to operate, and the \c
   *  tick function which causes the server to operate for a single
   *  interaction with some client.  The application should call this \c
   *  tick method frequently so that the server can function as required.
   *
   *  The rest of the interesting functions are the \c return_* functions,
   *  which will send to a given \c client_socket a fully-formed HTTP
   *  response carrying a payload of various type: PNG image, JSON data,
   *  or HTML web pages.  */

  struct Http_Server  :  Tcp_Server
  {
    /** The type of function-thing which implements the brawn behind a
     *  particular service call.  */
    using  Service_Method  =  function<void (Query_String const &, 
                                             int const client_socket)>;
    

    /** The type of object which holds the correspondence between server
     *  method names and the C++ function objects which implement the
     *  methods. */
    using  Service_Methods  =  map<string, Service_Method>;


    Service_Methods  service_methods;


    /** The sole object constructor.  Creates a ready-to-use server object
     *  which operates the HTTP protocol on the local \a port, and will
     *  call out to functions in the \a methods object.  The only action
     *  which can be performed on this object is to pass it to the \c tick
     *  function below, allowing the server to perform a small unit of
     *  work. */
    Http_Server   (uint16_t const port,  Service_Methods &&methods)
      : Tcp_Server {port}, service_methods {move (methods)}
    {}


    void  process_message  (vector<char> &&message,
                            int const socket)  override;



    /*  !!!!  We need to question whether we want this or not. */
    static  string  add_html_headers  (string const &&);



    /** Convenience function allowing a server method to send a \a html
     *  page back to the client on the given \a client_socket. */
    static void  return_html  (int const client_socket,  string const &html);


    /** Convenience function allowing a server method to send a \a text
     *  page back to the client on the given \a client_socket. */
    static void  return_text  (int const client_socket,  string const &text);


    /** Convenience function allowing a server method to send a \a CSS
     *  specification back to the client on the given \a client_socket. */
    static void  return_css  (int const client_socket,  string const &text);


    /** Convenience function allowing a server method to send a \a pdf
     *  document back to the client on the given \a client_socket. */
    static void  return_pdf  (int const client_socket,  string const &pdf);


    /** Convenience function allowing a server method to send a \a json
     *  data structure back to the client on the given \a
     *  client_socket. */
    static void  return_json  (int const client_socket,  string const &json,
                               const string_view  origin_url);


    /** Convenience function allowing a server method to send a response
     *  to a bad request to the client.  Note that we actually send a good
     *  response of an empty payload in this case (we have got this far;
     *  as far as the HTTP protocol is concerned any well-formatted
     *  response from us is a good one, even if it signals failure of the
     *  server to produce meaningful results). */
    static void return_bad (int const client_socket);


    /** Convenience function allowing a server method to send a \a png
     *  image back to the client on the given \a client_socket.  The \a
     *  png should be provided by the \c Png class available with this
     *  library. */
    static void return_png (int const client_socket, 
                            vector<uint8_t> const &png);


    /** Convenience function allowing a server method to send a \a svg
     *  image back to the client on the given \a client_socket.  The \a
     *  png should be provided by the \c Png class available with this
     *  library. */
    static void return_svg (int const client_socket, 
                            string const &svg);


  } ;  /* End of class Http_Server. */


} }  /* End of namespace DMBCS::Micro_Server. */



inline  void   add  (DMBCS::Micro_Server::Http_Server  &S,
                     std::string  &&method_name,
                     DMBCS::Micro_Server::Http_Server::Service_Method  &&M)
{   S.service_methods.emplace  (method_name,  M);   }



inline  bool  tick  (DMBCS::Micro_Server::Http_Server  &S,
                     const  std::chrono::system_clock::duration&  wait)
{  return  S.tick (wait);   }



#include <DMBCS/micro-server/http-server--impl.h>


#endif  /* Undefined DMBCS__MICRO_SERVER__HTTP_SERVER__H. */
