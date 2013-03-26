#pragma warning (disable: 4995)
#pragma warning (disable: 4996)
#include <tchar.h>
#include "editorContext.h"

#include <algorithm>
#include <functional>
#include <vector>
#include <string>
#include <cctype>

#include <strmif.h>

#include <unknwn.h>
#include <objidl.h>
#include <objbase.h>

#include "helpContexts.h"
#include "farManager.h"
#include "farEditor.h"
#include "parsing.h"
#include "findWindow.h"
#include "shiftDialog.h"
#include "ssaTime.h"
#include "exception.h"
#include "rot.h"
#include "oleString.h"
#include "language.h"

const IID IID_IGraphBuilder2 = { 0x165BE9D6, 0x0929, 0x4363,
{ 0x9B, 0xA3, 0x58, 0x0D, 0x73, 0x5A, 0xA0, 0xF6 } };


// these IDs are subject to change in future versions of Media Player Classic
// from resource.h:
const WPARAM ID_PLAY_FRAMESTEP = 891;
const WPARAM ID_PLAY_FRAMESTEPCANCEL = 892;
const WPARAM ID_SUBTITLES_SUBITEM_START = 2300;
// from code in MainFrm.cpp, function CMainFrame::SetupSubtitlesSubMenu:
const WPARAM ID_SUBTITLES_RELOAD = ID_SUBTITLES_SUBITEM_START + 2;

const wchar_t* PHRASE_SSA_TEMPLATE = L"Dialogue: Marked=0,0:00:00.00,0:00:00.00,*Default,,0000,0000,0000,,";
const wchar_t* PHRASE_ASS_TEMPLATE = L"Dialogue: 0,0:00:00.00,0:00:00.00,*Default,,0000,0000,0000,,";

LONGLONG inputTime(LONGLONG time, const wchar_t* title = 0, const wchar_t* prompt = 0,
	const wchar_t* historyName = 0, const wchar_t* helpTopic = 0)
{
	wchar_t s[11];
	formatTime(time, s, s + 10);
	s[10] = 0;
	if (!farManager.inputBox(&InputTimeId, title, prompt, historyName, s, s, 11, helpTopic, 0))
		return time;
	LONGLONG result = parseTime(s, s + 10);
	if (INVALID_TIME == result)
		return time;
	return result;
}

EditorContext::EditorContext()
:	pid_(0), hwndPlayer_(0)
{
}

class OnErrorDetach
{
private:
	EditorContext* context_;
public:
	explicit OnErrorDetach(EditorContext* context)
	: context_(context)
	{
	}

	~OnErrorDetach()
	{
		if (std::uncaught_exception())
		{
			context_->detach();
		}
	}
};

void EditorContext::detach()
{
	pid_ = 0;
	hwndPlayer_ = 0;
	pMediaControl_ = CComQIPtr<IMediaControl>();
	pMediaSeeking_ = CComQIPtr<IMediaSeeking>();
}

bool EditorContext::attached() const
{
	return pid_ != 0;
}

bool EditorContext::paused()
{
	if (!attached()) return true;

	OnErrorDetach guard(this);
	OAFilterState filterState;
	ASSERT_SUCCESS(pMediaControl_->GetState(1500, &filterState), "Cannot get player state");

	return filterState == State_Stopped || filterState == State_Paused;
}

void EditorContext::run()
{
	if (!attached()) return;

	OnErrorDetach guard(this);
	ASSERT_SUCCESS(pMediaControl_->Run(), "Cannot start playback");
}

void EditorContext::pause()
{
	if (!attached()) return;

	OnErrorDetach guard(this);
	ASSERT_SUCCESS(pMediaControl_->Pause(), "Cannot pause playback");
}

LONGLONG EditorContext::mediaTime()
{
	if (!attached()) return INVALID_TIME;

	OnErrorDetach guard(this);
	LONGLONG position;
	ASSERT_SUCCESS(pMediaSeeking_->GetCurrentPosition(&position), "Cannot get media time");
	return position;
}

void EditorContext::seek(LONGLONG time)
{
	if (!attached() || time == INVALID_TIME) return;

	OnErrorDetach guard(this);
	ASSERT_SUCCESS(pMediaSeeking_->SetPositions(
		&time, AM_SEEKING_AbsolutePositioning,
		&time, AM_SEEKING_NoPositioning), "Seek error");
}

void EditorContext::previousFrame()
{
	if (!attached()) return;

	PostMessage(hwndPlayer_, WM_COMMAND, ID_PLAY_FRAMESTEPCANCEL, 0);
}

void EditorContext::nextFrame()
{
	if (!attached()) return;

	PostMessage(hwndPlayer_, WM_COMMAND, ID_PLAY_FRAMESTEP, 0);
}

