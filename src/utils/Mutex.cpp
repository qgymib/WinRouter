#include "utils/win32.hpp"
#include "Mutex.hpp"

struct wr::Lock::Data
{
    Data();
    ~Data();

    CRITICAL_SECTION criticalSection;
};

wr::Lock::Data::Data()
{
    InitializeCriticalSection(&criticalSection);
}

wr::Lock::Data::~Data()
{
    DeleteCriticalSection(&criticalSection);
}

wr::Lock::Lock()
{
    m_data = new Data;
}

wr::Lock::~Lock()
{
    delete m_data;
}

void wr::Lock::Enter()
{
    EnterCriticalSection(&m_data->criticalSection);
}

void wr::Lock::Leave()
{
    LeaveCriticalSection(&m_data->criticalSection);
}
