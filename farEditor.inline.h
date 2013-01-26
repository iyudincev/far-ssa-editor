#ifndef FAREDITOR_INLINE_INCLUDED
#define FAREDITOR_INLINE_INCLUDED

#include "exception.h"

#include <sstream>

inline
GetEditorInfo::GetEditorInfo()
{
	StructSize = sizeof(EditorInfo);
	ASSERT_FAR(farManager.editorControl(ECTL_GETINFO, 0, this), "Cannot get editor info");
}

inline
EditorPositionSaver::EditorPositionSaver()
: committed_(false)
{
	StructSize = sizeof(EditorInfo);
	ASSERT_FAR(farManager.editorControl(ECTL_GETINFO, 0, this), "Cannot get editor info");
}

inline
EditorPositionSaver::~EditorPositionSaver()
{
	if (!committed_) revert();
}

inline
void
EditorPositionSaver::commit()
{
	committed_ = true;
}


inline
void
EditorPositionSaver::revert()
{
	EditorSetPosition esp = {
		sizeof(EditorSetPosition),
		CurLine,
		DEFAULT,
		CurTabPos,
		TopScreenLine,
		LeftPos,
		Overtype
	};
	static_cast<void>(farManager.editorControl(ECTL_SETPOSITION, 0, &esp));
}


inline
EditorLine::EditorLine(const EditorGetString& getString)
:	lineNumber_(getString.StringNumber),
	begin_(getString.StringText),
	size_(getString.StringLength),
	eol_(getString.StringEOL),
	selStart_(getString.SelStart),
	selEnd_(getString.SelEnd)
{
}

inline
EditorLine::const_iterator
EditorLine::begin() const
{
	return begin_;
}

inline
EditorLine::const_iterator
EditorLine::end() const
{
	return begin_ + size_;
}

inline
size_t
EditorLine::size() const
{
	return size_;
}

inline
EditorLine::const_iterator
EditorLine::eol() const
{
	return eol_;
}

inline
bool
EditorLine::selected() const
{
	return selStart_ != -1 && (selEnd_ > selStart_ || selEnd_ == -1);
}

inline
void
EditorLine::set(const wchar_t* begin, size_t size)
{
	EditorSetString setString = {
		sizeof(EditorSetString),
		lineNumber_, size,
		begin,
		eol_
	};
	ASSERT_FAR(farManager.editorControl(ECTL_SETSTRING, 0, &setString), "Cannot modify line");
}


inline
void
FarEditor::invalidate()
{
	dirty_ = true;
}

inline
void
FarEditor::redraw()
{
	if (dirty_)
	{
		ASSERT_FAR(farManager.editorControl(ECTL_REDRAW, 0, 0), "Cannot redraw editor");
		dirty_ = false;
	}
}

inline
EditorLine
FarEditor::currentLine()
{
	EditorGetString egs = {
		sizeof(EditorGetString),
		CURRENT_LINE };
	ASSERT_FAR(farManager.editorControl(ECTL_GETSTRING, 0, &egs), "Cannot get line from editor");
	return EditorLine(egs);
}

// Loops over all lines in the specified range [startLine, endLine).
// Applies the given predicate to each line.
// If predicate returns true, the loop is terminated.
// If predicate returns false for all lines, the previous editor position is restored.
template <typename Predicate>
bool
FarEditor::findLineIf(intptr_t startLine, intptr_t endLine, Predicate predicate)
{
	EditorPositionSaver info;
	if (startLine == CURRENT_LINE) startLine = info.CurLine;
	if (endLine == END_LINE) endLine = info.TotalLines;

	EditorSetPosition esp =
	{
		sizeof(EditorSetPosition),
		startLine,
		DEFAULT,
		info.CurTabPos,
		DEFAULT,
		info.LeftPos,
		DEFAULT
	};

	for (; esp.CurLine < endLine; ++esp.CurLine)
	{
		esp.TopScreenLine = (std::max)((intptr_t)0, esp.CurLine - (info.WindowSizeY - 1) / 2);

		ASSERT_FAR(farManager.editorControl(ECTL_SETPOSITION, 0, &esp),
			"Cannot set editor position");

		if (predicate(currentLine()))
		{
			invalidate();
			info.commit();
			return true;
		}
	}

	invalidate();
	return false;
}

template <typename Functor>
class False
{
public:
	explicit False(Functor functor)
	: functor_(functor)
	{
	}

	bool operator()(EditorLine& line) const
	{
		functor_(line);
		return false;
	}
private:
	Functor functor_;
};

template <typename Functor>
void
FarEditor::forEachLine(intptr_t startLine, intptr_t endLine, Functor functor)
{
	findLineIf(startLine, endLine, False<Functor>(functor));
}

inline
void
FarEditor::setPosition(intptr_t line, intptr_t pos, intptr_t tabPos,
	intptr_t topLine, intptr_t leftPos, intptr_t overtype)
{
	EditorSetPosition esp = {
		sizeof(EditorSetPosition),
		line,
		pos,
		tabPos,
		topLine,
		leftPos,
		overtype
	};
	ASSERT_FAR(farManager.editorControl(ECTL_SETPOSITION, 0, &esp), "Cannot set editor position");
}

inline
void
FarEditor::insertLine()
{
	ASSERT_FAR(farManager.editorControl(ECTL_INSERTSTRING, 0, NULL), "Cannot insert line");
}

inline
void
FarEditor::insertText(const wchar_t* text)
{
	ASSERT_FAR(farManager.editorControl(ECTL_INSERTTEXT, 0, const_cast<wchar_t*>(text)), "Cannot insert text");
}

inline
void
FarEditor::deleteLine()
{
	ASSERT_FAR(farManager.editorControl(ECTL_DELETESTRING, 0, NULL), "Cannot delete line");
}

inline
void
FarEditor::save()
{
	ASSERT_FAR(farManager.editorControl(ECTL_SAVEFILE, 0, NULL), "Cannot save file");
}

inline
void
FarEditor::startUndoBlock()
{
	EditorUndoRedo eur = {
		sizeof(EditorUndoRedo),
		EUR_BEGIN
	};
	ASSERT_FAR(farManager.editorControl(ECTL_UNDOREDO, 0, &eur), "Cannot start undo block");
}

inline
void
FarEditor::finishUndoBlock()
{
	EditorUndoRedo eur = {
		sizeof(EditorUndoRedo),
		EUR_END
	};
	ASSERT_FAR(farManager.editorControl(ECTL_UNDOREDO, 0, &eur), "Cannot finish undo block");
}

#endif
