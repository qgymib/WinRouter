#ifndef WR_UTILS_MUTEX_HPP
#define WR_UTILS_MUTEX_HPP

namespace wr
{

class Lock
{
public:
    Lock();
    ~Lock();

    void Enter();
    void Leave();

private:
    struct Data;
    Data* m_data;
};

template <typename T>
class Mutex
{
public:
    class Locker
    {
        friend class Mutex;

    protected:
        Locker(Lock* l, T* t) : m_lock(l), m_data(t)
        {
        }
        Locker(Locker& orig) = delete;
        Locker(Locker&&) = default;
        Locker& operator=(Locker&&) = default;

    public:
        ~Locker()
        {
            Drop();
        }

        T* operator->() const
        {
            return m_data;
        }

        void Drop()
        {
            if (m_lock != nullptr)
            {
                m_lock->Leave();
                m_lock = nullptr;
            }
            m_data = nullptr;
        }

    private:
        wr::Lock* m_lock;
        T*        m_data;
    };

public:
    Mutex() = default;

    template <typename... Args>
    explicit Mutex(Args&&... args) : m_data(std::forward<Args>(args)...)
    {
    }

    Locker Lock()
    {
        m_lock.Enter();
        return Locker(&m_lock, &m_data);
    }

private:
    wr::Lock m_lock;
    T        m_data;
};

} // namespace wr

#endif // WR_UTILS_MUTEX_HPP
