#ifndef FIND_WINDOW_INCLUDED
#define FIND_WINDOW_INCLUDED

#include <windows.h>
#include <string.h>
#include "exception.h"
#include "countof.h"

class FindMpcWindowByPid
{
public:
	explicit FindMpcWindowByPid(DWORD pid)
	: pid_(pid)
	{}

	bool run()
	{
		if (0 == EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this)))
		{
			DWORD code = GetLastError();
			if (0 == code)
			{
				return true;
			}
			throw Win32Exception(__FILE__ ":" STRINGIZE(__LINE__) ": Error enumerating windows", code);
		}
		return false; // not found
	}

	HWND hwnd() const
	{
		return hwnd_;
	}

private:
	DWORD pid_;
	HWND hwnd_;

	static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
	{
		return reinterpret_cast<FindMpcWindowByPid*>(lParam)->isMpcByPid(hWnd);
	}

	bool isMpcByPid(HWND hWnd)
	{
		DWORD pid;
		GetWindowThreadProcessId(hWnd, &pid);
		if (pid != pid_) return true;

		wchar_t type[20];
		ASSERT_NE(RealGetWindowClass(hWnd, type, COUNTOF(type)), 0, "Cannot get window class name");
		type[COUNTOF(type) - 1] = 0;
		//                     0----5---10----5---20
		if (0 != wcscmp(type, L"MediaPlayerClassicW")) return true;

		hwnd_ = hWnd;
		SetLastError(0);
		return false;
	}

};

#endif
