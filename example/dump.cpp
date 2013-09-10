#include <dbrpp/exch.hpp>
#include <dbrpp/pool.hpp>
#include <dbrpp/sqlite.hpp>

using namespace dbr;
using namespace std;

int
main(int argc, char* argv[])
{
    cout.sync_with_stdio(true);
    cerr.sync_with_stdio(true);
    try {
        Sqlite sqlite(1, "doobry.db");
        Pool pool;
        Exch exch(sqlite.journ(), sqlite.model(), pool);

        cout << "traders:\n";
        for (auto rec : exch.trecs()) {
            TraderRecRef ref(rec);
            cout << ref << endl;
            cout << ref.mnem() << " orders:" << endl;
            Trader trader = exch.trader(ref);
            for (auto ref : trader.orders())
                cout << OrderRef(ref) << endl;
        }

        cout << "accnts:\n";
        for (auto rec : exch.arecs()) {
            AccntRecRef ref(rec);
            cout << ref << endl;
            cout << ref.mnem() << " trades:" << endl;
            Accnt accnt = exch.accnt(ref);
            for (auto ref : accnt.trades())
                cout << TradeRef(ref) << endl;
            cout << ref.mnem() << " posns:" << endl;
            for (auto ref : accnt.posns())
                cout << PosnRef(ref) << endl;
        }

        cout << "contrs:\n";
        for (auto rec : exch.crecs()) {
            ContrRecRef ref(rec);
            cout << ref << endl;
            exch.book(ref, 20130827);
        }
        return 0;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
