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

// PATH_MAX
#if !defined(__APPLE__)
# include <linux/limits.h>
#else  // defined(__APPLE__)
# include <sys/syslimits.h>
#endif // defined(__APPLE__)

#pragma GCC diagnostic ignored "-Wunused-function"

using namespace dbr;
using namespace std;
using namespace std::placeholders;

namespace {

enum { BUF_MAX = 128, TMOUT = 5000 };

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

class PosixError : public runtime_error {
public:
	virtual
    ~PosixError() noexcept
    {
    }
    explicit
    PosixError()
        : runtime_error(strerror(errno))
    {
    }
};

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

vector<string>&
split(const string& s, vector<string>& v)
{
    istringstream is(s);
    string tok;
    while (is >> tok && tok[0] != '#')
        v.push_back(tok);
    return v;
}

// Read-eval-print loop (REPL).

class Repl {
    typedef pair<Arity, function<void (Arg, Arg)> > Cmd;
    typedef map<string, Cmd> Cmds;
    Cmds cmds_;
public:
    Repl() noexcept
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
    read(istream& is)
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
        string line;
        while (getline(is, line)) {
            line += '\n';
            try {
                lexer.exec(line.data(), line.size());
            } catch (const exception& e) {
                cerr << e.what() << endl;
            }
        }
    }
};

iterator_traits<Arg>::difference_type
argc(Arg begin, Arg end)
{
    return distance(begin, end);
}

