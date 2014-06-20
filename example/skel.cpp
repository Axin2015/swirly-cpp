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
#include <dbrpp/fig/ctx.hpp>
#include <dbrpp/fig/handler.hpp>
#include <dbrpp/ash/sem.hpp>

#include <dbr/fig/clnt.h>
#include <dbr/ash/log.h>

#include <functional>
#include <iostream>

using namespace dbr;
using namespace std;

class Handler : public IHandler<Handler> {
    Sem ready_;
public:
    void
    on_close(ClntRef clnt) noexcept
    {
        cout << "on_close\n";
    }
    void
    on_ready(ClntRef clnt) noexcept
    {
        cout << "on_ready\n";
        ready_.post();
    }
    void
    on_logon(ClntRef clnt, DbrIden req_id, DbrIden uid) noexcept
    {
        cout << "on_logon\n";
    }
    void
    on_logoff(ClntRef clnt, DbrIden req_id, DbrIden uid) noexcept
    {
        cout << "on_logoff\n";
    }
    void
    on_reset(ClntRef clnt) noexcept
    {
        cout << "on_reset\n";
    }
    void
    on_timeout(ClntRef clnt, DbrIden req_id) noexcept
    {
        cout << "on_timeout\n";
    }
    void
    on_status(ClntRef clnt, DbrIden req_id, int num, const char* msg) noexcept
    {
        cout << "on_status\n";
    }
    void
    on_exec(ClntRef clnt, DbrIden req_id, DbrExec& exec) noexcept
    {
        cout << "on_exec\n";
    }
    void
    on_posn(ClntRef clnt, DbrPosn& posn) noexcept
    {
        cout << "on_posn\n";
    }
    void
    on_view(ClntRef clnt, DbrView& view) noexcept
    {
        cout << "on_view\n";
    }
    void
    on_flush(ClntRef clnt) noexcept
    {
        cout << "on_flush\n";
    }
    void*
    on_async(ClntRef clnt, void* val) noexcept
    {
        cout << "on_async\n";
        auto fn = static_cast<std::function<void* (ClntRef)>*>(val);
        return (*fn)(clnt);
    }
    void
    wait()
    {
        ready_.wait();
    }
};

void*
call(Async& async, std::function<void* (ClntRef)> fn)
{
    async.send(&fn);
    return async.recv();
}

void
log_ios(int level, const char* msg)
{
    ostream& os = level > DBR_LOG_WARN ? cout : cerr;
    os << msg << endl;
}

int
main(int argc, char* argv[])
{
    dbr_log_setlogger(log_ios);
    try {
        Handler handler;
        Ctx ctx("tcp://localhost:3270", "tcp://localhost:3271",
                5000, 8 * 1024 * 1024, &handler);
        cout << ctx << endl;
        handler.wait();

        Async async = ctx.async();
        call(async, [](ClntRef clnt) {
                for (auto rec : clnt.crecs()) {
                    ContrRecRef ref(rec);
                    cout << ref << endl;
                }
                return nullptr;
            });
        cout << "exiting\n";
        return 0;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
