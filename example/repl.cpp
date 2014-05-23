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
#include <dbrpp/exec.hpp>
#include <dbrpp/lexer.hpp>
#include <dbrpp/pool.hpp>
#include <dbrpp/posn.hpp>
#include <dbrpp/handler.hpp>
#include <dbrpp/view.hpp>
#include <dbrpp/zmqctx.hpp>

#include <dbr/msg.h>
#include <dbr/util.h>

#include <algorithm>
#include <cassert>
#include <cerrno>
#include <climits> // PATH_MAX
#include <functional>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <sys/types.h>
#include <unistd.h>

//#pragma GCC diagnostic ignored "-Wunused-function"

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
    typedef pair<Arity, function<void (Arg, Arg)> > Cmd;
    typedef map<string, Cmd> Cmds;
    Cmds cmds_;
    bool quit_;
public:
    Repl() noexcept
    : quit_(false)
    {
    }
    void
    cmd(const string& name, Arity arity, const function<void (Arg, Arg)>& fun)
    {
        cmds_[name] = make_pair(arity, fun);
    }
    void
    eval(const string& name, Arg begin, Arg end)
    {
        Cmds::const_iterator cmd = cmds_.find(name);
        if (cmd != cmds_.end()) {
            // Remaining tokens are the arguments.
            const Arity arity = cmd->second.first;
            if (arity < 0 || arity == distance(begin, end)) {
                try {
                    cmd->second.second(begin, end);
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
        auto lexer = make_lexer([this, &toks](const char* tok, size_t len) {
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
                lexer.exec(line.data(), line.size());
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

class Handler {
public:
    void
    accnt(Arg begin, Arg end)
    {
    }
    void
    contr(Arg begin, Arg end)
    {
    }
    void
    logon(Arg begin, Arg end)
    {
    }
    void
    logoff(Arg begin, Arg end)
    {
    }
    void
    user(Arg begin, Arg end)
    {
    }
    void
    group(Arg begin, Arg end)
    {
    }
    void
    order(Arg begin, Arg end)
    {
    }
    void
    trade(Arg begin, Arg end)
    {
    }
    void
    posn(Arg begin, Arg end)
    {
    }
    void
    level(Arg begin, Arg end)
    {
    }
    void
    view(Arg begin, Arg end)
    {
    }
    void
    place(int action, Arg begin, Arg end)
    {
    }
    void
    revise(Arg begin, Arg end)
    {
    }
    void
    cancel(Arg begin, Arg end)
    {
    }
    void
    ack_trade(Arg begin, Arg end)
    {
    }
    void
    echo(Arg begin, Arg end)
    {
    }
    void
    penv(Arg begin, Arg end)
    {
    }
    void
    set(Arg begin, Arg end)
    {
    }
    void
    unset(Arg begin, Arg end)
    {
    }
    void
    quit(Arg begin, Arg end)
    {
        throw Quit();
    }
};
}

int
main(int argc, char* argv[])
{
    try {
        Handler handler;
        Repl repl;

        repl.cmd("accnt", 0, bind(&Handler::accnt, ref(handler), _1, _2));
        repl.cmd("contr", 0, bind(&Handler::contr, ref(handler), _1, _2));
        repl.cmd("logon", 0, bind(&Handler::logon, ref(handler), _1, _2));
        repl.cmd("logoff", 0, bind(&Handler::logoff, ref(handler), _1, _2));
        repl.cmd("user", 0, bind(&Handler::user, ref(handler), _1, _2));
        repl.cmd("group", 0, bind(&Handler::group, ref(handler), _1, _2));
        repl.cmd("order", 0, bind(&Handler::order, ref(handler), _1, _2));
        repl.cmd("trade", 0, bind(&Handler::trade, ref(handler), _1, _2));
        repl.cmd("posn", 0, bind(&Handler::posn, ref(handler), _1, _2));
        repl.cmd("level", 0, bind(&Handler::level, ref(handler), _1, _2));
        repl.cmd("view", 0, bind(&Handler::view, ref(handler), _1, _2));
        repl.cmd("buy", 2, bind(&Handler::place, ref(handler), DBR_ACTION_BUY, _1, _2));
        repl.cmd("sell", 2, bind(&Handler::place, ref(handler), DBR_ACTION_SELL, _1, _2));
        repl.cmd("revise", 2, bind(&Handler::revise, ref(handler), _1, _2));
        repl.cmd("cancel", -1, bind(&Handler::cancel, ref(handler), _1, _2));
        repl.cmd("ack_trade", -1, bind(&Handler::ack_trade, ref(handler), _1, _2));
        repl.cmd("echo", -1, bind(&Handler::echo, ref(handler), _1, _2));
        repl.cmd("penv", 0, bind(&Handler::penv, ref(handler), _1, _2));
        repl.cmd("set", 2, bind(&Handler::set, ref(handler), _1, _2));
        repl.cmd("unset", 1, bind(&Handler::unset, ref(handler), _1, _2));
        repl.cmd("quit", 0, bind(&Handler::quit, ref(handler), _1, _2));

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
