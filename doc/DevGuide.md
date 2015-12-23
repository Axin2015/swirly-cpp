Dev Guide {#DevGuide}
=========

Build Options
-------------

    $ cmake $SWIRLY_SOURCE -G "Unix Makefiles" \
                           -DCMAKE_INSTALL_PREFIX=$SWIRLY_HOME \
                           -DCMAKE_BUILD_TYPE=Debug \
                           -DCMAKE_C_COMPILER=/opt/local/bin/gcc-mp-5 \
                           -DCMAKE_CXX_COMPILER=/opt/local/bin/g++-mp-5 \
                           -DCMAKE_C_FLAGS_DEBUG="-g3 -gdwarf-2" \
                           -DCMAKE_CXX_FLAGS_DEBUG="-g3 -gdwarf-2"

Documentation
-------------

Testing
-------

Each header-file should have its own unit-test. The header-file should be the first file included in
the unit-test to ensure that the header-file is self-sufficient.
