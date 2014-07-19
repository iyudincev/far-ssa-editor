#ifndef FAR_MANAGER_INLINE_INCLUDED
#define FAR_MANAGER_INLINE_INCLUDED

#include "plugin.hpp"
#include "countof.h"
#include <initguid.h>

// {1119EEE2-680A-49E4-9E61-566E8F4F27B2}
DEFINE_GUID(PluginId,
0x1119eee2, 0x680a, 0x49e4, 0x9e, 0x61, 0x56, 0x6e, 0x8f, 0x4f, 0x27, 0xb2);

// {6F01DDFD-05ED-4F0E-84E6-BCA4C96B7F09}
DEFINE_GUID(InputTimeId,
0x6f01ddfd, 0x5ed, 0x4f0e, 0x84, 0xe6, 0xbc, 0xa4, 0xc9, 0x6b, 0x7f, 0x9);

// {B1C99A3D-CC83-4D65-BFEA-51BA1153BA43}
DEFINE_GUID(EditPhraseId,
0xb1c99a3d, 0xcc83, 0x4d65, 0xbf, 0xea, 0x51, 0xba, 0x11, 0x53, 0xba, 0x43);

// {16F6E715-3467-4149-B798-A66773672C86}
DEFINE_GUID(AttachMenuId,
0x16f6e715, 0x3467, 0x4149, 0xb7, 0x98, 0xa6, 0x67, 0x73, 0x67, 0x2c, 0x86);

// {FEFB637C-39A9-4735-AE1D-E47FE0B9B049}
DEFINE_GUID(SeekMenuId,
0xfefb637c, 0x39a9, 0x4735, 0xae, 0x1d, 0xe4, 0x7f, 0xe0, 0xb9, 0xb0, 0x49);

// {68AE706B-CF48-430C-BEEF-A95880BE3DBF}
DEFINE_GUID(PhraseMenuId,
0x68ae706b, 0xcf48, 0x430c, 0xbe, 0xef, 0xa9, 0x58, 0x80, 0xbe, 0x3d, 0xbf);

// {0A45475E-D8BC-41D3-A269-483F02439AA3}
DEFINE_GUID(ShowMessageId,
0xa45475e, 0xd8bc, 0x41d3, 0xa2, 0x69, 0x48, 0x3f, 0x2, 0x43, 0x9a, 0xa3);

// {C5A1EA86-8011-4ECD-879E-75724D660CCD}
DEFINE_GUID(StyleId,
0xc5a1ea86, 0x8011, 0x4ecd, 0x87, 0x9e, 0x75, 0x72, 0x4d, 0x66, 0xc, 0xcd);

// {164E472C-1C70-4311-B342-ADF32BD63B8C}
DEFINE_GUID(TimingMenuId,
0x164e472c, 0x1c70, 0x4311, 0xb3, 0x42, 0xad, 0xf3, 0x2b, 0xd6, 0x3b, 0x8c);

// {83AA6C54-F456-4450-A362-8D54E045D3BE}
DEFINE_GUID(ShiftMenuId,
0x83aa6c54, 0xf456, 0x4450, 0xa3, 0x62, 0x8d, 0x54, 0xe0, 0x45, 0xd3, 0xbe);

// {3AD1A000-DC71-482C-AF09-95E27266B8E3}
DEFINE_GUID(MainMenuId,
0x3ad1a000, 0xdc71, 0x482c, 0xaf, 0x9, 0x95, 0xe2, 0x72, 0x66, 0xb8, 0xe3);

// {94FBAA9E-B416-4A4B-AE57-E9F0D298FF12}
DEFINE_GUID(ErrorMsgId,
0x94fbaa9e, 0xb416, 0x4a4b, 0xae, 0x57, 0xe9, 0xf0, 0xd2, 0x98, 0xff, 0x12);

// {E87C3852-ABDA-43BA-B183-48BD5AD425AF}
DEFINE_GUID(ShiftDialogId,
0xe87c3852, 0xabda, 0x43ba, 0xb1, 0x83, 0x48, 0xbd, 0x5a, 0xd4, 0x25, 0xaf);


