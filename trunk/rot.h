#ifndef ROT_INCLUDED
#define ROT_INCLUDED

#include "exception.h"
#include <objidl.h>
#include <atlbase.h>
#include <objbase.h>

class RunningObjectTable
{
public:
	RunningObjectTable()
	{
		ASSERT_SUCCESS(GetRunningObjectTable(0, &pROT), "Cannot get Running Objects Table");
	}

	CComPtr<IEnumMoniker> EnumRunning() const
	{
		CComPtr<IEnumMoniker> result;
		ASSERT_SUCCESS(pROT->EnumRunning(&result), "Cannot get ROT enumerator");
		return result;
	}

	CComPtr<IUnknown> GetObject(CComPtr<IMoniker> pMoniker) const
	{
		CComPtr<IUnknown> result;
		ASSERT_SUCCESS(pROT->GetObject(pMoniker, &result), "Cannot get object from ROT");
		return result;
	}
private:
	CComPtr<IRunningObjectTable> pROT;
};

#endif
