Style {#Style}
=====

Filenames
---------

- [CamelCase] filenames;
- header files have `.hpp` extension;
- source files have `.cpp` extension.

Formatting
----------

- spaces instead of tabs;
- indent by 4 spaces;
- lines should not exceed 100 characters.

[ClangFormat] should be used to format source-code changes. The [GitClangFormat] script integrates
[ClangFormat] with [Git].

Naming
------

- [CamelCase] naming;
- types start with upper-case;
- functions start with lower-case.

Coding
------

- See [CppCoreGuidelines].

[CamelCase]: https://en.wikipedia.org/wiki/CamelCase
[ClangFormat]: http://clang.llvm.org/docs/ClangFormat.html
[Git]: https://git-scm.com/
[GitClangFormat]: https://github.com/llvm-mirror/clang/blob/master/tools/clang-format/git-clang-format
[CppCoreGuidelines]: https://github.com/isocpp/CppCoreGuidelines
