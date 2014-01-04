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



#include <iostream>
#include <cassert>
#include "parser.hpp"
#include "../../congruence/congruence.hpp"


using namespace std;

using congruence_t = dimitri::congruence_t<expr*, Args, Is_same, Num_args>;



// Simple congruence_t workout
void simple_test ()
{
  std::vector<expr*> mem_pool;
  expr_parser_t parser(mem_pool);
  congruence_t eq;

  // Expressions
  auto fa =    parser.parse( "f(a)"          );
  auto ffa =   parser.parse( "f(f(a))"       );
  auto ffffa = parser.parse( "f(f(f(f(a))))" );
  auto k =     parser.parse( "k()"           );
  auto b =     parser.parse( "b()"           );
  auto c =     parser.parse( "c()"           );
  auto d =     parser.parse( "d()"           );
  auto e =     parser.parse( "e()"           );
  auto t =     parser.parse( "t()"           );
  auto ggt =   parser.parse( "g(g(t))"       );
  auto gggt =  parser.parse( "g(g(g(t)))"    );

  // Equality axioms
  eq.set_congruent(fa,ffa);
  eq.set_congruent(k,b);
  eq.set_congruent(c,b);
  eq.set_congruent(c,d);
  eq.set_congruent(t,ggt);

  // Truths                               because
  assert(( eq.is_congruent(e,e) ));       // congruency is reflexive
  assert(( eq.is_congruent(d,k) ));       // transitivity and symmetry
  assert(( eq.is_congruent(ffffa,fa) ));  // f is a projection

  // Fallicies                            because
  assert(( !eq.is_congruent(t,gggt) ));   // t is only order 2
}



int main ()
{
  simple_test();
  return 0;
}