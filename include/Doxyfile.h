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
/**
 * @defgroup Base Base
 */

/**
 * Julian and Gregorian date conversions.
 * @defgroup Date Date
 * @ingroup Base
 * @example jdtoymd.c
 * @example ymdtojd.c
 */

/**
 * Thread-specific error information.
 * @defgroup Err Error
 * @ingroup Base
 * @example err.cpp
 */

/**
 * Lexical tokeniser.
 * @defgroup Shlex Lexer
 * @ingroup Base
 */

/**
 * Binary encoding.
 * @defgroup Pack Pack
 * @ingroup Base
 * @example pack.cpp
 */

/**
 * String functions.
 * @defgroup String String
 * @ingroup Base
 */

/**
 * Utility functions.
 * @defgroup Util Utility
 * @ingroup Base
 * @example util.cpp
 */

/**
 * Container types.
 * @defgroup Cont Container
 */

/**
 * Node-based types.
 * @defgroup Node Node
 * @ingroup Cont
 * @example slnode.cpp
 */

/**
 * Doubly-linked node.
 * @defgroup DlNode DlNode
 * @ingroup Node
 */

/**
 * Red-black node.
 * @defgroup RbNode RbNode
 * @ingroup Node
 */

/**
 * Singly-linked node.
 * @defgroup SlNode SlNode
 * @ingroup Node
 */

/**
 * @defgroup Iter Iterator
 * @ingroup Cont
 */

/**
 * Doubly-linked list.
 * @defgroup List List
 * @ingroup Cont
 */

/**
 * Priority queue.
 * @defgroup Prioq Prioq
 * @ingroup Cont
 * @example prioq.cpp
 */

/**
 * Singly-linked queue.
 * @defgroup Queue Queue
 * @ingroup Cont
 */

/**
 * Stack.
 * @defgroup Stack Stack
 * @ingroup Cont
 */

/**
 * Red-black tree.
 * @defgroup Tree Tree
 * @ingroup Cont
 */

/**
 * @defgroup System System
 */

/**
 * Process daemonisation.
 * @defgroup Daemon Daemon
 * @ingroup System
 */

/**
 * Thread-specific logger.
 * @defgroup Log Log
 * @ingroup System
 */

/**
 * @defgroup Mutex Mutex
 * @ingroup System
 */

/**
 * @defgroup Sem Semaphore
 * @ingroup System
 */

/**
 * @defgroup Domain Domain
 * @example json.cpp
 * @example proto.cpp
 */

/**
 * Simple type definitions.
 * @defgroup Simple Simple
 * @ingroup Domain
 */

/**
 * @defgroup Rec Record
 * @ingroup Domain
 */

/**
 * @defgroup AccntRec Accnt
 * @ingroup Rec
 */

/**
 * @defgroup ContrRec Contract
 * @ingroup Rec
 */

/**
 * @defgroup Memb Member
 * @ingroup Domain
 */

/**
 * @defgroup Common Common
 * @ingroup Domain
 */

/**
 * @defgroup Order Order
 * @ingroup Domain
 */

/**
 * @defgroup Exec Execution
 * @ingroup Domain
 */

/**
 * @defgroup Match Match
 * @ingroup Domain
 */

/**
 * @defgroup Posn Position
 * @ingroup Domain
 */

/**
 * Type conversions.
 * @defgroup Conv Conversion
 * @ingroup Domain
 */

/**
 * @defgroup Core Core
 */

/**
 * Memory pool.
 * @defgroup Pool Pool
 * @ingroup Core
 * @example pool.cpp
 */

/**
 * @defgroup IO IO
 * @ingroup Core
 */

/**
 * @defgroup Bank Bank
 * @ingroup IO
 */

/**
 * @defgroup Format Format
 * @ingroup IO
 * @example format.cpp
 */

/**
 * @defgroup Journ Journal
 * @ingroup IO
 */

/**
 * @defgroup Model Model
 * @ingroup IO
 * @example model.cpp
 */

/**
 * @defgroup Msg Msg
 * @ingroup IO
 */

/**
 * Restful parser.
 * @defgroup Rest Rest
 * @ingroup IO
 * @example rest.cpp
 */

/**
 * @defgroup Zmq ZeroMQ
 * @ingroup IO
 */

/**
 * Accounts represent both users and groups.
 * @defgroup Accnt Account
 * @ingroup Core
 */

/**
 * Order-book comprising bid and offer sides.
 * @defgroup Book Book
 * @ingroup Core
 */

/**
 * Side ordered by price and time.
 * @defgroup Side Side
 * @ingroup Book
 * @example side.cpp
 */

/**
 * Price level is an aggregation of orders by price.
 * @defgroup Level Level
 * @ingroup Book
 */

/**
 * @defgroup View View
 * @ingroup Book
 */

/**
 * @defgroup Sess Session
 * @ingroup Core
 */

/**
 * @defgroup Serv Server
 * @ingroup Core
 * @example dump.cpp
 * @example perf.c
 * @example serv.cpp
 */

/**
 * @defgroup Client Client
 * @ingroup Core
 */

/**
 * @defgroup Clnt Client
 * @ingroup Client
 */

/**
 * @defgroup Dispatch Dispatch
 * @ingroup Client
 */

/**
 * @defgroup Handler Handler
 * @ingroup Client
 * @example skel.cpp
 */

/**
 * @defgroup Ctx Context
 * @ingroup Client
 * @example skel.cpp
 */

/**
 * @defgroup Async Async
 * @ingroup Client
 * @example skel.cpp
 */

/**
 * @defgroup Task Task
 * @ingroup Client
 */
