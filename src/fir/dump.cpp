#include <dbrpp/ctx.hpp>
#include <dbrpp/pool.hpp>
#include <dbrpp/sqlite.hpp>

#include <iostream>

using namespace dbr;
using namespace std;

int
main(int argc, char* argv[])
{
    try {
        Pool pool;
        Sqlite sqlite(pool, 1, "doobry.db");
        Ctx ctx(pool, sqlite.journ(), sqlite.model());
        return 0;
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }
    return 1;
}
