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
#include <swirly/sqlite/Model.hpp>

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
            config.set("sqlite_model", argv[1]);
        }
        const BusinessDay busDay{MarketZone};

        const auto now = UnixClock::now();
        SqlModel model{config};
        {
            ofstream os{"asset.txt"};
            os << "id\tsymbol\tdisplay\ttype\n";
            model.readAsset([&os](auto ptr) {
                ptr->toDsv(os, '\t');
                os << '\n';
            });
        }
        {
            ofstream os{"instr.txt"};
            os << "id\tsymbol\tdisplay\tbase_asset\tterm_ccy\tlot_numer\tlot_denom\ttick_numer\t"
                  "tick_denom\tpip_dp\tmin_lots\tmax_lots\n";
            model.readInstr([&os](auto ptr) {
                ptr->toDsv(os, '\t');
                os << '\n';
            });
        }
        {
            ofstream os{"market.txt"};
            os << "id\tinstr\tsettl_day\tstate\tlast_lots\tlast_ticks\tlast_time\tmax_id\n";
            model.readMarket([&os](auto ptr) {
                ptr->toDsv(os, '\t');
                os << '\n';
            });
        }
        {
            ofstream os{"order.txt"};
            os << "accnt\tmarket_id\tinstr\tsettl_date\tid\tref\tstate\tside\tlots\tticks\t"
                  "resd_lots\texec_lots\texec_cost\tlast_lots\tlast_ticks\tmin_lots\tcreated\t"
                  "modified\n";
            model.readOrder([&os](auto ptr) {
                ptr->toDsv(os, '\t');
                os << '\n';
            });
        }
        {
            ofstream os{"exec.txt"};
            os << "accnt\tmarket_id\tinstr\tsettl_date\tid\torder_id\tref\tstate\tside\tlots\t"
                  "ticks\tresd_lots\texec_lots\texec_cost\tlast_lots\tlast_ticks\tmin_lots\t"
                  "match_id\tposn_lots\tposn_cost\tliq_ind\tcpty\tcreated\n";
            // One week ago.
            model.readExec(now - 604800000ms, [&os](auto ptr) {
                ptr->toDsv(os, '\t');
                os << '\n';
            });
        }
        {
            ofstream os{"trade.txt"};
            os << "accnt\tmarket_id\tinstr\tsettl_date\tid\torder_id\tref\tstate\tside\tlots\t"
                  "ticks\tresd_lots\texec_lots\texec_cost\tlast_lots\tlast_ticks\tmin_lots\t"
                  "match_id\tposn_lots\tposn_cost\tliq_ind\tcpty\tcreated\n";
            model.readTrade([&os](auto ptr) {
                ptr->toDsv(os, '\t');
                os << '\n';
            });
        }
        {
            ofstream os{"posn.txt"};
            os << "accnt\tmarket_id\tinstr\tsettl_date\tbuy_lots\tbuy_cost\tsell_lots\tsell_cost\n";
            model.readPosn(busDay(now), [&os](auto ptr) {
                ptr->toDsv(os, '\t');
                os << '\n';
            });
        }

        ret = 0;
    } catch (const exception& e) {
        cerr << "exception: " << e.what() << endl;
    }
    return ret;
}
