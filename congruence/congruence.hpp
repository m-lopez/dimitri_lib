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

//  Congruence Closure data structure
//
//  This is an abstract congruence closure data structure that stores
//  equalities for some expression type. It supports equality queries, identify
//  differences in expressions, and supports unification.
//
//  The expression type E must support the following operators:
//    args(E) -> Range(E)     -- returns a range of the arguments of an 
//                               expression
//    same_node(E,E) -> bool  -- true iff the expressions represent the same
//                               function symbol
//    num_args(E) -> Nat      -- the number of arguments of an expression



#ifndef DIMITRI_CONGRUENCE_HPP
#define DIMITRI_CONGRUENCE_HPP

#include <map>
#include <vector>
#include <algorithm>
#include <utility>



namespace dimitri {

  // ------------------ //
  // --- Maybe type --- //
  // ------------------ //
  template <typename X>
    struct maybe {
      maybe ();
      maybe (const X&);
      maybe (const maybe&);
      bool is_nothing () const { return !is_just; }
      bool is_just;
      X val;
    };



  // ------------------ //
  // --- Union Find --- //
  // ------------------ //

  struct union_find_t {
    using size_t = std::size_t;

    // -- Constructors
    union_find_t ();
    union_find_t (size_t);
    union_find_t (const union_find_t&);

    // -- Set algebra
    bool same_set (size_t, size_t);
    void union_sets (size_t, size_t);

    // -- Get the next free variable
    size_t new_free_variable ();

    //  -- Parent mapping
    std::vector<size_t> parent;

    //  -- Get the roots of the elements in the universe
    size_t root_of (size_t);
  };



  // ------------------------------ //
  // --- Canonical Element maps --- //
  // ------------------------------ //

  template <typename E>
    struct canonical_map_t
    {
      // using size_t = std::size_t;
      using expr_t = E;
      using size_t = std::size_t;

      canonical_map_t ();

      maybe<size_t> get (expr_t);
      void set (expr_t, size_t);

      // -- Representative elements
      std::map<expr_t,size_t> representatives;
    };

}



#endif // DIMITRI_CONGRUENCE_HPP