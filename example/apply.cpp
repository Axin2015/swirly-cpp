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
        auto fn = static_cast<std::function<void* (ClntRef)>*>(val);
        return (*fn)(clnt);
    }
};

void*
apply(Async& async, std::function<void* (ClntRef)> fn)
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

        cout << ctx << endl;
        Async async = ctx.async();
        apply(async, [](ClntRef clnt) {
                clnt.close();
                return nullptr;
            });

        cout << "exiting\n";
        return 0;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
