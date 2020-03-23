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


#include <DMBCS/micro-server/query-string.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <regex>
#include <sstream>


namespace DMBCS { namespace Micro_Server {


  static  void  scan_string  (Query_String &Q,  string const &a)
  {
    for (string::size_type name_start = 0; name_start < a.length (); )
      {
        auto const   name_end  =  a.find_first_of  ("&=",  name_start);

        auto const   name
          =  name_end == a.npos  ? a.substr (name_start)
                                 : a.substr (name_start, name_end - name_start);

        string::size_type value_end;

        if (name_end == a.npos  ||  a [name_end] == '&')
          {
            Q [name]  =  "1";
            value_end = name_end;
          }
        
        else
          {
            auto const  value_start  =  name_end + 1;
            
            value_end  =  a.find ('&', value_start);

            auto const  real_end  =  value_end == a.npos ? a.length () 
                                                         : value_end;

            Q [name]  =  decode_url (a.substr (value_start,
                                               real_end - value_start));
        }

        name_start = value_end
                    + ((value_end != a.npos  &&  a [value_end] == '&') ? 1 : 0);
      }
  }



  void  scan_multipart  (Query_String &Q,  string  const  &a)
  {
    auto const  boundary_end  =  a.find_first_of ("\r\n");

    if (boundary_end == a.npos)
      return;

    auto const  boundary  =  "\r\n" + a.substr (0, boundary_end);

    auto  last_boundary  =  size_t {0};

    for (;;)
      {
        auto const  next_boundary  =  a.find (boundary, last_boundary + 1);

        if (next_boundary == a.npos)
          return;

        auto const  name_start  =  a.find ("name=\"", last_boundary);

        if (name_start == a.npos  ||  name_start > next_boundary)
          return;

        auto const  name_end  =  a.find ("\"", name_start + 6);

        if (name_end == a.npos)
          return;

        auto const  name  =  a.substr (name_start + 6,
                                       name_end - name_start - 6);

        auto const  data_start  =  a.find ("\r\n\r\n", name_end);

        if (data_start == a.npos)
          return;
        
        if  (name == "upload-file")
          copy (a.begin () + data_start + 4,
                a.begin () + next_boundary,
                back_inserter (Q.upload_data));

        else
          Q [name]  =  a.substr (data_start + 4,
                                 next_boundary - (data_start + 4));

        last_boundary = next_boundary;
      }
  }



  Query_String  get_query_string  ()
  {
    auto  ret  =  Query_String {};

    auto const method  =  getenv ("REQUEST_METHOD");

    if (method.empty ()   ||   method  ==  "GET")
      {
        scan_string (ret,  getenv ("QUERY_STRING"));
      }
    
    else
      {
        ret.post_method = 1;
      
        auto const  input_length  =  atoi (getenv ("CONTENT_LENGTH").c_str ());
      
        auto  line  =  string {};
      
        for (int i = 0; i < input_length; ++i)
          line  +=  getchar ();
        
        if (line [0] == '-'  &&  line [1] == '-')
          scan_multipart (ret, line);
        
        else 
          scan_string (ret, line);
      }
    
    return ret;
  }
    


  Query_String  query_string  (string const &input)
  {
    Query_String ret;
    scan_string (ret, input);
    return ret;
  }



  string  query_string  (Query_String const &Q)
  {
    if (Q.empty ())   return string {};

    auto  separator  =  string {};
    auto  ret  =  string {};

    for (auto const &i  :  Q)
      {
        ret   +=  separator + i.first + '=' + i.second;
        separator = "&";
      }

    return ret;
  }



  vector <int>  Query_String::get_list  (string const &name) const
  {
    auto  ret  =  vector <int> {};
    
    char comma;

    auto  s  =  istringstream {get (name, string ())};

    for (;;)
      { 
        int k; s >> k;
        
        if (s)
          {
            ret.push_back (k);
            s >> comma;
          }
        
        else
          return ret;
      }
  }
    


  void  Query_String::make_list  (string const &name,
                                  vector <int> const  &list)
  {
    if (list.empty ())   return;

    auto  hold  =  ostringstream {};
        
    hold << list.front ();
        
    for (auto i = list.begin () + 1;  i != list.end ();  ++i)
      hold << "," << *i;
        
    (*this) [name]  =  hold.str ();
  }
    


  vector <int>  Query_String::get_checked_array  (string const &name_prefix)
                                                                          const
  {
    auto  ret  =  vector <int> {};
  
    for  (auto  i  :  *this)
      if (name_prefix == i.first.substr (0, name_prefix.length ()))
        ret.push_back (atol (i.first.substr (name_prefix.length ()).c_str ()));
    
    return ret;
  }


} }  /*  End of namespace  DMBCS::Micro_Server. */
