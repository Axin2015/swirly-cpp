/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <scpp/fig/serv.hpp>

#include <scpp/elm/json.hpp>
#include <scpp/elm/pool.hpp>
#include <scpp/elm/posn.hpp>

#include <scpp/fir/sqljourn.hpp>
#include <scpp/fir/sqlmodel.hpp>

using namespace sc;
using namespace std;

int
main(int argc, char* argv[])
{
    cout.sync_with_stdio(true);
    cerr.sync_with_stdio(true);
    try {
        Pool pool(8 * 1024 * 1024);
        auto journ = sqljourn_create("twirly.db");
        auto model = sqlmodel_create("twirly.db");
        Serv serv("twirly.bin", journ.get(), pool);
        serv.load(model.get());

        cout << "accnts:\n";
        for (auto rec : serv.arecs()) {
            AccntRecRef ref(rec);
            cout << json_write_accnt(rec) << endl;
            Accnt accnt = serv.accnt(ref);
            cout << ref.mnem() << " traders:" << endl;
            for (auto ref : accnt.traders())
                cout << json_write_perm(ref) << endl;
            cout << ref.mnem() << " giveups:" << endl;
            for (auto ref : accnt.giveups())
                cout << json_write_perm(ref) << endl;
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
