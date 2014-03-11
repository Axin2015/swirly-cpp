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
#include <dbrpp/exec.hpp>
#include <dbrpp/pool.hpp>
#include <dbrpp/posn.hpp>
#include <dbrpp/serv.hpp>

#include <dbrpp/sqljourn.hpp>
#include <dbrpp/sqlmodel.hpp>

using namespace dbr;
using namespace std;

int
main(int argc, char* argv[])
{
    cout.sync_with_stdio(true);
    cerr.sync_with_stdio(true);
    try {
        Pool pool(8 * 1024 * 1024);
        auto journ = sqljourn_create("doobry.db");
        auto model = sqlmodel_create("doobry.db");
        Serv serv("doobry.bin", journ.get(), pool);
        serv.load(model.get());

        cout << "traders:\n";
        for (auto rec : serv.trecs()) {
            TraderRecRef ref(rec);
            cout << ref << endl;
            cout << ref.mnem() << " orders:" << endl;
            Trader trader = serv.trader(ref);
            for (auto ref : trader.orders())
                cout << OrderRef(ref) << endl;
            cout << ref.mnem() << " trades:" << endl;
            for (auto ref : trader.trades())
                cout << ExecRef(ref) << endl;
        }

        cout << "accnts:\n";
        for (auto rec : serv.arecs()) {
            AccntRecRef ref(rec);
            cout << ref << endl;
            Accnt accnt = serv.accnt(ref);
            cout << ref.mnem() << " posns:" << endl;
            for (auto ref : accnt.posns())
                cout << PosnRef(ref) << endl;
        }

        cout << "contrs:\n";
        for (auto rec : serv.crecs()) {
            ContrRecRef ref(rec);
            cout << ref << endl;
            serv.book(ref, 20130827);
        }
        return 0;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