template <typename InIter>
void
EditorContext::setDialoguePrefix(InIter valueBegin, InIter valueEnd)
{
	EditorLine line = farEditor.currentLine();

	if (!isDialogue(line)) return;

	const wchar_t
		*const begin	= line.begin(),
		*const end		= next_comma(begin, line.end());
	if (end == line.end()) return; // syntax

	const size_t oldSize = (end - (begin + 1));
	std::vector<wchar_t> newLine(line.size() - oldSize + std::distance(valueBegin, valueEnd));

	std::copy			(end,			line.end(),
		std::copy		(valueBegin,	valueEnd,
			std::copy	(line.begin(),	begin,
				newLine.begin())));

	line.set(&newLine[0], newLine.size());
	farEditor.invalidate();
}


template <typename InIter>
void
EditorContext::setDialogueField(size_t fieldNo, InIter valueBegin, InIter valueEnd)
{
	EditorLine line = farEditor.currentLine();

	if (!isDialogue(line)) return;

	const wchar_t
		*const begin	= comma(fieldNo, line.begin(), line.end()),
		*const end		= next_comma(begin, line.end());
	if (begin == line.end() || end == line.end()) return; // syntax

	const size_t oldSize = (end - (begin + 1));
	std::vector<wchar_t> newLine(line.size() - oldSize + std::distance(valueBegin, valueEnd));

	std::copy			(end,			line.end(),
		std::copy		(valueBegin,	valueEnd,
			std::copy	(line.begin(),	begin + 1,
				newLine.begin())));

	line.set(&newLine[0], newLine.size());
	farEditor.invalidate();
}

void EditorContext::setLayer(const std::string& layer)
{
	setDialoguePrefix(layer.begin(), layer.end());
}

void EditorContext::setStyle(const std::string& styleName)
{
	// FIXME: assumes default Dialogue format
	setDialogueField(2, styleName.begin(), styleName.end());
}

void EditorContext::setName(const std::string& name)
{
	setDialogueField(3, name.begin(), name.end());
}

void EditorContext::setMarginL(const std::string& marginL)
{
	setDialogueField(4, marginL.begin(), marginL.end());
}

void EditorContext::setMarginR(const std::string& marginR)
{
	setDialogueField(5, marginR.begin(), marginR.end());
}

void EditorContext::setMarginV(const std::string& marginV)
{
	setDialogueField(6, marginV.begin(), marginV.end());
}

void EditorContext::setEffect(const std::string& effectName)
{
	setDialogueField(7, effectName.begin(), effectName.end());
}

void EditorContext::setStartTime(LONGLONG time)
{
	if (time < 0) return;
	// FIXME: assumes default Dialogue format
	wchar_t value[11];
	formatTime(time, value, value + 10);
	setDialogueField(0, value, value + 10);
}

void EditorContext::setEndTime(LONGLONG time)
{
	if (time < 0) return;
	// FIXME: assumes default Dialogue format
	wchar_t value[11];
	formatTime(time, value, value + 10);
	setDialogueField(1, value, value + 10);
}

template <typename InIter>
void EditorContext::setText(InIter begin, InIter end)
{
	EditorLine line = farEditor.currentLine();

	if (!isDialogue(line)) return;

	EditorLine::const_iterator headerEnd = comma(8, line.begin(), line.end());
	if (headerEnd == line.end()) return;

	std::vector<wchar_t> newText(line.begin(), headerEnd + 1);
	newText.insert(newText.end(), begin, end);

	line.set(&newText[0], newText.size());
	farEditor.invalidate();
}

bool EditorContext::scrollLock_;

void EditorContext::updateScrollLock(const INPUT_RECORD& rec)
{
	switch (rec.EventType)
	{
	case KEY_EVENT:
	case FARMACRO_KEY_EVENT:
		// FAR has an undocumented feature
		// where a null key event is fired each second.
		if (0 == rec.Event.KeyEvent.wVirtualKeyCode &&
			0 == rec.Event.KeyEvent.wVirtualScanCode &&
			0 == rec.Event.KeyEvent.uChar.AsciiChar) break;
		scrollLock_ = (rec.Event.KeyEvent.dwControlKeyState & SCROLLLOCK_ON)?true:false;
		break;
	case MOUSE_EVENT:
		scrollLock_ = (rec.Event.MouseEvent.dwControlKeyState & SCROLLLOCK_ON)?true:false;
		break;
	}
}

bool EditorContext::tracking()
{
	return attached() && !paused() && scrollLock_;
}

void EditorContext::togglePause()
{
	if (!attached()) return;

	if (paused())
	{
		run();
	}
	else
	{
		pause();
	}
}

class Pause
{
public:
	Pause(EditorContext& editor)
	: editor_(editor), wasPaused_(editor.paused())
	{
		editor.pause();
	}
	~Pause()
	{
		if (!wasPaused_)
		{
			editor_.run();
		}
	}
private:
	EditorContext& editor_;
	bool wasPaused_;
};

