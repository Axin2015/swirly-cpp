/*
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
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
#ifndef DBRPP_LEXER_HPP
#define DBRPP_LEXER_HPP

#include <dbrpp/except.hpp>

#include <dbr/lexer.h>

namespace dbr {

template <class DerivedT>
class LexerBase {
    DbrLexer lexer_;
    static void
    cb(void* ctx, const char* tok, size_t len) noexcept
    {
        static_cast<DerivedT*>(ctx)->cb(tok, len);
    }
protected:
    LexerBase() noexcept
    {
        dbr_lexer_init(&lexer_, cb, this);
    }
public:
    void
    reset() noexcept
    {
        dbr_lexer_reset(&lexer_);
    }
    void
    exec(const char* buf, size_t size)
    {
        if (!dbr_lexer_exec(&lexer_, buf, size))
            throw_exception();
    }
};
} // dbr

#endif // DBRPP_LEXER_HPP
