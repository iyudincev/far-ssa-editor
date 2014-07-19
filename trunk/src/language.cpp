//#include <tchar.h>
#include "language.h"

namespace lng
{
	namespace main
	{
		LanguageString* title ;
		LanguageString* attach;
		LanguageString* pause ;
		LanguageString* seek  ;
		LanguageString* reload;
		LanguageString* phrase;
		LanguageString* style ;
		LanguageString* timing;
	}

	namespace attach
	{
		LanguageString* title ;
		LanguageString* detach;
	}

	namespace seek
	{
		LanguageString* title;
		LanguageString* start;
		LanguageString* end;
		LanguageString* previousFrame;
		LanguageString* nextFrame;
		LanguageString* custom;

		namespace dialog
		{
			LanguageString* title;
			LanguageString* prompt;
		}
	}

	namespace phrase
	{
		LanguageString* title;
		LanguageString* edit;
		LanguageString* splitSequential;
		LanguageString* splitSimultaneous;
		LanguageString* splice;
		LanguageString* fixOverlay;
		LanguageString* moveUp;
		LanguageString* moveDown;
	}

	namespace style
	{
		LanguageString* title;
		LanguageString* notfound;
	}

	namespace timing
	{
		LanguageString* title;
		LanguageString* setStart;
		LanguageString* setEnd;
		LanguageString* shift;
		LanguageString* duration;

		namespace scope
		{
			LanguageString* title;
			LanguageString* currentPhrase;
			LanguageString* selection;
			LanguageString* remainder;
			LanguageString* wholeScript;
		}

		namespace dialog
		{
			LanguageString* shiftTitle;
			LanguageString* durationTitle;
			LanguageString* shiftBy;
			LanguageString* changeBy;
			LanguageString* startTime;
			LanguageString* endTime;
			LanguageString* duration;
			LanguageString* original;
			LanguageString* current;
		}
	}

	void init()
    {
		main::title  = new LanguageString(0);
		main::attach = new LanguageString(1);
		main::pause  = new LanguageString(2);
		main::seek   = new LanguageString(3);
		main::reload = new LanguageString(4);
		main::phrase = new LanguageString(5);
		main::style  = new LanguageString(6);
		main::timing = new LanguageString(7);

		attach::title  = new LanguageString(8);
		attach::detach = new LanguageString(9);

		seek::title         = new LanguageString(10);
		seek::start         = new LanguageString(11);
		seek::end           = new LanguageString(12);
		seek::previousFrame = new LanguageString(13);
		seek::nextFrame     = new LanguageString(14);
		seek::custom        = new LanguageString(15);

		seek::dialog::title     = new LanguageString(16);
		seek::dialog::prompt    = new LanguageString(17);

		phrase::title             = new LanguageString(18);
		phrase::edit              = new LanguageString(19);
		phrase::splitSequential   = new LanguageString(20);
		phrase::splitSimultaneous = new LanguageString(21);
		phrase::splice            = new LanguageString(22);
		phrase::fixOverlay        = new LanguageString(23);
		phrase::moveUp            = new LanguageString(24);
		phrase::moveDown          = new LanguageString(25);

		style::title       = new LanguageString(26);
		style::notfound    = new LanguageString(27);

		timing::title    = new LanguageString(28);
		timing::setStart = new LanguageString(29);
		timing::setEnd   = new LanguageString(30);
		timing::shift    = new LanguageString(31);
		timing::duration = new LanguageString(32);

		timing::scope::title         = new LanguageString(33);
		timing::scope::currentPhrase = new LanguageString(34);
		timing::scope::selection     = new LanguageString(35);
		timing::scope::remainder     = new LanguageString(36);
		timing::scope::wholeScript   = new LanguageString(37);

		timing::dialog::shiftTitle    = new LanguageString(38);
		timing::dialog::durationTitle = new LanguageString(39);
		timing::dialog::shiftBy       = new LanguageString(40);
		timing::dialog::changeBy      = new LanguageString(41);
		timing::dialog::startTime     = new LanguageString(42);
		timing::dialog::endTime       = new LanguageString(43);
		timing::dialog::duration      = new LanguageString(44);
		timing::dialog::original      = new LanguageString(45);
		timing::dialog::current       = new LanguageString(46);
	}

	void finalize()
	{
		delete main::title  ;
		delete main::attach ;
		delete main::pause  ;
		delete main::seek   ;
		delete main::reload ;
		delete main::phrase ;
		delete main::style  ;
		delete main::timing ;

		delete attach::title  ;
		delete attach::detach ;

		delete seek::title         ;
		delete seek::start         ;
		delete seek::end           ;
		delete seek::previousFrame ;
		delete seek::nextFrame     ;
		delete seek::custom        ;

		delete seek::dialog::title     ;
		delete seek::dialog::prompt    ;

		delete phrase::title             ;
		delete phrase::edit              ;
		delete phrase::splitSequential   ;
		delete phrase::splitSimultaneous ;
		delete phrase::splice            ;
		delete phrase::fixOverlay        ;
		delete phrase::moveUp            ;
		delete phrase::moveDown          ;

		delete style::title     ;
		delete style::notfound  ;

		delete timing::title    ;
		delete timing::setStart ;
		delete timing::setEnd   ;
		delete timing::shift    ;
		delete timing::duration ;

		delete timing::scope::title         ;
		delete timing::scope::currentPhrase ;
		delete timing::scope::selection     ;
		delete timing::scope::remainder     ;
		delete timing::scope::wholeScript   ;

		delete timing::dialog::shiftTitle   ;
		delete timing::dialog::durationTitle;
		delete timing::dialog::shiftBy      ;
		delete timing::dialog::changeBy     ;
		delete timing::dialog::startTime    ;
		delete timing::dialog::endTime      ;
		delete timing::dialog::duration     ;
		delete timing::dialog::original     ;
		delete timing::dialog::current      ;
	}
}
