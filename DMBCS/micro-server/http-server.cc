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


#include <DMBCS/micro-server/http-server.h>


namespace DMBCS { namespace Micro_Server {



  string  Http_Server::add_html_headers  (string const &&body)
  {
    auto  hold  =  ostringstream {};

    hold << "Content-type: text/html; charset=utf-8\r\n"
         << "Content-length: " << body.length () << "\r\n\r\n";

    return hold.str () + move (body);
  }



  void return_payload (const int         socket,
                       const string_view type,
                       const string_view origin_url,
                       const char *const payload,
                       const size_t      length)
  {
    ostringstream  out  {};

    out  <<  "HTTP/1.1 200 OK\r\n";
    
    if  (origin_url.size ())
      out  <<  "Access-Control-Allow-Origin: "  <<  origin_url  <<  "\r\n";
    
    out << "Content-type: " << type << "; charset=utf-8\r\n"
        << "Content-length: " << length
        << "\r\n\r\n"
        << string {payload, length};

    write  (socket,  out.str ().c_str (),  out.str ().length ());
  }



    static  string  extract_header  (const vector<char>&  request,
                                     const string&  tag)
    {
      static const  regex  re  {"[\r\n]" + tag + ":[[:space:]]*([^\r\n]*)"};
      match_results<vector<char>::const_iterator>  match;
      if  (regex_search  (begin (request), end (request), match, re))
                  return  match.str (1);
      return  {};
    }

void  Http_Server::process_message  (vector<char> &&request,
                                     int const socket)
  {
    static const   regex  protocol_re
      {"^(GET|POST|OPTIONS)[[:space:]]+([^[:space:]?]+)([?]([^[:space:]]*))?"};

    match_results<vector<char>::iterator>  match  {};
    if (! regex_search (begin (request), end (request), match, protocol_re))
      {
        Http_Server::return_bad (socket);
        return;
      }

    auto callback = service_methods.find (match.str(2) [0] == '/' 
                                              ? match.str(2).substr (1) 
                                              : match.str(2));

    if  (callback  ==  end (service_methods))
      {
        Http_Server::return_bad (socket);
        return;
      }


    Query_String  QS;
    
    /* We need to extract this in advance as we will destroy the request
     * buffer when we manufacture the Query_String object. */
    const string  access_url  { extract_header  (request,  "Origin") };

    if  (match.str (1)  ==  "OPTIONS")
       {
            std::ostringstream O;
            O << "HTTP/1.1 200 OK\r\n"
              << "Access-Control-Allow-Origin: " << access_url << "\r\n"
              << "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
              << "Access-Control-Max-Age: 86400\r\n\r\n";
        
            write  (socket,  O.str ().data (),  O.str ().size ());

            return;
       }

    
    /* We need to extract this in advance as we will destroy the request
     * buffer when we manufacture the Query_String object. */
    const string   cookie  { extract_header  (request,  "Cookie") };
    
    if  (match.str (1)  ==  "POST")
      {
        static  auto  const  length_re
          =  regex {"[\r\n]Content-Length:[[:space:]]*([^\r\n]*)"};
        
        auto  match  =  match_results<vector<char>::iterator>  {};

        if  (! regex_search
                        (begin (request),  end (request),  match,  length_re))
          {
            Http_Server::return_bad (socket);
            return;
          }

        auto  content_start  =  int {0};

        {
          static  auto  const  re  =  regex {"\r\n\r\n"};
          auto  match  =  match_results<vector<char>::iterator>  {};
          regex_search  (begin (request), end (request), match, re);
          content_start  =  match.position ();
        }

        auto  const  needed_len
               =  (size_t) (atoi (match.str (1).c_str ()) + content_start + 4);

        auto  len  =  request.size ();

        request.resize (needed_len);

        while  (len  <  needed_len)
          {
            auto  const  L
              =  read (socket, request.data () + len, needed_len - len);

            if  (L  <=  0)
              {
                Http_Server::return_bad (socket);
                return;
              }
            
            len += L;
          }
        
        scan_multipart  (QS,
                         {begin (request) + content_start + 4,  end (request)});

        QS.post_method  =  1;
      }

    else  /* GET */
      {
        QS  =  query_string (match.str (4));
      }
    
    if  (cookie.length ()  >  0)    QS.add ("__cookie__", cookie);
    QS.access_url  =  access_url;

    (callback->second) (move (QS), socket);
  }
  

} }  /* End of namespace DMBCS::Micro_Server. */
