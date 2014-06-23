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
 */

/**
 * Each thread has its own error information.
 * @defgroup Err Error
 * @ingroup Base
 * @example err.cpp
 */

/**
 * @defgroup Format Format
 * @ingroup Base
 * @example format.cpp
 */

/**
 * Lexical tokeniser.
 * @defgroup Shlex Lexer
 * @ingroup Base
 */

/**
 * @defgroup Pack Pack
 * @ingroup Base
 * @example pack.cpp
 */

/**
 * @defgroup String String
 * @ingroup Base
 */

/**
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
 * @defgroup Types Types
 */

/**
 * @defgroup Simple Simple
 * @ingroup Types
 */

/**
 * @defgroup Rec Record
 * @ingroup Types
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
 * @ingroup Types
 */

/**
 * @defgroup Common Common
 * @ingroup Types
 */

/**
 * @defgroup Order Order
 * @ingroup Types
 */

/**
 * @defgroup Exec Execution
 * @ingroup Types
 */

/**
 * @defgroup Match Match
 * @ingroup Types
 */

/**
 * @defgroup Posn Position
 * @ingroup Types
 */

/**
 * @defgroup Conv Conversion
 * @ingroup Types
 */

/**
 * @defgroup System System
 */

/**
 * @defgroup Daemon Daemon
 * @ingroup System
 */

/**
 * Each thread has its own logger.
 * @defgroup Log Log
 * @ingroup System
 */

/**
 * @defgroup Mutex Mutex
 * @ingroup System
 */

/**
 * Memory pool.
 * @defgroup Pool Pool
 * @ingroup System
 * @example pool.cpp
 */

/**
 * @defgroup Sem Semaphore
 * @ingroup System
 */

/**
 * @defgroup Thread Thread
 * @ingroup System
 */

/**
 * @defgroup IO IO
 */

/**
 * @defgroup Bank Bank
 * @ingroup IO
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
 * @defgroup Rest Rest
 * @ingroup IO
 */

/**
 * @defgroup Zmq ZeroMQ
 * @ingroup IO
 */

/**
 * Accounts represent both users and groups.
 * @defgroup Accnt Account
 */

/**
 * An Order Book comprises two sides: one for Bids and one for Offers.
 * @defgroup Book Book
 */

/**
 * The orders for each side are ordered by price and time.
 * @defgroup Side Side
 * @ingroup Book
 * @example side.cpp
 */

/**
 * Each level is an aggregation of orders by price.
 * @defgroup Level Level
 * @ingroup Book
 */

/**
 * @defgroup View View
 * @ingroup Book
 */

/**
 * @defgroup Sess Session
 */

/**
 * @defgroup Serv Server
 * @example serv.cpp
 */

/**
 * @defgroup Client Client
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
 */

/**
 * @defgroup Ctx Context
 * @ingroup Client
 */

/**
 * @defgroup Async Async
 * @ingroup Client
 */

/**
 * @defgroup Task Task
 * @ingroup Client
 */
