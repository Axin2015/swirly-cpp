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

/**
 * @addtogroup Handler
 * @{
 */

template <class DerivedT>
class IHandler : public DbrIHandler {
    static void
    on_close(DbrHandler handler) noexcept
    {
        static_cast<DerivedT*>(handler)->on_close();
    }
    static void
    on_ready(DbrHandler handler) noexcept
    {
        static_cast<DerivedT*>(handler)->on_ready();
    }
    static void
    on_logon(DbrHandler handler, DbrIden tid) noexcept
    {
        static_cast<DerivedT*>(handler)->on_logon(tid);
    }
    static void
    on_logoff(DbrHandler handler, DbrIden tid) noexcept
    {
        static_cast<DerivedT*>(handler)->on_logoff(tid);
    }
    static void
    on_reset(DbrHandler handler) noexcept
    {
        static_cast<DerivedT*>(handler)->on_reset();
    }
    static void
    on_timeout(DbrHandler handler, DbrIden req_id) noexcept
    {
        static_cast<DerivedT*>(handler)->on_timeout(req_id);
    }
    static void
    on_status(DbrHandler handler, DbrIden req_id, int num, const char* msg) noexcept
    {
        static_cast<DerivedT*>(handler)->on_status(req_id, num, msg);
    }
    static void
    on_exec(DbrHandler handler, DbrIden req_id, DbrExec* exec) noexcept
    {
        static_cast<DerivedT*>(handler)->on_exec(req_id, *exec);
    }
    static void
    on_posn(DbrHandler handler, DbrPosn* posn) noexcept
    {
        static_cast<DerivedT*>(handler)->on_posn(*posn);
    }
    static void
    on_view(DbrHandler handler, DbrView* view) noexcept
    {
        static_cast<DerivedT*>(handler)->on_view(*view);
    }
    static void
    on_flush(DbrHandler handler) noexcept
    {
        static_cast<DerivedT*>(handler)->on_flush();
    }
    static void*
    on_async(DbrHandler handler, void* val) noexcept
    {
        return static_cast<DerivedT*>(handler)->on_async(val);
    }
    static const DbrHandlerVtbl*
    vtbl() noexcept
    {
        static const DbrHandlerVtbl VTBL = {
            on_close,
            on_ready,
            on_logon,
            on_logoff,
            on_reset,
            on_timeout,
            on_status,
            on_exec,
            on_posn,
            on_view,
            on_flush,
            on_async
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
on_close(DbrHandler handler) noexcept
{
    handler->vtbl->on_close(handler);
}

inline void
on_ready(DbrHandler handler) noexcept
{
    handler->vtbl->on_ready(handler);
}

inline void
on_logon(DbrHandler handler, DbrIden tid) noexcept
{
    handler->vtbl->on_logon(handler, tid);
}

inline void
on_logoff(DbrHandler handler, DbrIden tid) noexcept
{
    handler->vtbl->on_logoff(handler, tid);
}

inline void
on_reset(DbrHandler handler) noexcept
{
    handler->vtbl->on_reset(handler);
}

inline void
on_timeout(DbrHandler handler, DbrIden req_id) noexcept
{
    handler->vtbl->on_timeout(handler, req_id);
}

inline void
on_status(DbrHandler handler, DbrIden req_id, int num, const char* msg) noexcept
{
    handler->vtbl->on_status(handler, req_id, num, msg);
}

inline void
on_exec(DbrHandler handler, DbrIden req_id, DbrExec& exec) noexcept
{
    handler->vtbl->on_exec(handler, req_id, &exec);
}

inline void
on_posn(DbrHandler handler, DbrPosn& posn) noexcept
{
    handler->vtbl->on_posn(handler, &posn);
}

inline void
on_view(DbrHandler handler, DbrView& view) noexcept
{
    handler->vtbl->on_view(handler, &view);
}

inline void
on_flush(DbrHandler handler) noexcept
{
    handler->vtbl->on_flush(handler);
}

inline void*
on_async(DbrHandler handler, void* val) noexcept
{
    return handler->vtbl->on_async(handler, val);
}

/** @} */

} // dbr

#endif // DBRPP_HANDLER_HPP
