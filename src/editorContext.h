#ifndef EDITOR_CONTEXT_INCLUDED
#define EDITOR_CONTEXT_INCLUDED

#include <windows.h>

#include <dshow.h>
#include <atlcomcli.h>

#include <string>

class EditorContext
{
private:
	DWORD pid_;
	CComQIPtr<IMediaControl> pMediaControl_;
	CComQIPtr<IMediaSeeking> pMediaSeeking_;
	HWND hwndPlayer_;
	bool attached() const;
	LONGLONG mediaTime();
	void seek(LONGLONG time);
	void previousFrame();
	void nextFrame();

	template <typename InIter>
	void setDialoguePrefix(InIter valueBegin, InIter valueEnd);
	template <typename InIter>
	void setDialogueField(size_t fieldNo, InIter valueBegin, InIter valueEnd);
	void setLayer(const std::string& layer);
	void setStyle(const std::string& styleName);
	void setName(const std::string& name);
	void setMarginL(const std::string& marginL);
	void setMarginR(const std::string& marginR);
	void setMarginV(const std::string& marginV);
	void setEffect(const std::string& effectName);
	template <typename InIter>
	void setText(InIter begin, InIter end);

	static bool scrollLock_;
	void updateScrollLock(const INPUT_RECORD& rec);
	bool tracking();
	bool isTrackingEvent(const INPUT_RECORD& rec) const;
	void track();

	void togglePause();

	void editPhrase();
	void splitPhraseSequential();
	void splitPhraseSimultaneous();
	void splicePhrase();
	void fixOverlay();

	void movePhraseUp();
	void movePhraseDown();

	bool attachMenu();
	bool seekMenu();
	bool phraseMenu();
	bool styleMenu();
	bool timingMenu();
	void changeDuration();
	bool shiftMenu();

	void reloadSubtitles();

	bool onKeyDown(WORD virtualKeyCode, DWORD controlKeyState);
	bool onKeyUp(WORD virtualKeyCode, DWORD controlKeyState);

public:
	EditorContext();

	void detach();

	bool paused();
	void run();
	void pause();

	void setStartTime(LONGLONG time);
	void setEndTime(LONGLONG time);

	bool mainMenu();
	intptr_t processEditorInput(const INPUT_RECORD& rec);
};

#endif