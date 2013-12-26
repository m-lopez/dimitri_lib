congruence
==========

This is an implementation of an abstract congruence closure and some common
operations that use equality axioms.



backgound
---------

Equality axioms are important in some areas of computer science. From a
collection of equalities, one can derive more equalities via substitutions. For
instance, when one knows <code>a = b</code> and <code>b = c</code>, then
<code>a = c</code> follows by substituting a variable from one equality into
another. By exhaustively deriving all equalities from a small collection, one
finds a simple, but rich, structure called an _equivalence closure_.

Most useful languages contain function symbols in addition to variables and
constants. A function symbol has an arity, the number of parameters that it
accepts. With function symbols one can write interesting equalities such as
<code>a+b = 4</code> and <code>f(f(a)) = f(a)</code>. The collection of all
possible equalities from some set of equalities is called a _congruence closure_.



congruence_t
------------

This data structure implements an abstract congruence closure. It is abstract in
that the expression language, the language over which we apply equational
reasoning, is abstract. Any data structure for which the following functions are
defined is compatible with <code>congruence_t</code>. For a type <code>e</code>
and an associtated integer type <code>Nat</code>

+   <code>args(e) -> Range(e)</code> - returns the arguments of an expression
+   <code>same_symbol(e,e) -> bool</code> - true iff the expressions represent
      the same function symbol. This is not the equality defined on
      <code>e</code>. It should return true when the function symbol being
      applied is the same function symbol. The arguments may be different.
+   <code>num_args(e) -> Nat</code> - the number of arguments applied to this
      function symbol.
+   <code>E < E -> bool</code> - the expression type must be weakly ordered so
      that <code>std::set</code> can be used to hash expressions

### Features ###

This data structure was created to support unification over any language X. As
such unification is the primary feature.

This data structure stores equailities and supports equality queries.



want more info?
---------------

For those of you that enjoy rigor, pretty symbols, and academic-ese, look at
_Abstract Congruence Closure_ by Bachmair, Tiwari, and Vigneron.



todo
----

+   break <code>maybe</code> off into its own library. Describe differences
      between <code>maybe</code> and <code>optional</code>
+   scoped congruence closure - for those of us in programming languages, symbols
      and facts in a language have scope. Equality axioms are no different. Add
      support for efficient commits and rollbacks if possible.