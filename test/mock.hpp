/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef MOCK_HPP
#define MOCK_HPP

#include <scpp/elm/journ.hpp>
#include <scpp/elm/model.hpp>

class Journ : public sc::IJourn<Journ> {
public:
    void
    destroy() noexcept;

    ScBool
    insert_exec_list(ScSlNode* first, ScBool enriched) noexcept;

    ScBool
    insert_exec(ScExec& exec, ScBool enriched) noexcept;

    ScBool
    update_exec(ScIden id, ScMillis modified) noexcept;
};

class Model : public sc::IModel<Model> {
public:
    void
    destroy() noexcept;

    ssize_t
    read_entity(int type, ScPool pool, ScSlNode*& first) noexcept;
};

#endif // MOCK_HPP
