#include <dbrpp/clnt.hpp>
#include <dbrpp/pool.hpp>
#include <dbrpp/zmqctx.hpp>

#include <zmq.h>

#include <iostream>

using namespace dbr;
using namespace std;

int
main(int argc, char* argv[])
{
    cout.sync_with_stdio(true);
    cerr.sync_with_stdio(true);
    try {
        ZmqCtx ctx;
        Pool pool;
        Clnt clnt(ctx.c_arg(), "tcp://localhost:3272", "WRAMIREZ", pool);

        cout << "traders:\n";
        for (auto rec : clnt.trecs()) {
            TraderRecRef ref(rec);
            cout << ref << endl;
        }
        cout << "accnts:\n";
        for (auto rec : clnt.arecs()) {
            AccntRecRef ref(rec);
            cout << ref << endl;
        }
        cout << "contrs:\n";
        for (auto rec : clnt.crecs()) {
            ContrRecRef ref(rec);
            cout << ref << endl;
        }
        return 0;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
