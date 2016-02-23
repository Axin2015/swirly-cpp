Style Guide {#Style}
===========

Filenames
---------

- [CamelCase] filenames;
- header files have `.hpp` extension;
- source files have `.cpp` extension.

Formatting
----------

- spaces instead of tabs;
- indent by 2 spaces;
- lines should not exceed 100 characters.

[ClangFormat] should be used to format source-code changes. The [GitClangFormat] script integrates
[ClangFormat] with [Git].

Naming
------

- [CamelCase] naming;
- types and integer constants start with upper-case;
- functions and non-integer constants start with lower-case.

Big Six
-------

The big six (below) are explicitly declared to make intentions regarding `constexpr`, `noexcept` and
inlining are obvious and clear to the reader. [WYSIWYG] coding leads to fewer surprises!

- default constructor;
- destructor;
- copy constructor;
- copy assignment;
- move constructor;
- move assignment.

Also note that the `noexcept` gaurantee is important in ensuring the atomicity (all or nothingness)
of matching-engine transactions.

Class Layout
------------

The `public` section is first, followed by `protected` and then `private`. Within each section,
members are generally ordered as follows:

- big six;
- const functions;
- non-const functions;
- data.

Const functions precede non-const functions for two reasons: 1) it is the order naturally imposed
when a mutable interface extends an immutable interface, and 2) mutators will most likely be placed
alongside the private data-member section, which is generally more useful for maintainers.

Best Practice
-------------

For issues not covered by this document, please consult [CppCoreGuidelines] for industry
best-practice.

[CamelCase]: https://en.wikipedia.org/wiki/CamelCase
[ClangFormat]: http://clang.llvm.org/docs/ClangFormat.html
[Git]: https://git-scm.com/
[GitClangFormat]: https://github.com/llvm-mirror/clang/blob/master/tools/clang-format/git-clang-format
[CppCoreGuidelines]: https://github.com/isocpp/CppCoreGuidelines
[WYSIWYG]: https://en.wikipedia.org/wiki/WYSIWYG