void EditorContext::editPhrase()
{
	const int editorWidth = 40, editorMinHeight = 3;
	const wchar_t newLine[] = L"\\N";

	Pause p(*this);

	EditorLine line = farEditor.currentLine();
	if (!isDialogue(line)) return;

	const wchar_t* begin = comma(8, line.begin(), line.end());
	if (begin == line.end()) return; // syntax
	++begin;

	std::wstring phrase(begin, line.end());

	std::vector<FarDialogItem> items;
	std::vector<std::wstring *> itemTexts;

	std::wstring::const_iterator i = phrase.begin(), end = phrase.end();
	while (i != end || items.size() < editorMinHeight)
	{
		std::wstring::const_iterator sep = std::search(i, end, newLine, newLine + 2);
		itemTexts.push_back(new std::wstring(i, sep));
		items.resize(items.size() + 1);
		items.back().Type = DI_EDIT;
		items.back().X1 = 3;
		items.back().Y1 = items.size();
		items.back().X2 = editorWidth + 2;
		items.back().Y2 = 0;
		items.back().Flags = DIF_EDITOR;
		if (1 == items.size())
			items.back().Flags |= DIF_FOCUS | DIF_DEFAULTBUTTON;
		items.back().History = 0;
		items.back().Data = itemTexts.back()->c_str();

		i = sep + (sep == end ? 0 : 2);
	}


	HANDLE hDlg = farManager.dialogInit(&EditPhraseId, &items[0], items.size(),
		editorWidth + 6, static_cast<intptr_t>(items.size()) + 2, help::editPhrase);
	if (-1 != farManager.dialogRun(hDlg)) {
		phrase.clear();
		intptr_t iDlgItem = 0;
		for (std::vector<FarDialogItem>::const_iterator i = items.begin();
			i != items.end(); ++i, ++iDlgItem)
		{
			std::wstring itemText = farManager.getDlgItemText(hDlg, iDlgItem);
			phrase += itemText + newLine;
		}
		while (phrase.rfind(newLine) + 2 == phrase.size())
			phrase.resize(phrase.size() - 2);

		phrase.insert(phrase.begin(), line.begin(), begin);
		line.set(phrase.c_str(), phrase.size());
	}

	farManager.dialogFree(hDlg);

	for (std::vector<std::wstring*>::const_iterator i = itemTexts.begin();
		i != itemTexts.end(); ++i)
		delete *i;
}

void EditorContext::splitPhraseSimultaneous()
{
	EditorLine line = farEditor.currentLine();
	if (!isDialogue(line)) return;

	LONGLONG
		startTime = getStartTime(line),
		endTime = getEndTime(line);
	std::string layer = getLayer(line);
	std::string style = getStyle(line);
	std::string name = getName(line);
	std::string marginL = getMarginL(line);
	std::string marginR = getMarginR(line);
	std::string marginV = getMarginV(line);
	std::string effect = getEffect(line);

	EditorPositionSaver info;

	farEditor.startUndoBlock();

	intptr_t phrasePos = std::distance(line.begin(), comma(8, line.begin(), line.end())) + 1;
	if (info.CurPos < phrasePos)
	{
		farEditor.setPosition(DEFAULT, phrasePos);
	}

	farEditor.insertLine();
	switch (farEditor.fileFormat)
	{
	case FF_SSA:
		farEditor.insertText(PHRASE_SSA_TEMPLATE);
		break;
	default: //FF_ASS
		farEditor.insertText(PHRASE_ASS_TEMPLATE);
		break;
	}

	setLayer(layer);
	setStartTime(startTime);
	setEndTime(endTime);
	setStyle(style);
	setName(name);
	setMarginL(marginL);
	setMarginR(marginR);
	setMarginV(marginV);
	setEffect(effect);

	farEditor.finishUndoBlock();
}

void EditorContext::splitPhraseSequential()
{
	EditorLine line = farEditor.currentLine();
	if (!isDialogue(line)) return;

	LONGLONG
		startTime = getStartTime(line),
		endTime = getEndTime(line),
		splitTime = mediaTime();
	if (splitTime < startTime || endTime <= splitTime)
	{
		splitTime = (startTime + endTime) / 2;
	}
	std::string layer = getLayer(line);
	std::string style = getStyle(line);
	std::string name = getName(line);
	std::string marginL = getMarginL(line);
	std::string marginR = getMarginR(line);
	std::string marginV = getMarginV(line);
	std::string effect = getEffect(line);

	EditorPositionSaver info;

	farEditor.startUndoBlock();

	setEndTime(splitTime);

	intptr_t phrasePos = std::distance(line.begin(), comma(8, line.begin(), line.end())) + 1;
	if (info.CurPos < phrasePos)
	{
		farEditor.setPosition(DEFAULT, phrasePos);
	}

	farEditor.insertLine();
	switch (farEditor.fileFormat)
	{
	case FF_SSA:
		farEditor.insertText(PHRASE_SSA_TEMPLATE);
		break;
	default: //FF_ASS
		farEditor.insertText(PHRASE_ASS_TEMPLATE);
		break;
	}

	setLayer(layer);
	setStartTime(splitTime);
	setEndTime(endTime);
	setStyle(style);
	setName(name);
	setMarginL(marginL);
	setMarginR(marginR);
	setMarginV(marginV);
	setEffect(effect);

	farEditor.finishUndoBlock();
}

