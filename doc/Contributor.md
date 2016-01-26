Contributor {#Contributor}
===========

Git Config
----------

Set your user-name and email:

    $ git config --global user.name <your name>
    $ git config --global user.email <your email>

Github Fork
-----------

Add remote upstream to fork:

    $ git remote add upstream git@github.com:swirlycloud/swirlyc.git

Sync local fork:

    $ git checkout master
    $ git fetch upstream
    $ git rebase upstream/master
    $ git push origin
