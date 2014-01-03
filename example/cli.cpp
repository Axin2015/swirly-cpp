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

enum { BUF_MAX = 128 };

typedef int Arity;
typedef vector<string>::const_iterator Arg;

typedef invalid_argument InvalidArgument;

const char*
straction(int action) noexcept
{
    const char* sym;
    switch (action) {
    case DBR_BUY:
        sym = "BUY";
        break;
    case DBR_SELL:
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
    case DBR_MAKER:
        sym = "MAKER";
        break;
    case DBR_TAKER:
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
    case DBR_NEW:
        sym = "NEW";
        break;
    case DBR_REVISE:
        sym = "REVISE";
        break;
    case DBR_CANCEL:
        sym = "CANCEL";
        break;
    case DBR_TRADE:
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
    bool quit_;
public:
    Repl() noexcept
    :   quit_(false)
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
        while (!quit_ && getline(is, line)) {
            line += '\n';
            try {
                lexer.exec(line.data(), line.size());
            } catch (const exception& e) {
                cerr << e.what() << endl;
            }
        }
    }
    void
    set_quit(bool quit = true) noexcept
    {
        quit_ = quit;
    }
    bool
    quit() const noexcept
    {
        return quit_;
    }
};

iterator_traits<Arg>::difference_type
argc(Arg begin, Arg end)
{
    return distance(begin, end);
}

