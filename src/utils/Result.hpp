#ifndef WR_UTILS_RESULT_HPP
#define WR_UTILS_RESULT_HPP

#include <variant>
#include <optional>

namespace wr
{

template <typename T, typename E>
struct Result
{
    static Result Ok(const T& t)
    {
        return Result{ true, t };
    }

    static Result Err(const E& e)
    {
        return Result{ false, e };
    }

    bool IsOk() const
    {
        return m_ok;
    }

    const T& Unwrap() const&
    {
        if (!m_ok)
        {
            throw std::runtime_error("Result::Unwrap() called on non-ok result");
        }
        return std::get<T>(m_data);
    }

    T Unwrap() &&
    {
        if (!m_ok)
        {
            throw std::runtime_error("Result::Unwrap() called on non-ok result");
        }
        return std::get<T>(std::move(m_data));
    }

    const E& UnwrapErr() const&
    {
        if (m_ok)
        {
            throw std::runtime_error("Result::UnwrapErr() called on ok result");
        }
        return std::get<E>(m_data);
    }

    std::optional<T> Ok() const&
    {
        return m_ok ? std::optional<T>(std::get<T>(m_data)) : std::nullopt;
    }

    std::optional<E> Err() const&
    {
        return !m_ok ? std::optional<E>(std::get<E>(m_data)) : std::nullopt;
    }

    bool               m_ok;
    std::variant<T, E> m_data;
};

} // namespace wr

#endif // WR_UTILS_RESULT_HPP
