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


#ifndef DMBCS__MICRO_SERVER__QUERY_STRING__H
#define DMBCS__MICRO_SERVER__QUERY_STRING__H


#include <DMBCS/micro-server/utils.h>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <stdexcept>


namespace DMBCS { namespace Micro_Server {
    

  /*  An object designed to hold the (name, value) pairs passed to a GCI
   *  program through the query part of the URL, and carries along with it
   *  the information of whether query string came from a HTTP POST or GET
   *  method, and if a message authentication code (MAC) was verified on
   *  the URL.  There is also the possibility that the object holds the
   *  contents of an uploaded file, if such was sent from a POSTed HTML
   *  form.
   *
   *  The chief methods are the /has/ and /get/ methods, to determine if a
   *  query string has a given value, and to get a value (which may be
   *  interpreted as any numeric or string type) returning some default if
   *  it is not there.
   *
   *  The main functions are the /query_string/s which produces a
   *  Query_String object from a CGI programʼs environment (as
   *  appropriate), and conversely produce a URI fragment with the
   *  contents of a given Query_String object. */

  struct Query_String : map <string, string>
  {
    /* The secret mixed into a URL before computing a cryptographic
     * fingerprint.  The value assigned here comes from the top-level
     * configure script via the local makefile.  */
    static constexpr  char const *  MAC_KEY  =  MAC;
    
    /* Either the HTTP method was POST, or else we assume GET. */
    bool  post_method    {0};

    /* Either the query contained a verified MAC, or it didnʼt. */
    bool  mac_verified_  {0};

    /* Will be empty unless this was a multipart POST with an uploaded
     * file in it. */
    vector <uint8_t>  upload_data;

    /* If we have been called from a browser exercising cross-site access
     * control, this is the URL of the other site from which we will allow
     * access. */
    string  access_url;


    /* Allow all bulk operations except copy, which might get a tad
     * expensive if done in the wrong place. */
    Query_String ()                                =  default;
    Query_String (Query_String &)                  =  delete;
    Query_String (Query_String &&)                 =  default;
    Query_String &operator= (Query_String const &) =  delete;
    Query_String &operator= (Query_String &&)      =  default;
    
    
    /* Add a new parameter to this query set, with the given name and
     * value. */
    template <typename T>
    void  add  (string const &name,  T const &value)
    {   ostringstream hold;   hold << value;   add (name, hold.str ());   }


    /* Add a new parameter to this query set, with the given name and a
     * value pulled from a parameter with that same name in the input
     * query string, or else use the fallback as the value. */
    template <typename T>
    void  add  (string const &name,
                Query_String const &input,
                T const &fallback)
    {   add  (name,  input.get (name, fallback));   }

    
    /* Return TRUE if the HTTP request indicated a POST method, else
     * return FALSE. */
    bool  posted  ()  const   { return post_method; }


    /* Return TRUE only if this query had a MAC which we have verified. */
    bool  mac_verified  ()  const   { return mac_verified_; }


    /* Return TRUE only if this query has a parameter with the given
     * name. */
    bool  has  (string const &name)  const   { return   find (name) != end (); }


    /* Get the value of the parameter with the given name in this query;
     * use the fallback value if this procedure fails (the fallback is
     * also useful to establish the desired return type, which may be any
     * numerical or string type). */ 
    template <typename T>
    T  get  (string const &name,  T const &fallback = {})  const;


    /* If the value of a parameter is a comma-separated list of integers,
     * the list is returned as a vector.  (This is the only list case we
     * currently implement; others may appear in future versions. */
    vector<int>  get_list  (string const &name)  const;


    /* Add a comma-separated list of integers to /this/ object. */
    void  make_list  (string const &name,  vector <int> const &list);


    /* If the parameters which start with the given name are present, the
     * remainder of the name should be a numerical sequence and the
     * numerical values of all these parameters are returned in the
     * unsorted vector of ints. */
    vector<int>  get_checked_array  (string const &name)  const;
  } ;


  
  /* Manufacture a Query_String according to environment variables and/or
   * standard input, as directed by the CGI specification. */
  Query_String  get_query_string  ();

  /* Manufacture a Query_String object to reflect the URI parameter list
   * found in the input. */
  Query_String  query_string  (string const &  input);
    
  /* Create a URI parameter list with the data held in the Query_String,
   * and add a guarding MAC to the end. */
  string  query_string_with_mac  (Query_String const &);

  /* Create a URI parameter list with the data held in the Query_String
   * (no MAC will be present). */
  string  query_string  (Query_String const &);

  void  scan_multipart  (Query_String &Q,  string  const  &a);



  /*********************   IMPLEMENTATION   ***********************/

  template <>
  inline string  Query_String::get  (string const &name,
                                     string const &fallback) const
    try { return at (name); }
    catch (out_of_range&) { return fallback; }

  
  template <typename T>
  inline T  Query_String::get  (string const &name, T const &fallback) const
    try   { if (at (name).length () == 0) return fallback;
            T ret; istringstream {at (name)} >> ret; return ret; }
    catch (out_of_range&) { return fallback; }

  
  template <>
  inline  void  Query_String::add  (string const &name, string const &value)
  {   (*this) [name] = value;   }

  
  template<> inline void
  Query_String::add (string const &name, Query_String const &input_query)
  {   add (name, input_query.get (name, string {}));  }

  
} }  /*  End of namespace DMBCS::Micro_Server. */


#endif  /* Undefined DMBCS__MICRO_SERVER__QUERY_STRING__H. */