void EditorContext::splicePhrase()
{
	EditorPositionSaver info;

	EditorLine line = farEditor.currentLine();
	if (!isDialogue(line)) return;

	farEditor.setPosition(info.CurLine + 1);
	EditorLine nextLine = farEditor.currentLine();
	if (!isDialogue(nextLine)) return;

	LONGLONG endTime = getEndTime(nextLine);

	EditorLine::const_iterator phraseBegin = comma(8, nextLine.begin(), nextLine.end());
	if (phraseBegin == nextLine.end()) return;

	++phraseBegin;
	const wchar_t newLine[] = L"\\N";
	std::vector<wchar_t> newText;
	newText.reserve(line.size() + 2 + std::distance(phraseBegin, nextLine.end()));
	newText.insert(newText.end(), line.begin(), line.end());
	newText.insert(newText.end(), newLine,      newLine + 2);
	newText.insert(newText.end(), phraseBegin,  nextLine.end());

	farEditor.startUndoBlock();

	farEditor.deleteLine();
	info.revert();
	line.set(&newText[0], newText.size());
	setEndTime(endTime);

	farEditor.finishUndoBlock();
}

class AddLine : public std::binary_function<
	EditorLine::const_iterator, EditorLine::const_iterator, void>
{
public:
	explicit AddLine(std::vector<std::string>& lines)
	: lines_(lines)
	{
	}

	void operator()(EditorLine::const_iterator begin, EditorLine::const_iterator end) const
	{
		lines_.push_back(std::string(begin, end));
	}
private:
	std::vector<std::string>& lines_;
};

void EditorContext::fixOverlay()
{
	typedef std::vector<wchar_t>::iterator iterator;

	EditorPositionSaver info;

	EditorLine currentL = farEditor.currentLine();
	if (!isDialogue(currentL)) return;
	std::vector<wchar_t> current(currentL.begin(), currentL.end());
	LONGLONG currentStartTime = getStartTime(current), currentEndTime = getEndTime(current);
	std::string layer = getLayer(current);
	std::string style = getStyle(current);
	std::string name = getName(current);
	std::string marginL = getMarginL(current);
	std::string marginR = getMarginR(current);
	std::string marginV = getMarginV(current);
	std::string effect = getEffect(current);
	iterator currentText = comma(8, current.begin(), current.end());
	if (currentText == current.end()) return;
	++currentText;

	farEditor.setPosition(info.CurLine + 1, DEFAULT, info.CurTabPos);

	EditorLine nextL = farEditor.currentLine();
	if (!isDialogue(nextL)) return;
	std::vector<wchar_t> next(nextL.begin(), nextL.end());
	LONGLONG nextStartTime = getStartTime(next), nextEndTime = getEndTime(next);
	iterator nextText = comma(8, next.begin(), next.end());
	if (nextText == next.end()) return;
	++nextText;
	if (nextStartTime != currentEndTime) return;

	const wchar_t newLine[] = L"\\N";
	for (iterator begin = currentText; begin != current.end();
		begin = afterNextSeparator(begin, current.end(), newLine, newLine + 2))
	{
		for (iterator end = current.end(); end != begin;
			end = previousSeparator(begin, end, newLine, newLine + 2))
		{
			std::iterator_traits<iterator>::difference_type
				overlayLength = std::distance(begin, end);
			for (iterator overlayBegin = nextText; overlayBegin != next.end();
				overlayBegin = afterNextSeparator(overlayBegin, next.end(), newLine, newLine + 2))
			{
				if (// too long
					overlayLength > std::distance(overlayBegin, next.end()) ||
					// not line end
					!(overlayBegin + overlayLength == next.end() ||
					overlayBegin[overlayLength] == newLine[0] &&
					overlayBegin + overlayLength + 1 != next.end() &&
					overlayBegin[overlayLength + 1] == newLine[1]) ||
					// not equal
					!std::equal(begin, end, overlayBegin))
				{
					continue;
				}

				farEditor.startUndoBlock();

				// add a phrase containing the overlay
				farEditor.setPosition(info.CurLine + 1, DEFAULT, 0);
				switch (farEditor.fileFormat)
				{
				case FF_SSA:
					farEditor.insertText(PHRASE_SSA_TEMPLATE);
					break;
				default: //FF_ASS
					farEditor.insertText(PHRASE_ASS_TEMPLATE);
					break;
				}
				farEditor.insertLine();
				farEditor.setPosition(info.CurLine + 1, DEFAULT, 0);
				setStartTime(currentStartTime);
				setEndTime(nextEndTime);
				setLayer(layer);
				setStyle(style);
				setName(name);
				setMarginL(marginL);
				setMarginR(marginR);
				setMarginV(marginV);
				setEffect(effect);
				setText(begin, end);

				farEditor.setPosition(info.CurLine + 2, DEFAULT, 0);
				bool hasSuffix = overlayBegin + overlayLength != next.end();
				if (nextText == overlayBegin && !hasSuffix)
				{
					farEditor.deleteLine();
				}
				else
				{
					if (hasSuffix)
					{
						next.erase(overlayBegin, overlayBegin + overlayLength + 2);
					}
					else
					{
						next.erase(overlayBegin - 2, overlayBegin + overlayLength);
					}
					nextL.set(&next[0], next.size());
				}

				info.revert();
				hasSuffix = end != current.end();
				if (currentText == begin && !hasSuffix)
				{
					farEditor.deleteLine();
				}
				else
				{
					if (hasSuffix)
					{
						current.erase(begin, end + 2);
					}
					else
					{
						current.erase(begin - 2, end);
					}
					currentL.set(&current[0], current.size());
				}

				farEditor.finishUndoBlock();

				farEditor.invalidate();
				return;
			}
		}
	}
}

