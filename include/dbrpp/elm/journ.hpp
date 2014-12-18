/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_ELM_JOURN_HPP
#define DBRPP_ELM_JOURN_HPP

#include <dbrpp/elm/exec.hpp>
#include <dbrpp/elm/order.hpp>

#include <dbrpp/ash/except.hpp>

#include <dbr/elm/journ.h>

#include <memory> // unique_ptr<>

namespace dbr {

/**
 * @addtogroup Journ
 * @{
 */

struct JournDelete {
    void
    operator()(DbrJourn journ) const noexcept
    {
        dbr_journ_destroy(journ);
    }
};

typedef std::unique_ptr<DbrIJourn, JournDelete> JournPtr;

template <class DerivedT>
class IJourn : public DbrIJourn {
    static void
    destroy(DbrJourn journ) noexcept
    {
        return static_cast<DerivedT*>(journ)->destroy();
    }
    static DbrBool
    insert_exec_list(DbrJourn journ, DbrSlNode* first, DbrBool enriched) noexcept
    {
        return static_cast<DerivedT*>(journ)->insert_exec_list(first, enriched);
    }
    static DbrBool
    insert_exec(DbrJourn journ, DbrExec* exec, DbrBool enriched) noexcept
    {
        return static_cast<DerivedT*>(journ)->insert_exec(*exec, enriched);
    }
    static DbrBool
    update_exec(DbrJourn journ, DbrIden id, DbrMillis modified) noexcept
    {
        return static_cast<DerivedT*>(journ)->update_exec(id, modified);
    }
    static const DbrJournVtbl*
    vtbl() noexcept
    {
        static const DbrJournVtbl VTBL = {
            destroy,
            insert_exec_list,
            insert_exec,
            update_exec
        };
        return &VTBL;
    }
protected:
    ~IJourn() noexcept
    {
    }
public:
    IJourn() noexcept
        : DbrIJourn{vtbl()}
    {
    }
};

inline void
destroy(DbrJourn journ)
{
    journ->vtbl->destroy(journ);
}

inline void
insert_exec_list(DbrJourn journ, DbrSlNode* first, DbrBool enriched)
{
    if (!journ->vtbl->insert_exec_list(journ, first, enriched))
        throw_exception();
}

inline void
insert_exec(DbrJourn journ, DbrExec& exec, DbrBool enriched)
{
    if (!journ->vtbl->insert_exec(journ, &exec, enriched))
        throw_exception();
}

inline void
update_exec(DbrJourn journ, DbrIden id, DbrMillis modified)
{
    if (!journ->vtbl->update_exec(journ, id, modified))
        throw_exception();
}

/** @} */

} // dbr

#endif // DBRPP_ELM_JOURN_HPP
