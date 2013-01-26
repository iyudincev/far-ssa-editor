#ifndef FAR_EDITOR_INCLUDED
#define FAR_EDITOR_INCLUDED

#include "plugin.hpp"

const int DEFAULT = -1;
const int CURRENT_LINE = -1;
const int END_LINE = -1;

enum SubFileFormat {
	FF_SSA,
	FF_ASS
};

class GetEditorInfo : public EditorInfo
{
public:
	GetEditorInfo();
};

class EditorPositionSaver : public EditorInfo
{
public:
	EditorPositionSaver();
	~EditorPositionSaver();
	void commit();
	void revert();
private:
	bool committed_;
};

class EditorLine
{
public:
	explicit EditorLine(const EditorGetString& getString);

	typedef const wchar_t* const_iterator;

	const_iterator begin() const;
	const_iterator end() const;
	size_t size() const;

	const_iterator eol() const;

	bool selected() const;

	void set(const wchar_t* begin, size_t size);

private:
	intptr_t lineNumber_;
	const wchar_t* const begin_;
	const wchar_t* const eol_;
	const size_t size_;
	const intptr_t selStart_, selEnd_;
};

extern
class FarEditor
{
private:
	bool dirty_;
public:
	void invalidate();
	void redraw();

	EditorLine currentLine();
	enum SubFileFormat fileFormat;

	template <typename Predicate>
	bool findLineIf(intptr_t startLine, intptr_t endLine, Predicate predicate);

	template <typename Functor>
	void forEachLine(intptr_t startLine, intptr_t endLine, Functor functor);

	void setPosition(intptr_t line = DEFAULT, intptr_t pos = DEFAULT, intptr_t tabPos = DEFAULT,
		intptr_t topLine = DEFAULT, intptr_t leftPos = DEFAULT, intptr_t overtype = DEFAULT);
	void insertLine();
	void insertText(const wchar_t* text);
	void deleteLine();

	void save();

	void startUndoBlock();
	void finishUndoBlock();
} farEditor;


#include "farEditor.inline.h"

#endif