void EditorContext::movePhraseUp()
{
	GetEditorInfo info;
	if (info.CurLine == 0) return;

	EditorLine line = farEditor.currentLine();
	std::vector<wchar_t> s(line.begin(), line.end());

	farEditor.startUndoBlock();

	farEditor.deleteLine();

	farEditor.setPosition(info.CurLine - 1, 0);
	farEditor.insertLine();

	farEditor.setPosition(info.CurLine - 1, 0);
	farEditor.currentLine().set(&s[0], s.size());

	farEditor.setPosition(info.CurLine - 1, DEFAULT, info.CurTabPos,
		(std::max)((intptr_t)0, info.TopScreenLine - 1), info.LeftPos);

	farEditor.finishUndoBlock();
}

void EditorContext::movePhraseDown()
{
	GetEditorInfo info;
	if (info.CurLine + 1 == info.TotalLines) return;

	EditorLine line = farEditor.currentLine();
	std::vector<wchar_t> s(line.begin(), line.end());

	farEditor.startUndoBlock();

	farEditor.deleteLine();

	farEditor.setPosition(info.CurLine + 1, 0);
	farEditor.insertLine();

	farEditor.setPosition(info.CurLine + 1, 0);
	farEditor.currentLine().set(&s[0], s.size());

	farEditor.setPosition(info.CurLine + 1, DEFAULT, info.CurTabPos,
		info.TopScreenLine + 1, info.LeftPos);

	farEditor.finishUndoBlock();
}

struct MediaPlayer
{
	DWORD pid;
	HWND hwnd;
	CComPtr<IUnknown> pFilterGraph;

	MediaPlayer(DWORD aPid = 0,
		HWND anHwnd = 0,
		CComPtr<IUnknown> apFilterGraph = 0)
	: pid(aPid), hwnd(anHwnd), pFilterGraph(apFilterGraph)
	{
	}
};

std::wstring GetName(HWND hwnd)
{
	std::vector<wchar_t> title;
	title.resize(SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0) + 1);
	SendMessage(hwnd, WM_GETTEXT,
			title.size(), reinterpret_cast<LPARAM>(&title[0]));
	return std::wstring(title.begin(), title.end());
}

