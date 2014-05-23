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
#include <dbrpp/shlex.hpp>
#include <dbrpp/handler.hpp>

#include <dbr/log.h>
#include <dbr/util.h>

#include <climits> // PATH_MAX
#include <functional>
#include <fstream>
#include <iostream>
#include <map>

using namespace dbr;
using namespace std;
using namespace std::placeholders;

namespace {

typedef int Arity;
typedef vector<string>::const_iterator Arg;

typedef invalid_argument InvalidArgument;

const char*
straction(int action) noexcept
{
    const char* sym;
    switch (action) {
    case DBR_ACTION_BUY:
        sym = "BUY";
        break;
    case DBR_ACTION_SELL:
        sym = "SELL";
        break;
    default:
        throw InvalidArgument("action");
    }
    return sym;
}

const char*
strrole(int role) noexcept
{
    const char* sym;
    switch (role) {
    case DBR_ROLE_MAKER:
        sym = "MAKER";
        break;
    case DBR_ROLE_TAKER:
        sym = "TAKER";
        break;
    default:
        throw InvalidArgument("role");
    }
    return sym;
}

const char*
strstate(int state) noexcept
{
    const char* sym;
    switch (state) {
    case DBR_STATE_NEW:
        sym = "NEW";
        break;
    case DBR_STATE_REVISE:
        sym = "REVISE";
        break;
    case DBR_STATE_CANCEL:
        sym = "CANCEL";
        break;
    case DBR_STATE_TRADE:
        sym = "TRADE";
        break;
    default:
        throw InvalidArgument("state");
    }
    return sym;
}

class InvalidState : public logic_error {
public:
	virtual
    ~InvalidState() noexcept
    {
    }
    explicit
    InvalidState(const string& arg)
        : logic_error(arg)
    {
    }
};

struct Quit { };

template <typename T>
class JoinIterator : public iterator<output_iterator_tag, void, void, void, void> {
    ostream* os_;
    const char* delim_;
    bool first_;
public:
    JoinIterator(ostream& os, const char* delim)
        : os_(&os),
          delim_(delim),
          first_(true)
    {
    }
    JoinIterator&
    operator =(T value)
    {
        if (first_)
            first_ = false;
        else
            *os_ << delim_;
        *os_ << value;
        return *this;
    }
    JoinIterator&
    operator *()
    {
        return *this;
    }
    JoinIterator&
    operator ++()
    {
        return *this;
    }
    JoinIterator&
    operator ++(int)
    {
        return *this;
    }
};

template <typename T>
T
ston(const char* s);

template<>
double
ston(const char* s)
{
    errno = 0;
    char* end;
    double d = strtod(s, &end);
    if (0 != errno || *end)
        throw InvalidArgument(s);
    return d;
}

template<>
long
ston(const char* s)
{
    errno = 0;
    char* end;
    long l = strtol(s, &end, 10);
    if (0 != errno || *end)
        throw InvalidArgument(s);
    return l;
}

template<>
int
ston(const char* s)
{
    long l = ston<long>(s);
    if (l < numeric_limits<int>::min() || numeric_limits<int>::max() < l)
        throw InvalidArgument(s);
    return static_cast<int>(l);
}

string
join(Arg begin, Arg end, const char* delim = " ")
{
    ostringstream ss;
    copy(begin, end, ostream_iterator<string>(ss, delim));
    return ss.str();
}

string
join(const vector<string>& v, const char* delim = " ")
{
    return join(v.begin(), v.end(), delim);
}

// Read-eval-print loop (REPL).

class Repl {
    typedef pair<function<void (Arg, Arg)>, Arity> Cmd;
    typedef map<string, Cmd> Cmds;
    Cmds cmds_;
    bool quit_;
public:
    Repl() noexcept
    : quit_(false)
    {
    }
    void
    bind(const string& name, const function<void (Arg, Arg)>& fun, Arity arity)
    {
        cmds_[name] = make_pair(fun, arity);
    }
    void
    eval(const string& name, Arg begin, Arg end)
    {
        Cmds::const_iterator cmd = cmds_.find(name);
        if (cmd != cmds_.end()) {
            // Remaining tokens are the arguments.
            const Arity arity = cmd->second.second;
            if (arity < 0 || arity == distance(begin, end)) {
                try {
                    cmd->second.first(begin, end);
                } catch (const DbrException& e) {
                    cerr << "exception: " << e.what() << endl;
                } catch (const InvalidArgument& e) {
                    cerr << "invalid argument: " << e.what() << endl;
                } catch (const InvalidState& e) {
                    cerr << "invalid state: " << e.what() << endl;
                } catch (const Quit&) {
                    quit_ = true;
                }
            } else
                cerr << "invalid argument(s): " << join(begin, end) << endl;
        } else
            cerr << "invalid command: " << name << endl;
    }
    void
    eval(const vector<string>& toks)
    {
        if (!toks.empty()) {
            Arg begin = toks.begin();
            const string& name = *begin++;
            eval(name, begin, toks.end());
        }
    }
    void
    read(istream& is, const char* prompt = nullptr)
    {
        vector<string> toks;
        auto shlex = make_shlex([this, &toks](const char* tok, size_t len) {
                if (tok)
                    toks.push_back(tok);
                else {
                    eval(toks);
                    toks.clear();
                }
            });
        if (prompt)
            cout << prompt;
        string line;
        while (!quit_ && getline(is, line)) {
            line += '\n';
            try {
                shlex.exec(line.data(), line.size());
            } catch (const exception& e) {
                cerr << e.what() << endl;
            }
            if (prompt)
                cout << prompt;
        }
    }
};

iterator_traits<Arg>::difference_type
argc(Arg begin, Arg end)
{
    return distance(begin, end);
}

void*
sendAndRecv(Async& async, std::function<void* (ClntRef)> fn)
{
    async.send(&fn);
    return async.recv();
}

class Handler : public IHandler<Handler> {
public:
    // IHandler<Handler>
    void
    on_close(ClntRef clnt) noexcept
    {
        cout << "on_close\n";
    }
    void
    on_ready(ClntRef clnt) noexcept
    {
        cout << "on_ready\n";
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
    }
    void*
    on_async(ClntRef clnt, void* val) noexcept
    {
        cout << "on_async\n";
        auto fn = static_cast<std::function<void* (ClntRef)>*>(val);
        return (*fn)(clnt);
    }
    // This.
    void
    accnt(Async& async, Arg begin, Arg end)
    {
    }
    void
    contr(Async& async, Arg begin, Arg end)
    {
        sendAndRecv(async, [](ClntRef clnt) {
                for (auto rec : clnt.crecs()) {
                    ContrRecRef ref(rec);
                    cout << ref << endl;
                }
                return nullptr;
            });
    }
    void
    logon(Async& async, Arg begin, Arg end)
    {
    }
    void
    logoff(Async& async, Arg begin, Arg end)
    {
    }
    void
    user(Async& async, Arg begin, Arg end)
    {
    }
    void
    group(Async& async, Arg begin, Arg end)
    {
    }
    void
    order(Async& async, Arg begin, Arg end)
    {
    }
    void
    trade(Async& async, Arg begin, Arg end)
    {
    }
    void
    posn(Async& async, Arg begin, Arg end)
    {
    }
    void
    level(Async& async, Arg begin, Arg end)
    {
    }
    void
    view(Async& async, Arg begin, Arg end)
    {
    }
    void
    buy(Async& async, Arg begin, Arg end)
    {
    }
    void
    sell(Async& async, Arg begin, Arg end)
    {
    }
    void
    revise(Async& async, Arg begin, Arg end)
    {
    }
    void
    cancel(Async& async, Arg begin, Arg end)
    {
    }
    void
    ack_trade(Async& async, Arg begin, Arg end)
    {
    }
    void
    echo(Async& async, Arg begin, Arg end)
    {
    }
    void
    penv(Async& async, Arg begin, Arg end)
    {
    }
    void
    set(Async& async, Arg begin, Arg end)
    {
    }
    void
    unset(Async& async, Arg begin, Arg end)
    {
    }
    void
    quit(Async& async, Arg begin, Arg end)
    {
        throw Quit();
    }
};

void
log_ios(int level, const char* msg)
{
    ostream& os = level > DBR_LOG_WARN ? cout : cerr;
    os << msg << endl;
}

}

