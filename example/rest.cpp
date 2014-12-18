/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <scpp/elm/rest.hpp>

#include <iostream>

using namespace sc;
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
            case SC_METHOD_POST | SC_RESRC_LOGON | SC_PARAM_ACCNT:
                cout << "post logon: accnt=" << rest.accnt() << endl;
                break;
            case SC_METHOD_POST | SC_RESRC_LOGOFF | SC_PARAM_ACCNT:
                cout << "post logoff: accnt=" << rest.accnt() << endl;
                break;
            case SC_METHOD_GET | SC_RESRC_ACCNT:
                cout << "get accnt" << endl;
                break;
            case SC_METHOD_GET | SC_RESRC_ACCNT | SC_PARAM_ACCNT:
                cout << "get accnt: mnem=" << rest.accnt() << endl;
                break;
            case SC_METHOD_GET | SC_RESRC_CONTR:
                cout << "get contr" << endl;
                break;
            case SC_METHOD_GET | SC_RESRC_CONTR | SC_PARAM_CONTR:
                cout << "get contr: mnem=" << rest.contr() << endl;
                break;
            case SC_METHOD_GET | SC_RESRC_TRADER | SC_PARAM_ACCNT:
                cout << "get trader: accnt=" << rest.accnt() << endl;
                break;
            case SC_METHOD_GET | SC_RESRC_GIVEUP | SC_PARAM_ACCNT:
                cout << "get giveup: accnt=" << rest.accnt() << endl;
                break;
            case SC_METHOD_DELETE | SC_RESRC_ORDER | SC_PARAM_ACCNT | SC_PARAM_ID:
                cout << "delete order: accnt=" << rest.accnt()
                     << ",id=" << rest.id()
                     << endl;
                break;
            case SC_METHOD_GET | SC_RESRC_ORDER | SC_PARAM_ACCNT:
                cout << "get order: accnt=" << rest.accnt() << endl;
                break;
            case SC_METHOD_GET | SC_RESRC_ORDER | SC_PARAM_ACCNT | SC_PARAM_ID:
                cout << "get order: accnt=" << rest.accnt()
                     << ",id=" << rest.id()
                     << endl;
                break;
            case SC_METHOD_POST | SC_RESRC_ORDER | SC_PARAM_ACCNT | SC_PARAM_GIVEUP
                | SC_PARAM_CONTR | SC_PARAM_SETTL_DATE | SC_PARAM_REF | SC_PARAM_ACTION
                | SC_PARAM_TICKS | SC_PARAM_LOTS | SC_PARAM_MIN_LOTS:
                cout << "post order: accnt=" << rest.accnt()
                     << ",giveup=" << rest.giveup()
                     << ",contr=" << rest.contr()
                     << ",settl_date=" << rest.settl_date()
                     << ",ref=" << rest.ref()
                     << ",action=" << rest.action()
                     << ",ticks=" << rest.ticks()
                     << ",lots=" << rest.lots()
                     << ",min_lots=" << rest.min_lots()
                     << endl;
                break;
            case SC_METHOD_PUT | SC_RESRC_ORDER | SC_PARAM_ACCNT | SC_PARAM_ID | SC_PARAM_LOTS:
                cout << "put order: accnt=" << rest.accnt()
                     << ",id=" << rest.id()
                     << ",lots=" << rest.lots()
                     << endl;
                break;
            case SC_METHOD_DELETE | SC_RESRC_TRADE | SC_PARAM_ACCNT | SC_PARAM_ID:
                cout << "delete trade: accnt=" << rest.accnt()
                     << ",id=" << rest.id()
                     << endl;
                break;
            case SC_METHOD_GET | SC_RESRC_TRADE | SC_PARAM_ACCNT:
                cout << "get trade: accnt=" << rest.accnt() << endl;
                break;
            case SC_METHOD_GET | SC_RESRC_TRADE | SC_PARAM_ACCNT | SC_PARAM_ID:
                cout << "get trade: accnt=" << rest.accnt()
                     << ",id=" << rest.id()
                     << endl;
                break;
            case SC_METHOD_GET | SC_RESRC_POSN | SC_PARAM_ACCNT:
                cout << "get posn: accnt=" << rest.accnt() << endl;
                break;
            case SC_METHOD_GET | SC_RESRC_VIEW:
                cout << "get view:" << endl;
                break;
            case SC_METHOD_GET | SC_RESRC_VIEW | SC_PARAM_CONTR:
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