bool EditorContext::attachMenu()
{
	FarMenuItems items;
	std::vector<std::wstring*> itemTexts;
	std::vector<MediaPlayer> players(2);

	items
		(*lng::attach::detach)(SelectIf(!attached()))
		();

	RunningObjectTable ROT;

	CComPtr<IBindCtx> pBindCtx;
	ASSERT_SUCCESS(CreateBindCtx(0, &pBindCtx), "Cannot create bind context");

	CComPtr<IEnumMoniker> pEnum = ROT.EnumRunning();
	for (CComPtr<IMoniker> pMoniker; S_OK == pEnum->Next(1, &pMoniker, 0); pMoniker = 0)
	{
		OleString displayName;
		ASSERT_SUCCESS(pMoniker->GetDisplayName(pBindCtx, 0, &displayName), "Cannot get display name");

		const wchar_t mediaPlayerClassicToken[] = L"(MPC)";
		if (displayName.end() == std::search(displayName.begin(), displayName.end(),
			mediaPlayerClassicToken, mediaPlayerClassicToken + COUNTOF(mediaPlayerClassicToken) - 1))
			continue;

		const wchar_t pidToken[] = L" pid ";
		const wchar_t* pidTokenBegin =
			std::search(displayName.begin(), displayName.end(), pidToken, pidToken + COUNTOF(pidToken) - 1);
		if (displayName.end() == pidTokenBegin) continue;
		pidTokenBegin += COUNTOF(pidToken) - 1; // skip keyword

		wchar_t* pidTokenEnd;
		DWORD pid = std::wcstol(pidTokenBegin, &pidTokenEnd, 16);
		if (pidTokenEnd != pidTokenBegin + 8) continue;

		FindMpcWindowByPid fw(pid);
		if (!fw.run()) continue;

		itemTexts.push_back(new std::wstring(GetName(fw.hwnd())));
		items(itemTexts.back()->c_str())(SelectIf(pid_ == pid));
		players.push_back(MediaPlayer(pid, fw.hwnd(), ROT.GetObject(pMoniker)));
	}

	intptr_t result = farManager.menu(&AttachMenuId, items, *lng::attach::title, 0, help::attach);

	for (std::vector<std::wstring*>::iterator i = itemTexts.begin(); i != itemTexts.end(); ++i)
		delete *i;

	if (result < 0) return false;

	if (result == 0)
	{
		detach();
		return true;
	}

	OnErrorDetach guard(this);
	pid_ = players[result].pid;
	hwndPlayer_ = players[result].hwnd;

	ASSERT_SUCCESS(players[result].pFilterGraph->QueryInterface(
		IID_IMediaControl, (void**)&pMediaControl_), "Failed to get IMediaControl");
	ASSERT_SUCCESS(pMediaControl_->QueryInterface(
		IID_IMediaSeeking, (void**)&pMediaSeeking_), "Failed to get IMediaSeeking");
	return true;
}

bool EditorContext::seekMenu()
{
	switch (farManager.menu(&SeekMenuId, FarMenuItems()
		(*lng::seek::start        )
		(*lng::seek::end          )
		(                        )
		(*lng::seek::previousFrame)
		(*lng::seek::nextFrame    )
		(                        )
		(*lng::seek::custom       ),
		*lng::seek::title, 0, help::seek))
	{
	case 0:
		seek(getStartTime(farEditor.currentLine()));
		break;
	case 1:
		seek(getEndTime(farEditor.currentLine()));
		break;
	case 3:
		previousFrame();
		break;
	case 4:
		nextFrame();
		break;
	case 6:
		seek(inputTime(mediaTime(),
			*lng::seek::dialog::title,
			*lng::seek::dialog::prompt,
			L"SSAEditor.Seek",
			(L"<" + std::wstring(farManager.moduleName()) + L">SeekCustom").c_str()));
		break;
	default:
		return false;
	}
	return true;
}

class CheckFileFormat : public std::unary_function<EditorLine, bool>
{
public:
	explicit CheckFileFormat()
	{
	}

	bool operator()(const EditorLine& line) const
	{
		if (!starts_with(line, "ScriptType: v4.00")) return false;
		size_t sz = line.size();
		farEditor.fileFormat = (sz==17) ? FF_SSA : FF_ASS;
		return true;
	}
};

bool EditorContext::phraseMenu()
{

	{
		EditorPositionSaver info;
		farEditor.findLineIf(0, END_LINE, CheckFileFormat());
	}
	switch (farManager.menu(&PhraseMenuId, FarMenuItems()
		(*lng::phrase::edit             )
		(*lng::phrase::splitSequential  )
		(*lng::phrase::splitSimultaneous)
		(*lng::phrase::splice           )
		(*lng::phrase::fixOverlay       )
		(*lng::phrase::moveUp           )
		(*lng::phrase::moveDown         ),
		*lng::phrase::title, 0, help::phrase))
	{
	case 0:
		editPhrase();
		break;
	case 1:
		splitPhraseSequential();
		break;
	case 2:
		splitPhraseSimultaneous();
		break;
	case 3:
		splicePhrase();
		break;
	case 4:
		fixOverlay();
		break;
	case 5:
		movePhraseUp();
		break;
	case 6:
		movePhraseDown();
		break;
	default:
		return false;
	}
	return true;
}

class ParseStyle : public std::unary_function<EditorLine, void>
{
private:
	FarMenuItems& items_;
	std::vector<std::string>& names_;
	std::string style_;
public:
	explicit ParseStyle(FarMenuItems& items,
		std::vector<std::string>& names, const std::string& style)
	: items_(items), names_(names), style_(style)
	{
	}

