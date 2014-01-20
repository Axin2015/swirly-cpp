/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
 *
 *  This file is part of Doobry written by Mark Aylett.
 *
 *  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program; if
 *  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA.
 */
#include <dbrpp/clnt.hpp>
#include <dbrpp/pool.hpp>
#include <dbrpp/handler.hpp>
#include <dbrpp/zmqctx.hpp>

#include <zmq.h>

#include <iostream>

using namespace dbr;
using namespace std;

class Handler : public dbr::IHandler<Handler> {
public:
    void
    up(int conn) noexcept
    {
    }
    void
    down(int conn) noexcept
    {
    }
    void
    timeout(DbrIden req_id) noexcept
    {
    }
    void
    status(DbrIden req_id, int num, const char* msg) noexcept
    {
    }
    void
    exec(DbrIden req_id, DbrExec& exec) noexcept
    {
    }
    void
    posn(DbrPosn& posn) noexcept
    {
    }
    void
    view(DbrView& view) noexcept
    {
    }
    void
    flush() noexcept
    {
    }
};

int
main(int argc, char* argv[])
{
    cout.sync_with_stdio(true);
    cerr.sync_with_stdio(true);
    try {
        ZmqCtx ctx;
        Pool pool;
        Clnt clnt("TEST", ctx.c_arg(), "tcp://localhost:3270", "tcp://localhost:3271", 1, pool);

        // TODO: more robust logic.
        Handler handler;
        do {
            cout << '.';
            clnt.poll(250, &handler);
        } while (!clnt.ready());
        cout << endl;

        cout << "traders:\n";
        for (auto rec : clnt.trecs()) {
            TraderRecRef ref(rec);
            cout << ref << endl;
        }
        cout << "accnts:\n";
        for (auto rec : clnt.arecs()) {
            AccntRecRef ref(rec);
            cout << ref << endl;
        }
        cout << "contrs:\n";
        for (auto rec : clnt.crecs()) {
            ContrRecRef ref(rec);
            cout << ref << endl;
        }
        return 0;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
