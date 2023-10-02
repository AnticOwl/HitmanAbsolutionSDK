#include <Glacier/Entity/ZObjectRef.h>

ZObjectRef::ZObjectRef(void* pObject, STypeID* const typeID)
{
	m_TypeID = typeID;
	m_pData = pObject;
}

STypeID* ZObjectRef::GetTypeID() const
{
	return m_TypeID;
}

void* ZObjectRef::GetData() const
{
	return m_pData;
}

const IType* ZObjectRef::GetType() const
{
	return m_TypeID->pTypeInfo;
}