	void operator()(const EditorLine& line) const
	{
		if (!starts_with(line, "Style:")) return;
		const wchar_t
			*const begin	= std::find_if(line.begin() + 6, line.end(),
											std::not1(std::ptr_fun(std::isspace))),
			*const end		= next_comma(begin, line.end());
		if (begin == line.end() || end == line.end()) return;

		std::string name(begin, end);
		if (name == "Default")
		{
			name.insert(name.begin(), '*');
		}

		names_.push_back(name);

		std::string caption;
		if (items_.size() < 36) {
			caption = "&";
			if (items_.size() < 9)
				caption += '1' + static_cast<char>(items_.size());
			else if (items_.size() == 9)
				caption += '0';
			else
				caption += 'A' - 10 + static_cast<char>(items_.size());
		}
		else {
			caption += " ";
		}
		caption += " " + name;
		items_(caption)(SelectIf(name == style_));
	}
};

bool EditorContext::styleMenu()
{
	FarMenuItems items;
	std::vector<std::string> names;

	farEditor.forEachLine(0, END_LINE,
		ParseStyle(items, names, getStyle(farEditor.currentLine())));

	if (items.size()==0)
	{
		farManager.showMessage(*lng::style::notfound);
		return true;
	}

	intptr_t result = farManager.menu(&StyleId, items, *lng::style::title, 0, help::style);
	if (-1 == result) return false;

	setStyle(names[result]);

	return true;
}

bool EditorContext::timingMenu()
{
	while (true)
	{
		switch (farManager.menu(&TimingMenuId, FarMenuItems()
			(*lng::timing::setStart)(EnableIf(attached()))
			(*lng::timing::setEnd  )(EnableIf(attached()))
			(                     )
			(*lng::timing::shift   )
			(*lng::timing::duration),
			*lng::timing::title, 0, help::timing))
		{
		case 0:
			setStartTime(mediaTime());
			break;
		case 1:
			setEndTime(mediaTime());
			break;
		case 3:
			if (!shiftMenu()) continue;
			break;
		case 4:
			changeDuration();
			break;
		default:
			return false;
		}
		break;
	}
	return true;
}

void EditorContext::changeDuration()
{
	EditorPositionSaver info;
	EditorLine line = farEditor.currentLine();
	LONGLONG startTime = getStartTime(line), endTime = getEndTime(line);
	if (startTime < 0 || endTime < 0) return;

	ShiftDialog dlg(*lng::timing::dialog::changeBy,
		*lng::timing::dialog::endTime, endTime,
		*lng::timing::dialog::duration, endTime - startTime,
		mediaTime(),
		help::duration);
	if (!dlg.execute()) return; // Cancelled dialog

	setEndTime(endTime + dlg.shift());
}

class InBlockAndHasValidTimes : public std::unary_function<EditorLine, bool>
{
private:
	LONGLONG& startTime_;
	LONGLONG& endTime_;
public:
	InBlockAndHasValidTimes(LONGLONG& startTime, LONGLONG& endTime)
	: startTime_(startTime), endTime_(endTime)
	{
	}
	bool operator()(const EditorLine& line) const
	{
		startTime_ = getStartTime(line);
		endTime_ = getEndTime(line);
		return line.selected() && startTime_ >= 0 && endTime_ >= 0;
	}
};

class HasValidTimes : public std::unary_function<EditorLine, bool>
{
private:
	LONGLONG& startTime_;
	LONGLONG& endTime_;
public:
	HasValidTimes(LONGLONG& startTime, LONGLONG& endTime)
	: startTime_(startTime), endTime_(endTime)
	{
	}
	bool operator()(const EditorLine& line) const
	{
		startTime_ = getStartTime(line);
		endTime_ = getEndTime(line);
		return startTime_ >= 0 && endTime_ >= 0;
	}
};

class ShiftPhrase : public std::unary_function<EditorLine, bool>
{
public:
	ShiftPhrase(EditorContext& editor, LONGLONG shift, bool blockOnly)
	: editor_(editor), shift_(shift), blockOnly_(blockOnly)
	{
	}

	bool operator()(EditorLine& line) const
	{
		if (blockOnly_ && !line.selected()) return true; // block has ended
		if (!isDialogue(line)) return false; // not a phrase
		LONGLONG startTime = getStartTime(line), endTime = getEndTime(line);
		if (startTime >= 0)
		{
			editor_.setStartTime(startTime + shift_ >= 0 ? startTime + shift_ : 0);
		}
		if (endTime >= 0)
		{
			editor_.setEndTime(endTime + shift_ >= 0 ? endTime + shift_ : 0);
		}
		return false;
	}
private:
	EditorContext& editor_;
	LONGLONG shift_;
	bool blockOnly_;
};

