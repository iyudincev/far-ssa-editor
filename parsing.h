#ifndef PARSING_INCLUDED
#define PARSING_INCLUDED

#include "ssaTime.h"
#include <algorithm>

template <typename FwdIter, typename FwdIter2, typename Functor>
void split(FwdIter begin, FwdIter end, FwdIter2 separatorBegin, FwdIter2 separatorEnd, Functor f)
{
	std::iterator_traits<FwdIter2>::difference_type
		separatorLength = std::distance(separatorBegin, separatorEnd);
	for (FwdIter separator = std::search(begin, end, separatorBegin, separatorEnd);
		separator != end; begin = separator, std::advance(begin, separatorLength))
	{
		f(begin, separator);
	}
	f(begin, end);
}

template <typename InIter, typename InIter2>
InIter afterNextSeparator(InIter begin, InIter end,
	InIter2 separatorBegin, InIter2 separatorEnd)
{
	InIter separator = std::search(begin, end, separatorBegin, separatorEnd);
	if (separator != end)
	{
		std::advance(separator, std::distance(separatorBegin, separatorEnd));
	}
	return separator;
}

template <typename BidiIter>
std::reverse_iterator<BidiIter>
make_reverse(BidiIter base)
{
	return std::reverse_iterator<BidiIter>(base);
}

template <typename BidiIter, typename BidiIter2>
BidiIter previousSeparator(BidiIter begin, BidiIter end,
	BidiIter2 separatorBegin, BidiIter2 separatorEnd)
{
	return afterNextSeparator(make_reverse(end), make_reverse(begin),
		make_reverse(separatorEnd), make_reverse(separatorBegin)).base();
}

template <typename InIter>
InIter first_comma(InIter begin, InIter end)
{
	return std::find(begin, end, ',');
}

template <typename InIter>
InIter next_comma(InIter begin, InIter end)
{
	if (begin == end) return end;
	return std::find(begin + 1, end, ',');
}

template <typename InIter>
InIter comma(size_t index, InIter begin, InIter end)
{
	for (begin = first_comma(begin, end); index --> 0; begin = next_comma(begin, end));
	return begin;
}

template <typename String>
bool starts_with(const String& string, const char* prefix)
{
	for (String::const_iterator i = string.begin(), e = string.end();
		i != e && *prefix && *i == *prefix; ++i, ++prefix);
	return !*prefix;
}

template <typename Line>
bool isDialogue(const Line& line)
{
	return starts_with(line, "Dialogue:") || starts_with(line, "Comment:");
}

template <typename Line>
std::string getPrefix(const Line& line)
{
	if (!isDialogue(line)) return ""; // not Dialogue
	// FIXME: assumes default Dialogue format
	Line::const_iterator
		begin = line.begin(),
		end   = next_comma(begin, line.end());
	if (end == line.end()) return ""; // syntax
	return std::string(begin, end);
}

template <typename Line>
std::string getField(const Line& line,int field)
{
	if (!isDialogue(line)) return ""; // not Dialogue
	// FIXME: assumes default Dialogue format
	Line::const_iterator
		begin = comma(field, line.begin(), line.end()),
		end   = next_comma(begin, line.end());
	if (begin == line.end() || end == line.end()) return ""; // syntax
	return std::string(begin + 1, end);
}

template <typename Line>
std::string getLayer(const Line& line)
{
	return getPrefix(line);
}

template <typename Line>
std::string getStyle(const Line& line)
{
	return getField(line,2);
}

template <typename Line>
std::string getName(const Line& line)
{
	return getField(line,3);
}

template <typename Line>
std::string getMarginL(const Line& line)
{
	return getField(line,4);
}

template <typename Line>
std::string getMarginR(const Line& line)
{
	return getField(line,5);
}

template <typename Line>
std::string getMarginV(const Line& line)
{
	return getField(line,6);
}

template <typename Line>
std::string getEffect(const Line& line)
{
	return getField(line,7);
}

template <typename Line>
LONGLONG getStartTime(const Line& line)
{
	if (!isDialogue(line)) return INVALID_TIME; // not Dialogue
	// FIXME: assumes default Dialogue format
	Line::const_iterator
		begin = comma(0, line.begin(), line.end()),
		end   = next_comma(begin, line.end());
	if (begin == line.end() || end == line.end()) return INVALID_TIME; // syntax
	return parseTime(begin + 1, end);
}

template <typename Line>
LONGLONG getEndTime(const Line& line)
{
	if (!isDialogue(line)) return INVALID_TIME; // not Dialogue
	// FIXME: assumes default Dialogue format
	Line::const_iterator
		begin = comma(1, line.begin(), line.end()),
		end   = next_comma(begin, line.end());
	if (begin == line.end() || end == line.end()) return INVALID_TIME; // syntax
	return parseTime(begin + 1, end);
}

#endif
