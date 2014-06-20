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
#include <dbrpp/fig/serv.hpp>

#include <dbrpp/elm/json.hpp>
#include <dbrpp/elm/pool.hpp>
#include <dbrpp/elm/posn.hpp>

#include <dbrpp/fir/sqljourn.hpp>
#include <dbrpp/fir/sqlmodel.hpp>

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

        cout << "accnts:\n";
        for (auto rec : serv.arecs()) {
            AccntRecRef ref(rec);
            cout << json_write_accnt(rec) << endl;
            Accnt accnt = serv.accnt(ref);
            cout << ref.mnem() << " users:" << endl;
            for (auto ref : accnt.users())
                cout << json_write_memb(ref) << endl;
            cout << ref.mnem() << " groups:" << endl;
            for (auto ref : accnt.groups())
                cout << json_write_memb(ref) << endl;
            cout << ref.mnem() << " orders:" << endl;
            for (auto ref : accnt.orders())
                cout << json_write_order(ref) << endl;
            cout << ref.mnem() << " trades:" << endl;
            for (auto ref : accnt.trades())
                cout << json_write_exec(ref) << endl;
            cout << ref.mnem() << " posns:" << endl;
            for (auto ref : accnt.posns())
                cout << json_write_posn(ref) << endl;
        }

        cout << "contrs:\n";
        for (auto rec : serv.crecs()) {
            cout << json_write_contr(rec) << endl;
        }
        return 0;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
