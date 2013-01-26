#pragma warning (disable: 4995)
#pragma warning (disable: 4996)
#include "shiftDialog.h"

#include "plugin.hpp"
#include "farManager.h"
#include "ssaTime.h"
#include "helpContexts.h"
#include "language.h"

#include <algorithm>


const wchar_t* ShiftDialog::label0_;
const wchar_t* ShiftDialog::label1_;
const wchar_t* ShiftDialog::label2_;
wchar_t ShiftDialog::buf1[TIME_BUF_LEN];
wchar_t ShiftDialog::buf2[TIME_BUF_LEN];
const wchar_t* ShiftDialog::help_;
LONGLONG ShiftDialog::init1_;
LONGLONG ShiftDialog::init2_;
LONGLONG ShiftDialog::mediaTime_;
LONGLONG ShiftDialog::shift_;

ShiftDialog::ShiftDialog(const wchar_t* label0,
		const wchar_t* label1, LONGLONG init1,
		const wchar_t* label2, LONGLONG init2,
		LONGLONG mediaTime,
		const wchar_t* help)
{
	label0_ = label0;
	label1_ = label1;
	label2_ = label2;
	help_ = help;
	init1_ = init1;
	init2_ = init2;
	mediaTime_ = mediaTime;
	shift_ = 0;
}

bool ShiftDialog::execute()
{
	FarDialogItem items[] =
	{
		{ DI_TEXT,     3, 1, 15, 0, 0, NULL, NULL          ,              0, label0_                       , 0, 0, 0, 0},
		{ DI_FIXEDIT, 18, 1, 28, 0, 1, NULL, L"#9:99:99.99",   DIF_MASKEDIT, L" 0:00:00.00"                , 0, 0, 0, 0},
		{ DI_TEXT,     3, 2, 15, 0, 0, NULL, NULL          ,              0, label1_                       , 0, 0, 0, 0},
		{ DI_FIXEDIT, 19, 2, 28, 0, 0, NULL, L"9:99:99.99" ,   DIF_MASKEDIT, buf1                          , 0, 0, 0, 0},
		{ DI_TEXT,     3, 3, 15, 0, 0, NULL, NULL          ,              0, label2_                       , 0, 0, 0, 0},
		{ DI_FIXEDIT, 19, 3, 28, 0, 0, NULL, L"9:99:99.99" ,   DIF_MASKEDIT, buf2                          , 0, 0, 0, 0},
		{ DI_BUTTON,  32, 1, 43, 0, 0, NULL, NULL          , DIF_BTNNOCLOSE, *lng::timing::dialog::original, 0, 0, 0, 0},
		{ DI_BUTTON,  32, 2, 43, 0, 0, NULL, NULL          , DIF_BTNNOCLOSE, *lng::timing::dialog::current , 0, 0, 0, 0}
	};

	formatTime(init1_, (wchar_t*)items[3].Data, (wchar_t*)items[3].Data + 10);
	formatTime(init2_, (wchar_t*)items[5].Data, (wchar_t*)items[5].Data + 10);

	if (mediaTime_ < 0)
	{
		items[7].Flags |= DIF_DISABLE;
	}

	return -1 != farManager.dialogEx(&ShiftDialogId, items, COUNTOF(items), 46, 5, help_, &dialogProc);
}

void ShiftDialog::shiftChanged(HANDLE hDlg, const FarDialogItem& item)
{
	shift_ = parseTime(item.Data + 1, item.Data + 11);
	if (L'-' == item.Data[0]) shift_ = -shift_;

	wchar_t buf[11];

	formatTime((std::max)(0LL, shift_ + init1_), buf, buf + 10); buf[10] = 0;
	farManager.setTextPtr(hDlg, 3, buf);

	formatTime((std::max)(0LL, shift_ + init2_), buf, buf + 10);
	farManager.setTextPtr(hDlg, 5, buf);
}

void ShiftDialog::time1Changed(HANDLE hDlg, const FarDialogItem& item)
{
	LONGLONG time1 = parseTime(item.Data, item.Data + 10);
	shift_ = time1 - init1_;

	wchar_t buf[12];

	buf[0] = shift_ < 0 ? '-' : ' ';
	formatTime(_abs64(shift_), buf + 1, buf + 11);
	buf[11] = 0;
	farManager.setTextPtr(hDlg, 1, buf);

	formatTime((std::max)(0LL, shift_ + init2_), buf, buf + 10);
	buf[10] = 0;
	farManager.setTextPtr(hDlg, 5, buf);
}
void ShiftDialog::time2Changed(HANDLE hDlg, const FarDialogItem& item)
{
	LONGLONG time2 = parseTime(item.Data, item.Data + 10);
	shift_ = time2 - init2_;

	wchar_t buf[12];

	buf[0] = shift_ < 0 ? '-' : ' ';
	formatTime(_abs64(shift_), buf + 1, buf + 11);
	buf[11] = 0;
	farManager.setTextPtr(hDlg, 1, buf);

	formatTime((std::max)(0LL, shift_ + init1_), buf, buf + 10);
	buf[10] = 0;
	farManager.setTextPtr(hDlg, 3, buf);
}

void ShiftDialog::originalClick(HANDLE hDlg)
{
	shift_ = 0LL;
	farManager.setTextPtr(hDlg, 1, L" 0:00:00.00");

	wchar_t buf[11];

	formatTime(init1_, buf, buf + 10);
	buf[10] = 0;
	farManager.setTextPtr(hDlg, 3, buf);

	formatTime(init2_, buf, buf + 10);
	farManager.setTextPtr(hDlg, 5, buf);
}

void ShiftDialog::currentClick(HANDLE hDlg)
{
	shift_ = mediaTime_ - init1_;

	wchar_t buf[12];

	buf[0] = shift_ < 0 ? '-' : ' ';
	formatTime(_abs64(shift_), buf + 1, buf + 11);
	buf[11] = 0;
	farManager.setTextPtr(hDlg, 1, buf);

	formatTime((std::max)(0LL, shift_ + init1_), buf, buf + 10);
	buf[10] = 0;
	farManager.setTextPtr(hDlg, 3, buf);

	formatTime((std::max)(0LL, shift_ + init2_), buf, buf + 10);
	buf[10] = 0;
	farManager.setTextPtr(hDlg, 5, buf);
}

intptr_t WINAPI ShiftDialog::dialogProc(HANDLE hDlg, intptr_t Msg, intptr_t Param1, void *Param2)
{
	static bool bModif;

	switch (Msg)
	{
	case DN_EDITCHANGE:
		switch (Param1)
		{
		case 1:
			if (!bModif) {
				bModif = true;
				shiftChanged(hDlg, *reinterpret_cast<FarDialogItem*>(Param2));
				bModif = false;
			}
			break;
		case 3:
			if (!bModif) {
				bModif = true;
				time1Changed(hDlg, *reinterpret_cast<FarDialogItem*>(Param2));
				bModif = false;
			}
			break;
		case 5:
			if (!bModif) {
				bModif = true;
				time2Changed(hDlg, *reinterpret_cast<FarDialogItem*>(Param2));
				bModif = false;
			}
			break;
		}
		break;
	case DN_BTNCLICK:
		switch (Param1)
		{
		case 6:
			originalClick(hDlg);
			break;
		case 7:
			currentClick(hDlg);
			break;
		}
		break;
	}
	return farManager.defDlgProc(hDlg, Msg, Param1, Param2);
}
