#!/bin/sh
# Raspberry PI Continual Integration script.

set -e

SWIRLY_BRANCH=${SWIRLY_BRANCH:-master}
SWIRLY_BUILD=${SWIRLY_BUILD:-$HOME/build/swirly}
SWIRLY_EMAIL=${SWIRLY_EMAIL:-mark.aylett@swirlycloud.com}
SWIRLY_SOURCE=${SWIRLY_SOURCE:-$HOME/repo/swirly}

cd $SWIRLY_SOURCE

git checkout $SWIRLY_BRANCH
git fetch origin

local=$(git rev-parse $SWIRLY_BRANCH)
remote=$(git rev-parse origin/$SWIRLY_BRANCH)

if [[ $local != $remote ]]; then
    git reset --hard origin/$SWIRLY_BRANCH
    git submodule update --init --recursive
    mkdir -p $SWIRLY_BUILD
    pushd $SWIRLY_BUILD
    cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_SHARED_LIBS=OFF $SWIRLY_SOURCE
    result=
    if make -j 2 all test swift; then
        result=pass
    else
        result=fail
    fi
    popd
    echo "Subject: build: $result" | msmtp -v $SWIRLY_EMAIL
fi
