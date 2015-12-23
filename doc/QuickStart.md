Quick Start {#QuickStart}
===========

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

- [GCC] or [Clang] - with support for C11;
- [CMake] - for Makefile generation;

Build from Source
-----------------

The following sections assume that `SWIRLY_SOURCE` is equal to `SOURCE_DIR/swirlyc`, and that
`SWIRLY_BUILD` is equal to `BUILD_DIR/swirlyc`.

### Download ###

Download the source from [GitHub](http://github.com/swirlycloud):

    $ cd $SOURCE_DIR
    $ git clone https://github.com/swirlycloud/swirlyc.git swirlyc

The `SOURCE_DIR/swirlyc` directory should now contain the source.

### Configure ###

Configure using `cmake` as follows from the build directory:

    $ cd $BUILD_DIR
    $ mkdir -p swirlyc
    $ cd swirlyc
    $ cmake $SOURCE_DIR/swirlyc -DCMAKE_INSTALL_PREFIX=$SWIRLY_HOME

This step will produce Unix Makefiles by default.
See CMake Options for more information.

### Make ###

Compile the code in the build directory using the `make` command:

    $ cd $BUILD_DIR/swirlyc
    $ make

### Install ###

Install to `CMAKE_INSTALL_PREFIX`:

    $ make install

[GCC]: http://gcc.gnu.org/
[Clang]: http://clang.llvm.org/
[CMake]: http://www.cmake.org/
