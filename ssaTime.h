#ifndef SSA_TIME_INCLUDED
#define SSA_TIME_INCLUDED

#include <windows.h>

#include <algorithm>
#include <stdexcept>

const LONGLONG INVALID_TIME = -1LL;

inline void add(LONGLONG& result, wchar_t ch, LONGLONG multiplier)
{
	if (result < 0) return;
	if (L'0' > ch || ch > L'9') result = INVALID_TIME;
	else result += multiplier * (ch - L'0');
}

const LONGLONG ONE_SECOND = 10000000;
const LONGLONG TENTH_SECOND = ONE_SECOND / 10;
const LONGLONG HUNDREDTH_SECOND = ONE_SECOND / 100;
const LONGLONG TEN_SECONDS = ONE_SECOND * 10;
const LONGLONG ONE_MINUTE = ONE_SECOND * 60;
const LONGLONG TEN_MINUTES = ONE_MINUTE * 10;
const LONGLONG ONE_HOUR = ONE_MINUTE * 60;

template <typename FwdIter>
inline LONGLONG parseTime(FwdIter begin, FwdIter end)
{
	LONGLONG result = 0;
	if (std::distance(begin, end) != 10) return INVALID_TIME; // syntax
	add(result, *begin++, ONE_HOUR);
	if (':' != *begin++) return INVALID_TIME;
	add(result, *begin++, TEN_MINUTES);
	add(result, *begin++, ONE_MINUTE);
	if (':' != *begin++) return INVALID_TIME;
	add(result, *begin++, TEN_SECONDS);
	add(result, *begin++, ONE_SECOND);
	if ('.' != *begin++) return INVALID_TIME;
	add(result, *begin++, TENTH_SECOND);
	add(result, *begin++, HUNDREDTH_SECOND);

	return result;
}

inline void formatTime(LONGLONG time, wchar_t* begin, wchar_t* end)
{
	if (end - begin != 10) throw std::runtime_error("Invalid arguments to formatTime");
	if (time < 0) throw std::runtime_error("Negative time passed to formatTime");

	time = (time / (HUNDREDTH_SECOND / 2) + 1) / 2; // time in hundredths, rounded
	*--end = L'0' + time % 10; time /= 10; // time in tenths
	*--end = L'0' + time % 10; time /= 10; // time in seconds
	*--end = L'.';
	*--end = L'0' + time % 10; time /= 10; // time in 10-second intervals
	*--end = L'0' + time %	6; time /=	6; // time in minutes
	*--end = L':';
	*--end = L'0' + time % 10; time /= 10; // time in 10-minute intervals
	*--end = L'0' + time %	6; time /=	6; // time in hours
	*--end = L':';
	*--end = L'0' + time % 10; // drop the 10-hour intervals on the floor
}

#endif
