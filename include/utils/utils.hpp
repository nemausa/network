/**
* @file utils.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-08-08-14-31
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef UTILS
#define UTILS

#include <memory>
#include <mutex>


template<typename T>
class singleton
{
public:
    static T& instance()
    {
        static T instance;
        return instance;
    }

    singleton(T&&) = delete;
    singleton(const T&) = delete;
    void operator= (const T&) = delete;

protected:
    singleton() = default;
    virtual ~singleton() = default;
};


template<typename T, bool is_thread_safe = true>
class lazy_singleton
{
private:
    static std::unique_ptr<T> t_;
    static std::mutex mtx_;

public:
    static T& instance()
    {
        if (is_thread_safe == false)
        {
            if (t_ == nullptr)
                t_ = std::unique_ptr<T>(new T);
            return *t_;
        }

        if (t_ == nullptr)
        {
            std::unique_lock<std::mutex> unique_locker(mtx_);
            if (t_ == nullptr)
                t_ = std::unique_ptr<T>(new T);
            return *t_;
        }

    }

    lazy_singleton(T&&) = delete;
    lazy_singleton(const T&) = delete;
    void operator= (const T&) = delete;

protected:
    lazy_singleton() = default;
    virtual ~lazy_singleton() = default;
};

template<typename T, bool is_thread_safe>
std::unique_ptr<T> lazy_singleton<T, is_thread_safe>::t_;

template<typename T, bool is_thread_safe>
std::mutex lazy_singleton<T, is_thread_safe>::mtx_;

template<typename T>
class eager_singleton
{
private:
    static T* t_;

public:
    static T& GetInstance()
    {
        return *t_;
    }

    eager_singleton(T&&) = delete;
    eager_singleton(const T&) = delete;
    void operator= (const T&) = delete;

protected:
    eager_singleton() = default;
    virtual ~eager_singleton() = default;
};

template<typename T>
T* eager_singleton<T>::t_ = new (std::nothrow) T;

#endif // UTILS