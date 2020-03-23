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


#ifndef DMBCS__MICRO_SERVER__HYPER_TAGS__H
#define DMBCS__MICRO_SERVER__HYPER_TAGS__H


#include <DMBCS/micro-server/query-string.h>
#include <algorithm>
#include <list>
#include <string>


namespace DMBCS { namespace Micro_Server {


  using namespace std;


  struct Hyper_Tags
  {
    /* A tag is an entry on a HTML template of the form [xxx/], which is
     * to be replaced with some dynamic content. */
      
    struct Tag
    {
      string name;
      string replacement;
    } ;


    /* A section is part of a HTML template surrounded by strings of the
     * form [xxx] and [/xxx]; either the whole section will be eliminated
     * (LOSE_SECTION) or just the surrounding tags will be stripped
     * leaving the content of the section in place (KEEP_SECTION). */
      
    enum Section_Keep { KEEP_SECTION = 1, LOSE_SECTION = 0 } ;
      
    struct Section
    {
      string name;
      Section_Keep keep;
    } ;

      
    /* New tags are inserted at the front of the list, so normally they
     * override older tag definitions.  This can be overridden by making a
     * new tag `hard', when the last_tag pointer will be placed after it,
     * and all future additions to the registry will go in *after* the
     * hard tags, and thus will *not* override the earlier meaning. */

    list<Tag> tags;

    list<Tag>::iterator last_hard;  /* Actually, the first ‪`soft'. */

    vector<Section> sections;


    Hyper_Tags () : last_hard {begin (tags)} {}
    Hyper_Tags (Hyper_Tags const &)            =  delete;
    Hyper_Tags (Hyper_Tags &&)                 =  default;
    Hyper_Tags &operator= (Hyper_Tags const &) =  delete;
    Hyper_Tags &operator= (Hyper_Tags &&)      =  default;

      
    static string remove_tags (string const &tag_name,
                               string &&input,
                               bool const &whole_section);

      
    Hyper_Tags &section (string const &name, Section_Keep const &keep)
    {
      sections.push_back  (Section {name, keep});
      sections.push_back  (Section {"!" + name,
                                    keep ? LOSE_SECTION : KEEP_SECTION});
      return *this;
    }
    
      
    void harden_last () { if (last_hard != end (tags)) ++last_hard; }
      

    bool has (string const &a) const
    {
      return  find_if (begin (tags),  end (tags),  [&a] (Tag const &T)
                                                        { return T.name == a; })
                 != end (tags);
    }
    
    Hyper_Tags &add  (Hyper_Tags const &registry,  int const harden = 0);

    Hyper_Tags &operator+= (const Hyper_Tags &tags) { return add (tags); }
      

    Hyper_Tags &add (const string &tag,
                     string  &&substitution_text)
    { last_hard  =  tags.insert  (last_hard,
                                  Tag {tag, move (substitution_text)});
      return *this; }

    template <typename T>
    Hyper_Tags &add (string const &tag, T const &value)
    {
      ostringstream hold;
      hold << value;
      return add (tag, hold.str ());
    }
    
    Hyper_Tags &add
          (string const &tag, double const &value, int const precision = 4);

    template <typename T>
    Hyper_Tags &add  (const string &tag, Query_String &qs, const T &fallback)
    { add (tag, qs.get (tag, fallback)); return *this; }

    Hyper_Tags &add_query  (Query_String const &);
      

    /* The tag is replaced with a string consisting of a comma-separated
     * (no spaces) list of integers. */

    template <typename Iterator>
    void add_int_list  (const string &tag, Iterator begin, Iterator end);

    void add_date (const string &tag,
                   const time_t &time,
                   const string &format);

    void add_file (const string &tag, const string &file_name)
    { add (tag, slurp_file (file_name)); }
    
      
    /* The trailing arguments are a list of const char *const, terminated
     * with nullptr. */
    void  add_passthrough_tag  (const Query_String&  query_string, ...)
      __attribute__ ((sentinel));
      
    Hyper_Tags&  keep_section  (const string&  name,  const bool  keep  =  1)
    {  return  section  (name,  keep ? KEEP_SECTION : LOSE_SECTION);  }
      
    Hyper_Tags&  lose_section  (const string&  name,  const bool  lose  =  1)
    {  return  section  (name,  lose ? LOSE_SECTION : KEEP_SECTION);  }
    
    string  substitute_  (string&&  html_template,  int  repeat);
    
    static string  strip_loose_tags  (string&&);

  } ;  /* End of class Hyper_Tags. */

    

  template <typename Iterator>
  inline  void  Hyper_Tags::add_int_list (const string &tag_name,
                                          Iterator begin,
                                          Iterator end)
  {
    auto  acc  =  ostringstream {};
    acc << *begin;
    for (++begin; begin != end; ++begin)   acc << ',' << *begin;
    add (tag_name, acc.str ());
  }

  
} }  /* End of namespace  DMBCS::Micro_Server. */



inline  std::string  substitute  (DMBCS::Micro_Server::Hyper_Tags &tags,
                                  std::string &&html_template,
                                  int repeat = 1)
{   return tags.substitute_ (std::move (html_template), repeat);   }



inline  std::string  substitute  (std::string &&html_template,
                                  DMBCS::Micro_Server::Hyper_Tags &tags,
                                  int repeat = 1)
{   return tags.substitute_ (std::move (html_template), repeat);   }



#endif /* Undefined DMBCS__MICRO_SERVER__HYPER_TAGS__H. */