class Handler : public IHandler<Handler> {
    Clnt& clnt_;
    DbrTrader trader_;
    DbrAccnt accnt_;
    DbrRec* crec_;
    DbrDate settl_date_;
    map<DbrIden, int> gtol_;
    map<int, DbrIden> ltog_;
    int id_;
    bool prompt_;
    int
    gtol(DbrIden gid)
    {
        // First element that is greater than or equivalent to.
        auto it = gtol_.lower_bound(gid);
        if (it == gtol_.end() || it->first > gid) {
            it = gtol_.insert(it, make_pair(gid, id_++));
            ltog_[it->second] = gid;
        }
        return it->second;
    }
    DbrIden
    ltog(int lid) const
    {
        auto it = ltog_.find(lid);
        if (it == ltog_.end())
            throw InvalidArgument("no such local-id");
        return it->second;
    }
public:
    explicit
    Handler(Clnt& clnt)
        : clnt_(clnt),
          trader_(nullptr),
          accnt_(nullptr),
          crec_(nullptr),
          settl_date_(0),
          id_(1),
          prompt_(false)
    {
        // TODO: more robust logic.
        do {
            cout << '.';
            clnt_.dispatch(250, this);
        } while (!clnt_.is_ready());
        cout << endl;
    }
    void
    on_close() noexcept
    {
        prompt_ = true;
        cout << endl;
        cout << "close\n";
    }
    void
    on_ready() noexcept
    {
        prompt_ = true;
        cout << endl;
        cout << "ready\n";
    }
    void
    on_logon(DbrIden tid) noexcept
    {
        cout << "logon\n";
        trader_ = clnt_.trader(*clnt_.trecs().find(tid));
    }
    void
    on_logoff(DbrIden tid) noexcept
    {
        cout << "logoff\n";
        trader_ = nullptr;
    }
    void
    on_timeout(DbrIden req_id) noexcept
    {
        prompt_ = true;
        cout << endl;
        cout << "timeout: " << req_id << endl;
    }
    void
    on_status(DbrIden req_id, int num, const char* msg) noexcept
    {
        prompt_ = true;
        cout << endl;
        if (num == 0)
            cout << "ok\n";
        else
            cout << dbr::strncpy(msg, DBR_ERRMSG_MAX) << " (" << num << ")\n";
    }
    void
    on_exec(DbrIden req_id, DbrExec& exec) noexcept
    {
        prompt_ = true;
        cout << endl;
        cout << "ok\n";
    }
    void
    on_posn(DbrPosn& posn) noexcept
    {
    }
    void
    on_view(DbrView& view) noexcept
    {
    }
    void
    on_flush() noexcept
    {
    }
    void*
    on_async(void* val) noexcept
    {
        return val;
    }
    void
    clear()
    {
        if (!clnt_.execs().empty()) {
            prompt_ = true;
            cout << endl;
            cout <<
                "|id   "
                "|order"
                "|arec      "
                "|crec      "
                "|settl_date"
                "|state     "
                "|action    "
                "|ticks     "
                "|lots      "
                "|resd      "
                "|exec      "
                "|last_ticks"
                "|last_lots "
                "|role      "
                "|cpty      "
                "|" << endl;
            cout <<
                "|-----"
                "+-----"
                "+----------"
                "+----------"
                "+----------"
                "+----------"
                "+----------"
                "+----------"
                "+----------"
                "+----------"
                "+----------"
                "+----------"
                "+----------"
                "+----------"
                "+----------"
                "|"
                 << endl;
            for (auto exec : clnt_.execs()) {
                ExecRef ref(exec);
                cout << '|' << right << setw(5) << gtol(ref.id())
                     << '|' << right << setw(5) << gtol(ref.order())
                     << '|' << left << setw(10) << ref.arec().mnem()
                     << '|' << left << setw(10) << ref.crec().mnem()
                     << '|' << left << setw(10) << ref.settl_date()
                     << '|' << left << setw(10) << strstate(ref.state())
                     << '|' << left << setw(10) << straction(ref.action())
                     << '|' << right << setw(10) << ref.ticks()
                     << '|' << right << setw(10) << ref.lots()
                     << '|' << right << setw(10) << ref.resd()
                     << '|' << right << setw(10) << ref.exec()
                     << '|' << right << setw(10) << ref.last_ticks()
                     << '|' << right << setw(10) << ref.last_lots();
                if (ref.state() == DBR_STATE_TRADE)
                    cout << '|' << left << setw(10) << strrole(ref.role())
                         << '|' << left << setw(10) << ref.cpty().mnem();
                else
                    cout << '|' << left << setw(10) << "N/A"
                         << '|' << left << setw(10) << "N/A";
                cout << '|' << endl;
            }
        }
        if (!clnt_.posnups().empty()) {
            prompt_ = true;
            cout << endl;
            cout <<
                "|crec      "
                "|settl_date"
                "|buy_ticks "
                "|buy_lots  "
                "|sell_ticks"
                "|sell_lots "
                "|" << endl;
            cout <<
                "|----------"
                "+----------"
                "+----------"
                "+----------"
                "+----------"
                "+----------"
                "|"
                 << endl;
            for (auto posn : clnt_.posnups()) {
                PosnRef ref(posn);
                const auto buy_ticks = static_cast<double>(ref.buy_licks()) / ref.buy_lots();
                const auto sell_ticks = static_cast<double>(ref.sell_licks()) / ref.sell_lots();
                cout << '|' << left << setw(10) << ref.crec().mnem()
                     << '|' << left << setw(10) << ref.settl_date()
                     << '|' << right << setw(10) << static_cast<DbrTicks>(buy_ticks + 0.5)
                     << '|' << right << setw(10) << ref.buy_lots()
                     << '|' << right << setw(10) << static_cast<DbrTicks>(sell_ticks + 0.5)
                     << '|' << right << setw(10) << ref.sell_lots()
                     << '|' << endl;
            }
        }
        if (!clnt_.viewups().empty()) {
            prompt_ = true;
            cout << endl;
            cout <<
                "|crec      "
                "|settl_date"
                "|level     "
                "|bid_count "
                "|bid_lots  "
                "|bid_ticks "
                "|ask_ticks "
                "|ask_lots  "
                "|ask_count "
                "|" << endl;
            cout <<
                "|----------"
                "+----------"
                "+----------"
                "+----------"
                "+----------"
                "+----------"
                "+----------"
                "+----------"
                "+----------"
                "|" << endl;
            for (auto view : clnt_.viewups()) {
                ViewRef ref(view);
                for (size_t i{0}; i < DBR_LEVEL_MAX; ++i) {
                    cout << '|' << left << setw(10) << ref.crec().mnem()
                         << '|' << left << setw(10) << ref.settl_date()
                         << '|' << right << setw(10) << (i + 1);
                    if (ref.bid_count(i) > 0) {
                        cout << '|' << right << setw(10) << ref.bid_count(i)
                             << '|' << right << setw(10) << ref.bid_lots(i)
                             << '|' << right << setw(10) << ref.bid_ticks(i);
                    } else {
                        cout << '|' << right << setw(10) << "- "
                             << '|' << right << setw(10) << "- "
                             << '|' << right << setw(10) << "- ";
                    }
                    if (ref.ask_count(i) > 0) {
                        cout << '|' << right << setw(10) << ref.ask_ticks(i)
                             << '|' << right << setw(10) << ref.ask_lots(i)
                             << '|' << right << setw(10) << ref.ask_count(i);
                    } else {
                        cout << '|' << right << setw(10) << "- "
                             << '|' << right << setw(10) << "- "
                             << '|' << right << setw(10) << "- ";
                    }
                    cout << '|' << endl;
                }
            }
        }
        clnt_.clear();
        if (prompt_) {
            prompt_ = false;
            cout << "> ";
            cout.flush();
        }
    }
    void
    accnts(Arg begin, Arg end)
    {
        cout <<
            "|mnem      "
            "|display                       "
            "|email                         "
            "|" << endl;
        cout <<
            "|----------"
            "+------------------------------"
            "+------------------------------"
            "|" << endl;
        for (auto rec : clnt_.arecs()) {
            AccntRecRef ref(rec);
            cout << '|' << left << setw(10) << ref.mnem()
                 << '|' << left << setw(30) << ref.display()
                 << '|' << left << setw(30) << ref.email()
                 << '|' << endl;
        }
    }
    void
    ack_trades(Arg begin, Arg end)
    {
        if (!trader_)
            throw InvalidState("not logged-on");

        while (begin != end) {
            const auto id = ltog(ston<int>((*begin++).c_str()));
            clnt_.ack_trade(trader_, id);
        }
    }
    void
    cancel(Arg begin, Arg end)
    {
        if (!trader_)
            throw InvalidState("not logged-on");

        while (begin != end) {
            const auto id = ltog(ston<int>((*begin++).c_str()));
            clnt_.cancel(trader_, id);
        }
    }
    void
    contrs(Arg begin, Arg end)
    {
        cout <<
            "|mnem      "
            "|display   "
            "|asset_type"
            "|asset     "
            "|ccy       "
            "|tick_numer"
            "|tick_denom"
            "|lot_numer "
            "|lot_denom "
            "|pip_dp    "
            "|min_lots  "
            "|max_lots  "
            "|" << endl;
        cout <<
            "|----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "|" << endl;
        for (auto rec : clnt_.crecs()) {
            ContrRecRef ref(rec);
            cout << '|' << left << setw(10) << ref.mnem()
                 << '|' << left << setw(10) << ref.display()
                 << '|' << left << setw(10) << ref.asset_type()
                 << '|' << left << setw(10) << ref.asset()
                 << '|' << left << setw(10) << ref.ccy()
                 << '|' << right << setw(10) << ref.tick_numer()
                 << '|' << right << setw(10) << ref.tick_denom()
                 << '|' << right << setw(10) << ref.lot_numer()
                 << '|' << right << setw(10) << ref.lot_denom()
                 << '|' << right << setw(10) << ref.pip_dp()
                 << '|' << right << setw(10) << ref.min_lots()
                 << '|' << right << setw(10) << ref.max_lots()
                 << '|' << endl;
        }
    }
    void
    env(Arg begin, Arg end)
    {
        cout <<
            "|name                "
            "|value               "
            "|" << endl;
        cout <<
            "|--------------------"
            "+--------------------"
            "|" << endl;
        if (accnt_)
            cout << '|' << left << setw(20) << "accnt"
                 << '|' << left << setw(20) << Accnt(accnt_).rec().mnem()
                 << '|' << endl;
        if (crec_)
            cout << '|' << left << setw(20) << "contr"
                 << '|' << left << setw(20) << ContrRecRef(*crec_).mnem()
                 << '|' << endl;
        if (settl_date_)
            cout << '|' << left << setw(20) << "settl_date"
                 << '|' << left << setw(20) << settl_date_
                 << '|' << endl;
    }
    void
    echo(Arg begin, Arg end)
    {
        for (; begin != end; ++begin)
            cout << *begin << endl;
    }
    void
    logon(Arg begin, Arg end)
    {
        const string& mnem = *begin++;
        auto it = clnt_.trecs().find(mnem.c_str());
        if (it == clnt_.trecs().end())
            throw InvalidArgument(mnem);
        clnt_.logon(clnt_.trader(*it));
    }
    void
    logoff(Arg begin, Arg end)
    {
        if (!trader_)
            throw InvalidState("not logged-on");
        clnt_.logoff(trader_);
    }
    void
    orders(Arg begin, Arg end)
    {
        if (!trader_)
            throw InvalidState("not logged-on");
        cout <<
            "|id   "
            "|arec      "
            "|crec      "
            "|settl_date"
            "|state     "
            "|action    "
            "|ticks     "
            "|lots      "
            "|resd      "
            "|exec      "
            "|last_ticks"
            "|last_lots "
            "|" << endl;
        cout <<
            "|-----"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "|"
             << endl;
        for (auto order : Trader(trader_).orders()) {
            OrderRef ref(order);
            cout << '|' << right << setw(5) << gtol(ref.id())
                 << '|' << left << setw(10) << ref.arec().mnem()
                 << '|' << left << setw(10) << ref.crec().mnem()
                 << '|' << left << setw(10) << ref.settl_date()
                 << '|' << left << setw(10) << strstate(ref.state())
                 << '|' << left << setw(10) << straction(ref.action())
                 << '|' << right << setw(10) << ref.ticks()
                 << '|' << right << setw(10) << ref.lots()
                 << '|' << right << setw(10) << ref.resd()
                 << '|' << right << setw(10) << ref.exec()
                 << '|' << right << setw(10) << ref.last_ticks()
                 << '|' << right << setw(10) << ref.last_lots()
                 << '|' << endl;
        }
    }
    void
    place(int action, Arg begin, Arg end)
    {
        if (!trader_)
            throw InvalidState("not logged-on");
        if (!accnt_)
            throw InvalidState("account not set");
        if (!crec_)
            throw InvalidState("contract not set");
        if (!settl_date_)
            throw InvalidState("settlement date not set");

        const auto lots = ston<DbrLots>((*begin++).c_str());
        const auto price = ston<double>((*begin++).c_str());
        const auto ticks = ContrRecRef(*crec_).price_to_ticks(price);

        clnt_.place(trader_, accnt_, *crec_, settl_date_,
                    nullptr, action, ticks, lots, 0);
    }
    void
    posns(Arg begin, Arg end)
    {
        if (!accnt_)
            throw InvalidState("account not set");
        cout <<
            "|crec      "
            "|settl_date"
            "|buy_ticks "
            "|buy_lots  "
            "|sell_ticks"
            "|sell_lots "
            "|" << endl;
        cout <<
            "|----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "|"
             << endl;
        for (auto posn : Accnt(accnt_).posns()) {
            PosnRef ref(posn);
            const auto buy_ticks = static_cast<double>(ref.buy_licks()) / ref.buy_lots();
            const auto sell_ticks = static_cast<double>(ref.sell_licks()) / ref.sell_lots();
            cout << '|' << left << setw(10) << ref.crec().mnem()
                 << '|' << left << setw(10) << ref.settl_date()
                 << '|' << right << setw(10) << static_cast<DbrTicks>(buy_ticks + 0.5)
                 << '|' << right << setw(10) << ref.buy_lots()
                 << '|' << right << setw(10) << static_cast<DbrTicks>(sell_ticks + 0.5)
                 << '|' << right << setw(10) << ref.sell_lots()
                 << '|' << endl;
        }
    }
    void
    quit(Arg begin, Arg end)
    {
        clnt_.close();
    }
    void
    revise(Arg begin, Arg end)
    {
        if (!trader_)
            throw InvalidState("not logged-on");

        const auto id = ltog(ston<int>((*begin++).c_str()));
        const auto lots = ston<DbrLots>((*begin++).c_str());

        clnt_.revise(trader_, id, lots);
    }
    void
    set(Arg begin, Arg end)
    {
        const string& name = *begin++;
        const string& value = *begin++;
        if (name == "accnt") {
            auto it = clnt_.arecs().find(value.c_str());
            if (it == clnt_.arecs().end())
                throw InvalidArgument(value);
            accnt_ = clnt_.accnt(*it);
        } else if (name == "contr") {
            auto it = clnt_.crecs().find(value.c_str());
            if (it == clnt_.crecs().end())
                throw InvalidArgument(value);
            crec_ = &*it;
        } else if (name == "settl_date") {
            settl_date_ = ston<DbrDate>(value.c_str());
        } else
            throw InvalidArgument(name);
    }
    void
    traders(Arg begin, Arg end)
    {
        cout <<
            "|mnem      "
            "|display                       "
            "|email                         "
            "|" << endl;
        cout <<
            "|----------"
            "+------------------------------"
            "+------------------------------"
            "|" << endl;
        for (auto rec : clnt_.trecs()) {
            TraderRecRef ref(rec);
            cout << '|' << left << setw(10) << ref.mnem()
                 << '|' << left << setw(30) << ref.display()
                 << '|' << left << setw(30) << ref.email()
                 << '|' << endl;
        }
    }
    void
    trades(Arg begin, Arg end)
    {
        if (!trader_)
            throw InvalidState("not logged-on");
        cout <<
            "|id   "
            "|order"
            "|arec      "
            "|crec      "
            "|settl_date"
            "|action    "
            "|ticks     "
            "|lots      "
            "|resd      "
            "|exec      "
            "|last_ticks"
            "|last_lots "
            "|role      "
            "|cpty      "
            "|" << endl;
        cout <<
            "|-----"
            "+-----"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "|"
             << endl;
        for (auto exec : Trader(trader_).trades()) {
            ExecRef ref(exec);
            cout << '|' << right << setw(5) << gtol(ref.id())
                 << '|' << right << setw(5) << gtol(ref.order())
                 << '|' << left << setw(10) << ref.arec().mnem()
                 << '|' << left << setw(10) << ref.crec().mnem()
                 << '|' << left << setw(10) << ref.settl_date()
                 << '|' << left << setw(10) << straction(ref.action())
                 << '|' << right << setw(10) << ref.ticks()
                 << '|' << right << setw(10) << ref.lots()
                 << '|' << right << setw(10) << ref.resd()
                 << '|' << right << setw(10) << ref.exec()
                 << '|' << right << setw(10) << ref.last_ticks()
                 << '|' << right << setw(10) << ref.last_lots()
                 << '|' << left << setw(10) << strrole(ref.role())
                 << '|' << left << setw(10) << ref.cpty().mnem()
                 << '|' << endl;
        }
    }
    void
    unset(Arg begin, Arg end)
    {
        const string& name = *begin++;
        if (name == "accnt") {
            accnt_ = nullptr;
        } else if (name == "contr") {
            crec_ = nullptr;
        } else if (name == "settl_date") {
            settl_date_ = 0;
        } else
            throw InvalidArgument(name);
    }
    void
    view(Arg begin, Arg end)
    {
        if (!crec_)
            throw InvalidState("contract not set");
        if (!settl_date_)
            throw InvalidState("settlement date not set");

        cout <<
            "|crec      "
            "|settl_date"
            "|level     "
            "|bid_count "
            "|bid_lots  "
            "|bid_ticks "
            "|ask_ticks "
            "|ask_lots  "
            "|ask_count "
            "|" << endl;
        cout <<
            "|----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "+----------"
            "|" << endl;

        auto it = clnt_.views().find(crec_->id, settl_date_);
        if (it == clnt_.views().end())
            return;

        ViewRef ref(*it);
        for (size_t i{0}; i < DBR_LEVEL_MAX; ++i) {
            cout << '|' << left << setw(10) << ref.crec().mnem()
                 << '|' << left << setw(10) << ref.settl_date()
                 << '|' << right << setw(10) << (i + 1);
            if (ref.bid_count(i) > 0) {
                cout << '|' << right << setw(10) << ref.bid_count(i)
                     << '|' << right << setw(10) << ref.bid_lots(i)
                     << '|' << right << setw(10) << ref.bid_ticks(i);
            } else {
                cout << '|' << right << setw(10) << "- "
                     << '|' << right << setw(10) << "- "
                     << '|' << right << setw(10) << "- ";
            }
            if (ref.ask_count(i) > 0) {
                cout << '|' << right << setw(10) << ref.ask_ticks(i)
                     << '|' << right << setw(10) << ref.ask_lots(i)
                     << '|' << right << setw(10) << ref.ask_count(i);
            } else {
                cout << '|' << right << setw(10) << "- "
                     << '|' << right << setw(10) << "- "
                     << '|' << right << setw(10) << "- ";
            }
            cout << '|' << endl;
        }
    }
    void
    set_prompt(bool prompt = true) noexcept
    {
        prompt_ = prompt;
    }
    bool
    prompt() const noexcept
    {
        return prompt_;
    }
};
}

