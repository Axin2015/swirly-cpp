Quick Start
===========

Environment Variables
---------------------

This guide may refer to the following environment variables:

- `DOOBRY_BUILD` - build directory;
- `DOOBRY_HOME` - installation directory;
- `DOOBRY_SOURCE` - source directory.

It is recommended that you also modify your environment, so that the binaries installed under
`DOOBRY_HOME` can be located at runtime:

    PATH=$PATH:$DOOBRY_HOME/bin; export PATH
    LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DOOBRY_HOME/lib; export LD_LIBRARY_PATH
    PYTHONPATH=$DOOBRY_HOME/lib; export PYTHONPATH

Dependencies
------------

To build Doobry from source, you will need:

- [GCC] or [Clang] - with support for C11;
- [CMake] - for Makefile generation;
- [Cython] - for generating the [Python] module;
- [Ragel] - if you intend to change `ragel.rl`;
- [SQLite] - for database management;
- [ZeroMQ] - for messaging.

Installing
----------

The following sections assume that `DOOBRY_SOURCE` is equal to `SOURCE_DIR/doobry`, and that
`DOOBRY_BUILD` is equal to `BUILD_DIR/doobry`.

### Download ###

Download the source from [GitHub](http://github.com/doobry-org):

    $ cd $SOURCE_DIR
    $ git clone https://github.com/doobry-org/doobry.git doobry

The `SOURCE_DIR/doobry` directory should now contain the source.

### Configuration ###

Configure using `cmake` as follows from the build directory:

    $ cd $BUILD_DIR
    $ mkdir -p doobry
    $ cd doobry
    $ cmake $SOURCE_DIR/doobry -DCMAKE_INSTALL_PREFIX=$DOOBRY_HOME

This step will produce Unix Makefiles by default.

### Build ###

Assuming that you have write access to the default installation path:

    $ cd $BUILD_DIR/doobry
    $ make
    $ make install

See CMake Options for more information.

[GCC]: http://gcc.gnu.org/
[Clang]: http://clang.llvm.org/
[CMake]: http://www.cmake.org/
[Cython]: http://cython.org/
[Ragel]: http://www.complang.org/ragel/
[SQLite]: http://sqlite.org/
[ZeroMQ]: http://zeromq.org/

### Tutorial ###