inline
std::wstring
widen( const std::string& in, std::locale loc  = std::locale() )
{
    std::wstring out( in.length(), 0 );
    std::string::const_iterator i = in.begin(), ie = in.end();
    std::wstring::iterator j = out.begin();

    for( ; i!=ie; ++i, ++j )
        *j = std::use_facet< std::ctype< wchar_t > > ( loc ).widen( *i );

    return out;
}

inline
FarMenuItem
menuItem(const wchar_t* title, bool selected, bool checked)
{
	FarMenuItem result = {
		(selected ? MIF_SELECTED : 0) |(checked ? MIF_CHECKED : 0),
		L"",
		{0, 0},
		NULL,
		{NULL, NULL}
	};
	result.Text = title;
	return result;
}

inline
EnableIf::EnableIf(bool condition)
: condition_(condition)
{
}

inline
void
EnableIf::operator()(FarMenuItem& item) const
{
	item.Flags |= condition_ ? 0 : MIF_DISABLE;
}

inline
CheckIf::CheckIf(bool condition)
: condition_(condition)
{
}

inline
void
CheckIf::operator()(FarMenuItem& item) const
{
	item.Flags |= condition_ ? MIF_CHECKED : 0;
}

inline
SelectIf::SelectIf(bool condition)
: condition_(condition)
{
}

inline
void
SelectIf::operator()(FarMenuItem& item) const
{
	item.Flags |= condition_ ? (MIF_SELECTED | MIF_CHECKED) : 0;
}

inline
FarMenuItems&
FarMenuItems::operator()(const wchar_t* title)
{
	resize(size() + 1);
	back().Text = title;
	back().Flags &= ~(MIF_SELECTED | MIF_CHECKED | MIF_SEPARATOR);
	return *this;
}

inline
FarMenuItems&
FarMenuItems::operator()(const std::string& text)
{
	texts.push_back(widen(text));

	resize(size() + 1);
	back().Text = texts.back().c_str();
	back().Flags &= ~(MIF_SELECTED | MIF_CHECKED | MIF_SEPARATOR);
	return *this;
}

inline
FarMenuItems&
FarMenuItems::operator()(LanguageString title)
{
	return (*this)(title.get());
}

inline
FarMenuItems&
FarMenuItems::operator()()
{
	push_back(menuSeparator);
	return *this;
}

template <typename Functor>
FarMenuItems&
FarMenuItems::operator()(const Functor& functor)
{
	functor(back());
	return *this;
}

inline
LanguageString::LanguageString(intptr_t id)
: id_(id)
{
}

inline
LanguageString::operator const wchar_t*() const
{
	return get();
}

inline
const wchar_t* LanguageString::get() const
{
	return farManager.getMsg(id_);
}

inline
FarManager&
FarManager::operator=(const PluginStartupInfo& other)
{
	PluginStartupInfo::operator=(other);
	return *this;
}

inline
const wchar_t*
FarManager::moduleName() const
{
	return ModuleName;
}

inline
intptr_t
FarManager::editorControl(
	enum EDITOR_CONTROL_COMMANDS Command,
	intptr_t Param1,
	void* Param2)
{
	return EditorControl(-1, Command, Param1, Param2);
}

inline
intptr_t
FarManager::inputBox(const GUID* id,
	const wchar_t* title, const wchar_t* prompt, const wchar_t* history,
	const wchar_t* srcText, wchar_t* destText, size_t destLength,
	const wchar_t* help,
	DWORD flags)
{
	return InputBox(&PluginId, id, title, prompt, history,
		srcText, destText, destLength,
		help,
		flags);
}

inline
intptr_t
FarManager::menu(const GUID* id,
	const FarMenuItem* items, size_t itemCount,
	const wchar_t* title,
	const wchar_t* bottom,
	const wchar_t* help,
	const FarKey* breakKeys,
	intptr_t* breakCode)
{
	return Menu(&PluginId, id,
		-1, -1, 0,
		FMENU_WRAPMODE,
		title,
		bottom,
		help,
		breakKeys, breakCode,
		items, itemCount);
}