int
main(int argc, char* argv[])
{
    cout.sync_with_stdio(true);
    cerr.sync_with_stdio(true);
    try {
        ZmqCtx ctx;
        Pool pool(8 * 1024 * 1024);
        // epgm://239.192.1.1:3270
        Clnt clnt(ctx.c_arg(), "test", "tcp://localhost:3270", "tcp://localhost:3271",
                  dbr_millis(), TMOUT, pool);
        Handler handler(clnt);
        Repl repl;

        repl.cmd("accnts", 0, bind(&Handler::accnts, ref(handler), _1, _2));
        repl.cmd("ack_trades", -1, bind(&Handler::ack_trades, ref(handler), _1, _2));
        repl.cmd("buy", 2, bind(&Handler::place, ref(handler), DBR_ACTION_BUY, _1, _2));
        repl.cmd("cancel", -1, bind(&Handler::cancel, ref(handler), _1, _2));
        repl.cmd("contrs", 0, bind(&Handler::contrs, ref(handler), _1, _2));
        repl.cmd("echo", -1, bind(&Handler::echo, ref(handler), _1, _2));
        repl.cmd("env", 0, bind(&Handler::env, ref(handler), _1, _2));
        repl.cmd("logon", 1, bind(&Handler::logon, ref(handler), _1, _2));
        repl.cmd("logoff", 0, bind(&Handler::logoff, ref(handler), _1, _2));
        repl.cmd("orders", 0, bind(&Handler::orders, ref(handler), _1, _2));
        repl.cmd("posns", 0, bind(&Handler::posns, ref(handler), _1, _2));
        repl.cmd("quit", 0, bind(&Handler::quit, ref(handler), _1, _2));
        repl.cmd("revise", 2, bind(&Handler::revise, ref(handler), _1, _2));
        repl.cmd("sell", 2, bind(&Handler::place, ref(handler), DBR_ACTION_SELL, _1, _2));
        repl.cmd("set", 2, bind(&Handler::set, ref(handler), _1, _2));
        repl.cmd("traders", 0, bind(&Handler::traders, ref(handler), _1, _2));
        repl.cmd("trades", 0, bind(&Handler::trades, ref(handler), _1, _2));
        repl.cmd("unset", 1, bind(&Handler::unset, ref(handler), _1, _2));
        repl.cmd("view", 0, bind(&Handler::view, ref(handler), _1, _2));

        char path[PATH_MAX];
        sprintf(path, "%s/.dbr_clirc", getenv("HOME"));
        ifstream cfg(path);
        if (cfg.is_open())
            repl.read(cfg);

        vector<string> toks;
        auto lexer = make_lexer([&repl, &toks](const char* tok, size_t len) {
                if (tok)
                    toks.push_back(tok);
                else {
                    repl.eval(toks);
                    toks.clear();
                }
            });

        // FIXME: migrate to async model.
        assert(0);
        zmq_pollitem_t* items = nullptr;
#if 0
        // Add stdin to descriptor set.
        zmq_pollitem_t templ{ NULL, 0, ZMQ_POLLIN, 0 };
        zmq_pollitem_t* items = clnt.setitems(&templ, 1);
#endif
        cout << "> ";
        cout.flush();
        char buf[BUF_MAX];
        do {

            clnt.dispatch(30000, &handler);
            if ((items[0].revents & ZMQ_POLLIN)) {

                const ssize_t n{::read(0, buf, sizeof(buf))};
                if (n < 0)
                    throw PosixError();
                if (n == 0) {
                    // End-of file.
                    clnt.close();
                    break;
                }
                try {
                    handler.set_prompt(true);
                    lexer.exec(buf, n);
                } catch (const exception& e) {
                    cerr << e.what() << endl;
                }
            }
            handler.clear();
        } while (clnt.is_ready());
        return 0;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
