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
#include <dbrpp/ctx.hpp>
#include <dbrpp/handler.hpp>

#include <dbr/clnt.h>
#include <dbr/util.h>

#include <functional>
#include <iostream>

using namespace dbr;
using namespace std;

class Handler : public IHandler<Handler> {
public:
    void
    on_close(DbrClnt clnt) noexcept
    {
    }
    void
    on_ready(DbrClnt clnt) noexcept
    {
    }
    void
    on_logon(DbrClnt clnt, DbrIden req_id, DbrIden uid) noexcept
    {
    }
    void
    on_logoff(DbrClnt clnt, DbrIden req_id, DbrIden uid) noexcept
    {
    }
    void
    on_reset(DbrClnt clnt) noexcept
    {
    }
    void
    on_timeout(DbrClnt clnt, DbrIden req_id) noexcept
    {
    }
    void
    on_status(DbrClnt clnt, DbrIden req_id, int num, const char* msg) noexcept
    {
    }
    void
    on_exec(DbrClnt clnt, DbrIden req_id, DbrExec& exec) noexcept
    {
    }
    void
    on_posn(DbrClnt clnt, DbrPosn& posn) noexcept
    {
    }
    void
    on_view(DbrClnt clnt, DbrView& view) noexcept
    {
    }
    void
    on_flush(DbrClnt clnt) noexcept
    {
    }
    void*
    on_async(DbrClnt clnt, void* val) noexcept
    {
        auto fn = static_cast<std::function<void* (DbrClnt)>*>(val);
        return (*fn)(clnt);
    }
};

void*
apply(Async& async, std::function<void* (DbrClnt)> fn)
{
    async.send(&fn);
    return async.recv();
}

int
main(int argc, char* argv[])
{
    try {
        Handler handler;
        Ctx ctx("tcp://localhost:3270", "tcp://localhost:3271",
                dbr_millis(), 5000, 8 * 1024 * 1024, &handler);
        Async async = ctx.async();
        apply(async, [](DbrClnt clnt) {
                dbr_clnt_close(clnt);
                return nullptr;
            });

        cout << "exiting\n";
        return 0;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
