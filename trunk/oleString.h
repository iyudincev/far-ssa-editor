#ifndef OLE_STRING_INCLUDED
#define OLE_STRING_INCLUDED

#include <windows.h>
#include <dshow.h>
#include <atlbase.h>
#include <objidl.h>
#include <objbase.h>

#include <stdexcept>

class OleString
{
private:
	const wchar_t* begin_;
	mutable bool isValid_;
	mutable const wchar_t* end_;
public:
	OleString()
	: begin_(0), isValid_(false), end_(0)
	{}

	~OleString()
	{
		CoTaskMemFree(const_cast<wchar_t*>(begin_));
	}

	wchar_t** operator&()
	{
		if (0 != begin_) throw std::logic_error("OleString not empty");
		isValid_ = false;
		return const_cast<wchar_t**>(&begin_);
	}

	const wchar_t* begin() const
	{
		return begin_;
	}

	const wchar_t* end() const
	{
		if (!isValid_)
		{
			validate();
		}
		return end_;
	}

	const size_t size() const
	{
		return end() - begin();
	}

private:
	void validate() const
	{
		for (end_ = begin_; 0 != *end_; ++end_);
	}
};

#endif
