/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_ELM_PROTO_HPP
#define DBRPP_ELM_PROTO_HPP

#include <dbrpp/ash/except.hpp>

#include <dbr/elm/proto.h>

namespace dbr {

/**
 * @addtogroup AccntRec
 * @{
 */

inline size_t
proto_accnt_len(const DbrRec& arec) noexcept
{
    return dbr_proto_accnt_len(&arec);
}

inline char*
proto_write_accnt(char* buf, const DbrRec& arec) noexcept
{
    return dbr_proto_write_accnt(buf, &arec);
}

inline const char*
proto_read_accnt(const char* buf, DbrRec& arec)
{
    buf = dbr_proto_read_accnt(buf, &arec);
    if (!buf)
        throw_exception();
    return buf;
}

/** @} */

/**
 * @addtogroup ContrRec
 * @{
 */

inline size_t
proto_contr_len(const DbrRec& crec) noexcept
{
    return dbr_proto_contr_len(&crec);
}

inline char*
proto_write_contr(char* buf, const DbrRec& crec) noexcept
{
    return dbr_proto_write_contr(buf, &crec);
}

inline const char*
proto_read_contr(const char* buf, DbrRec& crec)
{
    buf = dbr_proto_read_contr(buf, &crec);
    if (!buf)
        throw_exception();
    return buf;
}

/** @} */

/**
 * @addtogroup Perm
 * @{
 */

inline size_t
proto_perm_len(const DbrPerm& perm, DbrBool enriched) noexcept
{
    return dbr_proto_perm_len(&perm, enriched);
}

inline char*
proto_write_perm(char* buf, const DbrPerm& perm, DbrBool enriched) noexcept
{
    return dbr_proto_write_perm(buf, &perm, enriched);
}

inline const char*
proto_read_perm(const char* buf, DbrPerm& perm)
{
    buf = dbr_proto_read_perm(buf, &perm);
    if (!buf)
        throw_exception();
    return buf;
}

/** @} */

/**
 * @addtogroup Order
 * @{
 */

inline size_t
proto_order_len(const DbrOrder& order, DbrBool enriched) noexcept
{
    return dbr_proto_order_len(&order, enriched);
}

inline char*
proto_write_order(char* buf, const DbrOrder& order, DbrBool enriched) noexcept
{
    return dbr_proto_write_order(buf, &order, enriched);
}

inline const char*
proto_read_order(const char* buf, DbrOrder& order)
{
    buf = dbr_proto_read_order(buf, &order);
    if (!buf)
        throw_exception();
    return buf;
}

/** @} */

/**
 * @addtogroup Exec
 * @{
 */

inline size_t
proto_exec_len(const DbrExec& exec, DbrBool enriched) noexcept
{
    return dbr_proto_exec_len(&exec, enriched);
}

inline char*
proto_write_exec(char* buf, const DbrExec& exec, DbrBool enriched) noexcept
{
    return dbr_proto_write_exec(buf, &exec, enriched);
}

inline const char*
proto_read_exec(const char* buf, DbrExec& exec)
{
    buf = dbr_proto_read_exec(buf, &exec);
    if (!buf)
        throw_exception();
    return buf;
}

/** @} */

/**
 * @addtogroup Posn
 * @{
 */

inline size_t
proto_posn_len(const DbrPosn& posn, DbrBool enriched) noexcept
{
    return dbr_proto_posn_len(&posn, enriched);
}

inline char*
proto_write_posn(char* buf, const DbrPosn& posn, DbrBool enriched) noexcept
{
    return dbr_proto_write_posn(buf, &posn, enriched);
}

inline const char*
proto_read_posn(const char* buf, DbrPosn& posn)
{
    buf = dbr_proto_read_posn(buf, &posn);
    if (!buf)
        throw_exception();
    return buf;
}

/** @} */

/**
 * @addtogroup View
 * @{
 */

inline size_t
proto_view_len(const DbrView& view, DbrBool enriched) noexcept
{
    return dbr_proto_view_len(&view, enriched);
}

inline char*
proto_write_view(char* buf, const DbrView& view, DbrBool enriched) noexcept
{
    return dbr_proto_write_view(buf, &view, enriched);
}

inline const char*
proto_read_view(const char* buf, DbrView& view)
{
    buf = dbr_proto_read_view(buf, &view);
    if (!buf)
        throw_exception();
    return buf;
}

/** @} */

} // dbr

#endif // DBRPP_ELM_PROTO_HPP
