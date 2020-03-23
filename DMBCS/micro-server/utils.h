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


#ifndef DMBCS__MICRO_SERVER__UTILS__H
#define DMBCS__MICRO_SERVER__UTILS__H


#include  <fstream>
#include  <string>
#include  <vector>


namespace DMBCS { namespace Micro_Server  {


  using namespace std;
    

  /* Get a string containing the entire contents of the file at file_name.
   * If there is a problem reading this file then the Slurp_Error will be
   * thrown. */
  struct Slurp_Error : runtime_error
  {   Slurp_Error () : runtime_error {"file slurp error"} {}
      explicit  Slurp_Error (const string&  file_name)
        : runtime_error {"file slurp error: ‘" + file_name + "’"}  {} } ;
  string slurp_file_with_error (string const &file_name);
    
  /* Read the entire contents of the file at file_name, returning an
   * empty string object if this cannot be done. */
  string slurp_file (string const &file_name);

  /* Read the entire contents of the file attached to the input stream,
   * returning an empty string object if this cannot be done. */
  string slurp_file (ifstream  &file_name);

  /* Replace _all_ occurences of ‘find’ with ‘replace’. */
  string transform  (string &&,  string const &find,  string const &replace);

  /* Replace _all_ occurences of dog-ears with the corresponding HTML
   * entity. */
  string htmlize_quotes (string &&);

  /* Make sure the given string displays verbatim in HTML pages. */
  string htmlize (string &&);

  /* To remove the network wastage of sending indented HTML, this function
   * replaces any spaces at the start of a line with a single space (just
   * in case this is significant to the parsing of the document).  */
  string strip_leading_space (string &&);
  
  /* Replace apostrophes, dog-ears and the escape back-slash with
   * escaped versions.  */
  string escape_quotes (string &&);

  /* Expand out all space-plusses and unicode markers to their correct
   * character representatives. */
  string decode_url  (string &&);

  /* Return a hex-string representation of the data values in the buffer.
   * The return string will be exactly twice buffer_size long. */
  string hexalize  (unsigned char const *const buffer,
                    size_t const buffer_size);

  /* Return the value of the environment variable env if possible, or else
   * just return an empty string. */
  string  getenv  (string const &  env);
    

} }  /* End of namespace  DMBCS::Micro_Server. */


#endif  /* Undefined DMBCS_DMBCS__MICRO_SERVER__UTILS__H. */
