#pragma warning (disable: 4995)
#pragma warning (disable: 4996)
#include <tchar.h>
#include <windows.h>
#include <objbase.h>

#include <map>
#include <stdexcept>

#include "plugin.hpp"

#include "farManager.h"
#include "farEditor.h"
#include "exception.h"
#include "countof.h"
#include "editorContext.h"
#include "language.h"
#include "ver.h"


bool needUninitialize;
std::map<intptr_t, EditorContext> *editors;


int __cdecl main() {}

void WINAPI SetStartupInfoW(const PluginStartupInfo* Info)
{
	if (editors==NULL) {
		editors = new std::map<intptr_t, EditorContext>;
		lng::init();
	}
	farManager = *Info;
	needUninitialize = false;
	switch (HRESULT hr = CoInitialize(0))
	{
	case S_OK: // success
	case S_FALSE: // success -- A different plugin has already
		// initialized COM on this thread in single-thread
		// apartment mode
		needUninitialize = true;
	case RPC_E_CHANGED_MODE: // success -- A different plugin
		// has already initialized COM on this thread in
		// multithread apartment mode
		return;
	default:
		ASSERT_SUCCESS(hr, "Cannot initialize COM");
	}
}

void WINAPI GetGlobalInfoW(GlobalInfo *Info)
{
  Info->StructSize = sizeof(GlobalInfo);
  Info->MinFarVersion = MAKEFARVERSION(3,0,0,2927,VS_RELEASE);
  Info->Version = MAKEFARVERSION(VER_MAJOR,VER_MINOR,0,VER_BUILD,VS_ALPHA);
  Info->Guid = PluginId;
  Info->Title = L"SSA Editor";
  Info->Description = L"SSA Subtitles Editor";
  Info->Author = _T(VER_AUTHOR);
}

void WINAPI GetPluginInfoW(PluginInfo* Info)
{
	static const wchar_t pluginName[] = L"SSA Editor";
	static const wchar_t* const pluginCommands[] = {
		pluginName
	};

	Info->StructSize = sizeof(PluginInfo);
	Info->Flags = PF_DISABLEPANELS | PF_EDITOR;
	Info->DiskMenu.Strings = 0;
	Info->DiskMenu.Guids = NULL;
	Info->DiskMenu.Count = 0;
	Info->PluginMenu.Strings = pluginCommands;
	Info->PluginMenu.Count = COUNTOF(pluginCommands);
	Info->PluginMenu.Guids = &PluginId;
	Info->PluginConfig.Strings = 0;
	Info->PluginConfig.Count = 0;
	Info->CommandPrefix = 0;
}

HANDLE WINAPI OpenW(const struct OpenInfo *OInfo)
{
	try
	{
		(*editors)[GetEditorInfo().EditorID].mainMenu();
	}
	catch (const std::exception& e)
	{
		farManager.showError(e);
	}

	return INVALID_HANDLE_VALUE;
}

void WINAPI ExitFARW(ExitInfo* Info)
{
	editors->clear();
	lng::finalize();
	delete editors;
	if (needUninitialize)
		CoUninitialize();
}

intptr_t WINAPI ProcessEditorEventW(const struct ProcessEditorEventInfo *Info)
{
	try
	{
		switch (Info->Event)
		{
		case EE_CLOSE:
			editors->erase(Info->EditorID);
			break;
		};
	}
	catch (const std::exception& e)
	{
		farManager.showError(e);
	}
	return 0;
}

intptr_t WINAPI ProcessEditorInputW(const ProcessEditorInputInfo *InputInfo)
{
	try
	{
		return (*editors)[GetEditorInfo().EditorID].processEditorInput(InputInfo->Rec);
	}
	catch (const std::exception& e)
	{
		farManager.showError(e);
	}
	return 0;
}
