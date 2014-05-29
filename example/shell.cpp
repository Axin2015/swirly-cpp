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
#include <dbrpp/exec.hpp>
#include <dbrpp/handler.hpp>
#include <dbrpp/memb.hpp>
#include <dbrpp/posn.hpp>
#include <dbrpp/shlex.hpp>
#include <dbrpp/view.hpp>

#include <dbr/log.h>
#include <dbr/util.h>

#include <algorithm>
#include <climits> // PATH_MAX
#include <functional>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>

using namespace dbr;
using namespace std;
using namespace std::placeholders;

namespace {

typedef int Arity;
typedef vector<string>::const_iterator Arg;

typedef invalid_argument InvalidArgument;

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
ston(const string& s);

template<>
double
ston(const string& s)
{
    errno = 0;
    char* end;
    double d = strtod(s.c_str(), &end);
    if (0 != errno || *end)
        throw InvalidArgument(s);
    return d;
}

template<>
long
ston(const string& s)
{
    errno = 0;
    char* end;
    long l = strtol(s.c_str(), &end, 10);
    if (0 != errno || *end)
        throw InvalidArgument(s);
    return l;
}

template<>
int
ston(const string& s)
{
    long l = ston<long>(s.c_str());
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

void
call(Async& async, std::function<void (ClntRef)> fn)
{
    async.send(&fn);
    async.recv();
}

template <size_t COLS>
array<size_t, COLS>
head_width(const array<const char*, COLS>& head)
{
    array<size_t, COLS> width;
    for (size_t i = 0; i < COLS; ++i)
        width[i] = strlen(head[i] + 1);
    return width;
}

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

AccntRecRef
get_arec(ClntRef clnt, DbrIden id)
{
    auto it = clnt.arecs().find(id);
    if (it == clnt.arecs().end())
        throw InvalidArgument(to_string(id));
    return AccntRecRef(*it);
}

AccntRecRef
get_arec(ClntRef clnt, const char* mnem)
{
    auto it = clnt.arecs().find(mnem);
    if (it == clnt.arecs().end())
        throw InvalidArgument(mnem);
    return AccntRecRef(*it);
}

ContrRecRef
get_crec(ClntRef clnt, DbrIden id)
{
    auto it = clnt.crecs().find(id);
    if (it == clnt.crecs().end())
        throw InvalidArgument(to_string(id));
    return ContrRecRef(*it);
}

ContrRecRef
get_crec(ClntRef clnt, const char* mnem)
{
    auto it = clnt.crecs().find(mnem);
    if (it == clnt.crecs().end())
        throw InvalidArgument(mnem);
    return ContrRecRef(*it);
}

template <size_t COLS>
void
print_table(ostream& os,
            const array<const char*, COLS>& head,
            const array<size_t, COLS>& width,
            const vector<array<string, COLS>>& rows)
{
    os << '|';
    for (size_t i = 0; i < COLS; ++i) {
        auto flags = os.flags();
        os.setf(ios_base::left, ios_base::adjustfield);
        os.width(width[i]);
        os << head[i] + 1;
        os.flags(flags);
        os << '|';
    }
    os << endl;

    os << '+';
    for (size_t i = 0; i < COLS; ++i) {
        auto fill = os.fill('-');
        os.width(width[i]);
        os << "";
        os.fill(fill);
        os << '+';
    }
    os << endl;

    for (const auto& row : rows) {
        os << '|';
        for (size_t i = 0; i < COLS; ++i) {
            auto flags = os.flags();
            os.setf(*head[i] == '>' ? ios_base::right : ios_base::left,
                    ios_base::adjustfield);
            os.width(width[i]);
            os << row[i];
            os.flags(flags);
            os << '|';
        }
        os << endl;
    }
}

class Handler : public IHandler<Handler> {
    // Main thread.
    map<string, string> env_;
    // Context thread.

    const string&
    get(const string& name)
    {
        auto it = env_.find(name);
        if (it == env_.end())
            throw InvalidState(string(name).append(" not set"));
        return it->second;
    }
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
        try {
            auto fn = static_cast<std::function<void (ClntRef)>*>(val);
            (*fn)(clnt);
        } catch (const DbrException& e) {
            cerr << "exception: " << e.what() << endl;
        } catch (const InvalidArgument& e) {
            cerr << "invalid argument: " << e.what() << endl;
        } catch (const InvalidState& e) {
            cerr << "invalid state: " << e.what() << endl;
        }
        return nullptr;
    }
    // This.
    void
    accnt(Async& async, Arg begin, Arg end)
    {
        enum { COLS = 3 };
        typedef array<string, COLS> row;

        const array<const char*, COLS> head{
            "<mnem",
            "<display",
            "<email"};
        auto width = head_width(head);
        vector<row> rows;
        call(async, [&width, &rows](ClntRef clnt) {
                for (auto rec : clnt.arecs()) {
                    AccntRecRef ref(rec);
                    row r{
                        to_string(ref.mnem()),
                        to_string(ref.display()),
                        to_string(ref.email())
                    };
                    for (size_t i = 0; i < COLS; ++i)
                        width[i] = max(width[i], r[i].size());
                    rows.emplace_back(r);
                }
            });
        print_table(cout, head, width, rows);
    }
    void
    contr(Async& async, Arg begin, Arg end)
    {
        enum { COLS = 12 };
        typedef array<string, COLS> row;

        const array<const char*, COLS> head{
            "<mnem",
            "<display",
            "<asset_type",
            "<asset",
            "<ccy",
            ">tick_numer",
            ">tick_denom",
            ">lot_numer",
            ">lot_denom",
            ">pip_dp",
            ">min_lots",
            ">max_lots"};
        auto width = head_width(head);
        vector<row> rows;
        call(async, [&width, &rows](ClntRef clnt) {
                for (auto rec : clnt.crecs()) {
                    ContrRecRef ref(rec);
                    row r{
                        to_string(ref.mnem()),
                        to_string(ref.display()),
                        to_string(ref.asset_type()),
                        to_string(ref.asset()),
                        to_string(ref.ccy()),
                        to_string(ref.tick_numer()),
                        to_string(ref.tick_denom()),
                        to_string(ref.lot_numer()),
                        to_string(ref.lot_denom()),
                        to_string(ref.pip_dp()),
                        to_string(ref.min_lots()),
                        to_string(ref.max_lots())
                    };
                    for (size_t i = 0; i < COLS; ++i)
                        width[i] = max(width[i], r[i].size());
                    rows.emplace_back(r);
                }
            });
        print_table(cout, head, width, rows);
    }
    void
    logon(Async& async, Arg begin, Arg end)
    {
        const string umnem = get("user");
        call(async, [umnem](ClntRef clnt) {
                clnt.logon(clnt.accnt(get_arec(clnt, umnem.c_str())));
            });
    }
    void
    logoff(Async& async, Arg begin, Arg end)
    {
        const string umnem = get("user");
        call(async, [umnem](ClntRef clnt) {
                clnt.logoff(clnt.accnt(get_arec(clnt, umnem.c_str())));
            });
    }
    void
    user(Async& async, Arg begin, Arg end)
    {
        const string umnem = get("user");

        enum { COLS = 2 };
        typedef array<string, COLS> row;

        const array<const char*, COLS> head{
            "<user",
            "<group"};
        auto width = head_width(head);
        vector<row> rows;

        call(async, [umnem, &width, &rows](ClntRef clnt) {
                auto urec = get_arec(clnt, umnem.c_str());
                for (auto memb : Accnt(clnt.accnt(urec)).users()) {
                    MembRef ref(memb);
                    row r{
                        to_string(ref.urec().mnem()),
                        to_string(ref.grec().mnem())
                    };
                    for (size_t i = 0; i < COLS; ++i)
                        width[i] = max(width[i], r[i].size());
                    rows.emplace_back(r);
                }
            });
        print_table(cout, head, width, rows);
    }
    void
    group(Async& async, Arg begin, Arg end)
    {
        const string umnem = get("user");

        enum { COLS = 2 };
        typedef array<string, COLS> row;

        const array<const char*, COLS> head{
            "<user",
            "<group"};
        auto width = head_width(head);
        vector<row> rows;

        call(async, [umnem, &width, &rows](ClntRef clnt) {
                auto urec = get_arec(clnt, umnem.c_str());
                for (auto memb : Accnt(clnt.accnt(urec)).groups()) {
                    MembRef ref(memb);
                    row r{
                        to_string(ref.urec().mnem()),
                        to_string(ref.grec().mnem())
                    };
                    for (size_t i = 0; i < COLS; ++i)
                        width[i] = max(width[i], r[i].size());
                    rows.emplace_back(r);
                }
            });
        print_table(cout, head, width, rows);
    }
    void
    order(Async& async, Arg begin, Arg end)
    {
        const string umnem = get("user");

        enum { COLS = 12 };
        typedef array<string, COLS> row;

        const array<const char*, COLS> head{
            "<id",
            "<group",
            "<contr",
            "<settl_date",
            "<state",
            "<action",
            ">price",
            ">lots",
            ">resd",
            ">exec",
            ">last_price",
            ">last_lots"};
        auto width = head_width(head);
        vector<row> rows;

        call(async, [umnem, &width, &rows](ClntRef clnt) {
                auto urec = get_arec(clnt, umnem.c_str());
                for (auto order : Accnt(clnt.accnt(urec)).orders()) {
                    OrderRef ref(order);
                    row r{
                        to_string(ref.id()),
                        to_string(ref.grec().mnem()),
                        to_string(ref.crec().mnem()),
                        to_string(dbr_jd_to_iso(ref.settl_day())),
                        strstate(ref.state()),
                        straction(ref.action()),
                        to_string(ref.crec().ticks_to_price(ref.ticks())),
                        to_string(ref.lots()),
                        to_string(ref.resd()),
                        to_string(ref.exec()),
                        to_string(ref.crec().ticks_to_price(ref.last_ticks())),
                        to_string(ref.last_lots()),
                    };
                    for (size_t i = 0; i < COLS; ++i)
                        width[i] = max(width[i], r[i].size());
                    rows.emplace_back(r);
                }
            });
        print_table(cout, head, width, rows);
    }
    void
    trade(Async& async, Arg begin, Arg end)
    {
        const string umnem = get("user");

        enum { COLS = 15 };
        typedef array<string, COLS> row;

        const array<const char*, COLS> head{
            "<id",
            "<order",
            "<group",
            "<contr",
            "<settl_date",
            "<state",
            "<action",
            ">price",
            ">lots",
            ">resd",
            ">exc",
            ">last_price",
            ">last_lots",
            "<role",
            "<cpty"};
        auto width = head_width(head);
        vector<row> rows;

        call(async, [umnem, &width, &rows](ClntRef clnt) {
                auto urec = get_arec(clnt, umnem.c_str());
                for (auto trade : Accnt(clnt.accnt(urec)).trades()) {
                    ExecRef ref(trade);
                    row r{
                        to_string(ref.id()),
                        to_string(ref.order()),
                        to_string(ref.grec().mnem()),
                        to_string(ref.crec().mnem()),
                        to_string(dbr_jd_to_iso(ref.settl_day())),
                        strstate(ref.state()),
                        straction(ref.action()),
                        to_string(ref.crec().ticks_to_price(ref.ticks())),
                        to_string(ref.lots()),
                        to_string(ref.resd()),
                        to_string(ref.exec()),
                        to_string(ref.crec().ticks_to_price(ref.last_ticks())),
                        to_string(ref.last_lots()),
                        strrole(ref.role()),
                        to_string(ref.cpty().mnem())
                    };
                    for (size_t i = 0; i < COLS; ++i)
                        width[i] = max(width[i], r[i].size());
                    rows.emplace_back(r);
                }
            });
        print_table(cout, head, width, rows);
    }
    void
    posn(Async& async, Arg begin, Arg end)
    {
        const string gmnem = get("group");

        enum { COLS = 6 };
        typedef array<string, COLS> row;

        const array<const char*, COLS> head{
            "<contr",
            "<settl_date",
            ">buy_price",
            ">buy_lots",
            ">sell_price",
            ">sell_lots"};
        auto width = head_width(head);
        vector<row> rows;

        call(async, [gmnem, &width, &rows](ClntRef clnt) {
                auto grec = get_arec(clnt, gmnem.c_str());
                for (auto posn : Accnt(clnt.accnt(grec)).posns()) {
                    PosnRef ref(posn);
                    const auto buy_ticks = static_cast<double>(ref.buy_licks()) / ref.buy_lots();
                    const auto sell_ticks = static_cast<double>(ref.sell_licks()) / ref.sell_lots();
                    row r{
                        to_string(ref.crec().mnem()),
                        to_string(dbr_jd_to_iso(ref.settl_day())),
                        to_string(buy_ticks * ref.crec().price_inc()),
                        to_string(ref.buy_lots()),
                        to_string(sell_ticks * ref.crec().price_inc()),
                        to_string(ref.sell_lots())
                    };
                    for (size_t i = 0; i < COLS; ++i)
                        width[i] = max(width[i], r[i].size());
                    rows.emplace_back(r);
                }
            });
        print_table(cout, head, width, rows);
    }
    void
    top(Async& async, Arg begin, Arg end)
    {
        enum {
            BID_PRICE = 2,
            BID_LOTS,
            BID_COUNT,
            OFFER_PRICE,
            OFFER_LOTS,
            OFFER_COUNT,
            COLS
        };
        typedef array<string, COLS> row;

        const array<const char*, COLS> head{
            "<contr",
            "<settl_date",
            ">bid_price",
            ">bid_lots",
            ">bid_count",
            ">offer_price",
            ">offer_lots",
            ">offer_count"};
        auto width = head_width(head);
        vector<row> rows;

        call(async, [&width, &rows](ClntRef clnt) {
                for (auto view : clnt.views()) {
                    ViewRef ref(view);
                    row r{
                        to_string(ref.crec().mnem()),
                        to_string(dbr_jd_to_iso(ref.settl_day())),
                        "-",
                        "-",
                        "-",
                        "-",
                        "-",
                        "-",
                    };
                    if (ref.bid_count(0) > 0) {
                        r[BID_PRICE] = to_string(ref.crec().ticks_to_price(ref.bid_ticks(0)));
                        r[BID_LOTS] = to_string(ref.bid_lots(0));
                        r[BID_COUNT] = to_string(ref.bid_count(0));
                    }
                    if (ref.offer_count(0) > 0) {
                        r[OFFER_PRICE] = to_string(ref.crec().ticks_to_price(ref.offer_ticks(0)));
                        r[OFFER_LOTS] = to_string(ref.offer_lots(0));
                        r[OFFER_COUNT] = to_string(ref.offer_count(0));
                    }
                    for (size_t i = 0; i < COLS; ++i)
                        width[i] = max(width[i], r[i].size());
                    rows.emplace_back(r);
                }
            });
        print_table(cout, head, width, rows);
    }
    void
    depth(Async& async, Arg begin, Arg end)
    {
        const string cmnem = get("contr");
        const auto settl_day = dbr_iso_to_jd(ston<DbrIsoDate>(get("settl_date")));

        enum {
            BID_PRICE = 3,
            BID_LOTS,
            BID_COUNT,
            OFFER_PRICE,
            OFFER_LOTS,
            OFFER_COUNT,
            COLS
        };
        typedef array<string, COLS> row;

        const array<const char*, COLS> head{
            "<contr",
            "<settl_date",
            ">level",
            ">bid_price",
            ">bid_lots",
            ">bid_count",
            ">offer_price",
            ">offer_lots",
            ">offer_count"};
        auto width = head_width(head);
        vector<row> rows;

        call(async, [cmnem, settl_day, &width, &rows](ClntRef clnt) {
                auto crec = get_crec(clnt, cmnem.c_str());
                auto it = clnt.views().find(crec.id(), settl_day);
                if (it == clnt.views().end())
                    return;

                ViewRef ref(*it);
                for (size_t i = 0; i < DBR_LEVEL_MAX; ++i) {
                    row r{
                        to_string(ref.crec().mnem()),
                        to_string(dbr_jd_to_iso(ref.settl_day())),
                        to_string(i),
                        "-",
                        "-",
                        "-",
                        "-",
                        "-",
                        "-",
                    };
                    if (ref.bid_count(i) > 0) {
                        r[BID_PRICE] = to_string(ref.crec().ticks_to_price(ref.bid_ticks(i)));
                        r[BID_LOTS] = to_string(ref.bid_lots(i));
                        r[BID_COUNT] = to_string(ref.bid_count(i));
                    }
                    if (ref.offer_count(i) > 0) {
                        r[OFFER_PRICE] = to_string(ref.crec().ticks_to_price(ref.offer_ticks(i)));
                        r[OFFER_LOTS] = to_string(ref.offer_lots(i));
                        r[OFFER_COUNT] = to_string(ref.offer_count(i));
                    }
                    for (size_t i = 0; i < COLS; ++i)
                        width[i] = max(width[i], r[i].size());
                    rows.emplace_back(r);
                }
            });
        print_table(cout, head, width, rows);
    }
    void
    buy(Async& async, Arg begin, Arg end)
    {
        const string umnem = get("user");
        const string gmnem = get("group");
        const string cmnem = get("contr");
        const auto settl_day = dbr_iso_to_jd(ston<DbrIsoDate>(get("settl_date")));
        const auto price = ston<double>(*begin++);
        const auto lots = ston<DbrLots>(*begin++);
        call(async, [umnem, gmnem, cmnem, settl_day, price, lots](ClntRef clnt) {
                auto urec = get_arec(clnt, umnem.c_str());
                auto grec = get_arec(clnt, gmnem.c_str());
                auto crec = get_crec(clnt, cmnem.c_str());
                return clnt.place(clnt.accnt(urec), clnt.accnt(grec), crec, settl_day,
                                  nullptr, DBR_ACTION_BUY, crec.price_to_ticks(price), lots, 0);
            });
    }
    void
    sell(Async& async, Arg begin, Arg end)
    {
        const string umnem = get("user");
        const string gmnem = get("group");
        const string cmnem = get("contr");
        const auto settl_day = dbr_iso_to_jd(ston<DbrIsoDate>(get("settl_date")));
        const auto price = ston<double>(*begin++);
        const auto lots = ston<DbrLots>(*begin++);
        call(async, [umnem, gmnem, cmnem, settl_day, price, lots](ClntRef clnt) {
                auto urec = get_arec(clnt, umnem.c_str());
                auto grec = get_arec(clnt, gmnem.c_str());
                auto crec = get_crec(clnt, cmnem.c_str());
                return clnt.place(clnt.accnt(urec), clnt.accnt(grec), crec, settl_day,
                                  nullptr, DBR_ACTION_SELL, crec.price_to_ticks(price), lots, 0);
            });
    }
    void
    revise(Async& async, Arg begin, Arg end)
    {
        const string umnem = get("user");
        const auto id = ston<DbrIden>(*begin++);
        const auto lots = ston<DbrLots>(*begin++);
        call(async, [umnem, id, lots](ClntRef clnt) {
                auto urec = get_arec(clnt, umnem.c_str());
                clnt.revise(clnt.accnt(urec), id, lots);
            });
    }
    void
    cancel(Async& async, Arg begin, Arg end)
    {
        const string umnem = get("user");
        vector<DbrIden> ids;
        transform(begin, end, back_inserter(ids), ston<DbrIden>);
        call(async, [umnem, ids](ClntRef clnt) {
                auto urec = get_arec(clnt, umnem.c_str());
                auto user = clnt.accnt(urec);
                for (auto id : ids)
                    clnt.cancel(user, id);
            });
    }
    void
    ack(Async& async, Arg begin, Arg end)
    {
        const string umnem = get("user");
        vector<DbrIden> ids;
        transform(begin, end, back_inserter(ids), ston<DbrIden>);
        call(async, [umnem, ids](ClntRef clnt) {
                auto urec = get_arec(clnt, umnem.c_str());
                auto user = clnt.accnt(urec);
                for (auto id : ids)
                    clnt.ack_trade(user, id);
            });
    }
    void
    echo(Async& async, Arg begin, Arg end)
    {
        copy(begin, end, JoinIterator<string>(cout, " "));
        cout << endl;
    }
    void
    penv(Async& async, Arg begin, Arg end)
    {
        enum { COLS = 2 };
        typedef array<string, COLS> row;

        const array<const char*, COLS> head{
            "<name",
            "<value"};
        auto width = head_width(head);
        vector<row> rows;
        for (auto nv : env_) {
            row r{nv.first, nv.second};
            for (size_t i = 0; i < COLS; ++i)
                width[i] = max(width[i], r[i].size());
            rows.emplace_back(r);
        }
        print_table(cout, head, width, rows);
    }
    void
    set(Async& async, Arg begin, Arg end)
    {
        const string& name = *begin++;
        const string& value = *begin++;
        env_[name] = value;
    }
    void
    unset(Async& async, Arg begin, Arg end)
    {
        const string& name = *begin++;
        env_.erase(name);
    }
    void
    quit(Async& async, Arg begin, Arg end)
    {
        async.close();
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
        repl.bind("top", bind(&Handler::top, ref(handler), ref(async), _1, _2), 0);
        repl.bind("depth", bind(&Handler::depth, ref(handler), ref(async), _1, _2), 0);
        repl.bind("buy", bind(&Handler::buy, ref(handler), ref(async), _1, _2), 2);
        repl.bind("sell", bind(&Handler::sell, ref(handler), ref(async), _1, _2), 2);
        repl.bind("revise", bind(&Handler::revise, ref(handler), ref(async), _1, _2), 2);
        repl.bind("cancel", bind(&Handler::cancel, ref(handler), ref(async), _1, _2), -1);
        repl.bind("ack", bind(&Handler::ack, ref(handler), ref(async), _1, _2), -1);
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
        repl.read(cin, "doobry> ");
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
