#ifndef FAR_MANAGER_INCLUDED
#define FAR_MANAGER_INCLUDED

#include "plugin.hpp"

#include <exception>
#include <string>
#include <sstream>
#include <vector>

class LanguageString
{
public:
	explicit LanguageString(intptr_t id);

	const wchar_t* get() const;
	operator const wchar_t*() const;
private:
	intptr_t id_;
};

FarMenuItem menuItem(const wchar_t* title, bool selected, bool checked);

const FarMenuItem menuSeparator = {
	MIF_SEPARATOR, L"", 0, 0, 0, 0
};

class EnableIf
{
public:
	explicit EnableIf(bool condition);

	void operator()(FarMenuItem& item) const;
private:
	bool condition_;
};

class CheckIf
{
public:
	explicit CheckIf(bool condition);

	void operator()(FarMenuItem& item) const;
private:
	bool condition_;
};

class SelectIf
{
public:
	explicit SelectIf(bool condition);

	void operator()(FarMenuItem& item) const;
private:
	bool condition_;
};


class FarMenuItems : public std::vector<FarMenuItem>
{
public:
	FarMenuItems& operator()(const wchar_t* title);
	FarMenuItems& operator()(const std::string& text);
	FarMenuItems& operator()(LanguageString title);
	FarMenuItems& operator()();
	template <typename Functor>
	FarMenuItems& operator()(const Functor& functor);
private:
	std::vector<std::wstring> texts;
};

extern
class FarManager : private PluginStartupInfo
{
public:
	FarManager& operator=(const PluginStartupInfo& other);

	const wchar_t* moduleName() const;

	intptr_t editorControl(
		enum EDITOR_CONTROL_COMMANDS Command,
		intptr_t Param1,
		void* Param2);

	intptr_t inputBox(const GUID* id,
		const wchar_t* title, const wchar_t* prompt, const wchar_t* history,
		const wchar_t* srcText, wchar_t* destText, size_t destLength,
		const wchar_t* help,
		DWORD flags);

	intptr_t menu(const GUID* id,
		const FarMenuItem* items, size_t itemCount,
		const wchar_t* title = 0,
		const wchar_t* bottom = 0,
		const wchar_t* help = 0,
		const FarKey* breakKeys = 0,
		intptr_t* breakCode = 0);
	intptr_t menu(const GUID* id,
		const FarMenuItems& items,
		const wchar_t* title = 0,
		const wchar_t* bottom = 0,
		const wchar_t* help = 0,
		const FarKey* breakKeys = 0,
		intptr_t* breakCode = 0);
	intptr_t dialog(const GUID* id,
		FarDialogItem* items, size_t itemCount,
		intptr_t width, intptr_t height,
		const wchar_t* help = 0);
	intptr_t dialogEx(const GUID* id,
		FarDialogItem* items, size_t itemCount,
		intptr_t width, intptr_t height,
		const wchar_t* help,
		FARWINDOWPROC dlgProc);

	HANDLE dialogInit(const GUID* id,
		FarDialogItem* items, size_t itemCount,
		intptr_t width, intptr_t height,
		const wchar_t* help = 0);
	intptr_t dialogRun(HANDLE hDlg);
	void dialogFree(HANDLE hDlg);
	std::wstring getDlgItemText(HANDLE hDlg, intptr_t id);

	intptr_t setTextPtr(HANDLE hDlg, intptr_t itemId, const wchar_t* text);
	intptr_t defDlgProc(HANDLE hDlg, intptr_t Msg, intptr_t Param1, void *Param2);

	const wchar_t* getMsg(intptr_t id) const;

	void showMessage(const wchar_t* text, const wchar_t* title = L"") const;
	void showMessage(const std::string& s, const wchar_t* title = L"") const;
	void showMessage(const std::ostringstream& oss, const wchar_t* title = L"") const;
	void showError(const wchar_t* text) const;
	void showError(const std::exception& e) const;

} farManager;

#include "farManager.inline.h"

#endif