/*********************************************************************************
* Copyright(C), MIT
* FileName:    Observeable.hpp
* Author:      xxx
* Version :    Ver0.1
* Date:        2021-06-30
* Description: The declaration of class Observeable
*
*********************************************************************************/
#ifndef __OBSERVEABLE_HPP__
#define __OBSERVEABLE_HPP__

/*********************************************************************************
*                      Include                                                   *
**********************************************************************************/

#include <memory>
#include <type_traits>
#include <vector>

//TODO: #include <BaseMutexLock.h>

namespace COMMON
{

/*********************************************************************************
*                      Macro                                                     *
**********************************************************************************/


/*********************************************************************************
*                      Enum/Struct                                               *
**********************************************************************************/


/*********************************************************************************
*                      Variable                                                  *
**********************************************************************************/


/*********************************************************************************
*                      Function                                                  *
**********************************************************************************/

template<bool>
struct specialized_tag : ::std::false_type {};

template<>
struct specialized_tag<true>: ::std::true_type {};

template<bool threadSafe>
class ThreadSafeObserveableBase
{
public:
    mutable CBaseMutexLock m_Mutex;
};

template<>
class ThreadSafeObserveableBase<false>
{
    // Intentionally left blank.
};

template<typename> struct void_
{
    typedef void type;
};

template <typename T, typename = void>
struct HasPtr : public std::false_type { };

template <typename T>
struct HasPtr <T, typename void_<typename T::Ptr>::type> : public std::true_type { };

/**
 * @brief This class provides the base for observeable classes.
 * @tparam T Listener interface type.
 * @tparam threadSafe If true, method calls become threadsafe via a mutex guard.
 * @tparam TP Pointer type to T.
 * @note The template parameter TP needs to be a pointer to T (can be a smart pointer).
 *       If T proved a nested Ptr-typedef, it gets used for default.
 *
 * In order to implement the listener design pattern just derive a class from this one. It provides the
 * add- and removeListener methods and the stored listeners in the list m_listener.
 *
 * Example:
 * @code
 * class Listener
 * {
 * public:
 *   typedef ::std::shared_ptr<Listener> Ptr;
 *   virtual ~Listener() = default;
 *   virtual void notify() = 0;
 * };
 *
 * class MyObserveable : public Observeable<Listener>
 * {
 * public:
 *   void doSomething()
 *   {
 *      changeOrUpdateInternalState();
 *      notifyListeners(&Listener::notify);
 *   }
 * };
 * @endcode
 */
template<class T,
         bool threadSafe = false,
         class TP = typename ::std::conditional<HasPtr<T>::value, typename T::Ptr, T *>::type>
class Observeable : private ThreadSafeObserveableBase<threadSafe>
{
private:
    /// Defines the listener container.
    typedef ::std::vector<TP> Listeners_t;

    // private helper methods for tag dispatch, two helpers for each actual method.
    void addListenerHelper(::std::true_type, const TP listener);
    void addListenerHelper(::std::false_type, const TP listener);
    void removeListenerHelper(::std::true_type, const TP listener);
    void removeListenerHelper(::std::false_type, const TP listener);
    bool emptyHelper(::std::true_type) const;
    bool emptyHelper(::std::false_type) const;
    typename Listeners_t::iterator beginHelper(::std::true_type);
    typename Listeners_t::iterator beginHelper(::std::false_type);
    typename Listeners_t::iterator endHelper(::std::true_type);
    typename Listeners_t::iterator endHelper(::std::false_type);
    typename Listeners_t::const_iterator cbeginHelper(::std::true_type) const;
    typename Listeners_t::const_iterator cbeginHelper(::std::false_type) const;
    typename Listeners_t::const_iterator cendHelper(::std::true_type) const;
    typename Listeners_t::const_iterator cendHelper(::std::false_type) const;
    template<typename func, typename ...Args> void notifyListenerHelper(::std::true_type, func f, Args &&... args);
    template<typename func, typename ...Args> void notifyListenerHelper(::std::false_type, func f, Args &&... args);

    using spec_tag = typename specialized_tag<threadSafe>::type;

public:
    typedef ::std::shared_ptr<Observeable> Ptr;
    typedef TP ListenerPtr;

    /**
     * @brief Adds a listener.
     * @param[in] listener The listener that shall be added.
     * @note The notification order is the order in which the listeners were added. The same listener can be registered multiple times.
     */
    void addListener(const TP listener)
    {
        addListenerHelper(spec_tag{}, listener);
    }

    /**
     * @brief Remove listener.
     * @param[in] listener The listener that shall be removed.
     */
    void removeListener(const TP listener)
    {
        removeListenerHelper(spec_tag{}, listener);
    }

    /**
     * @brief Returns true if there are no listeners, else false.
     */
    bool empty() const
    {
        return emptyHelper(spec_tag{});
    }

    /**
     * @brief Returns an iterator pointing to the first listener.
     */
    typename Listeners_t::iterator begin()
    {
        return beginHelper(spec_tag{});
    }

    /**
     * @brief Returns an iterator pointing to the first listener.
     */
    const typename Listeners_t::iterator begin() const
    {
        auto *p = const_cast<Observeable<T, threadSafe, TP>*>(this);
        return p->beginHelper(spec_tag{});
    }

    /**
     * @brief Returns an iterator referring to the past-the-end element in the listener vector container.
     */
    typename Listeners_t::iterator end()
    {
        return endHelper(spec_tag{});
    }

