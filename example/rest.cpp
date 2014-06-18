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
                cout << "post logon: accnt=" << rest.accnt() << endl;
                break;
            case DBR_METHOD_POST | DBR_RESRC_LOGOFF | DBR_PARAM_ACCNT:
                cout << "post logoff: accnt=" << rest.accnt() << endl;
                break;
            case DBR_METHOD_GET | DBR_RESRC_ACCNT:
                cout << "get accnt" << endl;
                break;
            case DBR_METHOD_GET | DBR_RESRC_ACCNT | DBR_PARAM_ACCNT:
                cout << "get accnt: mnem=" << rest.accnt() << endl;
                break;
            case DBR_METHOD_GET | DBR_RESRC_CONTR:
                cout << "get contr" << endl;
                break;
            case DBR_METHOD_GET | DBR_RESRC_CONTR | DBR_PARAM_CONTR:
                cout << "get contr: mnem=" << rest.contr() << endl;
                break;
            case DBR_METHOD_GET | DBR_RESRC_USER | DBR_PARAM_ACCNT:
                cout << "get user: accnt=" << rest.accnt() << endl;
                break;
            case DBR_METHOD_GET | DBR_RESRC_GROUP | DBR_PARAM_ACCNT:
                cout << "get group: accnt=" << rest.accnt() << endl;
                break;
            case DBR_METHOD_DELETE | DBR_RESRC_ORDER | DBR_PARAM_ACCNT | DBR_PARAM_ID:
                cout << "delete order: accnt=" << rest.accnt()
                     << ",id=" << rest.id()
                     << endl;
                break;
            case DBR_METHOD_GET | DBR_RESRC_ORDER | DBR_PARAM_ACCNT:
                cout << "get order: accnt=" << rest.accnt() << endl;
                break;
            case DBR_METHOD_GET | DBR_RESRC_ORDER | DBR_PARAM_ACCNT | DBR_PARAM_ID:
                cout << "get order: accnt=" << rest.accnt()
                     << ",id=" << rest.id()
                     << endl;
                break;
            case DBR_METHOD_POST | DBR_RESRC_ORDER | DBR_PARAM_ACCNT | DBR_PARAM_GROUP
                | DBR_PARAM_CONTR | DBR_PARAM_SETTL_DATE | DBR_PARAM_REF | DBR_PARAM_ACTION
                | DBR_PARAM_TICKS | DBR_PARAM_LOTS | DBR_PARAM_MIN_LOTS:
                cout << "post order: accnt=" << rest.accnt()
                     << ",group=" << rest.group()
                     << ",contr=" << rest.contr()
                     << ",settl_date=" << rest.settl_date()
                     << ",ref=" << rest.ref()
                     << ",action=" << rest.action()
                     << ",ticks=" << rest.ticks()
                     << ",lots=" << rest.lots()
                     << ",min_lots=" << rest.min_lots()
                     << endl;
                break;
            case DBR_METHOD_PUT | DBR_RESRC_ORDER | DBR_PARAM_ACCNT | DBR_PARAM_ID | DBR_PARAM_LOTS:
                cout << "put order: accnt=" << rest.accnt()
                     << ",id=" << rest.id()
                     << ",lots=" << rest.lots()
                     << endl;
                break;
            case DBR_METHOD_DELETE | DBR_RESRC_TRADE | DBR_PARAM_ACCNT | DBR_PARAM_ID:
                cout << "delete trade: accnt=" << rest.accnt()
                     << ",id=" << rest.id()
                     << endl;
                break;
            case DBR_METHOD_GET | DBR_RESRC_TRADE | DBR_PARAM_ACCNT:
                cout << "get trade: accnt=" << rest.accnt() << endl;
                break;
            case DBR_METHOD_GET | DBR_RESRC_TRADE | DBR_PARAM_ACCNT | DBR_PARAM_ID:
                cout << "get trade: accnt=" << rest.accnt()
                     << ",id=" << rest.id()
                     << endl;
                break;
            case DBR_METHOD_GET | DBR_RESRC_POSN | DBR_PARAM_GROUP:
                cout << "get posn: group=" << rest.group() << endl;
                break;
            case DBR_METHOD_GET | DBR_RESRC_MARKET:
                cout << "get view:" << endl;
                break;
            case DBR_METHOD_GET | DBR_RESRC_MARKET | DBR_PARAM_CONTR:
                cout << "get depth: contr=" << rest.contr() << endl;
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
