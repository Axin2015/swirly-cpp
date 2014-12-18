/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ELM_JOURN_HPP
#define SCPP_ELM_JOURN_HPP

#include <scpp/elm/exec.hpp>
#include <scpp/elm/order.hpp>

#include <scpp/ash/except.hpp>

#include <sc/elm/journ.h>

#include <memory> // unique_ptr<>

namespace sc {

/**
 * @addtogroup Journ
 * @{
 */

struct JournDelete {
    void
    operator()(ScJourn journ) const noexcept
    {
        sc_journ_destroy(journ);
    }
};

typedef std::unique_ptr<ScIJourn, JournDelete> JournPtr;

template <class DerivedT>
class IJourn : public ScIJourn {
    static void
    destroy(ScJourn journ) noexcept
    {
        return static_cast<DerivedT*>(journ)->destroy();
    }
    static ScBool
    insert_exec_list(ScJourn journ, ScSlNode* first, ScBool enriched) noexcept
    {
        return static_cast<DerivedT*>(journ)->insert_exec_list(first, enriched);
    }
    static ScBool
    insert_exec(ScJourn journ, ScExec* exec, ScBool enriched) noexcept
    {
        return static_cast<DerivedT*>(journ)->insert_exec(*exec, enriched);
    }
    static ScBool
    update_exec(ScJourn journ, ScIden id, ScMillis modified) noexcept
    {
        return static_cast<DerivedT*>(journ)->update_exec(id, modified);
    }
    static const ScJournVtbl*
    vtbl() noexcept
    {
        static const ScJournVtbl VTBL = {
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
        : ScIJourn{vtbl()}
    {
    }
};

inline void
destroy(ScJourn journ)
{
    journ->vtbl->destroy(journ);
}

inline void
insert_exec_list(ScJourn journ, ScSlNode* first, ScBool enriched)
{
    if (!journ->vtbl->insert_exec_list(journ, first, enriched))
        throw_exception();
}

inline void
insert_exec(ScJourn journ, ScExec& exec, ScBool enriched)
{
    if (!journ->vtbl->insert_exec(journ, &exec, enriched))
        throw_exception();
}

inline void
update_exec(ScJourn journ, ScIden id, ScMillis modified)
{
    if (!journ->vtbl->update_exec(journ, id, modified))
        throw_exception();
}

/** @} */

} // sc

#endif // SCPP_ELM_JOURN_HPP
