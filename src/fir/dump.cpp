#include <dbrpp/ctx.hpp>
#include <dbrpp/pool.hpp>
#include <dbrpp/sqlite.hpp>

using namespace dbr;
using namespace std;

int
main(int argc, char* argv[])
{
    try {
        Sqlite sqlite(1, "doobry.db");
        Pool pool;
        Ctx ctx(sqlite.journ(), sqlite.model(), pool);

        cout << "contrs:\n";
        for (auto ref : ctx.crecs()) {
            ContrRecRef rec(ref);
            cout << rec << endl;
            ctx.book(rec, 20130827);
        }

        cout << "traders:\n";
        for (auto ref : ctx.trecs()) {
            TraderRecRef rec(ref);
            cout << rec << endl;
            cout << rec.mnem() << " orders:" << endl;
            Trader trader = ctx.trader(rec);
            for (auto ref : trader.orders())
                cout << OrderRef(ref) << endl;
        }

        cout << "accnts:\n";
        for (auto ref : ctx.arecs()) {
            AccntRecRef rec(ref);
            cout << rec << endl;
            cout << rec.mnem() << " trades:" << endl;
            Accnt accnt = ctx.accnt(rec);
            for (auto ref : accnt.trades())
                cout << TradeRef(ref) << endl;
            cout << rec.mnem() << " posns:" << endl;
            for (auto ref : accnt.posns())
                cout << PosnRef(ref) << endl;
        }

        return 0;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
