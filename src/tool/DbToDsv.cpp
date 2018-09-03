/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#include <swirly/db/SqliteModel.hpp>

#include <swirly/fin/Asset.hpp>
#include <swirly/fin/Date.hpp>
#include <swirly/fin/Exec.hpp>
#include <swirly/fin/Instr.hpp>
#include <swirly/fin/Market.hpp>
#include <swirly/fin/Model.hpp>
#include <swirly/fin/Order.hpp>
#include <swirly/fin/Posn.hpp>

#include <swirly/util/Config.hpp>
#include <swirly/util/Stream.hpp>

#include <swirly/util/Time.hpp>

#include <fstream>

using namespace std;
using namespace swirly;

int main(int argc, char* argv[])
{
    int ret = 1;
    try {

        Config config;
        if (argc > 1) {
            config.set("db_type", "sqlite");
            config.set("db_name", argv[1]);
        }
        const BusinessDay bus_day{MarketZone};

        const auto now = UnixClock::now();
        SqliteModel model{config};
        {
            ofstream os{"asset.txt"};
            os << "id\tsymbol\tdisplay\ttype\n";
            model.read_asset([&os](auto ptr) {
                ptr->to_dsv(os, '\t');
                os << '\n';
            });
        }
        {
            ofstream os{"instr.txt"};
            os << "id\tsymbol\tdisplay\tbase_asset\tterm_ccy\tlot_numer\tlot_denom\ttick_numer\t"
                  "tick_denom\tpip_dp\tmin_lots\tmax_lots\n";
            model.read_instr([&os](auto ptr) {
                ptr->to_dsv(os, '\t');
                os << '\n';
            });
        }
        {
            ofstream os{"market.txt"};
            os << "id\tinstr\tsettl_day\tstate\tlast_time\tlast_lots\tlast_ticks\tmax_id\n";
            model.read_market([&os](auto ptr) {
                ptr->to_dsv(os, '\t');
                os << '\n';
            });
        }
        {
            ofstream os{"order.txt"};
            os << "created\tmodified\taccnt\tmarket_id\tinstr\tsettl_date\tid\tref\tstate\tside\t"
                  "lots\tticks\tresd_lots\texec_lots\texec_cost\tlast_lots\tlast_ticks\tmin_lots\n";
            model.read_order([&os](auto ptr) {
                ptr->to_dsv(os, '\t');
                os << '\n';
            });
        }
        {
            ofstream os{"exec.txt"};
            os << "created\taccnt\tmarket_id\tinstr\tsettl_date\tid\torder_id\tref\tstate\tside\t"
                  "lots\tticks\tresd_lots\texec_lots\texec_cost\tlast_lots\tlast_ticks\tmin_lots\t"
                  "match_id\tposn_lots\tposn_cost\tliq_ind\tcpty\n";
            // One week ago.
            model.read_exec(now - 604800000ms, [&os](auto ptr) {
                ptr->to_dsv(os, '\t');
                os << '\n';
            });
        }
        {
            ofstream os{"trade.txt"};
            os << "created\taccnt\tmarket_id\tinstr\tsettl_date\tid\torder_id\tref\tstate\tside\t"
                  "lots\tticks\tresd_lots\texec_lots\texec_cost\tlast_lots\tlast_ticks\tmin_lots\t"
                  "match_id\tposn_lots\tposn_cost\tliq_ind\tcpty\n";
            model.read_trade([&os](auto ptr) {
                ptr->to_dsv(os, '\t');
                os << '\n';
            });
        }
        {
            ofstream os{"posn.txt"};
            os << "accnt\tmarket_id\tinstr\tsettl_date\tbuy_lots\tbuy_cost\tsell_lots\tsell_cost\n";
            model.read_posn(bus_day(now), [&os](auto ptr) {
                ptr->to_dsv(os, '\t');
                os << '\n';
            });
        }

        ret = 0;
    } catch (const exception& e) {
        cerr << "exception: " << e.what() << endl;
    }
    return ret;
}
