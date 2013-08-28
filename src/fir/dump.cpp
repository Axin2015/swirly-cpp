#include <dbrpp/ctx.hpp>
#include <dbrpp/pool.hpp>
#include <dbrpp/sqlite.hpp>

using namespace dbr;
using namespace std;

int
main(int argc, char* argv[])
{
    try {
        Pool pool;
        Sqlite sqlite(pool, 1, "doobry.db");
        Ctx ctx(pool, sqlite.journ(), sqlite.model());

        cout << "contrs:\n";
        for (auto ref : ctx.crecs()) {
            ContrRec rec(ref);
            cout << rec << endl;
            ctx.book(rec, 20130827);
        }

        cout << "traders:\n";
        for (auto ref : ctx.trecs()) {
            TraderRec rec(ref);
            cout << rec << endl;
            cout << rec.mnem() << " orders:" << endl;
            Trader trader = ctx.trader(rec);
            for (auto ref : trader.orders())
                cout << Order(ref) << endl;
        }

        cout << "accnts:\n";
        for (auto ref : ctx.arecs()) {
            AccntRec rec(ref);
            cout << rec << endl;
            cout << rec.mnem() << " trades:" << endl;
            Accnt accnt = ctx.accnt(rec);
            for (auto ref : accnt.trades())
                cout << Trade(ref) << endl;
            cout << rec.mnem() << " posns:" << endl;
            for (auto ref : accnt.posns())
                cout << Posn(ref) << endl;
        }

        return 0;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
