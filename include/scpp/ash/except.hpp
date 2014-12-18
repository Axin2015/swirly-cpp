/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ASH_EXCEPT_HPP
#define SCPP_ASH_EXCEPT_HPP

#include <sc/ash/err.h>

#include <cstring>
#include <stdexcept>

namespace sc {

/**
 * @addtogroup Err
 * @{
 */

class ScException : public std::exception {
    int num_;
    const char* file_;
    int line_;
    char msg_[SC_ERRMSG_MAX + 1];
public:
    virtual
    ~ScException() noexcept
    {
    }
    ScException(int num, const char* file, int line, const char* msg) noexcept
    {
        num_ = num;
        file_ = file;
        line_ = line;
        strncpy(msg_, msg, SC_ERRMSG_MAX);
        msg_[SC_ERRMSG_MAX] = '\0';
    }
    ScException() noexcept
        : ScException{sc_err_num(), sc_err_file(), sc_err_line(), sc_err_msg()}
    {
    }
    virtual const char*
    what() const noexcept
    {
        return msg_;
    }
    int
    num() const noexcept
    {
        return num_;
    }
    const char*
    file() const noexcept
    {
        return file_;
    }
    int
    line() const noexcept
    {
        return line_;
    }
};

template <int Num>
class BasicException : public ScException {
public:
    virtual
    ~BasicException() noexcept
    {
    }
    BasicException(const char* file, int line, const char* msg) noexcept
        : ScException{Num, file, line, msg}
    {
    }
};

typedef BasicException<SC_EINTR>    IntrException;
typedef BasicException<SC_EIO>      IoException;
typedef BasicException<SC_ENOMEM>   NoMemException;
typedef BasicException<SC_EACCES>   AccesException;
typedef BasicException<SC_EBUSY>    BusyException;
typedef BasicException<SC_EEXIST>   ExistException;
typedef BasicException<SC_EINVAL>   InvalException;
typedef BasicException<SC_ETIMEOUT> TimeoutException;
typedef BasicException<SC_ESYSTEM>  SystemException;

inline void
throw_exception(int num, const char* file, int line, const char* msg)
{
    switch (num) {
    case SC_EINTR:
        throw IntrException{file, line, msg};
        break;
    case SC_EIO:
        throw IoException{file, line, msg};
        break;
    case SC_ENOMEM:
        throw NoMemException{file, line, msg};
        break;
    case SC_EACCES:
        throw AccesException{file, line, msg};
        break;
    case SC_EBUSY:
        throw BusyException{file, line, msg};
        break;
    case SC_EEXIST:
        throw ExistException{file, line, msg};
        break;
    case SC_EINVAL:
        throw InvalException{file, line, msg};
        break;
    case SC_ETIMEOUT:
        throw TimeoutException{file, line, msg};
        break;
    case SC_ESYSTEM:
        throw SystemException{file, line, msg};
        break;
    default:
        throw ScException{num, file, line, msg};
        break;
    }
}

inline void
throw_exception()
{
    throw_exception(sc_err_num(), sc_err_file(), sc_err_line(), sc_err_msg());
}

/** @} */

} // sc

#endif // SCPP_ASH_EXCEPT_HPP
