/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
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
