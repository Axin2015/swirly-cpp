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
#ifndef DBRPP_HANDLER_HPP
#define DBRPP_HANDLER_HPP

#include <dbr/handler.h>

namespace dbr {

template <class DerivedT>
class IHandler : public DbrIHandler {
    static void
    up(DbrHandler handler, int conn) noexcept
    {
        static_cast<DerivedT*>(handler)->up(conn);
    }
    static void
    down(DbrHandler handler, int conn) noexcept
    {
        static_cast<DerivedT*>(handler)->down(conn);
    }
    static void
    timeout(DbrHandler handler, DbrIden req_id) noexcept
    {
        static_cast<DerivedT*>(handler)->timeout(req_id);
    }
    static void
    status(DbrHandler handler, DbrIden req_id, int num, const char* msg) noexcept
    {
        static_cast<DerivedT*>(handler)->status(req_id, num, msg);
    }
    static void
    exec(DbrHandler handler, DbrIden req_id, DbrExec* exec) noexcept
    {
        static_cast<DerivedT*>(handler)->exec(req_id, *exec);
    }
    static void
    posn(DbrHandler handler, DbrPosn* posn) noexcept
    {
        static_cast<DerivedT*>(handler)->posn(*posn);
    }
    static void
    view(DbrHandler handler, DbrView* view) noexcept
    {
        static_cast<DerivedT*>(handler)->view(*view);
    }
    static void
    flush(DbrHandler handler) noexcept
    {
        static_cast<DerivedT*>(handler)->flush();
    }
    static const DbrHandlerVtbl*
    vtbl() noexcept
    {
        static const DbrHandlerVtbl VTBL = {
            up,
            down,
            timeout,
            status,
            exec,
            posn,
            view,
            flush
        };
        return &VTBL;
    }
protected:
    ~IHandler() noexcept
    {
    }
public:
    IHandler() noexcept
        : DbrIHandler{vtbl()}
    {
    }
};

inline void
up(DbrHandler handler, int conn)
{
    handler->vtbl->up(handler, conn);
}

inline void
down(DbrHandler handler, int conn)
{
    handler->vtbl->down(handler, conn);
}

inline void
timeout(DbrHandler handler, DbrIden req_id)
{
    handler->vtbl->timeout(handler, req_id);
}

inline void
status(DbrHandler handler, DbrIden req_id, int num, const char* msg)
{
    handler->vtbl->status(handler, req_id, num, msg);
}

inline void
exec(DbrHandler handler, DbrIden req_id, DbrExec& exec)
{
    handler->vtbl->exec(handler, req_id, &exec);
}

inline void
posn(DbrHandler handler, DbrPosn& posn)
{
    handler->vtbl->posn(handler, &posn);
}

inline void
view(DbrHandler handler, DbrView& view)
{
    handler->vtbl->view(handler, &view);
}

inline void
flush(DbrHandler handler)
{
    handler->vtbl->flush(handler);
}

/** @} */

} // dbr

#endif // DBRPP_HANDLER_HPP
