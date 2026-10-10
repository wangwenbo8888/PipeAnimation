

//----------------------------------------------------------------------------
template <class T>
TList<T>::TList ()
{
    m_pkNext = 0;
}
//----------------------------------------------------------------------------
template <class T>
TList<T>::TList (const T& rtItem, TList* pkNext)
    :
    m_tItem(rtItem)
{
    m_pkNext = pkNext;
}
//----------------------------------------------------------------------------
template <class T>
TList<T>::~TList ()
{
}
//----------------------------------------------------------------------------
template <class T>
void TList<T>::SetItem (const T& rtItem)
{
    m_tItem = rtItem;
}
//----------------------------------------------------------------------------
template <class T>
T& TList<T>::Item ()
{
    return m_tItem;
}
//----------------------------------------------------------------------------
template <class T>
const T& TList<T>::GetItem () const
{
    return m_tItem;
}
//----------------------------------------------------------------------------
template <class T>
void TList<T>::SetNext (TList* pkNext)
{
    m_pkNext = pkNext;
}
//----------------------------------------------------------------------------
template <class T>
TList<T>*& TList<T>::Next ()
{
    return m_pkNext;
}
//----------------------------------------------------------------------------
template <class T>
const TList<T>* TList<T>::GetNext () const
{
    return m_pkNext;
}
//----------------------------------------------------------------------------
template <class T>
int TList<T>::GetQuantity () const
{
    int iQuantity = 0;
    for (const TList<T>* pkIter = this; pkIter; pkIter = pkIter->m_pkNext)
        iQuantity++;
    return iQuantity;
}
//重载取址操作符，主要是用于stream读取数据时的读写合并操作
//----------------------------------------------------------------------------
template <class T>
TList<T>::operator const T* () const
{
	return m_tItem;
}
//----------------------------------------------------------------------------
template <class T>
TList<T>::operator T* ()
{
	return m_tItem;
}
//----------------------------------------------------------------------------