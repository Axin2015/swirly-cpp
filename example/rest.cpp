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
#include <dbrpp/rest.hpp>

#include <iostream>

using namespace dbr;
using namespace std;

int
main(int argc, char* argv[])
{
    try {
        Rest rest;
        cout << "rest> ";

        string line;
        while (getline(cin, line)) {
            if (!rest.json(line.data(), line.size()))
                continue;

            switch (rest.fields()) {
            case DBR_METHOD_POST | DBR_RESRC_LOGON | DBR_PARAM_ACCNT:
                cout << "post logon: " << rest.accnt() << endl;
                break;
            case DBR_METHOD_POST | DBR_RESRC_LOGOFF | DBR_PARAM_ACCNT:
                cout << "post logoff: " << rest.accnt() << endl;
                break;
            case DBR_METHOD_GET | DBR_RESRC_ACCNT:
                cout << "get accnt" << endl;
                break;
            case DBR_METHOD_GET | DBR_RESRC_ACCNT | DBR_PARAM_ACCNT:
                cout << "get accnt: " << rest.accnt() << endl;
                break;
            case DBR_METHOD_GET | DBR_RESRC_CONTR:
                cout << "get contr" << endl;
                break;
            case DBR_METHOD_GET | DBR_RESRC_CONTR | DBR_PARAM_CONTR:
                cout << "get contr: " << rest.contr() << endl;
                break;
            default:
                cerr << "invalid request\n";
                break;
            }

            rest.reset();
            cout << "rest> ";
        }
        return 0;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
