/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ELM_PROTO_HPP
#define SCPP_ELM_PROTO_HPP

#include <scpp/ash/except.hpp>

#include <sc/elm/proto.h>

namespace sc {

/**
 * @addtogroup AccntRec
 * @{
 */

inline size_t
proto_accnt_len(const ScRec& arec) noexcept
{
    return sc_proto_accnt_len(&arec);
}

inline char*
proto_write_accnt(char* buf, const ScRec& arec) noexcept
{
    return sc_proto_write_accnt(buf, &arec);
}

inline const char*
proto_read_accnt(const char* buf, ScRec& arec)
{
    buf = sc_proto_read_accnt(buf, &arec);
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
proto_contr_len(const ScRec& crec) noexcept
{
    return sc_proto_contr_len(&crec);
}

inline char*
proto_write_contr(char* buf, const ScRec& crec) noexcept
{
    return sc_proto_write_contr(buf, &crec);
}

inline const char*
proto_read_contr(const char* buf, ScRec& crec)
{
    buf = sc_proto_read_contr(buf, &crec);
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
proto_perm_len(const ScPerm& perm, ScBool enriched) noexcept
{
    return sc_proto_perm_len(&perm, enriched);
}

inline char*
proto_write_perm(char* buf, const ScPerm& perm, ScBool enriched) noexcept
{
    return sc_proto_write_perm(buf, &perm, enriched);
}

inline const char*
proto_read_perm(const char* buf, ScPerm& perm)
{
    buf = sc_proto_read_perm(buf, &perm);
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
proto_order_len(const ScOrder& order, ScBool enriched) noexcept
{
    return sc_proto_order_len(&order, enriched);
}

inline char*
proto_write_order(char* buf, const ScOrder& order, ScBool enriched) noexcept
{
    return sc_proto_write_order(buf, &order, enriched);
}

inline const char*
proto_read_order(const char* buf, ScOrder& order)
{
    buf = sc_proto_read_order(buf, &order);
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
proto_exec_len(const ScExec& exec, ScBool enriched) noexcept
{
    return sc_proto_exec_len(&exec, enriched);
}

inline char*
proto_write_exec(char* buf, const ScExec& exec, ScBool enriched) noexcept
{
    return sc_proto_write_exec(buf, &exec, enriched);
}

inline const char*
proto_read_exec(const char* buf, ScExec& exec)
{
    buf = sc_proto_read_exec(buf, &exec);
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
proto_posn_len(const ScPosn& posn, ScBool enriched) noexcept
{
    return sc_proto_posn_len(&posn, enriched);
}

inline char*
proto_write_posn(char* buf, const ScPosn& posn, ScBool enriched) noexcept
{
    return sc_proto_write_posn(buf, &posn, enriched);
}

inline const char*
proto_read_posn(const char* buf, ScPosn& posn)
{
    buf = sc_proto_read_posn(buf, &posn);
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
proto_view_len(const ScView& view, ScBool enriched) noexcept
{
    return sc_proto_view_len(&view, enriched);
}

inline char*
proto_write_view(char* buf, const ScView& view, ScBool enriched) noexcept
{
    return sc_proto_write_view(buf, &view, enriched);
}

inline const char*
proto_read_view(const char* buf, ScView& view)
{
    buf = sc_proto_read_view(buf, &view);
    if (!buf)
        throw_exception();
    return buf;
}

/** @} */

} // sc

#endif // SCPP_ELM_PROTO_HPP
