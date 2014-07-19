#ifndef LANGUAGE_INCLUDED
#define LANGUAGE_INCLUDED

#include "farManager.h"

namespace lng
{
	namespace main
	{
		/*const LanguageString title (0);
		const LanguageString attach(1);
		const LanguageString pause (2);
		const LanguageString seek  (3);
		const LanguageString reload(4);
		const LanguageString phrase(5);
		const LanguageString style (6);
		const LanguageString timing(7);*/

		extern LanguageString* title ;
		extern LanguageString* attach;
		extern LanguageString* pause ;
		extern LanguageString* seek  ;
		extern LanguageString* reload;
		extern LanguageString* phrase;
		extern LanguageString* style ;
		extern LanguageString* timing;
	}

	namespace attach
	{
		/*const LanguageString title (8);
		const LanguageString detach(9);*/
		extern LanguageString* title ;
		extern LanguageString* detach;
	}

	namespace seek
	{
		/*const LanguageString title        (10);
		const LanguageString start        (11);
		const LanguageString end          (12);
		const LanguageString previousFrame(13);
		const LanguageString nextFrame    (14);
		const LanguageString custom       (15);*/
		extern LanguageString* title;
		extern LanguageString* start;
		extern LanguageString* end;
		extern LanguageString* previousFrame;
		extern LanguageString* nextFrame;
		extern LanguageString* custom;

		namespace dialog
		{
			/*const LanguageString title    (16);
			const LanguageString prompt   (17);*/
			extern LanguageString* title;
			extern LanguageString* prompt;
		}
	}

	namespace phrase
	{
		/*const LanguageString title            (18);
		const LanguageString edit             (19);
		const LanguageString splitSequential  (20);
		const LanguageString splitSimultaneous(21);
		const LanguageString splice           (22);
		const LanguageString fixOverlay       (23);
		const LanguageString moveUp           (24);
		const LanguageString moveDown         (25);*/
		extern LanguageString* title;
		extern LanguageString* edit;
		extern LanguageString* splitSequential;
		extern LanguageString* splitSimultaneous;
		extern LanguageString* splice;
		extern LanguageString* fixOverlay;
		extern LanguageString* moveUp;
		extern LanguageString* moveDown;
	}

	namespace style
	{
		/*const LanguageString title(26);*/
		extern LanguageString* title;
		extern LanguageString* notfound;
	}

	namespace timing
	{
		/*const LanguageString title   (27);
		const LanguageString setStart(28);
		const LanguageString setEnd  (29);
		const LanguageString shift   (30);
		const LanguageString duration(31);*/
		extern LanguageString* title;
		extern LanguageString* setStart;
		extern LanguageString* setEnd;
		extern LanguageString* shift;
		extern LanguageString* duration;

		namespace scope
		{
			/*const LanguageString title        (32);
			const LanguageString currentPhrase(33);
			const LanguageString selection    (34);
			const LanguageString remainder    (35);
			const LanguageString wholeScript  (36);*/
			extern LanguageString* title;
			extern LanguageString* currentPhrase;
			extern LanguageString* selection;
			extern LanguageString* remainder;
			extern LanguageString* wholeScript;
		}

		namespace dialog
		{
			/*const LanguageString shiftTitle   (37);
			const LanguageString durationTitle(38);
			const LanguageString shiftBy      (39);
			const LanguageString changeBy     (40);
			const LanguageString startTime    (41);
			const LanguageString endTime      (42);
			const LanguageString duration     (43);
			const LanguageString original     (44);
			const LanguageString current      (45);*/
			extern LanguageString* shiftTitle;
			extern LanguageString* durationTitle;
			extern LanguageString* shiftBy;
			extern LanguageString* changeBy;
			extern LanguageString* startTime;
			extern LanguageString* endTime;
			extern LanguageString* duration;
			extern LanguageString* original;
			extern LanguageString* current;
		}
	}

	void init();
	void finalize();
}
#endif