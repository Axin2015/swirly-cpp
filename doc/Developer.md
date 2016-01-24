Developer {#Developer}
=========

Environment Variables
---------------------

This guide may refer to the following environment variables:

- `SWIRLY_BUILD` - build directory;
- `SWIRLY_HOME` - installation directory;
- `SWIRLY_SOURCE` - source directory.

It is recommended that you also modify your environment, so that the binaries installed under
`SWIRLY_HOME` can be located at runtime:

    PATH=$PATH:$SWIRLY_HOME/bin; export PATH
    LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SWIRLY_HOME/lib; export LD_LIBRARY_PATH

Dependencies
------------

To build Swirly from source, you will need:

- [CMake] - for Makefile generation;
- [GCC] or [Clang] - with support for C++14;
- [Boost] - for additional library dependencies.

Build from Source
-----------------

The following sections assume that `SWIRLY_SOURCE` is equal to `SOURCE_DIR/swirlyc`, and that
`SWIRLY_BUILD` is equal to `BUILD_DIR/swirlyc`.

### Download ###

Clone the project from the [master repository](http://github.com/swirlycloud/swirlyc) or your own
personal fork:

    $ cd $SOURCE_DIR
    $ git clone https://github.com/swirlycloud/swirlyc.git swirlyc

The `SOURCE_DIR/swirlyc` directory should now contain the source.

A [Zip archive](https://github.com/swirlycloud/swirlyc/archive/master.zip) can also be download as
an alternative.

### Configure ###

Configure using `cmake` as follows from the build directory:

    $ cd $BUILD_DIR
    $ mkdir -p swirlyc
    $ cd swirlyc
    $ cmake $SOURCE_DIR/swirlyc -DCMAKE_INSTALL_PREFIX=$SWIRLY_HOME

This step will produce Unix Makefiles by default. The build-type and compiler can be set using the
following options:

    $ cmake $SOURCE_DIR/swirlyc -G "Unix Makefiles" \
        -DCMAKE_INSTALL_PREFIX=$SWIRLY_HOME \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_COMPILER=gcc-5 \
        -DCMAKE_CXX_COMPILER=g++-5

See CMake Options for more information.

### Make ###

Compile the code in the build directory using the `make` command:

    $ cd $BUILD_DIR/swirlyc
    $ make

### Install ###

Install to `CMAKE_INSTALL_PREFIX`:

    $ make install

Documentation
-------------

Testing
-------

Each header-file should have its own unit-test. The header-file should be the first file included in
the unit-test to ensure that the header-file is self-sufficient.

[Boost]: http://www.boost.org/
[Clang]: http://clang.llvm.org/
[CMake]: http://www.cmake.org/
[GCC]: http://gcc.gnu.org/
