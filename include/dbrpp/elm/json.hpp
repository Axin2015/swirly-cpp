/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_ELM_JSON_HPP
#define DBRPP_ELM_JSON_HPP

#include <dbr/elm/json.h>

#include <string>

namespace dbr {

/**
 * @addtogroup AccntRec
 * @{
 */

inline size_t
json_accnt_len(const DbrRec& arec) noexcept
{
    return dbr_json_accnt_len(&arec);
}

inline char*
json_write_accnt(char* buf, const DbrRec& arec) noexcept
{
    return dbr_json_write_accnt(buf, &arec);
}

inline std::string
json_write_accnt(const DbrRec& arec)
{
    std::string s;
    s.resize(dbr_json_accnt_len(&arec) + 1);
    dbr_json_write_accnt(&s[0], &arec);
    s.resize(s.size() - 1);
    return s;
}

/** @} */

/**
 * @addtogroup ContrRec
 * @{
 */

inline size_t
json_contr_len(const DbrRec& crec) noexcept
{
    return dbr_json_contr_len(&crec);
}

inline char*
json_write_contr(char* buf, const DbrRec& crec) noexcept
{
    return dbr_json_write_contr(buf, &crec);
}

inline std::string
json_write_contr(const DbrRec& crec)
{
    std::string s;
    s.resize(dbr_json_contr_len(&crec) + 1);
    dbr_json_write_contr(&s[0], &crec);
    s.resize(s.size() - 1);
    return s;
}

/** @} */

/**
 * @addtogroup Perm
 * @{
 */

inline size_t
json_perm_len(const DbrPerm& perm) noexcept
{
    return dbr_json_perm_len(&perm);
}

inline char*
json_write_perm(char* buf, const DbrPerm& perm) noexcept
{
    return dbr_json_write_perm(buf, &perm);
}

inline std::string
json_write_perm(const DbrPerm& perm) noexcept
{
    std::string s;
    s.resize(dbr_json_perm_len(&perm) + 1);
    dbr_json_write_perm(&s[0], &perm);
    s.resize(s.size() - 1);
    return s;
}

/** @} */

/**
 * @addtogroup Order
 * @{
 */

inline size_t
json_order_len(const DbrOrder& order) noexcept
{
    return dbr_json_order_len(&order);
}

inline char*
json_write_order(char* buf, const DbrOrder& order) noexcept
{
    return dbr_json_write_order(buf, &order);
}

inline std::string
json_write_order(const DbrOrder& order) noexcept
{
    std::string s;
    s.resize(dbr_json_order_len(&order) + 1);
    dbr_json_write_order(&s[0], &order);
    s.resize(s.size() - 1);
    return s;
}

/** @} */

/**
 * @addtogroup Exec
 * @{
 */

inline size_t
json_exec_len(const DbrExec& exec) noexcept
{
    return dbr_json_exec_len(&exec);
}

inline char*
json_write_exec(char* buf, const DbrExec& exec) noexcept
{
    return dbr_json_write_exec(buf, &exec);
}

inline std::string
json_write_exec(const DbrExec& exec) noexcept
{
    std::string s;
    s.resize(dbr_json_exec_len(&exec) + 1);
    dbr_json_write_exec(&s[0], &exec);
    s.resize(s.size() - 1);
    return s;
}

/** @} */

/**
 * @addtogroup Posn
 * @{
 */

inline size_t
json_posn_len(const DbrPosn& posn) noexcept
{
    return dbr_json_posn_len(&posn);
}

inline char*
json_write_posn(char* buf, const DbrPosn& posn) noexcept
{
    return dbr_json_write_posn(buf, &posn);
}

inline std::string
json_write_posn(const DbrPosn& posn) noexcept
{
    std::string s;
    s.resize(dbr_json_posn_len(&posn) + 1);
    dbr_json_write_posn(&s[0], &posn);
    s.resize(s.size() - 1);
    return s;
}

/** @} */

/**
 * @addtogroup View
 * @{
 */

inline size_t
json_view_len(const DbrView& view) noexcept
{
    return dbr_json_view_len(&view);
}

inline char*
json_write_view(char* buf, const DbrView& view) noexcept
{
    return dbr_json_write_view(buf, &view);
}

inline std::string
json_write_view(const DbrView& view) noexcept
{
    std::string s;
    s.resize(dbr_json_view_len(&view) + 1);
    dbr_json_write_view(&s[0], &view);
    s.resize(s.size() - 1);
    return s;
}

/** @} */

} // dbr

#endif // DBRPP_ELM_JSON_HPP
