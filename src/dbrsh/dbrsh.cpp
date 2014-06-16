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
#include <dbrpp/mutex.hpp>
#include <dbrpp/posn.hpp>
#include <dbrpp/shlex.hpp>
#include <dbrpp/string.hpp>
#include <dbrpp/view.hpp>

#include <dbr/log.h>
#include <dbr/util.h> // dbr_millis()

#include <algorithm>
#include <array>
#include <climits>    // PATH_MAX
#include <functional>
#include <fstream>
#include <iomanip>
#include <list>
#include <map>

#include <unistd.h>   // usleep()

// Suppress complaints by Clang 5.1.0.
#pragma GCC diagnostic ignored "-Wmissing-braces"
#pragma GCC diagnostic ignored "-Wunused-function"

using namespace dbr;
using namespace std;
using namespace std::placeholders;

namespace {

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

typedef int Arity;
typedef vector<string>::const_iterator Arg;

// Read-eval-print loop (REPL).

class Repl {
    typedef pair<function<void (Arg, Arg)>, Arity> Cmd;
    typedef map<string, Cmd> Cmds;
    Cmds cmds_;
    function<vector<string> (const string&)> predict_;
    function<void ()> flush_;
    bool quit_;
public:
    Repl(const function<vector<string> (const string&)>& predict,
         const function<void ()>& flush) noexcept
      : predict_(predict),
        flush_(flush),
        quit_(false)
    {
    }
    void
    bind(const string& name, const function<void (Arg, Arg)>& fun, Arity arity)
    {
        cmds_[name] = {fun, arity};
    }
    void
    eval(const string& abbrev, Arg begin, Arg end)
    {
        vector<string> names = predict_(abbrev);
        const auto size = names.size();
        if (size != 1) {
            cerr << "invalid command: " << abbrev << endl;
            if (size > 1) {
                cerr << "did you mean? ";
                copy(names.begin(), names.end(), ostream_iterator<string>(cerr, " "));
                cerr << endl;
            }
            return;
        }

        const string& name = names.front();
        const auto& cmd = cmds_[name];
        // Remaining tokens are the arguments.
        const Arity arity = cmd.second;
        if (arity < 0 || arity == distance(begin, end)) {
            try {
                cmd.first(begin, end);
            } catch (const DbrException& e) {
                cerr << name << ": exception: " << e.what() << endl;
            } catch (const InvalidArgument& e) {
                cerr << name << ": invalid argument: " << e.what() << endl;
            } catch (const InvalidState& e) {
                cerr << name << ": invalid state: " << e.what() << endl;
            } catch (const Quit&) {
                quit_ = true;
            }
        } else
            cerr << name << ": invalid argument(s): " << join(begin, end) << endl;
    }
    void
    eval(const vector<string>& toks)
    {
        if (!toks.empty()) {
            Arg begin = toks.begin();
            const string& abbrev = *begin++;
            eval(abbrev, begin, toks.end());
        }
    }
    void
    read(istream& is, const char* prompt = nullptr)
    {
        vector<string> toks;
        auto shlex = make_shlex([this, prompt, &toks](const char* tok, size_t len) {
                if (tok)
                    toks.push_back(tok);
                else {
                    eval(toks);
                    toks.clear();
                    // Wait 25ms for response when running interactively.
                    if (prompt)
                        usleep(25 * 1000);
                    flush_();
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
action_str(int action) noexcept
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
role_str(int role) noexcept
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
state_str(int state) noexcept
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
        dbr_log_info("close received");
    }
    void
    on_ready(ClntRef clnt) noexcept
    {
        dbr_log_info("ready received");
    }
    void
    on_logon(ClntRef clnt, DbrIden req_id, DbrIden uid) noexcept
    {
        dbr_log_info("logon received");
    }
    void
    on_logoff(ClntRef clnt, DbrIden req_id, DbrIden uid) noexcept
    {
        dbr_log_info("logoff received");
    }
    void
    on_reset(ClntRef clnt) noexcept
    {
        dbr_log_info("reset received");
    }
    void
    on_timeout(ClntRef clnt, DbrIden req_id) noexcept
    {
        dbr_log_info("timeout received");
    }
    void
    on_status(ClntRef clnt, DbrIden req_id, int num, const char* msg) noexcept
    {
        dbr_log_info("status received");
    }
    void
    on_exec(ClntRef clnt, DbrIden req_id, DbrExec& exec) noexcept
    {
        dbr_log_info("exec received");
    }
    void
    on_posn(ClntRef clnt, DbrPosn& posn) noexcept
    {
        dbr_log_info("posn received");
    }
    void
    on_view(ClntRef clnt, DbrView& view) noexcept
    {
        dbr_log_info("view received");
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
            dbr_log_error("exception: {}", e.what());
        } catch (const InvalidArgument& e) {
            dbr_log_error("invalid argument: {}", e.what());
        } catch (const InvalidState& e) {
            dbr_log_error("invalid state: {}", e.what());
        }
        return nullptr;
    }
    // This.
    void
    accnt(Async& async, Arg begin, Arg end)
    {
        enum { COLS = 3 };
        typedef array<string, COLS> row;

        const array<const char*, COLS> head = {
            "<mnem",
            "<display",
            "<email"
        };
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

        const array<const char*, COLS> head = {
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
            ">max_lots"
        };
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

        const array<const char*, COLS> head = {
            "<user",
            "<group"
        };
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

        const array<const char*, COLS> head = {
            "<user",
            "<group"
        };
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

        const array<const char*, COLS> head = {
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
            ">last_lots"
        };
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
                        state_str(ref.state()),
                        action_str(ref.action()),
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

        const array<const char*, COLS> head = {
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
            "<cpty"
        };
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
                        state_str(ref.state()),
                        action_str(ref.action()),
                        to_string(ref.crec().ticks_to_price(ref.ticks())),
                        to_string(ref.lots()),
                        to_string(ref.resd()),
                        to_string(ref.exec()),
                        to_string(ref.crec().ticks_to_price(ref.last_ticks())),
                        to_string(ref.last_lots()),
                        role_str(ref.role()),
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

        const array<const char*, COLS> head = {
            "<contr",
            "<settl_date",
            ">buy_price",
            ">buy_lots",
            ">sell_price",
            ">sell_lots"
        };
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

        const array<const char*, COLS> head = {
            "<contr",
            "<settl_date",
            ">bid_price",
            ">bid_lots",
            ">bid_count",
            ">offer_price",
            ">offer_lots",
            ">offer_count"
        };
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
        const auto settl_day = dbr_iso_to_jd(ston<DbrIsoDate>(get("settl_date").c_str()));

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

        const array<const char*, COLS> head = {
            "<contr",
            "<settl_date",
            ">level",
            ">bid_price",
            ">bid_lots",
            ">bid_count",
            ">offer_price",
            ">offer_lots",
            ">offer_count"
        };
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
        const auto settl_day = dbr_iso_to_jd(ston<DbrIsoDate>(get("settl_date").c_str()));
        const auto price = ston<double>((*begin++).c_str());
        const auto lots = ston<DbrLots>((*begin++).c_str());
        call(async, [umnem, gmnem, cmnem, settl_day, price, lots](ClntRef clnt) {
                auto urec = get_arec(clnt, umnem.c_str());
                auto grec = get_arec(clnt, gmnem.c_str());
                auto crec = get_crec(clnt, cmnem.c_str());
                clnt.place(clnt.accnt(urec), clnt.accnt(grec), crec, settl_day,
                           nullptr, DBR_ACTION_BUY, crec.price_to_ticks(price), lots, 0);
            });
    }
    void
    sell(Async& async, Arg begin, Arg end)
    {
        const string umnem = get("user");
        const string gmnem = get("group");
        const string cmnem = get("contr");
        const auto settl_day = dbr_iso_to_jd(ston<DbrIsoDate>(get("settl_date").c_str()));
        const auto price = ston<double>((*begin++).c_str());
        const auto lots = ston<DbrLots>((*begin++).c_str());
        call(async, [umnem, gmnem, cmnem, settl_day, price, lots](ClntRef clnt) {
                auto urec = get_arec(clnt, umnem.c_str());
                auto grec = get_arec(clnt, gmnem.c_str());
                auto crec = get_crec(clnt, cmnem.c_str());
                clnt.place(clnt.accnt(urec), clnt.accnt(grec), crec, settl_day,
                           nullptr, DBR_ACTION_SELL, crec.price_to_ticks(price), lots, 0);
            });
    }
    void
    revise(Async& async, Arg begin, Arg end)
    {
        const string umnem = get("user");
        const auto id = ston<DbrIden>((*begin++).c_str());
        const auto lots = ston<DbrLots>((*begin++).c_str());
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
        transform(begin, end, back_inserter(ids), [](const string& arg) {
                return ston<DbrIden>(arg.c_str());
            });
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
        transform(begin, end, back_inserter(ids), [](const string& arg) {
                return ston<DbrIden>(arg.c_str());
            });
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

        const array<const char*, COLS> head = {
            "<name",
            "<value"
        };
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
        throw Quit();
    }
};

class LogBuffer {
    typedef tuple<DbrMillis, int, string> Entry;
    Mutex mutex_;
    list<Entry> entries_;
    void
    print(ostream& os, const Entry& entry) const
    {
        DbrMillis ms;
        int level;
        string msg;

        tie(ms, level, msg) = entry;

        const time_t now = ms / 1000;
        struct tm tm;
        localtime_r(&now, &tm);

        char buf[sizeof("Mar 12 06:26:39")];
        strftime(buf, sizeof(buf), "%b %d %H:%M:%S", &tm);

        os << buf << '.';

        auto fill = os.fill('0');
        os.width(3);
        os << (ms % 1000);
        os.fill(fill);

        os << ' ';

        auto flags = os.flags();
        os.setf(ios_base::left, ios_base::adjustfield);
        os.width(6);
        os << dbr_log_label(level);
        os.flags(flags);

        os << msg << endl;
    }
public:
    ~LogBuffer() noexcept
    {
        flush();
    }
    void
    append(int level, const char* msg)
    {
        Lock l(mutex_);
        entries_.emplace_back(dbr_millis(), level, msg);
    }
    void
    flush()
    {
        list<Entry> entries;
        {
            Lock l(mutex_);
            entries.swap(entries_);
        }
        for (const auto& entry : entries)
            print(cout, entry);
    }
} log_buffer;

void
log_ios(int level, const char* msg)
{
    log_buffer.append(level, msg);
}

size_t
prefix(string::const_iterator it, string::const_iterator end, const char* s)
{
    const char* p = s;
    while (it != end && *it == *p)
        ++it, ++p;
    return it == end ? p - s : 0;
}

vector<string>
match(string::const_iterator it, string::const_iterator end, const char* s1)
{
    vector<string> v;
    const auto p1 = prefix(it, end, s1);
    if (p1 != 0)
        v.emplace_back(s1);
    return v;
}

vector<string>
match(string::const_iterator it, string::const_iterator end, const char* s1, const char* s2)
{
    vector<string> v;
    const auto p1 = prefix(it, end, s1);
    const auto p2 = prefix(it, end, s2);
    if (p1 != 0 || p2 != 0) {
        if (p1 >= p2)
            v.emplace_back(s1);
        if (p2 >= p1)
            v.emplace_back(s2);
    }
    return v;
}

vector<string>
predict(const string& tok)
{
    vector<string> v;
    switch (tok[0]) {
    case 'a':
        v = match(tok.begin(), tok.end(), "accnt", "ack");
        break;
    case 'b':
        v = match(tok.begin(), tok.end(), "buy");
        break;
    case 'c':
        v = match(tok.begin(), tok.end(), "cancel", "contr");
        break;
    case 'd':
        v = match(tok.begin(), tok.end(), "depth");
        break;
    case 'e':
        v = match(tok.begin(), tok.end(), "echo");
        break;
    case 'g':
        v = match(tok.begin(), tok.end(), "group");
        break;
    case 'l':
        v = match(tok.begin(), tok.end(), "logoff", "logon");
        break;
    case 'o':
        v = match(tok.begin(), tok.end(), "order");
        break;
    case 'p':
        v = match(tok.begin(), tok.end(), "penv", "posn");
        break;
    case 'q':
        v = match(tok.begin(), tok.end(), "quit");
        break;
    case 'r':
        v = match(tok.begin(), tok.end(), "revise");
        break;
    case 's':
        v = match(tok.begin(), tok.end(), "sell", "set");
        break;
    case 't':
        v = match(tok.begin(), tok.end(), "top", "trade");
        break;
    case 'u':
        v = match(tok.begin(), tok.end(), "unset", "user");
        break;
    }
    return v;
}

}

int
main(int argc, char* argv[])
{
    dbr_log_setlevel(DBR_LOG_INFO);
    dbr_log_setlogger(log_ios);
    try {
        Handler handler;
        Ctx ctx("tcp://localhost:3270", "tcp://localhost:3271", 5000,
                8 * 1024 * 1024, &handler);

        Async async = ctx.async();

        Repl repl(predict, bind(&LogBuffer::flush, ref(log_buffer)));
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
        sprintf(path, "%s/.dbrshrc", getenv("HOME"));
        ifstream cfg(path);
        if (cfg.is_open())
            repl.read(cfg);
        repl.read(cin, "dbrsh> ");
        cout << endl;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
