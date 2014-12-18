/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <scpp/fig/ctx.hpp>
#include <scpp/fig/handler.hpp>

#include <scpp/ash/sem.hpp>

#include <sc/fig/clnt.h>

#include <sc/ash/log.h>

#include <functional>
#include <iostream>

using namespace sc;
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
    on_logon(ClntRef clnt, ScIden req_id, ScIden aid) noexcept
    {
        cout << "on_logon\n";
    }
    void
    on_logoff(ClntRef clnt, ScIden req_id, ScIden aid) noexcept
    {
        cout << "on_logoff\n";
    }
    void
    on_reset(ClntRef clnt) noexcept
    {
        cout << "on_reset\n";
    }
    void
    on_timeout(ClntRef clnt, ScIden req_id) noexcept
    {
        cout << "on_timeout\n";
    }
    void
    on_status(ClntRef clnt, ScIden req_id, int num, const char* msg) noexcept
    {
        cout << "on_status\n";
    }
    void
    on_exec(ClntRef clnt, ScIden req_id, ScExec& exec) noexcept
    {
        cout << "on_exec\n";
    }
    void
    on_posn(ClntRef clnt, ScPosn& posn) noexcept
    {
        cout << "on_posn\n";
    }
    void
    on_view(ClntRef clnt, ScView& view) noexcept
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
    ostream& os = level > SC_LOG_WARN ? cout : cerr;
    os << msg << endl;
}

int
main(int argc, char* argv[])
{
    sc_log_setlogger(log_ios);
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