bool EditorContext::shiftMenu()
{
	EditorPositionSaver info;

	EditorLine line = farEditor.currentLine();
	LONGLONG startTime = getStartTime(line), endTime = getEndTime(line);

	intptr_t scope = farManager.menu(&ShiftMenuId, FarMenuItems()
		(*lng::timing::scope::currentPhrase)(EnableIf(startTime >= 0 && endTime >= 0))
		(*lng::timing::scope::selection)(EnableIf(info.BlockType != BTYPE_NONE))
		(*lng::timing::scope::remainder)
		(*lng::timing::scope::wholeScript),
		*lng::timing::scope::title, NULL, help::shift);

	switch (scope)
	{
	case 0:
		break;
	case 1:
		farEditor.findLineIf(info.BlockStartLine, END_LINE, InBlockAndHasValidTimes(startTime, endTime));
		break;
	case 2:
		farEditor.findLineIf(CURRENT_LINE, END_LINE, HasValidTimes(startTime, endTime));
		break;
	case 3:
		farEditor.findLineIf(0, END_LINE, HasValidTimes(startTime, endTime));
		break;
	default:
		return false;
	}
	if (startTime < 0 || endTime < 0) return true; // no lines to be processed, and we only found it out just now

	info.revert();

	ShiftDialog dlg(*lng::timing::dialog::shiftBy,
		*lng::timing::dialog::startTime, startTime,
		*lng::timing::dialog::endTime, endTime,
		mediaTime(),
		help::shift);
	if (!dlg.execute()) return true; // Cancelled dialog

	ShiftPhrase shift(*this, dlg.shift(), scope == 1);

	farEditor.startUndoBlock();
	switch (scope)
	{
	case 0:
		shift(line);
		break;
	case 1:
		farEditor.findLineIf(info.BlockStartLine, END_LINE, shift);
		break;
	case 2:
		farEditor.findLineIf(CURRENT_LINE, END_LINE, shift);
		break;
	case 3:
		farEditor.findLineIf(0, END_LINE, shift);
		break;
	}
	info.revert();
	farEditor.finishUndoBlock();

	return true;
}

bool EditorContext::mainMenu()
{
	do
	{
		switch (farManager.menu(&MainMenuId, FarMenuItems()
			(*lng::main::attach)(CheckIf(attached()))
			(                 )
			(*lng::main::pause )(EnableIf(attached()))(CheckIf(paused()))
			(*lng::main::seek  )(EnableIf(attached() && isDialogue(farEditor.currentLine())))
			(*lng::main::reload)
			(                 )
			(*lng::main::phrase)
			(*lng::main::style )
			(*lng::main::timing),
			*lng::main::title, 0, help::mainMenu))
		{
		case 0:
			if (!attachMenu()) continue;
			break;
		case 2:
			togglePause();
			break;
		case 3:
			if (!seekMenu()) continue;
			break;
		case 4:
			reloadSubtitles();
			break;
		case 6:
			if (!phraseMenu()) continue;
			break;
		case 7:
			if (!styleMenu()) continue;
			break;
		case 8:
			if (!timingMenu()) continue;
			break;
		default:
			return false;
		}
		break;
	} while (true);
	return true;
}

bool EditorContext::isTrackingEvent(const INPUT_RECORD& rec) const
{
	return rec.EventType == KEY_EVENT &&
		rec.Event.KeyEvent.wVirtualKeyCode == 0;
}

class ContainsTime : public std::unary_function<EditorLine, bool>
{
public:
	explicit ContainsTime(LONGLONG time)
	: time_(time)
	{
	}

	bool operator()(const EditorLine& line) const
	{
		if (!isDialogue(line)) return false;
		LONGLONG startTime = getStartTime(line), endTime = getEndTime(line);
		if (INVALID_TIME == startTime || INVALID_TIME == endTime) return false; // syntax
		return startTime <= time_ && time_ < endTime;
	}
private:
	LONGLONG time_;
};

class TimeIsAfter : public std::unary_function<EditorLine, bool>
{
public:
	explicit TimeIsAfter(LONGLONG time)
	: time_(time)
	{
	}

	bool operator()(const EditorLine& line) const
	{
		if (!isDialogue(line)) return false;
		LONGLONG startTime = getStartTime(line), endTime = getEndTime(line);
		if (INVALID_TIME == startTime || INVALID_TIME == endTime) return false; // syntax
		return time_ < startTime;
	}
private:
	LONGLONG time_;
};

void EditorContext::track()
{
	if (!farEditor.findLineIf(0, END_LINE, ContainsTime(mediaTime())))
		farEditor.findLineIf(0, END_LINE, TimeIsAfter(mediaTime()));
	farEditor.redraw();
}

void EditorContext::reloadSubtitles()
{
	if (!attached()) return;
	SendMessage(hwndPlayer_, WM_COMMAND, ID_SUBTITLES_RELOAD, 0);
}

intptr_t EditorContext::processEditorInput(const INPUT_RECORD& rec)
{
	static const DWORD SHIFT_MASK =
		LEFT_ALT_PRESSED   |
		LEFT_CTRL_PRESSED  |
		RIGHT_ALT_PRESSED  |
		RIGHT_CTRL_PRESSED |
		SHIFT_PRESSED;
	updateScrollLock(rec);
	if (tracking() && isTrackingEvent(rec))
	{
		track();
	}

	return 0;
}
