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
#include <dbrpp/dispatch.hpp>
#include <dbrpp/pool.hpp>
#include <dbrpp/handler.hpp>
#include <dbrpp/zmqctx.hpp>

#include <iostream>

#include <zmq.h>

using namespace dbr;
using namespace std;

class Handler : public dbr::IHandler<Handler> {
public:
    void
    on_close(ClntRef clnt) noexcept
    {
    }
    void
    on_ready(ClntRef clnt) noexcept
    {
    }
    void
    on_logon(ClntRef clnt, DbrIden req_id, DbrIden uid) noexcept
    {
    }
    void
    on_logoff(ClntRef clnt, DbrIden req_id, DbrIden uid) noexcept
    {
    }
    void
    on_reset(ClntRef clnt) noexcept
    {
    }
    void
    on_timeout(ClntRef clnt, DbrIden req_id) noexcept
    {
    }
    void
    on_status(ClntRef clnt, DbrIden req_id, int num, const char* msg) noexcept
    {
    }
    void
    on_exec(ClntRef clnt, DbrIden req_id, DbrExec& exec) noexcept
    {
    }
    void
    on_posn(ClntRef clnt, DbrPosn& posn) noexcept
    {
    }
    void
    on_view(ClntRef clnt, DbrView& view) noexcept
    {
    }
    void
    on_flush(ClntRef clnt) noexcept
    {
    }
    void*
    on_async(ClntRef clnt, void* val) noexcept
    {
        return val;
    }
};

int
main(int argc, char* argv[])
{
    cout.sync_with_stdio(true);
    cerr.sync_with_stdio(true);
    try {
        ZmqCtx ctx;
        Pool pool(8 * 1024 * 1024);
        Clnt clnt(ctx.c_arg(), NULL, "tcp://localhost:3270", "tcp://localhost:3271",
                  1, 5000, pool);

        // TODO: more robust logic.
        Handler handler;
        do {
            cout << '.';
            dispatch(clnt, 250, &handler);
        } while (!clnt.is_ready());
        cout << endl;

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
