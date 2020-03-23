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


#include <DMBCS/micro-server/hyper-tags.h>
#include <cstdarg>
#include <sstream>
#include <iomanip>
#include <vector>
#include <regex>


namespace DMBCS { namespace Micro_Server {


  /* Note that we need to take a copy of the replacement texts, since it
   * is understood that the original _reg_ is to be left intact. */

  Hyper_Tags &Hyper_Tags::add  (Hyper_Tags const &reg,  int const harden)
  {
    for  (auto  i  :  reg.tags)
      {
        add  (i.name,  i.replacement);

        if  (harden)   harden_last ();
      }

    return *this;
  }



  Hyper_Tags &Hyper_Tags::add (string const &tag,
                               double const &value,
                               int const precision)
  {
    auto  hold  =  ostringstream {};
    hold << setprecision (precision) << value;
    return add (tag, hold.str ());
  }


  void Hyper_Tags::add_date (string const &tag,
                             time_t const &time,
                             string const &format)
  {
    array<char, 200> buffer;
    strftime (& buffer [0], buffer.size (), format.c_str (), localtime (&time));
    add (tag, & buffer [0]);
  }


  Hyper_Tags  &  Hyper_Tags::add_query  (Query_String const &Q)
  {
    for (auto const &q  :  Q)     add (q.first, q.second);
    return *this;
  }



  void Hyper_Tags::add_passthrough_tag (Query_String const &query_string,
                                        ...)
  {
    va_list arguments;   va_start (arguments, query_string);

    auto  acc  =  string {};

    const char *query_name;

    while ((query_name = va_arg (arguments, const char*)))
      {
        auto const query_value  =  query_string.get (query_name, string {});

        if (query_value.length ())
          {
            acc += string {"<input type=\"hidden\" name=\""}
                      + query_name
                      + "\" value=\""
                      + query_value
                      + "\"/>";
          }
      }

    add ("pass-through", move (acc));

    va_end (arguments);
  }



  string Hyper_Tags::substitute_  (string &&input, int repeat)
  {
    while (repeat-- > 0)
      {
        for  (auto const &i  :  sections)
          input = remove_tags (i.name,  move (input),  i.keep == LOSE_SECTION);

        for  (auto const &i  :  tags)
          {
            auto hold  =  ostringstream {};
            hold << '[' << i.name << "/]";
            input  =  transform  (move (input),  hold.str (),  i.replacement);
          }
      }

    return input;
  }



  string  Hyper_Tags::remove_tags  (string const &tag_name,
                                    string &&input,
                                    bool const &whole_section)
  {
    auto const   open_tag  =  "[" + tag_name + "]";
    auto const   close_tag =  "[/" + tag_name + "]";

    for (auto start_end = input.find (close_tag);
         start_end  != input.npos;
         start_end = input.find (close_tag))
      {
        auto const start_open  =  input.rfind (open_tag, start_end);

        if (start_open == input.npos)   return input;

        if (whole_section)
          input.erase (start_open,
                       start_end + close_tag.length () - start_open);

        else
          {
            input.erase (start_end, close_tag.length ());
            input.erase (start_open, open_tag.length ());
          }
      }

    return input;
  }



  string  Hyper_Tags::strip_loose_tags  (string&&  in)
  {
    static const regex  re  {"\\[[-!_[:alnum:]]*/\\]"};
    cmatch  match  {};
    for (int i = 0;
         regex_search (in.c_str () + i, match, re);
         i += match.position ())
      in.erase (i + match.position (), match.length ());
    return in;
  }


} }  /* End of namespace DMBCS::Micro_Server. */
