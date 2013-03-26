#ifndef EXCEPTION_INCLUDED
#define EXCEPTION_INCLUDED

#include <exception>
#include <stdexcept>
#include <string>


class Win32Exception : public std::runtime_error
{
public:
	Win32Exception(const std::string& what_arg)
	: std::runtime_error(what_arg), code_(GetLastError())
	{
	}

	Win32Exception(const std::string& what_arg, HRESULT hResult)
	: std::runtime_error(what_arg), code_(hResult)
	{
	}

	virtual const char* what() const throw()
	{
		static char message[4096];
		char* end = message + sizeof(message);
		std::strncpy(message, std::runtime_error::what(), sizeof(message) - 1);
		*(end - 1) = 0;

		char* cur = message + std::strlen(message);
		*cur++ = ':';
		*cur++ = ' ';

		if (!FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, code_, 0,
			cur, static_cast<DWORD>(end - cur), 0))
		{
			_snprintf(cur, end - cur - 1, "Error code %u", code_);
			*(end - 1) = 0;
		}

		return message;
	}
private:
	DWORD code_;
};

template <typename Value, typename ErrorValue>
Value assertNotEqual(Value result, ErrorValue errorValue, const char* message)
{
	if (result == errorValue)
	{
		throw Win32Exception(message);
	}
	return result;
}

inline HRESULT assertSuccess(HRESULT hResult, const char* message)
{
	if (FAILED(hResult))
	{
		throw Win32Exception(message, hResult);
	}
	return hResult;
}

inline intptr_t assertFar(intptr_t result, const char* message)
{
	if (!result)
	{
		throw std::runtime_error(message);
	}
	return result;
}

#define STRINGIZE1(x) #x
#define STRINGIZE(x) STRINGIZE1(x)

#ifndef YK_UNCHECKED

#define ASSERT_NE(result, errorValue, message) assertNotEqual(result, errorValue, __FILE__ ":" STRINGIZE(__LINE__) ": " message)
#define ASSERT_SUCCESS(hResult, message) assertSuccess(hResult, __FILE__ ":" STRINGIZE(__LINE__) ": " message)
#define ASSERT_FAR(result, message) assertFar(result, __FILE__ ":" STRINGIZE(__LINE__) ": " message)

#else

#define ASSERT_NE(result, errorValue, message) static_cast<void>(result)
#define ASSERT_SUCCESS(hResult, message) static_cast<void>(hResult)
#define ASSERT_FAR(result, message) static_cast<void>(result)

#endif

#endif
