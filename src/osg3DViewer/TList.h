

#ifndef TLIST_H
#define TLIST_H

// The class T is either native data or is class data that has the following
// member functions:
//   T::T ()
//   T::T (const T&);
//   T& T::operator= (const T&)



template <class T>
class TList
{
public:
    // construction
    TList ();
    TList (const T& rtItem, TList* pkNext);
    ~TList ();

    // member access
    void SetItem (const T& rtItem);
    T& Item ();
    const T& GetItem () const;
    void SetNext (TList* pkNext);
    TList*& Next ();
    const TList* GetNext () const;
    
    //重载取址操作符，主要是用于stream读取数据时的读写合并操作    
    operator const T* () const;
    operator T* ();
    
    // total number of nodes in the list, including this one
    int GetQuantity () const;

private:
    T m_tItem;
    TList* m_pkNext;
};

#include "TList.inl"



#endif
