#!/usr/bin/env python

import os, shlex, sys

def parse(lex, handler):
    toks = []
    while True:
        tok = lex.get_token()
        if tok == lex.eof:
            break
        if tok == ';':
            if len(toks) > 0:
                fn = getattr(handler, toks[0])
                fn(*toks[1:])
                toks = []
        else:
            toks.append(tok)

class Client(object):
    def accnts(self):
        print('accnts')

    def ack_trades(self, *args):
        print('ack_trades: ' + ','.join(args))

    def buy(self, lots, price):
        print('buy: {0},{1}'.format(lots, price))

    def cancel(self, *args):
        print('cancel: ' + ','.join(args))

    def contrs(self):
        print('contrs')

    def echo(self, *args):
        print('echo: ' + ','.join(args))

    def env(self):
        print('env')

    def logon(self, tid):
        print('logon: {0}'.format(tid))

    def logoff(self):
        print('logoff')

    def orders(self):
        print('orders')

    def posns(self):
        print('posns')

    def quit(self):
        print('quit')

    def revise(self, id, lots):
        print('revise: {0},{1}'.format(id, lots))

    def sell(self, lots, price):
        print('sell: {0},{1}'.format(lots, price))

    def set(self, name, value):
        print('set: {0},{1}'.format(name, value))

    def traders(self):
        print('traders')

    def trades(self):
        print('trades')

    def unset(self, name):
        print('unset: {0}'.format(name))

    def view(self):
        print('view')

if __name__ == '__main__':
    try:
        lex = shlex.shlex(sys.stdin)
        home = os.environ['HOME']
        rcfile = os.path.join(home, '.dbr_clirc')
        if os.path.isfile(rcfile):
            lex.push_source(open(rcfile, 'r'), rcfile)
        parse(lex, Client())
    except Error as e:
        print 'error: ' + str(e)
