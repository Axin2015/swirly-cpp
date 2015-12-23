Dev Guide {#DevGuide}
=========

CMake Options
-------------

    $ cmake $SWIRLY_SOURCE -G "Unix Makefiles" \
                           -DCMAKE_INSTALL_PREFIX=$SWIRLY_HOME \
                           -DCMAKE_BUILD_TYPE=Debug \
                           -DCMAKE_C_COMPILER=/opt/local/bin/gcc-mp-4.8 \
                           -DCMAKE_CXX_COMPILER=/opt/local/bin/g++-mp-4.8 \
                           -DCMAKE_C_FLAGS_DEBUG="-g3 -gdwarf-2" \
                           -DCMAKE_CXX_FLAGS_DEBUG="-g3 -gdwarf-2"

Third Party
-----------

### Doxygen ###

### SQLite ###

### Tree ###