class Sess {
    Clnt& clnt_;
    DbrRec* arec_;
    DbrRec* crec_;
    DbrDate settl_date_;
    map<DbrIden, int> gtol_;
    map<int, DbrIden> ltog_;
    int id_;
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
    Sess(Clnt& clnt)
        : clnt_(clnt),
          arec_(nullptr),
          crec_(nullptr),
          settl_date_(0),
          id_(1)
    {
        // TODO: more robust logic.
        do {
            cout << '.';
            DbrStatus status;
            clnt_.poll(250, status);
        } while (!clnt_.ready());
        cout << endl;
    }
    void
    flush()
    {
        if (!clnt_.execs().empty()) {
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
                if (ref.state() == DBR_TRADE)
                    cout << '|' << left << setw(10) << strrole(ref.role())
                         << '|' << left << setw(10) << ref.cpty().mnem();
                else
                    cout << '|' << left << setw(10) << "N/A"
                         << '|' << left << setw(10) << "N/A";
                cout << '|' << endl;
            }
        }
        if (!clnt_.posnups().empty()) {
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
            cout << endl;
            cout <<
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
                "|" << endl;
            for (auto view : clnt_.viewups()) {
                ViewRef ref(view);
                if (ref.bid_count() > 0) {
                    cout << '|' << right << setw(10) << ref.bid_count()
                         << '|' << right << setw(10) << ref.bid_lots()
                         << '|' << right << setw(10) << ref.bid_ticks();
                } else {
                    cout << '|' << right << setw(10) << "- "
                         << '|' << right << setw(10) << "- "
                         << '|' << right << setw(10) << "- ";
                }
                if (ref.ask_count() > 0) {
                    cout << '|' << right << setw(10) << ref.ask_ticks()
                         << '|' << right << setw(10) << ref.ask_lots()
                         << '|' << right << setw(10) << ref.ask_count();
                } else {
                    cout << '|' << right << setw(10) << "- "
                         << '|' << right << setw(10) << "- "
                         << '|' << right << setw(10) << "- ";
                }
                cout << '|' << endl;
            }
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
        while (begin != end) {
            const auto id = ltog(ston<int>((*begin++).c_str()));
            clnt_.ack_trade(id);
        }
    }
    void
    view(Arg begin, Arg end)
    {
        if (!crec_)
            throw InvalidState("contr");
        if (!settl_date_)
            throw InvalidState("settl_date");

        cout <<
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
            "|" << endl;

        auto it = clnt_.views().find(crec_->id, settl_date_);
        if (it == clnt_.views().end())
            return;

        ViewRef ref(*it);
        if (ref.bid_count() > 0) {
            cout << '|' << right << setw(10) << ref.bid_count()
                 << '|' << right << setw(10) << ref.bid_lots()
                 << '|' << right << setw(10) << ref.bid_ticks();
        } else {
            cout << '|' << right << setw(10) << "- "
                 << '|' << right << setw(10) << "- "
                 << '|' << right << setw(10) << "- ";
        }
        if (ref.ask_count() > 0) {
            cout << '|' << right << setw(10) << ref.ask_ticks()
                 << '|' << right << setw(10) << ref.ask_lots()
                 << '|' << right << setw(10) << ref.ask_count();
        } else {
            cout << '|' << right << setw(10) << "- "
                 << '|' << right << setw(10) << "- "
                 << '|' << right << setw(10) << "- ";
        }
        cout << '|' << endl;
    }
    void
    cancel(Arg begin, Arg end)
    {
        while (begin != end) {
            const auto id = ltog(ston<int>((*begin++).c_str()));
            clnt_.cancel(id);
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
    echo(Arg begin, Arg end)
    {
        for (; begin != end; ++begin)
            cout << *begin << endl;
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
        if (arec_)
            cout << '|' << left << setw(20) << "accnt"
                 << '|' << left << setw(20) << AccntRecRef(*arec_).mnem()
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
    orders(Arg begin, Arg end)
    {
        auto trader = clnt_.trader();
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
        for (auto order : trader.orders()) {
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
        if (!arec_)
            throw InvalidState("accnt");
        if (!crec_)
            throw InvalidState("contr");
        if (!settl_date_)
            throw InvalidState("settl_date");

        const auto lots = ston<DbrLots>((*begin++).c_str());
        const auto price = ston<double>((*begin++).c_str());
        const auto ticks = ContrRecRef(*crec_).price_to_ticks(price);

        clnt_.place(arec_->mnem, crec_->mnem, settl_date_, nullptr, action, ticks, lots, 0);
    }
    void
    posns(Arg begin, Arg end)
    {
        if (!arec_)
            throw InvalidState("accnt");
        auto accnt = clnt_.accnt(*arec_);
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
        for (auto posn : accnt.posns()) {
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
        throw Quit();
    }
    void
    revise(Arg begin, Arg end)
    {
        const auto id = ltog(ston<int>((*begin++).c_str()));
        const auto lots = ston<DbrLots>((*begin++).c_str());

        clnt_.revise(id, lots);
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
            arec_ = &*it;
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
        auto trader = clnt_.trader();
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
        for (auto exec : trader.trades()) {
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
            arec_ = nullptr;
        } else if (name == "contr") {
            crec_ = nullptr;
        } else if (name == "settl_date") {
            settl_date_ = 0;
        } else
            throw InvalidArgument(name);
    }
};
}

int
main(int argc, char* argv[])
{
    if (argc != 2) {
        cerr << "usage: dbr_cli <trader>\n";
        return 1;
    }
    const char* const trader = argv[1];

    cout.sync_with_stdio(true);
    cerr.sync_with_stdio(true);
    try {
        ZmqCtx ctx;
        Pool pool;
        // epgm://239.192.1.1:3270
        Clnt clnt(ctx.c_arg(), "tcp://localhost:3270", "tcp://localhost:3271", trader,
                  dbr_millis(), pool);
        Sess sess(clnt);
        Repl repl;

        repl.cmd("accnts", 0, bind(&Sess::accnts, ref(sess), _1, _2));
        repl.cmd("ack_trades", -1, bind(&Sess::ack_trades, ref(sess), _1, _2));
        repl.cmd("buy", 2, bind(&Sess::place, ref(sess), DBR_BUY, _1, _2));
        repl.cmd("cancel", -1, bind(&Sess::cancel, ref(sess), _1, _2));
        repl.cmd("contrs", 0, bind(&Sess::contrs, ref(sess), _1, _2));
        repl.cmd("echo", -1, bind(&Sess::echo, ref(sess), _1, _2));
        repl.cmd("env", 0, bind(&Sess::env, ref(sess), _1, _2));
        repl.cmd("orders", 0, bind(&Sess::orders, ref(sess), _1, _2));
        repl.cmd("posns", 0, bind(&Sess::posns, ref(sess), _1, _2));
        repl.cmd("quit", 0, bind(&Sess::quit, ref(sess), _1, _2));
        repl.cmd("revise", 2, bind(&Sess::revise, ref(sess), _1, _2));
        repl.cmd("sell", 2, bind(&Sess::place, ref(sess), DBR_SELL, _1, _2));
        repl.cmd("set", 2, bind(&Sess::set, ref(sess), _1, _2));
        repl.cmd("traders", 0, bind(&Sess::traders, ref(sess), _1, _2));
        repl.cmd("trades", 0, bind(&Sess::trades, ref(sess), _1, _2));
        repl.cmd("unset", 1, bind(&Sess::unset, ref(sess), _1, _2));
        repl.cmd("view", 0, bind(&Sess::view, ref(sess), _1, _2));

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

        // Add stdin to descriptor set.
        zmq_pollitem_t templ{ NULL, 0, ZMQ_POLLIN, 0 };
        zmq_pollitem_t* items = clnt.setitems(&templ, 1);

        cout << "> ";
        cout.flush();
        char buf[BUF_MAX];
        while (!repl.quit()) {

            DbrStatus status;
            clnt.poll(30000, status);

            if (status.req_id > 0) {
                cout << endl;
                cout << "status: ";
                if (status.num == 0)
                    cout << "ok\n";
                else
                    cout << dbr::strncpy(status.msg, DBR_ERRMSG_MAX) << " (" << status.num << ")\n";
            }

            if ((items[0].revents & ZMQ_POLLIN)) {

                const ssize_t n{::read(0, buf, sizeof(buf))};
                if (n < 0)
                    throw PosixError();
                if (n == 0) {
                    // End-of file.
                    repl.set_quit();
                    break;
                }
                try {
                    lexer.exec(buf, n);
                } catch (const exception& e) {
                    cerr << e.what() << endl;
                }
            }

            sess.flush();
            clnt.clear();
            cout << "> ";
            cout.flush();
        }
        return 0;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
