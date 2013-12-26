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
  // The mighty union find data structure (or a simpler version of it). This
  // data structure maintains a partition of the integers [0,parent.size()).

  struct union_find_t {
    using size_t = std::size_t;

    // -- Constructors
    union_find_t ();
    union_find_t (size_t);
    union_find_t (const union_find_t&);

    // -- Set algebra
    bool in_same_set (size_t, size_t);
    void union_sets (size_t, size_t);

    // -- Get a fresh variable
    size_t fresh_variable ();

    //  -- Parent mapping
    std::vector<size_t> parent;

    //  -- Get the roots of the elements in the universe
    size_t root_of (size_t);
  };



  // ------------------------------ //
  // --- Canonical element maps --- //
  // ------------------------------ //
  // This data structure simply maintatins a mapping from expressions to
  // integers in the partition. It glues the expression language to the
  // union_find_t type.

  template <typename E>
    struct canonical_map_t
    {
      using expr_t = E;
      using size_t = std::size_t;

      canonical_map_t ();

      maybe<size_t> get (expr_t);
      void set (expr_t, size_t);

      // -- Representative elements
      std::map<expr_t,size_t> representatives;
    };



  // ---------------------------- //
  // --- Expression Traversal --- //
  // ---------------------------- //
  // Ad-hoc expression traverser
  template <
    typename Expr,
    typename Args,
    typename Same_symbol,
    typename Num_args
  >
    struct expr_traversal {
      using expr_t = Expr;
      using expr_pair_t = std::pair<expr_t,expr_t>;

      expr_traversal (const Args&, const Same_symbol&, const Num_args&);
      std::vector<expr_pair_t> traverse (expr_t e1, expr_t e2);

      // The differences between the expressions
      std::vector<expr_pair_t> expr_pairs;

      Args args;
      Same_symbol is_same_symbol;
      Num_args num_args;
    };



  // -------------------------- //
  // --- Congruence closure --- //
  // -------------------------- //
  // This data structure computes and maintains a congruence closure over some
  // expression type Expr. The requirements on the expression language are
  // enumerated in the README.md file.
  // NOTE is there a way to remove the num args requirement? If Args is a
  // function that returns a random access iterator range, then Num args is not
  // necessary. However, as with GCC, this is not always the case. Hmm. I think
  // it can go. Must confer with brain.

  template <
    typename Expr,
    typename Args,
    typename Same_symbol,
    typename Num_args
  >
    struct congruence_t {

      using expr_t = Expr;
      using expr_pair_t = std::pair<expr_t,expr_t>;
      using size_t = std::size_t;

      congruence_t (
        const Args& = Args(), const Same_symbol& = Same_symbol(),
        const Num_args& = Num_args());

      // Congruence Interface
      bool is_congruent (expr_t, expr_t);
      std::vector<expr_pair_t> report_differences (expr_t, expr_t);
      void set_congruent (expr_t, expr_t);

      // Expression algebra
      Args args;
      Same_symbol is_same_symbol;
      Num_args num_args;

      // Auxiliary data structures
      canonical_map_t<expr_t> reps;
      union_find_t sets;

      // Congruence algebra
      std::vector<expr_pair_t> differences (expr_t, expr_t);
      size_t get_or_gen_canonical (expr_t);
      bool not_directly_congruent (expr_pair_t);
    };



//// ----------------------------------------------------------------------- ////
//// ----- implementation details ------------------------------------------ ////
//// ----------------------------------------------------------------------- ////

  // ------------- //
  // --- Maybe --- //
  // ------------- //
  
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



  // ------------------ //
  // --- Union Find --- //
  // ------------------ //

  union_find_t::union_find_t ()
    : parent ()
  { }

  // -- Set partition of [0,n) o be singletons
  // FIXME necessary?
  union_find_t::union_find_t (size_t n)
    : parent (n,0)
  { for (size_t i = 0; i < n; ++i) parent[i] = i; }

  union_find_t::union_find_t (const union_find_t& c)
    : parent(c.parent)
  { }

  // -- true iff m and n are in the same set
  bool union_find_t::in_same_set (size_t m, size_t n)
  {
    return m == n or root_of(m) == root_of(n);
  }

  // -- union the sets in the partition
  // axiom: !in_same_set(m,n)
  void union_find_t::union_sets (size_t m, size_t n)
  {
    n = root_of(n);
    parent[n] = m;
  }

  // -- return a fresh variable
  size_t union_find_t::fresh_variable ()
  {
    size_t var = parent.size();
    parent.push_back(var);
    return var;
  }

  // -- get the canonical element of the set containing n
  auto union_find_t::root_of (size_t n) -> size_t
  {
    size_t parent_of_n = parent[n];
    while (n != parent_of_n) {
      n = parent_of_n;
      parent_of_n = parent[n]; }
    return n;
  }



  // ------------------------------ //
  // --- Canonical Element maps --- //
  // ------------------------------ //

  template <typename Expr>
    canonical_map_t<Expr>::canonical_map_t ()
      : representatives()
    { }

  template <typename Expr>
    maybe<size_t> canonical_map_t<Expr>::get (expr_t e)
    {
      auto i = representatives.find(e);
      if (representatives.end() == i)
        return maybe<size_t>();
      return maybe<size_t>(i->second);
    }

  template <typename Expr>
    void canonical_map_t<Expr>::set (expr_t e, size_t rep)
    {
      representatives.insert(std::make_pair(e,rep));
    }



  // ---------------------------- //
  // --- Expression Traversal --- //
  // ---------------------------- //

  template <
    typename Expr,
    typename Args,
    typename Same_symbol,
    typename Num_args
  >
  expr_traversal<Expr, Args, Same_symbol, Num_args>::expr_traversal (
      const Args& args, const Same_symbol& is_same_symbol,
      const Num_args& num_args)
      : args(args), is_same_symbol(is_same_symbol), num_args(num_args)
    { }

  template <
    typename Expr,
    typename Args,
    typename Same_symbol,
    typename Num_args
  >
    auto
    expr_traversal<Expr, Args, Same_symbol, Num_args>::traverse
      (expr_t e1, expr_t e2) -> std::vector<expr_pair_t>
    {
      std::vector<expr_pair_t> diffs;
      if (!is_same_symbol(e1,e2))
        diffs.push_back(std::make_pair(e1,e2));
      else {
        auto e1_args = begin(args(e1));
        auto e2_args = begin(args(e2));
        for (size_t n = 0; n < num_args(e1); ++n, ++e1_args, ++e2_args)
          traverse(*e1_args,*e2_args); }
      return diffs;
    }



}



#endif // DIMITRI_CONGRUENCE_HPP