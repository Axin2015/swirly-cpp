Quick Start {#QuickStart}
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

Dependencies
------------

To build Doobry from source, you will need:

- [GCC] or [Clang] - with support for C11;
- [CMake] - for Makefile generation;
- [LibUuid] - for session identifiers;
- [Ragel] - if you intend to change `shlex.rl`;
- [SQLite] - for database management;
- [ZeroMQ] - for messaging.

Build from Source
-----------------

The following sections assume that `DOOBRY_SOURCE` is equal to `SOURCE_DIR/doobry`, and that
`DOOBRY_BUILD` is equal to `BUILD_DIR/doobry`.

### Download ###

Download the source from [GitHub](http://github.com/doobry-org):

    $ cd $SOURCE_DIR
    $ git clone https://github.com/doobry-org/doobry.git doobry

The `SOURCE_DIR/doobry` directory should now contain the source.

### Configure ###

Configure using `cmake` as follows from the build directory:

    $ cd $BUILD_DIR
    $ mkdir -p doobry
    $ cd doobry
    $ cmake $SOURCE_DIR/doobry -DCMAKE_INSTALL_PREFIX=$DOOBRY_HOME

This step will produce Unix Makefiles by default.
See CMake Options for more information.

### Make ###

Compile the code in the build directory using the `make` command:

    $ cd $BUILD_DIR/doobry
    $ make

### Install ###

Install to `CMAKE_INSTALL_PREFIX`:

    $ make install

Next Steps
----------

- use the [admin guide](AdminGuide.html) to create a database and start the daemon;
- start the shell and walk through the steps in the [tutorial](Tutorial.html).

[GCC]: http://gcc.gnu.org/
[Clang]: http://clang.llvm.org/
[CMake]: http://www.cmake.org/
[LibUuid]: http://linux.die.net/man/3/libuuid
[Ragel]: http://www.complang.org/ragel/
[SQLite]: http://sqlite.org/
[ZeroMQ]: http://zeromq.org/
