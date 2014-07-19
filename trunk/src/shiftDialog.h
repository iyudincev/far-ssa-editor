#ifndef SHIFT_DIALOG_INCLUDED
#define SHIFT_DIALOG_INCLUDED

#include <windows.h>

#include "plugin.hpp"

class ShiftDialog
{
public:
	ShiftDialog(const wchar_t* label0,
		const wchar_t* label1, LONGLONG init1,
		const wchar_t* label2, LONGLONG init2,
		LONGLONG mediaTime,
		const wchar_t* help);
	bool execute();
	LONGLONG shift() const
	{
		return shift_;
	}
private:
	static const int TIME_BUF_LEN=512;
	static const wchar_t* label0_;
	static const wchar_t* label1_;
	static const wchar_t* label2_;
	static wchar_t buf1[TIME_BUF_LEN];
	static wchar_t buf2[TIME_BUF_LEN];
	static const wchar_t* help_;
	static LONGLONG init1_, init2_, mediaTime_, shift_;
	static intptr_t WINAPI dialogProc(HANDLE hDlg, intptr_t Msg, intptr_t Param1, void *Param2);

	static void shiftChanged(HANDLE hDlg, const FarDialogItem& item);
	static void time1Changed(HANDLE hDlg, const FarDialogItem& item);
	static void time2Changed(HANDLE hDlg, const FarDialogItem& item);

	static void originalClick(HANDLE hDlg);
	static void  currentClick(HANDLE hDlg);
};

#endif