inline
intptr_t
FarManager::menu(const GUID* id,
	const FarMenuItems& items,
	const wchar_t* title,
	const wchar_t* bottom,
	const wchar_t* help,
	const FarKey* breakKeys,
	intptr_t* breakCode)
{
	return menu(id, &items[0], items.size(), title, bottom, help, breakKeys, breakCode);
}

inline
intptr_t
FarManager::dialog(const GUID* id,
	FarDialogItem* items, size_t itemCount,
	intptr_t width, intptr_t height,
	const wchar_t* help)
{
	HANDLE hDlg = DialogInit(&PluginId, id,
		-1, -1, width, height,
		help,
		items, itemCount, 0, 0, NULL, NULL);
	intptr_t res = DialogRun(hDlg);
	DialogFree(hDlg);
	return res;
}

inline
intptr_t
FarManager::dialogEx(const GUID* id,
	FarDialogItem* items, size_t itemCount,
	intptr_t width, intptr_t height,
	const wchar_t* help,
	FARWINDOWPROC dlgProc)
{
	HANDLE hDlg = DialogInit(&PluginId, id,
		-1, -1, width, height,
		help,
		items, itemCount, 0, 0, dlgProc, NULL);
	intptr_t res = DialogRun(hDlg);
	DialogFree(hDlg);
	return res;
}

inline
HANDLE FarManager::dialogInit(const GUID* id,
		FarDialogItem* items, size_t itemCount,
		intptr_t width, intptr_t height,
		const wchar_t* help)
{
	return DialogInit(&PluginId, id,
		-1, -1, width, height,
		help,
		items, itemCount, 0, 0, NULL, NULL);
}
inline
intptr_t
FarManager::dialogRun(HANDLE hDlg)
{
	return DialogRun(hDlg);
}

inline
void
FarManager::dialogFree(HANDLE hDlg)
{
	return DialogFree(hDlg);
}

inline
std::wstring
FarManager::getDlgItemText(HANDLE hDlg, intptr_t id)
{
	FarGetDialogItem DialogItem = {sizeof(FarGetDialogItem)};
	DialogItem.Size = SendDlgMessage(hDlg, DM_GETDLGITEM, id, 0);
	DialogItem.Item = (FarDialogItem*)malloc(DialogItem.Size);
	SendDlgMessage(hDlg, DM_GETDLGITEM, id, &DialogItem);
	std::wstring result(DialogItem.Item->Data);
	free(DialogItem.Item);
	return result;
}

inline
intptr_t
FarManager::setTextPtr(HANDLE hDlg, intptr_t itemId, const wchar_t* text)
{
	return SendDlgMessage(hDlg, DM_SETTEXTPTR,
		itemId, const_cast<void*>(reinterpret_cast<const void*>(text)));
}

inline
intptr_t
FarManager::defDlgProc(HANDLE hDlg, intptr_t Msg, intptr_t Param1, void *Param2)
{
	return DefDlgProc(hDlg, Msg, Param1, Param2);
}

inline
const wchar_t*
FarManager::getMsg(intptr_t id) const
{
	return GetMsg(&PluginId, id);
}

inline
void
FarManager::showMessage(
	const wchar_t* text, const wchar_t* title) const
{
	const wchar_t* items[2];
	items[0] = title;
	items[1] = text;
	Message(&PluginId, &ShowMessageId,
		FMSG_MB_OK, 0, items, 2, 0);
}

inline
void
FarManager::showMessage(const std::string& s, const wchar_t* title) const
{
	return showMessage(s.c_str(), title);
}

inline
void
FarManager::showMessage(const std::ostringstream& oss, const wchar_t* title) const
{
	return showMessage(oss.str().c_str(), title);
}

inline
void
FarManager::showError(const wchar_t* text) const
{
	const wchar_t* items[] = {L"Error", text};
	Message(&PluginId, &ErrorMsgId,
		FMSG_WARNING | FMSG_MB_OK, 0, items, COUNTOF(items), 0);
}

inline
void
FarManager::showError(const std::exception& e) const
{
	showError(widen(e.what()).c_str());
}

#endif