int
main(int argc, char* argv[])
{
    dbr_log_setlogger(log_ios);
    try {
        Handler handler;
        Ctx ctx("tcp://localhost:3270", "tcp://localhost:3271",
                dbr_millis(), 5000, 8 * 1024 * 1024, &handler);
        cout << ctx << endl;

        Async async = ctx.async();

        Repl repl;
        repl.bind("accnt", bind(&Handler::accnt, ref(handler), ref(async), _1, _2), 0);
        repl.bind("contr", bind(&Handler::contr, ref(handler), ref(async), _1, _2), 0);
        repl.bind("logon", bind(&Handler::logon, ref(handler), ref(async), _1, _2), 0);
        repl.bind("logoff", bind(&Handler::logoff, ref(handler), ref(async), _1, _2), 0);
        repl.bind("user", bind(&Handler::user, ref(handler), ref(async), _1, _2), 0);
        repl.bind("group", bind(&Handler::group, ref(handler), ref(async), _1, _2), 0);
        repl.bind("order", bind(&Handler::order, ref(handler), ref(async), _1, _2), 0);
        repl.bind("trade", bind(&Handler::trade, ref(handler), ref(async), _1, _2), 0);
        repl.bind("posn", bind(&Handler::posn, ref(handler), ref(async), _1, _2), 0);
        repl.bind("level", bind(&Handler::level, ref(handler), ref(async), _1, _2), 0);
        repl.bind("view", bind(&Handler::view, ref(handler), ref(async), _1, _2), 0);
        repl.bind("buy", bind(&Handler::buy, ref(handler), ref(async), _1, _2), 2);
        repl.bind("sell", bind(&Handler::sell, ref(handler), ref(async), _1, _2), 2);
        repl.bind("revise", bind(&Handler::revise, ref(handler), ref(async), _1, _2), 2);
        repl.bind("cancel", bind(&Handler::cancel, ref(handler), ref(async), _1, _2), -1);
        repl.bind("ack_trade", bind(&Handler::ack_trade, ref(handler), ref(async), _1, _2), -1);
        repl.bind("echo", bind(&Handler::echo, ref(handler), ref(async), _1, _2), -1);
        repl.bind("penv", bind(&Handler::penv, ref(handler), ref(async), _1, _2), 0);
        repl.bind("set", bind(&Handler::set, ref(handler), ref(async), _1, _2), 2);
        repl.bind("unset", bind(&Handler::unset, ref(handler), ref(async), _1, _2), 1);
        repl.bind("quit", bind(&Handler::quit, ref(handler), ref(async), _1, _2), 0);

        char path[PATH_MAX];
        sprintf(path, "%s/.doobryrc", getenv("HOME"));
        ifstream cfg(path);
        if (cfg.is_open())
            repl.read(cfg);
        repl.read(cin, "> ");
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
