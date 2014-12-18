/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_FIG_HANDLER_HPP
#define SCPP_FIG_HANDLER_HPP

#include <scpp/fig/clnt.hpp>

#include <sc/fig/handler.h>

namespace sc {

/**
 * @addtogroup Handler
 * @{
 */

template <class DerivedT>
class IHandler : public ScIHandler {
    static void
    on_close(ScHandler handler, ScClnt clnt) noexcept
    {
        static_cast<DerivedT*>(handler)->on_close(ClntRef(clnt));
    }
    static void
    on_ready(ScHandler handler, ScClnt clnt) noexcept
    {
        static_cast<DerivedT*>(handler)->on_ready(ClntRef(clnt));
    }
    static void
    on_logon(ScHandler handler, ScClnt clnt, ScIden req_id, ScIden aid) noexcept
    {
        static_cast<DerivedT*>(handler)->on_logon(ClntRef(clnt), req_id, aid);
    }
    static void
    on_logoff(ScHandler handler, ScClnt clnt, ScIden req_id, ScIden aid) noexcept
    {
        static_cast<DerivedT*>(handler)->on_logoff(ClntRef(clnt), req_id, aid);
    }
    static void
    on_reset(ScHandler handler, ScClnt clnt) noexcept
    {
        static_cast<DerivedT*>(handler)->on_reset(ClntRef(clnt));
    }
    static void
    on_timeout(ScHandler handler, ScClnt clnt, ScIden req_id) noexcept
    {
        static_cast<DerivedT*>(handler)->on_timeout(ClntRef(clnt), req_id);
    }
    static void
    on_status(ScHandler handler, ScClnt clnt, ScIden req_id, int num, const char* msg) noexcept
    {
        static_cast<DerivedT*>(handler)->on_status(ClntRef(clnt), req_id, num, msg);
    }
    static void
    on_exec(ScHandler handler, ScClnt clnt, ScIden req_id, ScExec* exec) noexcept
    {
        static_cast<DerivedT*>(handler)->on_exec(ClntRef(clnt), req_id, *exec);
    }
    static void
    on_posn(ScHandler handler, ScClnt clnt, ScPosn* posn) noexcept
    {
        static_cast<DerivedT*>(handler)->on_posn(ClntRef(clnt), *posn);
    }
    static void
    on_view(ScHandler handler, ScClnt clnt, ScView* view) noexcept
    {
        static_cast<DerivedT*>(handler)->on_view(ClntRef(clnt), *view);
    }
    static void
    on_flush(ScHandler handler, ScClnt clnt) noexcept
    {
        static_cast<DerivedT*>(handler)->on_flush(ClntRef(clnt));
    }
    static void*
    on_async(ScHandler handler, ScClnt clnt, void* val) noexcept
    {
        return static_cast<DerivedT*>(handler)->on_async(ClntRef(clnt), val);
    }
    static const ScHandlerVtbl*
    vtbl() noexcept
    {
        static const ScHandlerVtbl VTBL = {
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
        : ScIHandler{vtbl()}
    {
    }
};

inline void
on_close(ScHandler handler, ScClnt clnt) noexcept
{
    handler->vtbl->on_close(handler, clnt);
}

inline void
on_ready(ScHandler handler, ScClnt clnt) noexcept
{
    handler->vtbl->on_ready(handler, clnt);
}

inline void
on_logon(ScHandler handler, ScClnt clnt, ScIden req_id, ScIden aid) noexcept
{
    handler->vtbl->on_logon(handler, clnt, req_id, aid);
}

inline void
on_logoff(ScHandler handler, ScClnt clnt, ScIden req_id, ScIden aid) noexcept
{
    handler->vtbl->on_logoff(handler, clnt, req_id, aid);
}

inline void
on_reset(ScHandler handler, ScClnt clnt) noexcept
{
    handler->vtbl->on_reset(handler, clnt);
}

inline void
on_timeout(ScHandler handler, ScClnt clnt, ScIden req_id) noexcept
{
    handler->vtbl->on_timeout(handler, clnt, req_id);
}

inline void
on_status(ScHandler handler, ScClnt clnt, ScIden req_id, int num, const char* msg) noexcept
{
    handler->vtbl->on_status(handler, clnt, req_id, num, msg);
}

inline void
on_exec(ScHandler handler, ScClnt clnt, ScIden req_id, ScExec& exec) noexcept
{
    handler->vtbl->on_exec(handler, clnt, req_id, &exec);
}

inline void
on_posn(ScHandler handler, ScClnt clnt, ScPosn& posn) noexcept
{
    handler->vtbl->on_posn(handler, clnt, &posn);
}

inline void
on_view(ScHandler handler, ScClnt clnt, ScView& view) noexcept
{
    handler->vtbl->on_view(handler, clnt, &view);
}

inline void
on_flush(ScHandler handler, ScClnt clnt) noexcept
{
    handler->vtbl->on_flush(handler, clnt);
}

inline void*
on_async(ScHandler handler, ScClnt clnt, void* val) noexcept
{
    return handler->vtbl->on_async(handler, clnt, val);
}

/** @} */

} // sc

#endif // SCPP_FIG_HANDLER_HPP
