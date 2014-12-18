/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef MOCK_HPP
#define MOCK_HPP

#include <dbrpp/elm/journ.hpp>
#include <dbrpp/elm/model.hpp>

class Journ : public dbr::IJourn<Journ> {
public:
    void
    destroy() noexcept;

    DbrBool
    insert_exec_list(DbrSlNode* first, DbrBool enriched) noexcept;

    DbrBool
    insert_exec(DbrExec& exec, DbrBool enriched) noexcept;

    DbrBool
    update_exec(DbrIden id, DbrMillis modified) noexcept;
};

class Model : public dbr::IModel<Model> {
public:
    void
    destroy() noexcept;

    ssize_t
    read_entity(int type, DbrPool pool, DbrSlNode*& first) noexcept;
};

#endif // MOCK_HPP