    /**
     * @brief Returns an iterator referring to the past-the-end element in the listener vector container.
     */
    const typename Listeners_t::iterator end() const
    {
        auto *p = const_cast<Observeable<T, threadSafe, TP>*>(this);
        return p->endHelper(spec_tag{});
    }

    /**
     * @brief Returns a const iterator pointing to the first listener.
     */
    typename Listeners_t::const_iterator cbegin() const
    {
        return cbeginHelper(spec_tag{});
    }

    /**
     * @brief Returns a const iterator referring to the past-the-end element in the listener vector container.
     */
    typename Listeners_t::const_iterator cend() const
    {
        return cendHelper(spec_tag{});
    }

protected:
    /** @brief Notifies all listeners.
      * @param[in] f    Pointer to method of template class T.
      * @param[in] args The arguments that shall be forwarded to the function.
     */
    template<typename func, typename ...Args>
    void notifyListener(func f, Args &&... args)
    {
        notifyListenerHelper(spec_tag{}, f, ::std::forward<Args>(args)...);
    }

private:
    /// Stores the listeners.
    Listeners_t m_Listeners;
};

template<class T, bool threadSafe, class TP>
void Observeable<T, threadSafe, TP>::addListenerHelper(::std::true_type, const TP listener)
{
    CAutoLock al(this->m_Mutex);
    addListenerHelper(::std::false_type{}, listener);
}

template<class T, bool threadSafe, class TP>
void Observeable<T, threadSafe, TP>::addListenerHelper(::std::false_type, const TP listener)
{
    if (nullptr != listener)
    {
        m_Listeners.push_back(listener);
    }
}

template<class T, bool threadSafe, class TP>
void Observeable<T, threadSafe, TP>::removeListenerHelper(::std::true_type, const TP listener)
{
    CAutoLock al(this->m_Mutex);
    removeListenerHelper(::std::false_type{}, listener);
}

template<class T, bool threadSafe, class TP>
void Observeable<T, threadSafe, TP>::removeListenerHelper(::std::false_type, const TP listener)
{
    typename Listeners_t::iterator it = m_Listeners.begin();

    auto end = m_Listeners.end();
    for (; it != end; ++it)
    {
        if (*it == listener)
        {
            m_Listeners.erase(it);
            break;
        }
    }
}

template<class T, bool threadSafe, class TP>
bool Observeable<T, threadSafe, TP>::emptyHelper(::std::true_type) const
{
    CAutoLock al(this->m_Mutex);
    return emptyHelper(::std::false_type{});
}

template<class T, bool threadSafe, class TP>
bool Observeable<T, threadSafe, TP>::emptyHelper(::std::false_type) const
{
    return m_Listeners.empty();
}

template<class T, bool threadSafe, class TP>
typename Observeable<T, threadSafe, TP>::Listeners_t::iterator Observeable<T, threadSafe, TP>::beginHelper(::std::true_type)
{
    CAutoLock al(this->m_Mutex);
    return beginHelper(::std::false_type{});
}

template<class T, bool threadSafe, class TP>
typename Observeable<T, threadSafe, TP>::Listeners_t::iterator Observeable<T, threadSafe, TP>::beginHelper(::std::false_type)
{
    return m_Listeners.begin();
}

template<class T, bool threadSafe, class TP>
typename Observeable<T, threadSafe, TP>::Listeners_t::iterator Observeable<T, threadSafe, TP>::endHelper(::std::true_type)
{
    CAutoLock al(this->m_Mutex);
    return endHelper(::std::false_type{});
}

template<class T, bool threadSafe, class TP>
typename Observeable<T, threadSafe, TP>::Listeners_t::iterator Observeable<T, threadSafe, TP>::endHelper(::std::false_type)
{
    return m_Listeners.end();
}

template<class T, bool threadSafe, class TP>
typename Observeable<T, threadSafe, TP>::Listeners_t::const_iterator Observeable<T, threadSafe, TP>::cbeginHelper(::std::true_type) const
{
    CAutoLock al(this->m_Mutex);
    return cbeginHelper(::std::false_type{});
}

template<class T, bool threadSafe, class TP>
typename Observeable<T, threadSafe, TP>::Listeners_t::const_iterator Observeable<T, threadSafe, TP>::cbeginHelper(::std::false_type) const
{
    return m_Listeners.cbegin();
}

template<class T, bool threadSafe, class TP>
typename Observeable<T, threadSafe, TP>::Listeners_t::const_iterator Observeable<T, threadSafe, TP>::cendHelper(::std::true_type) const
{
    CAutoLock al(this->m_Mutex);
    return cendHelper(::std::false_type{});
}

template<class T, bool threadSafe, class TP>
typename Observeable<T, threadSafe, TP>::Listeners_t::const_iterator Observeable<T, threadSafe, TP>::cendHelper(::std::false_type) const
{
    return m_Listeners.cend();
}

template<class T, bool threadSafe, class TP>
template<typename func, typename ...Args>
void Observeable<T, threadSafe, TP>::notifyListenerHelper(::std::true_type, func f, Args &&... args)
{
    CAutoLock al(this->m_Mutex);
    notifyListenerHelper(::std::false_type{}, f, ::std::forward<Args>(args)...);
}

template<class T, bool threadSafe, class TP>
template<typename func, typename ...Args>
void Observeable<T, threadSafe, TP>::notifyListenerHelper(::std::false_type, func f, Args &&... args)
{
    for (auto it : m_Listeners)
    {
        ((*it).*f)(::std::forward<Args>(args)...);
    }
}

} /*COMMON*/
#endif //__OBSERVEABLE_HPP__
