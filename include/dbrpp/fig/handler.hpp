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
#ifndef DBRPP_FIG_HANDLER_HPP
#define DBRPP_FIG_HANDLER_HPP

#include <dbrpp/fig/clnt.hpp>

#include <dbr/fig/handler.h>

namespace dbr {

/**
 * @addtogroup Handler
 * @{
 */

template <class DerivedT>
class IHandler : public DbrIHandler {
    static void
    on_close(DbrHandler handler, DbrClnt clnt) noexcept
    {
        static_cast<DerivedT*>(handler)->on_close(ClntRef(clnt));
    }
    static void
    on_ready(DbrHandler handler, DbrClnt clnt) noexcept
    {
        static_cast<DerivedT*>(handler)->on_ready(ClntRef(clnt));
    }
    static void
    on_logon(DbrHandler handler, DbrClnt clnt, DbrIden req_id, DbrIden aid) noexcept
    {
        static_cast<DerivedT*>(handler)->on_logon(ClntRef(clnt), req_id, aid);
    }
    static void
    on_logoff(DbrHandler handler, DbrClnt clnt, DbrIden req_id, DbrIden aid) noexcept
    {
        static_cast<DerivedT*>(handler)->on_logoff(ClntRef(clnt), req_id, aid);
    }
    static void
    on_reset(DbrHandler handler, DbrClnt clnt) noexcept
    {
        static_cast<DerivedT*>(handler)->on_reset(ClntRef(clnt));
    }
    static void
    on_timeout(DbrHandler handler, DbrClnt clnt, DbrIden req_id) noexcept
    {
        static_cast<DerivedT*>(handler)->on_timeout(ClntRef(clnt), req_id);
    }
    static void
    on_status(DbrHandler handler, DbrClnt clnt, DbrIden req_id, int num, const char* msg) noexcept
    {
        static_cast<DerivedT*>(handler)->on_status(ClntRef(clnt), req_id, num, msg);
    }
    static void
    on_exec(DbrHandler handler, DbrClnt clnt, DbrIden req_id, DbrExec* exec) noexcept
    {
        static_cast<DerivedT*>(handler)->on_exec(ClntRef(clnt), req_id, *exec);
    }
    static void
    on_posn(DbrHandler handler, DbrClnt clnt, DbrPosn* posn) noexcept
    {
        static_cast<DerivedT*>(handler)->on_posn(ClntRef(clnt), *posn);
    }
    static void
    on_view(DbrHandler handler, DbrClnt clnt, DbrView* view) noexcept
    {
        static_cast<DerivedT*>(handler)->on_view(ClntRef(clnt), *view);
    }
    static void
    on_flush(DbrHandler handler, DbrClnt clnt) noexcept
    {
        static_cast<DerivedT*>(handler)->on_flush(ClntRef(clnt));
    }
    static void*
    on_async(DbrHandler handler, DbrClnt clnt, void* val) noexcept
    {
        return static_cast<DerivedT*>(handler)->on_async(ClntRef(clnt), val);
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
on_close(DbrHandler handler, DbrClnt clnt) noexcept
{
    handler->vtbl->on_close(handler, clnt);
}

inline void
on_ready(DbrHandler handler, DbrClnt clnt) noexcept
{
    handler->vtbl->on_ready(handler, clnt);
}

inline void
on_logon(DbrHandler handler, DbrClnt clnt, DbrIden req_id, DbrIden aid) noexcept
{
    handler->vtbl->on_logon(handler, clnt, req_id, aid);
}

inline void
on_logoff(DbrHandler handler, DbrClnt clnt, DbrIden req_id, DbrIden aid) noexcept
{
    handler->vtbl->on_logoff(handler, clnt, req_id, aid);
}

inline void
on_reset(DbrHandler handler, DbrClnt clnt) noexcept
{
    handler->vtbl->on_reset(handler, clnt);
}

inline void
on_timeout(DbrHandler handler, DbrClnt clnt, DbrIden req_id) noexcept
{
    handler->vtbl->on_timeout(handler, clnt, req_id);
}

inline void
on_status(DbrHandler handler, DbrClnt clnt, DbrIden req_id, int num, const char* msg) noexcept
{
    handler->vtbl->on_status(handler, clnt, req_id, num, msg);
}

inline void
on_exec(DbrHandler handler, DbrClnt clnt, DbrIden req_id, DbrExec& exec) noexcept
{
    handler->vtbl->on_exec(handler, clnt, req_id, &exec);
}

inline void
on_posn(DbrHandler handler, DbrClnt clnt, DbrPosn& posn) noexcept
{
    handler->vtbl->on_posn(handler, clnt, &posn);
}

inline void
on_view(DbrHandler handler, DbrClnt clnt, DbrView& view) noexcept
{
    handler->vtbl->on_view(handler, clnt, &view);
}

inline void
on_flush(DbrHandler handler, DbrClnt clnt) noexcept
{
    handler->vtbl->on_flush(handler, clnt);
}

inline void*
on_async(DbrHandler handler, DbrClnt clnt, void* val) noexcept
{
    return handler->vtbl->on_async(handler, clnt, val);
}

/** @} */

} // dbr

#endif // DBRPP_FIG_HANDLER_HPP
