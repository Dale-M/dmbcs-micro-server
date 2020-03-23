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


#include  <DMBCS/micro-server/utils.h>
#include  <fstream>
#include  <iostream>
#include  <sstream>
#include  <unistd.h>


namespace DMBCS { namespace Micro_Server {


  string  slurp_file  (ifstream&  input)
  {
    input.seekg (0,  ios::end);
    string  line  (input.tellg (),  char {});
    input.seekg (0,  ios::beg);
    input.read  (line.data (),  line.length ());
    return line;
  }



  string  slurp_file_with_error  (const string&  file_name)
  {
    ifstream  input  {file_name};
    if (! input.good ())    throw Slurp_Error {file_name};
    return  slurp_file  (input);
  }



  string  slurp_file  (string const &  file_name)
    try   {   return  slurp_file_with_error (file_name);   }
    catch (Slurp_Error&)
      {
        cerr << "SLURP FILE NOT FOUND: ‘" << file_name << "’" << endl;
        return  {};
      }



  string  transform  (string &&  in,
                      string const &find,
                      string const &replace)
  {
    for  (auto i  =  string::size_type {0};
          (i = in.find (find, i))  !=  in.npos;
          i += replace.length ())
      in.replace  (i,  find.length (),  replace);

    return in;
  }
            


  string  htmlize_quotes  (string &&  input)
  {
    return  transform  (move (input), "\"", "&quot;");
  }



  string  escape_quotes  (string &&  a)
  {
    return  transform (transform (transform (move (a), "\\", "\\\\"),
                                  "'", "\\\'"),
                       "\"", "\\\"");
  }



  string  decode_url  (string &&  url)
  {
    url  =  transform  (move (url), "+", " ");

    for (string::size_type i = 0; (i = url.find ('%', i)) != url.npos; ++i)
      {
        if (i < url.length () - 1  &&  url [i + 1] == '%')
          {
            url.erase (i);
          }

        else if (i < url.length () - 2  &&  isxdigit (url [i+1])
                                        &&  isxdigit (url [i+2]))
          {
            url [i] = (char) strtol (url.substr (i+1, 2).c_str (), 0, 16);
            url.erase (i + 1, 2);
          }
      }

    return url;
  }



  string  strip_leading_space  (string &&  in)
  {
    auto  start_space  =  string::size_type {0};

    for (;;)
      {
        /* Scan for first newline which is immediately followed by more white
           space (this is the candidate for removal). */
        while (start_space < in.length () - 1
               &&  (in [start_space] != '\n'
                                   ||  !isspace (in [start_space + 1])))
          ++ start_space;

        if (start_space >= in.length () - 1)
          return in;

        /* Want to leave one space behind. */
        auto  start_text  =  string::size_type {start_space += 2};

        while (start_text < in.length ()  &&  isspace (in [start_text]))
          ++ start_text;

        in.erase  (start_space,  start_text - start_space);
      }

    return in;
  }



  string  htmlize  (string &&  entry)
  {
    static   auto const  A  =  vector <pair <char, char const *>>
                                   { {'&', "&amp;"},   {'"', "&quot;"},
                                     /*{'%', "&#37;"},*/
                                     {'<', "&lt;"},    {'>', "&gt;"} } ;
    
    for (auto &i  :  A)
      entry  =  transform  (move (entry),  string {i.first},  i.second);

    return entry;
  }



  string  hexalize  (unsigned char const *const  buffer,
                     size_t const  buffer_size)
  {
    auto ret  =  string (2 * buffer_size, ' ');

    for (size_t i = 0; i < buffer_size; ++i)
      sprintf (ret.data () + 2 * i, "%2.2x", (unsigned int) buffer [i]);

    return ret;
  }



  string  getenv  (string const &  env)
  {
    auto const  a  =  ::getenv (env.c_str ());
    return  a  ?  string {a}  :  string {};
  }


} }  /* End of namespace  DMBCS::Micro_Server. */
