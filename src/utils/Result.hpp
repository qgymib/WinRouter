#ifndef WR_UTILS_RESULT_HPP
#define WR_UTILS_RESULT_HPP

#include <variant>
#include <optional>

namespace wr
{

/**
 * @brief Generic result type.
 * @tparam[in] T Result type.
 * @tparam[in] E Error type.
 */
template <typename T, typename E>
class Result
{
protected:
    struct ok_tag
    {
    };
    struct err_tag
    {
    };

    Result(ok_tag, const T& t) : m_ok(true), m_data{ std::in_place_index<0>, t }
    {
    }
    Result(err_tag, const E& e) : m_ok(false), m_data{ std::in_place_index<1>, e }
    {
    }

public:
    static Result Ok(const T& t)
    {
        return Result(ok_tag{}, t);
    }

    static Result Err(const E& e)
    {
        return Result(err_tag{}, e);
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
        return std::get<0>(m_data);
    }

    T Unwrap() &&
    {
        if (!m_ok)
        {
            throw std::runtime_error("Result::Unwrap() called on non-ok result");
        }
        return std::get<0>(std::move(m_data));
    }

    const E& UnwrapErr() const&
    {
        if (m_ok)
        {
            throw std::runtime_error("Result::UnwrapErr() called on ok result");
        }
        return std::get<1>(m_data);
    }

    E UnwrapErr() &&
    {
        if (m_ok)
        {
            throw std::runtime_error("Result::UnwrapErr() called on ok result");
        }
        return std::get<1>(std::move(m_data));
    }

    std::optional<T> Ok() const&
    {
        return m_ok ? std::optional<T>(std::get<0>(m_data)) : std::nullopt;
    }

    std::optional<E> Err() const&
    {
        return !m_ok ? std::optional<E>(std::get<1>(m_data)) : std::nullopt;
    }

protected:
    bool               m_ok;
    std::variant<T, E> m_data;
};

} // namespace wr

#endif // WR_UTILS_RESULT_HPP
