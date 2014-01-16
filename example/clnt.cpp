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
#include <dbrpp/sess.hpp>
#include <dbrpp/zmqctx.hpp>

#include <zmq.h>

#include <iostream>

using namespace dbr;
using namespace std;

class Sess : public dbr::ISess<Sess> {
public:
    void
    up_handler(int conn) noexcept
    {
    }
    void
    down_handler(int conn) noexcept
    {
    }
    void
    timeout_handler(DbrIden req_id) noexcept
    {
    }
    void
    status_handler(DbrIden req_id, int num, const char* msg) noexcept
    {
    }
    void
    exec_handler(DbrIden req_id, DbrExec& exec) noexcept
    {
    }
    void
    posn_handler(DbrPosn& posn) noexcept
    {
    }
    void
    view_handler(DbrView& view) noexcept
    {
    }
    void
    flush_handler() noexcept
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
        Clnt clnt(ctx.c_arg(), "tcp://localhost:3270", "tcp://localhost:3271",
                  "WRAMIREZ", 1, pool);

        // TODO: more robust logic.
        Sess sess;
        do {
            cout << '.';
            clnt.poll(250, &sess);
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
