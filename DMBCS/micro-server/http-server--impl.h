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
#error  The http-server--impl.h header should not be included in application code.
#endif


#include <regex>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>


namespace DMBCS { namespace Micro_Server {


  /* This is the detailed implementation of the \c return_* methods
   * below, which form the public interface to be used by
   * applications. */
  void  return_payload  (const int      client_socket,
                         const string_view   http_type,
                         const string_view   origin_url,
                         const char *const payload,
                         const size_t   length);
    


  inline void Http_Server::return_html (int const socket,  string const &html)
  {
    return_payload (socket, "text/html", {}, html.c_str (), html.length ());
  }



  inline void Http_Server::return_text (int const socket,  string const &text)
  {
    return_payload (socket, "text/plain", {}, text.c_str (), text.length ());
  }



  inline void Http_Server::return_css (int const socket,  string const &text)
  {
    return_payload (socket, "text/css", {}, text.c_str (), text.length ());
  }



  inline void Http_Server::return_pdf (int const socket,  string const &pdf)
  {
    return_payload (socket, "application/pdf", {}, pdf.c_str (), pdf.length ());
  }



  inline void Http_Server::return_json (int const socket,
                                        string const &json,
                                        const string_view  origin_url)
  {
    return_payload  (socket,  "application/json",  origin_url,
                     json.c_str (),  json.length ());
  }



  inline void Http_Server::return_bad (int const socket) 
  {   return_payload (socket, "text/plain",  {},  nullptr, 0);   }



  inline void Http_Server::return_png (int const socket,
                                       vector<uint8_t> const &png)
  {
    return_payload (socket, "image/png",  {},
                    (char const *) png.data (), png.size ());
  }


  inline void Http_Server::return_svg (int const socket,  string const &svg)
  {
    return_payload (socket, "image/svg+xml",  {}, svg.c_str (),  svg.length ());
  }


} }  /* End of namespace DMBCS::Micro_Server. */
