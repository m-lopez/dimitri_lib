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



#include "parser.hpp"
#include <iostream>



expr::expr (const std::string& name)
  : name(name), args()
{ }

expr::expr (const std::string& name, const std::vector<expr*>& args)
  : name(name), args(args)
{ }

void print_expr (std::ostream& out, expr* e) {
  out << e->name;
  if (e->args.empty())
    return;
  out << '(';
  print_expr(out,e->args[0]);
  for (auto n = 1u; n < e->args.size(); ++n) {
    out << ',';
    print_expr(out,e->args[n]); }
  out << ')';
}

std::ostream& operator<< (std::ostream& out, expr* e)
{
  print_expr(out,e);
  return out;
}

std::vector<expr*>& Args::operator() (expr* e)
{
  return e->args;
}

bool Is_same::operator() (expr* e1, expr* e2)
{
  return e1->name == e2->name and e1->args.size() == e2->args.size();
}

std::size_t Num_args::operator() (expr* e)
{
  return e->args.size();
}

expr_parser_t::expr_parser_t (std::vector<expr*>& mem_pool)
  : src_ptr(nullptr), mem(mem_pool)
{ }

expr_parser_t::~expr_parser_t ()
{
  for (auto e : mem) delete e;
}

expr* expr_parser_t::parse (const std::string& str)
{
  src_ptr = new std::istringstream(str);
  auto e = parse_expr();
  delete src_ptr;
  return e;
}

expr* expr_parser_t::parse_expr ()
{
  remove_whitespace();
  std::string name(parse_name());
  detail::maybe<std::vector<expr*>> args(parse_params());
  expr* e = args.is_just ? new expr(name,args.val) : new expr(name);
  if (args.is_just)
    e = new expr(name,args.val);
  else
    e = new expr(name);
  mem.push_back(e);
  return e;
}

detail::maybe<std::vector<expr*>> expr_parser_t::parse_params ()
{
  remove_whitespace();
  if (src().good() and src().peek() != '(')
    return {};
  require_character('(');
  detail::maybe<std::vector<expr*>> args(parse_args());
  require_character(')');
  return args;
}

std::vector<expr*> expr_parser_t::parse_args ()
{
  std::vector<expr*> args;
  try { args.push_back(parse_expr()); }
  catch (...) { return std::vector<expr*>(); }
  remove_whitespace();
  while (src().peek() == ',') {
    src().get();
    args.push_back(parse_expr());
    remove_whitespace();
  }
  return args;
}

std::string expr_parser_t::parse_name ()
{
  remove_whitespace();
  std::string name;
  while (src().good() and !src().eof()) {
    char c = src().peek();
    if (is_character(c))
      name.push_back(c);
    else if (is_whitespace(c) or is_symbol(c))
      break;
    else
      throw (std::string("parsing name, but got: ") + c).c_str();
    src().get();
  }
  if (name.empty()) throw "expected a name, got come thing else";
  return name;
}

void expr_parser_t::remove_whitespace ()
{
  while (src().good() and is_whitespace(src().peek()))
    src().get();
}

bool expr_parser_t::is_whitespace (char c)
{
  return c == ' ' or c == '\t' or c == '\n';
}

bool expr_parser_t::is_character (char c)
{
  return (65 <= c and c <= 90) or (97 <= c and c <= 122);
}

bool expr_parser_t::is_symbol (char c)
{
  switch (c) {
    case '(': case ')': case ',': case '=':
      return true;
    default:
      return false;
  }
}

void expr_parser_t::require_character (char c)
{
  char got = src().get();
  if (got == c)
    return;
  std::string err("expected '");
  err += c;
  err += "', but got '";
  err += got;
  err += "'\n";
  throw err.c_str();
}

std::istringstream& expr_parser_t::src ()
{
  return *src_ptr;
}
