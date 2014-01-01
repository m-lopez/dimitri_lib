// Copyright 2013 Michael Lopez
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.



#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>
#include <sstream>
#include <iosfwd>



// Expression language 
struct expr {
  expr (const std::string&);
  expr (const std::string&, const std::vector<expr*>&);
  std::string name;
  std::vector<expr*> args;
};

void print_expr (std::ostream&, expr*);

std::ostream& operator<< (std::ostream&, expr*);

// Expression algebra
struct Args {
  std::vector<expr*>& operator() (expr*);
};

struct Is_same {
  bool operator() (expr*, expr*);
};

struct Num_args {
  std::size_t operator() (expr*);
};



namespace detail {

  template <typename X>
    struct maybe {
      maybe ();
      maybe (const X&);
      maybe (const maybe&);
      bool is_nothing () const { return !is_just; }
      bool is_just;
      X val;
    };

  template <typename X>
    maybe<X>::maybe ()
      : is_just(false), val()
    { }

  template <typename X>
    maybe<X>::maybe (const X& x)
      : is_just(true), val(x)
    { }

  template <typename X>
    maybe<X>::maybe (const maybe& m)
      : is_just(m.is_just), val(m.val)
    { }
}



struct parser_t {
  parser_t ();
  ~parser_t ();

  expr* parse (const std::string&);
  expr* parse_expr ();
  detail::maybe<std::vector<expr*>> parse_params ();
  std::vector<expr*> parse_args ();
  std::string parse_name ();
  void remove_whitespace ();
  bool is_whitespace (char);
  bool is_character (char);
  bool is_symbol (char);
  void require_character(char);

  std::istringstream& src ();
  std::istringstream* src_ptr;
  std::vector<expr*> mem;
};



#endif // PARSER_HPP