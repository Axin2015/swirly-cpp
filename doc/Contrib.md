Contributing {#Contrib}
============

Git Config
----------

Set your user-name and email:

    $ git config --global user.name <your name>
    $ git config --global user.email <your email>

The following options may also be useful:

    $ git config --global alias.head 'log --graph --oneline --decorate'
    $ git config --global core.editor emacsclient
    $ git config --global rebase.autosquash true

Github Fork
-----------

Add remote upstream to fork:

    $ git remote add upstream git@github.com:swirlycloud/swirlyc.git

Sync local fork:

    $ git fetch upstream
    $ git rebase upstream/master master
    $ git push origin

Style
-----

See [the style guide](@ref Style).

Unit Testing
------------

Each header-file should have its own unit-test. The header-file should be the first file included in
the unit-test to ensure that the header-file is self-sufficient.

Functional Testing
------------------

The Swirly Functional Testing framework is called Swift. Swift tests are located in the `test/swift`
directory.

Continual Integration
---------------------

The continual integration environment is hosted courtesy of [TravisCI].

Dependencies
------------

Runtime Type Information (RTTI) bloats executable sizes by approximately 10% and adds unwanted
symbols to library export tables. RTTI has been disabled in Swirly to avoid this cost. As-of this
writing, Boost's program\_options and unit\_test_framework libraries both depend on RTTI, so they
are deliberately avoided.

[TravisCI]: http://travis-ci.org/swirlycloud/swirlyc
