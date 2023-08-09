#pragma once

#include<vector>
#include<mutex>

template<class T>
class InfoStack
{
public:
    typedef std::vector<T> Cont;
private:
    Cont Store;
    std::mutex Lock;
public:
    Cont& GetStoreRaw()
    {
        return Store;
    }
    Cont GetCopy()
    {
        Lock.lock();
        Cont Ret = Store;
        Lock.unlock();
        return Ret;
    }
    void GetCopy(Cont& Target)
    {
        Lock.lock();
        Target = Store;
        Lock.unlock();
    }
    void SetCont(const Cont& Source)
    {
        Lock.lock();
        Store = Source;
        Lock.unlock();
    }
    void Clear()
    {
        Lock.lock();
        Store.clear();
        Lock.unlock();
    }
    Cont GetCopyAndClear()
    {
        Lock.lock();
        Cont Ret = Store;
        Store.clear();
        Lock.unlock();
        return Ret;
    }
    void GetCopyAndClear(Cont& Target)
    {
        Lock.lock();
        Target = Store;
        Store.clear();
        Lock.unlock();
    }
    void Push(const T& Ct)
    {
        Lock.lock();
        Store.push_back(Ct);
        Lock.unlock();
    }
};
