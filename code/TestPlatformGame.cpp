#include "TestPlatformGame.hpp"

// --------------------------------------------------------------------------------
TestPlatformGame::WhenFocusDecorator::WhenFocusDecorator ()
	: QGAMES::Widget::ComplexDecorator ()
{
	addDecorator (new QGAMES::Widget::BlinkingBackgroundDecorator 
		(QGAMES::Color (__QGAMES_WHITECOLOR__), 4, 100, 200, 1));
	addDecorator (new QGAMES::Widget::RoundingDecorator 
		(QGAMES::Color (__QGAMES_WHITECOLOR__), 2, QGAMES::Color (__QGAMES_TRANSPARENTCOLOR__), 5));
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
void TestPlatformGame::ControlWidget::processEvent (const QGAMES::Event& e)
{
	if (e.code () == __QGAMES_WIDGETGETFOCUSEVENT__)
		((QGAMES::Widget*) e.data ()) -> setDecorator (new TestPlatformGame::WhenFocusDecorator ());
	else
	if (e.code () == __QGAMES_WIDGETLOSESFOCUSEVENT__)
		((QGAMES::Widget*) e.data ()) -> setDecorator (NULL);
		
	QGAMES::SelectionOptionsGameState::MenuControlWidget::processEvent (e);
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
QGAMES::Widget* TestPlatformGame::GUISystemBuilder::createWidget (const QGAMES::GUISystemBuilder::WidgetDefinition& def)
{
	QGAMES::Widget* result = NULL;

	if (def._id == __QGAMES_STDGUISYSTEMMAINWIDGET__ ||
		def._id == __QGAMES_STDGUISYSTEMMAINCONFWIDGET__) 
		result = new TestPlatformGame::ControlWidget (def._id);
	else
		result = QGAMES::StandardGUISystemBuilder::createWidget (def);

	return (result);
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
void TestPlatformGame::InputHandlerBehaviour::onKeyboardStatus (const std::vector <bool>& kS)
{
	// In this class a direct invocation to the main character is done
	// The right way to run this stuff would be to notify here an event
	// and to process it from the main character, who should be listening them.

	assert (game () -> activeState () && game () -> activeState () -> nestedState ());

	// The active state is always the Control State.
	// The very real state being executed the one nested to that other.
	if (game () -> activeState () -> nestedState () -> type () != __GAMETEST_PLAYINGGAMESTATE__ ||
		(game () -> activeState () -> nestedState () -> type () == __GAMETEST_PLAYINGGAMESTATE__ &&
		 ((TestPlatformGame::ControllingGameStates*) game () -> activeState ()) -> isStopped ()))
		return; // If no playing, or playing but stopped, then nothing to execute...

	// If using joystick has been configured, here makes no sense to continue...
	if (((TestPlatformGame::Game::Conf*) game () -> configuration ()) -> useJoystick ())
		return;

	TestPlatformGame::Knight* chr = 
		dynamic_cast <TestPlatformGame::Knight*> (game () -> entity (__GAMETEST_MAINCHARACTERID__));
	assert (chr); // Just in case...

	int dX = kS [_keys [__QGAMES_KEYLEFT__]] ? -1 : (kS [_keys [__QGAMES_KEYRIGHT__]] ? 1 : 0);
	int dY = kS [_keys [__QGAMES_KEYUP__]] ? -1 : (kS [_keys [__QGAMES_KEYDOWN__]] ? 1 : 0);
	if (dX != 0 || dY != 0) chr -> toWalk (QGAMES::Vector (__BD dX, __BD dY, __BD 0));
	else chr -> toStay ();

	if (kS [_keys [__QGAMES_KEYFIRE__]])
		chr -> toAttack ();
}

// ---
void TestPlatformGame::InputHandlerBehaviour::manageKeyOnState (QGAMES::GameState* st, int kP)
{
	// In this class a direct invocation to the main character is done
	// The right way to run this stuff would be to notify here an event
	// and to process it from the main character, who should be listening them.

	assert (st && st -> nestedState ());

	// The active state is always the Control State.
	// The very real state being executed the one nested to that other
	if (st -> nestedState () -> type () != __GAMETEST_PLAYINGGAMESTATE__)
		return;

	// To catch or to leave a thing
	TestPlatformGame::Knight* chr = 
		dynamic_cast <TestPlatformGame::Knight*> (game () -> entity (__GAMETEST_MAINCHARACTERID__));
	assert (chr); // Just in case...
	if (kP == _keys [__GAMETEST_KEYITERATE__])
		chr -> toIterateThings ();
	else if (kP == _keys [__GAMETEST_KEYCATCHLEAVE__])
		chr -> toCatchLeave ();

	// To pause / or to continue the game...
	if (kP == _keys [__QGAMES_KEYPAUSE__])
	{
		if (game () -> isGamePaused ()) game () -> continueGame ();
		else game () -> pauseGame ();
	}

	// To save the current status of the game...
	if (kP == _keys [__QGAMES_KEYSAVE__])
		((TestPlatformGame::Game*) game ()) -> 
			saveConfiguration (QGAMES::StandardInitialSelectionOptionsGameState::_CONFFILENAME);
}

// ---
void TestPlatformGame::InputHandlerBehaviour::manageJoystickMovementOnState 
	(QGAMES::GameState* st, int nJ, const QGAMES::Vector& dr)
{
	// In this class a direct invocation to the main character is done
	// The right way to run this stuff would be to notify here an event
	// and to process it from the main character, who should be listening them.

	assert (st && st -> nestedState ());

	// The active state is always the Control State.
	// The very real state being executed the one nested to that other
	if (st -> nestedState () -> type () != __GAMETEST_PLAYINGGAMESTATE__ ||
		(game () -> activeState () -> nestedState () -> type () == __GAMETEST_PLAYINGGAMESTATE__ &&
		((TestPlatformGame::ControllingGameStates*) game () -> activeState ()) -> isStopped ()) ||
		nJ != 0)
		return; // If no playing, or playing but stopped, or the joystick is not the 0, then nothing to execute...

	// If using joystick hasn't been configured, here makes no sense to continue...
	if (!((TestPlatformGame::Game::Conf*) game () -> configuration ()) -> useJoystick ())
		return;

	TestPlatformGame::Knight* chr = 
		dynamic_cast <TestPlatformGame::Knight*> (game () -> entity (__GAMETEST_MAINCHARACTERID__));
	assert (chr); // Just in case...

	if (dr != QGAMES::Vector::_cero)
	{
		int dX = (dr.posX () > 0) ? 1 : ((dr.posX () < 0) ? -1 : 0);
		int dY = (dr.posY () > 0) ? 1 : ((dr.posY () < 0) ? -1 : 0);
		chr -> toWalk (QGAMES::Vector (__BD dX, __BD dY, __BD 0));
	}
	else
		chr -> toStay ();
}

// ---
void TestPlatformGame::InputHandlerBehaviour::manageJoystickButtonOnState (QGAMES::GameState* st, int nJ, bool p, int nB, int bF)
{
	// In this class a direct invocation to the main character is done
	// The right way to run this stuff would be to notify here an event
	// and to process it from the main character, who should be listening them.

	assert (st && st -> nestedState ());

	// The active state is always the Control State.
	// The very real state being executed the one nested to that other
	if (st -> nestedState () -> type () != __GAMETEST_PLAYINGGAMESTATE__ ||
		(game () -> activeState () -> nestedState () -> type () == __GAMETEST_PLAYINGGAMESTATE__ &&
		((TestPlatformGame::ControllingGameStates*) game () -> activeState ()) -> isStopped ()) ||
		nJ != 0)
		return; // If no playing, or playing but stopped, or the joysticj is not the 0, then nothing to execute...

	TestPlatformGame::Knight* chr = 
		dynamic_cast <TestPlatformGame::Knight*> (game () -> entity (__GAMETEST_MAINCHARACTERID__));
	assert (chr); // Just in case...
	if (!p && nB == 0)
		chr -> toAttack ();
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::NumberOfLivesScoreObject::NumberOfLivesScoreObject ()
	: QGAMES::ScoreObject (__GAMETEST_LIVESCOREOBJECTID__)
{
	_forms.insert (QGAMES::Forms::value_type (__GAMETEST_LIVESCOREOBJECTFORM__, 
		QGAMES::Game::game () -> form (__GAMETEST_LIVESCOREOBJECTFORM__)));
	setCurrentForm (__GAMETEST_LIVESCOREOBJECTFORM__);
	setPosition (QGAMES::Position (__BD 10, __BD 0, __BD 0));
}

// ---
void TestPlatformGame::NumberOfLivesScoreObject::drawOn (QGAMES::Screen* scr, const QGAMES::Position& p)
{
	for (int i = ((TestPlatformGame::Game*) game ()) -> lives (); i > 0; i--)
		currentForm () -> drawOn (scr, 0, p + QGAMES::Position (__BD (10 + (i - 1) * 55), __BD 10, __BD 0));
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::NumberOfPointsScoreObject::NumberOfPointsScoreObject ()
	: QGAMES::ScoreObjectNumber (__GAMETEST_NUMBERPOINTSSCOREOBJECTID__, 0, QGAMES::Forms (), 16)
{ 
	_forms.insert (QGAMES::Forms::value_type (__QGAMES_RAVIE24BLUELETTERS__, 
		QGAMES::Game::game () -> form (__QGAMES_RAVIE24BLUELETTERS__)));
	setCurrentForm (__QGAMES_RAVIE24BLUELETTERS__); 
	setNumberLength (5);
	setSpace (0);
	setPosition (QGAMES::Position (__BD 10, __BD 120, __BD 0));
}

// ---
void TestPlatformGame::NumberOfPointsScoreObject::updatePositions ()
{
	QGAMES::ScoreObjectNumber::updatePositions ();

	setNumber (((TestPlatformGame::Game*) game ()) -> score ());
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::ThingsBeingCarriedScoreObject::ThingsBeingCarriedScoreObject ()
	: QGAMES::ScoreObject (__GAMETEST_THINGSBEINGCARRIEDSCOREOBJECTID__)
{
	_forms.insert (QGAMES::Forms::value_type (__GAMETEST_THINGSCANBECAUGHTFORM__, 
		QGAMES::Game::game () -> form (__GAMETEST_THINGSCANBECAUGHTFORM__)));
	setCurrentForm (__GAMETEST_THINGSCANBECAUGHTFORM__); 
}

// ---
void TestPlatformGame::ThingsBeingCarriedScoreObject::drawOn (QGAMES::Screen* scr, const QGAMES::Position& p)
{
	// Draws all things...
	std::vector <int> tC = ((TestPlatformGame::Game*) game ()) -> thingsCarried ();

	for (int i = 0; i < (int) tC.size (); i++)
		if (tC [i] != -1) // If there is something being carried at that position...
			currentForm () -> frame (tC [i]) -> 
				drawOn (scr, p + QGAMES::Position (
					__BD (__GAMETEST_SCREENWIDTH__ - 10 - (__GAMETEST_MAXNUMBERTHINGSCARRIED__ * 64 ) + (i * 64)), 
					__BD 20, __BD 0));

	// Round the first one, just if it exists...
	if (tC [__GAMETEST_MAXNUMBERTHINGSCARRIED__ - 1] != -1)
		scr -> drawCircle (p + QGAMES::Position (__BD (__GAMETEST_SCREENWIDTH__ - 10 - 36), __BD 10 + 32, __BD 10), 
			QGAMES::Vector::_zNormal, 32, 32, __QGAMES_REDCOLOR__, false);
}

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::RemainingTimeScoreObject::RemainingTimeScoreObject ()
	: QGAMES::ScoreObjectNumber (__GAMETEST_TIMETOLEFTSCOREOBJECTID__, 0, QGAMES::Forms (), 16)
{ 
	_forms.insert (QGAMES::Forms::value_type (__QGAMES_RAVIE24ORANGELETTERS__, 
		QGAMES::Game::game () -> form (__QGAMES_RAVIE24ORANGELETTERS__)));
	setCurrentForm (__QGAMES_RAVIE24ORANGELETTERS__); 
	setNumberLength (5);
	setSpace (0);
	setPosition (QGAMES::Position (__BD 10, __BD (__GAMETEST_SCREENHEIGHT__ - 60), __BD 0));
}

// ---
void TestPlatformGame::RemainingTimeScoreObject::updatePositions ()
{
	setNumber (((TestPlatformGame::Scene*) 
		((__AGM game ()) -> activeWorld () -> activeScene ())) -> remainingTime ());

	QGAMES::ScoreObjectNumber::updatePositions ();
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::KnightEnergyLevelScoreObject::KnightEnergyLevelScoreObject ()
	: QGAMES::StandardEnergyBarScoreObject (
			__GAMETEST_KNIGHTENERGYLEVELSCOREOBJECTID__,
			QGAMES::Position (__BD (__GAMETEST_SCREENWIDTH__ - 315), __BD (__GAMETEST_SCREENHEIGHT__ - 70), __BD 0),
			QGAMES::StandardEnergyBarScoreObject::Color::_YELLOW)
{
	// Nothing else to do...
}

// ---
void TestPlatformGame::KnightEnergyLevelScoreObject::updatePositions ()
{
	QGAMES::StandardEnergyBarScoreObject::updatePositions ();

	setPercentage (((TestPlatformGame::Knight*) game () -> entity (__GAMETEST_MAINCHARACTERID__)) -> energyLevel ());
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
void TestPlatformGame::Knight::toIterateThings ()
{
	((TestPlatformGame::Game*) game ()) -> iterateThingsCarried (); 
}

// ---
void TestPlatformGame::Knight::toCatchLeave ()
{
	TestPlatformGame::ThingToCatch* thg = NULL;

	// Looks for something that could be caught!
	const QGAMES::Entities& eties = map () -> scene () -> entities ();
	for (QGAMES::Entities::const_iterator i = eties.begin (); i != eties.end () && thg == NULL; i++)
		if ((*i).second != this && // Not me,
			(*i).second -> isVisible () && // Visible?,
			dynamic_cast <TestPlatformGame::ThingToCatch*> ((*i).second) && // Something to catch?,
			hasCollisionWith ((*i).second) && // Being touched by the knight?
			((TestPlatformGame::ThingToCatch*) (*i).second) -> canBeCaught ())  // and can it be caught?
				thg = ((TestPlatformGame::ThingToCatch*) (*i).second); 

	// Something to catch?
	TestPlatformGame::ThingToCatchLocation toLeave;
	if (thg)
	{
		int tL = ((TestPlatformGame::Game*) game ()) -> carryThing (thg -> description ()._thingType); // I got it...
		((TestPlatformGame::Game*) game ()) -> removeThing (thg -> description ()); // No longer in the maze!!...
		thg -> setVisible (false); // Not visible now...

		// Happy to get a thing finally...
		game () -> sound (__QGAMES_EATWAVSOUND__) -> play (__GAMETEST_KNIGHTSOUNDCHANNEL__);

		if (tL != -1)
		{
			toLeave = thg -> description (); // To reuse the position of the original mainly...
			toLeave._thingType = tL; // the new type...
			// The status should be the same than the things just taken...
		}
	}
	else
	{
		int tL = ((TestPlatformGame::Game*) game ()) -> carryThing (-1); // To leave something...

		if (tL != -1)
		{
			QGAMES::Position pC = baseZone ().toBase ().center () -
				game () -> form (__GAMETEST_THINGSCANBECAUGHTFORM__) -> baseZone (0).toBase ().center ();
			toLeave = TestPlatformGame::ThingToCatchLocation (
					tL, 
					((TestPlatformGame::Game*) game ()) -> mazeScene (),
					pC + (30 * _lastOrientation)); // Leave it following the same orientation the knight was...
			toLeave._canBeDestroyed = false; toLeave._canBeCaught = true; // Already destroyed, and ready to be caught
		}
	}

	// There is something to leave eventually?
	if (toLeave._thingType != -1)
	{
		((TestPlatformGame::Game*) game ()) -> leaveThing (toLeave);
		notify (QGAMES::Event (__GAMETEST_TOREBUILDTHESCENE__, this)); // To rebuild the elements inside...
	}
}

// ---
void TestPlatformGame::Knight::toStay (const QGAMES::Vector& o)
{
	QGAMES::Vector to = (o != QGAMES::Vector::_noPoint) ? o : _lastOrientation;

	if (alive () &&
		(!staying () || (staying () && to != currentStateOrientation ())) &&
		(!attacking () || (attacking () && endOfAttack ())))
	{
		// In terms of movement in a diedric world, being V(x,y) the orientation vector:
		if (to.posX () == __BD 1)
		{
			// (1,1)   : Going to the right - down, and the form representing that is the down one.
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_KNIGHTIDLESTATEDOWN__);
			// (1,-1)  : Going to the right - up, and the form representing that is the right one.
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_KNIGHTIDLESTATERIGHT__);
			// (1,0)   : Going to the right, and the form representing that is the right - down one.
			else setCurrentState (__GAMETEST_KNIGHTIDLESTATERIGHTDOWN__);
		}
		else if (to.posX () == __BD -1)
		{
			// (-1, 1) : Going to the left - down, and the form representing that is the left one.
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_KNIGHTIDLESTATELEFT__);
			// (-1, -1): Going to the left - up, and the form representing that is the up one.
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_KNIGHTIDLESTATEUP__);
			// (-1, 0) : Going to the left, and the form representing that is the left - up one.
			else setCurrentState (__GAMETEST_KNIGHTIDLESTATELEFTUP__);
		}
		else
		{
			// (0, 1)  : Going to down, and the form representing that is the left - down one.
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_KNIGHTIDLESTATELEFTDOWN__);
			// (0, -1) : Going to up, and the form representing that is the right - up one.
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_KNIGHTIDLESTATERIGHTUP__);
			// (0, 0)  : Not moving, no no makes sense in this context, probably it will represent a mistake...
			else assert (0); // Just in case, to generate a mistake in debug mode!!
		}

		_lastOrientation = to;
	}
}

bool TestPlatformGame::Knight::staying () const
{
	return (currentState () && 
		(currentState () -> id () >= __GAMETEST_KNIGHTIDLESTATEBASE__ && 
		 currentState () -> id () <  __GAMETEST_KNIGHTIDLESTATEBASE__ + __GAMETEST_KNIGHTDIRECTIONSPERSTATE__));
}

// ---
void TestPlatformGame::Knight::toWalk (const QGAMES::Vector& o)
{
	QGAMES::Vector to = (o != QGAMES::Vector::_noPoint) ? o : _lastOrientation;

	// @see toStay comments for further understanding about how the method works
	if (alive () && 
		(!walking () || (walking () && currentStateOrientation () != to)) &&
		(!attacking () || (attacking () && endOfAttack ())))
	{
		if (to.posX () == __BD 1)
		{
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_KNIGHTWALKSTATEDOWN__);
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_KNIGHTWALKSTATERIGHT__);
			else setCurrentState (__GAMETEST_KNIGHTWALKSTATERIGHTDOWN__);
		}
		else if (to.posX () == __BD -1)
		{
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_KNIGHTWALKSTATELEFT__);
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_KNIGHTWALKSTATEUP__);
			else setCurrentState (__GAMETEST_KNIGHTWALKSTATELEFTUP__);
		}
		else
		{
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_KNIGHTWALKSTATELEFTDOWN__);
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_KNIGHTWALKSTATERIGHTUP__);
			else assert (0); // Just in case, to generate a mistake in debug mode!!
		}

		_lastOrientation = to;
	}
}

// --
bool TestPlatformGame::Knight::walking () const
{
	return (currentState () && 
		(currentState () -> id () >= __GAMETEST_KNIGHTWALKSTATEBASE__ && 
		 currentState () -> id () <  __GAMETEST_KNIGHTWALKSTATEBASE__ + __GAMETEST_KNIGHTDIRECTIONSPERSTATE__));
}

// ---
void TestPlatformGame::Knight::toAttack (const QGAMES::Vector& o)
{
	QGAMES::Vector to = (o != QGAMES::Vector::_noPoint) ? o : _lastOrientation;

	// @see toStay comments for further understanding about how the method works
	if (alive () && !attacking ())
	{
		if (to.posX () == __BD 1)
		{
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_KNIGHTATTACKSTATEDOWN__);
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_KNIGHTATTACKSTATERIGHT__);
			else setCurrentState (__GAMETEST_KNIGHTATTACKSTATERIGHTDOWN__);
		}
		else if (to.posX () == __BD -1)
		{
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_KNIGHTATTACKSTATELEFT__);
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_KNIGHTATTACKSTATEUP__);
			else setCurrentState (__GAMETEST_KNIGHTATTACKSTATELEFTUP__);
		}
		else
		{
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_KNIGHTATTACKSTATELEFTDOWN__);
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_KNIGHTATTACKSTATERIGHTUP__);
			else assert (0); // Just in case, to generate a mistake in debug mode!!
		}

		_lastOrientation = to;
	}
}

// ---
bool TestPlatformGame::Knight::attacking () const
{
	return (currentState () && 
		(currentState () -> id () >= __GAMETEST_KNIGHTATTACKSTATEBASE__ && 
		 currentState () -> id () <  __GAMETEST_KNIGHTATTACKSTATEBASE__ + __GAMETEST_KNIGHTDIRECTIONSPERSTATE__));
}

// ---
bool TestPlatformGame::Knight::endOfAttack () const
{
	assert (attacking ()); // It has to be attacking...

	return (((QGAMES::Entity::FormAnimation*) animation (currentState () -> animationId ())) -> end ());
}

// ---
void TestPlatformGame::Knight::toDie (const QGAMES::Vector& o)
{
	QGAMES::Vector to = (o != QGAMES::Vector::_noPoint) ? o : _lastOrientation;

	// @see toStay comments for further understanding about how the method works
	if (alive ())
	{
		if (to.posX () == __BD 1)
		{
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_KNIGHTDIEINGSTATEDOWN__);
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_KNIGHTDIEINGSTATERIGHT__);
			else setCurrentState (__GAMETEST_KNIGHTDIEINGSTATERIGHTDOWN__);
		}
		else if (to.posX () == __BD -1)
		{
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_KNIGHTDIEINGSTATELEFT__);
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_KNIGHTDIEINGSTATEUP__);
			else setCurrentState (__GAMETEST_KNIGHTDIEINGSTATELEFTUP__);
		}
		else
		{
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_KNIGHTDIEINGSTATELEFTDOWN__);
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_KNIGHTDIEINGSTATERIGHTUP__);
			else assert (0); // Just in case, to generate a mistake in debug mode!!
		}

		_lastOrientation = to;

		game () -> sound (__GAMETEST_CRYSOUNDID__) -> play (__GAMETEST_KNIGHTSOUNDCHANNEL__);
	}

	// Once the dieing animation finishes the state has to be change to die...
}

// ---
bool TestPlatformGame::Knight::dieing () const
{
	return (currentState () && 
		(currentState () -> id () >= __GAMETEST_KNIGHTDIEINGSTATEBASE__ && 
		 currentState () -> id () <  __GAMETEST_KNIGHTDIEINGSTATEBASE__ + __GAMETEST_KNIGHTDIRECTIONSPERSTATE__));
}

// ---
bool TestPlatformGame::Knight::died () const
{
	return (currentState () && currentState () -> id () == __GAMETEST_KNIGHTDIEDSTATE__);
}

// ---
bool TestPlatformGame::Knight::canMove (const QGAMES::Vector& d, const QGAMES::Vector& a)
{
	QGAMES::Position oldPos = position ();
	setPosition (position () + d);

	// Not possible to move if there is going to be a collicion with something that can be caught...
	bool result = true;
	const QGAMES::Entities& eties = map () -> scene () -> entities ();
	for (QGAMES::Entities::const_iterator i = eties.begin (); i != eties.end () && result; i++)
		if ((*i).second != this && // Not me...
			(*i).second -> isVisible () && dynamic_cast <TestPlatformGame::ThingToCatch*> ((*i).second) && // Something visible to catch
			hasCollisionWith ((*i).second)) // ...and with collision
			result = false; // No movement possible...
	// Take into account that collisions are not detected with meal!

	setPosition (oldPos);

	return (result);
}

// ---
void TestPlatformGame::Knight::initialize ()
{
	QGAMES::PlatformArtist::initialize ();

	// The initial state is stangind looking to down...
	setCurrentState (__GAMETEST_KNIGHTIDLESTATELEFTDOWN__);
	_lastOrientation = QGAMES::Vector (__BD 0, __BD 1, __BD 0);

	reStartAllCounters ();

	_energyLevel = 100;
}

// ---
void TestPlatformGame::Knight::updatePositions ()
{
	QGAMES::PlatformArtist::updatePositions ();
	
	// To control the level of energy of the knight...
	if (counter (_COUNTERDECREMENTENERGY) -> isEnd ())
	{
		// The more difficult the game is the quicker the energy is being reduced!
		setEnergyLevel (energyLevel () - 
			(((TestPlatformGame::Game::Conf*) game () -> configuration ()) -> difficulty () + 1));
		notify (QGAMES::Event (__GAMETEST_KNIGHTENERGYLEVELCHANGED__, this));
		if (energyLevel () == 0)
		{
			toDie (); // The knight dies...
			notify (QGAMES::Event (__GAMETEST_KNIGHTENERGYLEVELTO0__, this));
		}
	}

	// When the artist has died eventually...
	if (dieing () && 
		((QGAMES::Entity::FormAnimation*) animation (currentState () -> animationId ())) -> end ())
		notify (QGAMES::Event (__GAMETEST_KNIGHTDIED__, this));

	// After attacking, to stay again...
	if (attacking () && endOfAttack ())
		toStay ();

	// If the end of the current scene has been reached a notification is thrown
	// The notification will be different attending to the limit reached
	// Take into account that the base position is taken into account!!!
	if (basePosition ().posX () <= __BD 0) // Left limit
		notify (QGAMES::Event (__GAMETEST_KNIGHTREACHEDLEFTLIMIT__, this));
	else if (basePosition ().posY () <= __BD 0) // Top limit
		notify (QGAMES::Event (__GAMETEST_KNIGHTREACHEDUPLIMIT__, this));
	else if (basePosition ().posX () >= 
		__BD ((__AGM game ()) -> activeWorld () -> activeScene () -> activeMap () -> width () >> 1))
		notify (QGAMES::Event (__GAMETEST_KNIGHTREACHEDRIGHTLIMIT__, this));
	else if (basePosition ().posY () >=
		__BD ((__AGM game ()) -> activeWorld () -> activeScene () -> activeMap () -> height ()))
		notify (QGAMES::Event (__GAMETEST_KNIGHTREACHEDDOWNLIMIT__, this));
}

// ---
void TestPlatformGame::Knight::drawOn (QGAMES::Screen* s, const QGAMES::Position& p)
{
	if (!isVisible ())
		return;

	// Draws the shadow...
	game () -> form (currentForm () -> id () + __GAMETEST_KNIGHTINCREMENTFORSHADOWFORM__) -> 
		drawOn (s, currentAspect (), position (), _alphaLevel);
	//Draws the artist...
	QGAMES::PlatformArtist::drawOn (s, p);
	// ...and finally the sword...
	game () -> form (currentForm () -> id () + __GAMETEST_KNIGHTINCREMENTFORSWORDFORM__) -> 
		drawOn (s, currentAspect (), position (), _alphaLevel);
}

// ---
void TestPlatformGame::Knight::whenCollisionWith (QGAMES::Entity* e)
{
	// If it is a meal...
	if (dynamic_cast <TestPlatformGame::Meal*> (e))
	{
		TestPlatformGame::Meal* ml = (TestPlatformGame::Meal*) e;

		((TestPlatformGame::Game*) game ()) -> addScore ((ml -> description ()._mealType + 1) * 100); 
		// Add points...(event when posisson)

		// Happy for get energy or time!!
		game () -> sound (__QGAMES_EATWAVSOUND__) -> play (__GAMETEST_KNIGHTSOUNDCHANNEL__);

		if (ml -> isForIncrementingTime ())
			notify (QGAMES::Event (__GAMETEST_SCENETIMETOBEINCREMENTED__, this));
		else if (ml -> isPoisson ()) // It is a poisson...
			toDie (); // ... so knight dies!
		else if (ml -> isForIncrementingEnergy ()) // Increments the energy
			setEnergyLevel (energyLevel () + 5); // Just 5%
	}
	else 
	// It is something to catch and knight is attacking, so it can be cought...
	if (dynamic_cast <TestPlatformGame::ThingToCatch*> (e) && attacking () && 
			((TestPlatformGame::ThingToCatch*) e) -> canBeDestroyed ()) // If the thing is the state to be destroyed...
		((TestPlatformGame::ThingToCatch*) e) -> toBeDestroyed ();
}

// ---
QGAMES::Vector TestPlatformGame::Knight::currentStateOrientation () const
{
	assert (currentState ());

	QGAMES::Vector result = QGAMES::Vector::_cero;
	switch (currentState () -> id ())
	{
		case __GAMETEST_KNIGHTIDLESTATEDOWN__:
		case __GAMETEST_KNIGHTWALKSTATEDOWN__:
		case __GAMETEST_KNIGHTATTACKSTATEDOWN__:
		case __GAMETEST_KNIGHTDIEINGSTATEDOWN__:
			result = QGAMES::Vector (__BD 1, __BD 1, __BD 0);
			break;

		case __GAMETEST_KNIGHTIDLESTATERIGHTDOWN__:
		case __GAMETEST_KNIGHTWALKSTATERIGHTDOWN__:
		case __GAMETEST_KNIGHTATTACKSTATERIGHTDOWN__:
		case __GAMETEST_KNIGHTDIEINGSTATERIGHTDOWN__:
			result = QGAMES::Vector (__BD 1, __BD 0, __BD 0);
			break;

		case __GAMETEST_KNIGHTIDLESTATERIGHT__:
		case __GAMETEST_KNIGHTWALKSTATERIGHT__:
		case __GAMETEST_KNIGHTATTACKSTATERIGHT__:
		case __GAMETEST_KNIGHTDIEINGSTATERIGHT__:
			result = QGAMES::Vector (__BD 1, __BD -1, __BD 0);
			break;

		case __GAMETEST_KNIGHTIDLESTATERIGHTUP__:
		case __GAMETEST_KNIGHTWALKSTATERIGHTUP__:
		case __GAMETEST_KNIGHTATTACKSTATERIGHTUP__:
		case __GAMETEST_KNIGHTDIEINGSTATERIGHTUP__:
			result = QGAMES::Vector (__BD 0, __BD -1, __BD 0);
			break;

		case __GAMETEST_KNIGHTIDLESTATEUP__:
		case __GAMETEST_KNIGHTWALKSTATEUP__:
		case __GAMETEST_KNIGHTATTACKSTATEUP__:
		case __GAMETEST_KNIGHTDIEINGSTATEUP__:
			result = QGAMES::Vector (__BD -1, __BD -1, __BD 0);
			break;

		case __GAMETEST_KNIGHTIDLESTATELEFTUP__:
		case __GAMETEST_KNIGHTWALKSTATELEFTUP__:
		case __GAMETEST_KNIGHTATTACKSTATELEFTUP__:
		case __GAMETEST_KNIGHTDIEINGSTATELEFTUP__:
			result = QGAMES::Vector (__BD -1, __BD 0, __BD 0);
			break;

		case __GAMETEST_KNIGHTIDLESTATELEFT__:
		case __GAMETEST_KNIGHTWALKSTATELEFT__:
		case __GAMETEST_KNIGHTATTACKSTATELEFT__:
		case __GAMETEST_KNIGHTDIEINGSTATELEFT__:
			result = QGAMES::Vector (__BD -1, __BD 1, __BD 0);
			break;

		case __GAMETEST_KNIGHTIDLESTATELEFTDOWN__:
		case __GAMETEST_KNIGHTWALKSTATELEFTDOWN__:
		case __GAMETEST_KNIGHTATTACKSTATELEFTDOWN__:
		case __GAMETEST_KNIGHTDIEINGSTATELEFTDOWN__:
			result = QGAMES::Vector (__BD 0, __BD 1, __BD 0);
			break;

		default:
			assert (0); // It should't be here...
	}

	return (result);
}

// ---
__IMPLEMENTCOUNTERS__ (TestPlatformGame::Knight::Counters)
{
	addCounter (new QGAMES::Counter (_COUNTERDECREMENTENERGY, 
		(int) (2 * QGAMES::Game::game () -> framesPerSecond ()), 0, true, true));
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
void TestPlatformGame::Villaner::setDescription (const TestPlatformGame::VillanerLocation& vL)
{
	_description = vL;
	switch (_description._status)
	{
		case 0: toStay (_description._orientation); break;
		case 1: toWalk (_description._orientation); break;
		case 2: toDie (_description._orientation); break;
		default:
			assert (0); // It shouldn't be here...
	};

	setPosition (_description._position - // The position kept is always with no z coordinate...
		QGAMES::Vector (__BD 0, __BD 0, __BD 32) -
		QGAMES::Vector (__BD 0, __BD 0, 
			__BD (currentForm () -> frameHeightForFrame (currentAspect ()) - 
				 (currentForm () -> frameWidthForFrame (currentAspect ()) >> 1))));

	setVisible (true);
}

// ---
void TestPlatformGame::Villaner::setNoDescription ()
{
	_description = TestPlatformGame::VillanerLocation ();

	setVisible (false);
}

// ---
bool TestPlatformGame::Villaner::canMove (const QGAMES::Vector& d, const QGAMES::Vector& a)
{
	if (!isVisible ())
		return (true); // Nothing to stop it while it is not visible...

	QGAMES::Position oldPos = position ();
	setPosition (position () + d);

	// Not possible to move if there is going to be a collicion with something that can be caught...
	bool result = true;
	const QGAMES::Entities& eties = map () -> scene () -> entities ();
	for (QGAMES::Entities::const_iterator i = eties.begin (); i != eties.end () && result; i++)
		if ((*i).second != this && // Not me...
			(*i).second -> isVisible () && dynamic_cast <TestPlatformGame::ThingToCatch*> ((*i).second) && // Something visible to catch
			hasCollisionWith ((*i).second)) // ...and with collision
			result = false; // No movement possible...
	// Take into account that collisions are not detected with meal!

	setPosition (oldPos);

	return (result);
}

// ---
void TestPlatformGame::Villaner::initialize ()
{
	QGAMES::PlatformArtist::initialize ();

	// The initial state is stangind looking to down...
	setCurrentState (__GAMETEST_VILLANERIDLESTATELEFTDOWN__);
	_lastOrientation = QGAMES::Vector (__BD 0, __BD 1, __BD 0);

	_energyLevel = 100;
}

// ---
void TestPlatformGame::Villaner::updatePositions ()
{
	QGAMES::PlatformArtist::updatePositions ();

	// The position is updated whether the villaner is visible or not...
	// The way the villaner moves is quite similar to knight's but it isn't controlled bu the user!

	// When the artist has died eventually...
	if (dieing () && 
		((QGAMES::Entity::FormAnimation*) animation (currentState () -> animationId ())) -> end ())
		notify (QGAMES::Event (__GAMETEST_VILLANERDIED__, this));

	// If the end of the current scene has been reached a notification is thrown
	// The notification will be different attending to the limit reached
	// Take into account that the base position is taken into account!!!
	if (basePosition ().posX () <= __BD 0) // Left limit
		notify (QGAMES::Event (__GAMETEST_VILLANERREACHEDLEFTLIMIT__, this));
	else if (basePosition ().posY () <= __BD 0) // Top limit
		notify (QGAMES::Event (__GAMETEST_VILLANERREACHEDUPLIMIT__, this));
	else if (basePosition ().posX () >= 
		__BD ((__AGM game ()) -> activeWorld () -> activeScene () -> activeMap () -> width () >> 1))
		notify (QGAMES::Event (__GAMETEST_VILLANERREACHEDRIGHTLIMIT__, this));
	else if (basePosition ().posY () >=
		__BD ((__AGM game ()) -> activeWorld () -> activeScene () -> activeMap () -> height ()))
		notify (QGAMES::Event (__GAMETEST_VILLANERREACHEDDOWNLIMIT__, this));
}

// ---
void TestPlatformGame::Villaner::drawOn (QGAMES::Screen* s, const QGAMES::Position& p)
{
	if (!isVisible ())
		return; // The villaner can be in another room scene so it won't be drawn!

	QGAMES::PlatformArtist::drawOn (s, p);
}

// ---
void TestPlatformGame::Villaner::whenCollisionWith (QGAMES::Entity* e)
{
	// TODO
}

// ---
void TestPlatformGame::Villaner::toStay (const QGAMES::Vector& o)
{
	QGAMES::Vector to = (o != QGAMES::Vector::_noPoint) ? o : _lastOrientation;

	if (alive () &&
		(!staying () || (staying () && to != currentStateOrientation ())))
	{
		// In terms of movement in a diedric world, being V(x,y) the orientation vector:
		if (to.posX () == __BD 1)
		{
			// (1,1)   : Going to the right - down, and the form representing that is the down one.
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_VILLANERIDLESTATEDOWN__);
			// (1,-1)  : Going to the right - up, and the form representing that is the right one.
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_VILLANERIDLESTATERIGHT__);
			// (1,0)   : Going to the right, and the form representing that is the right - down one.
			else setCurrentState (__GAMETEST_KNIGHTIDLESTATERIGHTDOWN__);
		}
		else if (to.posX () == __BD -1)
		{
			// (-1, 1) : Going to the left - down, and the form representing that is the left one.
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_VILLANERIDLESTATELEFT__);
			// (-1, -1): Going to the left - up, and the form representing that is the up one.
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_VILLANERIDLESTATEUP__);
			// (-1, 0) : Going to the left, and the form representing that is the left - up one.
			else setCurrentState (__GAMETEST_VILLANERIDLESTATELEFTUP__);
		}
		else
		{
			// (0, 1)  : Going to down, and the form representing that is the left - down one.
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_VILLANERIDLESTATELEFTDOWN__);
			// (0, -1) : Going to up, and the form representing that is the right - up one.
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_VILLANERIDLESTATERIGHTUP__);
			// (0, 0)  : Not moving, no no makes sense in this context, probably it will represent a mistake...
			else assert (0); // Just in case, to generate a mistake in debug mode!!
		}

		_lastOrientation = to;
	}
}

bool TestPlatformGame::Villaner::staying () const
{
	return (currentState () && 
		(currentState () -> id () >= __GAMETEST_VILLANERIDLESTATEBASE__ && 
		 currentState () -> id () <  __GAMETEST_VILLANERIDLESTATEBASE__ + __GAMETEST_VILLANERDIRECTIONSPERSTATE__));
}

// ---
void TestPlatformGame::Villaner::toWalk (const QGAMES::Vector& o)
{
	QGAMES::Vector to = (o != QGAMES::Vector::_noPoint) ? o : _lastOrientation;

	// @see toStay comments for further understanding about how the method works
	if (alive () && 
		(!walking () || (walking () && currentStateOrientation () != to)))
	{
		if (to.posX () == __BD 1)
		{
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_VILLANERWALKSTATEDOWN__);
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_VILLANERWALKSTATERIGHT__);
			else setCurrentState (__GAMETEST_VILLANERWALKSTATERIGHTDOWN__);
		}
		else if (to.posX () == __BD -1)
		{
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_VILLANERWALKSTATELEFT__);
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_VILLANERWALKSTATEUP__);
			else setCurrentState (__GAMETEST_VILLANERWALKSTATELEFTUP__);
		}
		else
		{
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_VILLANERWALKSTATELEFTDOWN__);
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_VILLANERWALKSTATERIGHTUP__);
			else assert (0); // Just in case, to generate a mistake in debug mode!!
		}

		_lastOrientation = to;
	}
}

// --
bool TestPlatformGame::Villaner::walking () const
{
	return (currentState () && 
		(currentState () -> id () >= __GAMETEST_VILLANERWALKSTATEBASE__ && 
		 currentState () -> id () <  __GAMETEST_VILLANERWALKSTATEBASE__ + __GAMETEST_VILLANERDIRECTIONSPERSTATE__));
}

// ---
void TestPlatformGame::Villaner::toDie (const QGAMES::Vector& o)
{
	QGAMES::Vector to = (o != QGAMES::Vector::_noPoint) ? o : _lastOrientation;

	// @see toStay comments for further understanding about how the method works
	if (alive ())
	{
		if (to.posX () == __BD 1)
		{
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_VILLANERDIEINGSTATEDOWN__);
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_VILLANERDIEINGSTATERIGHT__);
			else setCurrentState (__GAMETEST_VILLANERDIEINGSTATERIGHTDOWN__);
		}
		else if (to.posX () == __BD -1)
		{
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_VILLANERDIEINGSTATELEFT__);
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_VILLANERDIEINGSTATEUP__);
			else setCurrentState (__GAMETEST_VILLANERDIEINGSTATELEFTUP__);
		}
		else
		{
			if (to.posY () == __BD 1) setCurrentState (__GAMETEST_VILLANERDIEINGSTATELEFTDOWN__);
			else if (to.posY () == __BD -1) setCurrentState (__GAMETEST_VILLANERDIEINGSTATERIGHTUP__);
			else assert (0); // Just in case, to generate a mistake in debug mode!!
		}

		_lastOrientation = to;

		game () -> sound (__GAMETEST_CRYSOUNDID__) -> play (__GAMETEST_VILLANERSOUNDCHANNEL__);
	}

	// Once the dieing animation finishes the state has to be change to die...
}

// ---
bool TestPlatformGame::Villaner::dieing () const
{
	return (currentState () && 
		(currentState () -> id () >= __GAMETEST_VILLANERDIEINGSTATEBASE__ && 
		 currentState () -> id () <  __GAMETEST_VILLANERDIEINGSTATEBASE__ + __GAMETEST_VILLANERDIRECTIONSPERSTATE__));
}

// ---
bool TestPlatformGame::Villaner::died () const
{
	return (currentState () && currentState () -> id () == __GAMETEST_KNIGHTDIEDSTATE__);
}

// ---
QGAMES::Vector TestPlatformGame::Villaner::currentStateOrientation () const
{
	assert (currentState ());

	QGAMES::Vector result = QGAMES::Vector::_cero;
	switch (currentState () -> id ())
	{
		case __GAMETEST_VILLANERIDLESTATEDOWN__:
		case __GAMETEST_VILLANERWALKSTATEDOWN__:
		case __GAMETEST_VILLANERDIEINGSTATEDOWN__:
			result = QGAMES::Vector (__BD 1, __BD 1, __BD 0);
			break;

		case __GAMETEST_VILLANERIDLESTATERIGHTDOWN__:
		case __GAMETEST_VILLANERWALKSTATERIGHTDOWN__:
		case __GAMETEST_VILLANERDIEINGSTATERIGHTDOWN__:
			result = QGAMES::Vector (__BD 1, __BD 0, __BD 0);
			break;

		case __GAMETEST_VILLANERIDLESTATERIGHT__:
		case __GAMETEST_VILLANERWALKSTATERIGHT__:
		case __GAMETEST_VILLANERDIEINGSTATERIGHT__:
			result = QGAMES::Vector (__BD 1, __BD -1, __BD 0);
			break;

		case __GAMETEST_VILLANERIDLESTATERIGHTUP__:
		case __GAMETEST_VILLANERWALKSTATERIGHTUP__:
		case __GAMETEST_VILLANERDIEINGSTATERIGHTUP__:
			result = QGAMES::Vector (__BD 0, __BD -1, __BD 0);
			break;

		case __GAMETEST_VILLANERIDLESTATEUP__:
		case __GAMETEST_VILLANERWALKSTATEUP__:
		case __GAMETEST_VILLANERDIEINGSTATEUP__:
			result = QGAMES::Vector (__BD -1, __BD -1, __BD 0);
			break;

		case __GAMETEST_VILLANERIDLESTATELEFTUP__:
		case __GAMETEST_VILLANERWALKSTATELEFTUP__:
		case __GAMETEST_VILLANERDIEINGSTATELEFTUP__:
			result = QGAMES::Vector (__BD -1, __BD 0, __BD 0);
			break;

		case __GAMETEST_VILLANERIDLESTATELEFT__:
		case __GAMETEST_VILLANERWALKSTATELEFT__:
		case __GAMETEST_VILLANERDIEINGSTATELEFT__:
			result = QGAMES::Vector (__BD -1, __BD 1, __BD 0);
			break;

		case __GAMETEST_VILLANERIDLESTATELEFTDOWN__:
		case __GAMETEST_VILLANERWALKSTATELEFTDOWN__:
		case __GAMETEST_VILLANERDIEINGSTATELEFTDOWN__:
			result = QGAMES::Vector (__BD 0, __BD 1, __BD 0);
			break;

		default:
			assert (0); // It should't be here...
	}

	return (result);
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
const int TestPlatformGame::ThingToCatch::_INTENSITIES [6] = { 255, 200, 155, 100, 55, 0 };

// ---
void TestPlatformGame::ThingToCatch::setDescription (const TestPlatformGame::ThingToCatchLocation& tL)
{ 
	_description = tL; 

	setCurrentForm (_description._canBeCaught ? __GAMETEST_THINGSCANBECAUGHTFORM__ : __GAMETEST_THINGSBEINGCARRIEDFORM__);
	setCurrentAspect (_description._thingType);
	setVisible (true);
	
	setPosition (_description._position - // The position kept is always with no z coordinate...
		QGAMES::Vector (__BD 0, __BD 0, __BD 32) -
		QGAMES::Vector (__BD 0, __BD 0, 
			__BD (currentForm () -> frameHeightForFrame (currentAspect ()) - 
				 (currentForm () -> frameWidthForFrame (currentAspect ()) >> 1))));

	reStartAllCounters ();
	reStartAllOnOffSwitches ();
}

// ---
void TestPlatformGame::ThingToCatch::setNoDescription ()
{
	_description = TestPlatformGame::ThingToCatchLocation ();

	setVisible (false);
}

// ---
void TestPlatformGame::ThingToCatch::toBeDestroyed ()
{
	if (canBeDestroyed ())
	{
		onOffSwitch (_SWITCHCONTAINERBEINGDESTROYED) -> set (true);

		game () -> sound (__GAMETEST_THINGFALLINGAPART__) -> play (__GAMETEST_THINGSSOUNDCHANNEL__);
	}
}

// ---
void TestPlatformGame::ThingToCatch::initialize ()
{
	QGAMES::PlatformArtist::initialize ();

	// The aspect by default...it never changes!!...
	setCurrentForm (__GAMETEST_THINGSBEINGCARRIEDFORM__);
	setCurrentAspect (0);

	reStartAllCounters ();
	reStartAllOnOffSwitches ();
}

// ---
void TestPlatformGame::ThingToCatch::updatePositions ()
{
	QGAMES::PlatformArtist::updatePositions ();

	if (!isVisible ())
		return; // No needed...

	if (onOffSwitch (_SWITCHCONTAINERBEINGDESTROYED) -> isOn () &&
	    counter (_COUNTERDECREMENTINTENSITY) -> isEnd () &&
		counter (_COUNTERINTENSITY) -> isEnd ())
	{
		onOffSwitch (_SWITCHCONTAINERBEINGDESTROYED) -> set (false);
		toBeCaught (); // Now the thing can be caught...
		notify (QGAMES::Event (__GAMETEST_THINGTOCATCHDESTROYED__, this));
	}
}

// ---
void TestPlatformGame::ThingToCatch::drawOn (QGAMES::Screen* s, const QGAMES::Position& p)
{
	if (!isVisible ())
		return;

	// Draws the shadow...
	game () -> 
		form (canBeCaught () ? __GAMETEST_THINGSCANBECAUGHTSHADOWFORM__ : __GAMETEST_THINGSBEINGCARRIEDSHADOWFORM__) -> 
			drawOn (s, 0, position (), _alphaLevel); // Depending on the status...

	//Draws the artist...
	setAlphaLevel (_INTENSITIES [counter (_COUNTERINTENSITY) -> value ()]);
	QGAMES::PlatformArtist::drawOn (s, p);
}

// ---
__IMPLEMENTCOUNTERS__ (TestPlatformGame::ThingToCatch::Counters)
{
	addCounter (new QGAMES::Counter 
		(TestPlatformGame::ThingToCatch::_COUNTERDECREMENTINTENSITY, 
			TestPlatformGame::Game::game () -> framesPerSecond () / 10, 0, true, true));
	addCounter (new QGAMES::Counter 
		(TestPlatformGame::ThingToCatch::_COUNTERINTENSITY, 6, 0, true, false));
}

// ---
__IMPLEMENTONOFFSWITCHES__ (TestPlatformGame::ThingToCatch::OnOffSwitches)
{
	addOnOffSwitch (new QGAMES::OnOffSwitch (TestPlatformGame::ThingToCatch::_SWITCHCONTAINERBEINGDESTROYED, false));
}

// ---
void TestPlatformGame::ThingToCatch::toBeCaught ()
{
	TestPlatformGame::ThingToCatchLocation oldD = _description;

	_description._canBeDestroyed = false;
	_description._canBeCaught = true;

	// ...and the form also changes...
	QGAMES::Position oPos = baseZone ().center ().projectOver (QGAMES::Position::_cero, QGAMES::Vector::_zNormal);
	setCurrentForm (__GAMETEST_THINGSCANBECAUGHTFORM__);
	setCurrentAspect (_description._thingType);
	QGAMES::Position nPos = baseZone ().center ().projectOver (QGAMES::Position::_cero, QGAMES::Vector::_zNormal);
	// ...and also the position...
	_description._position = position ().projectOver (QGAMES::Position::_cero, QGAMES::Vector::_zNormal) + (oPos - nPos);
	setPosition (_description._position -
		QGAMES::Vector (__BD 0, __BD 0, __BD 32) -
		QGAMES::Vector (__BD 0, __BD 0, 
			__BD (currentForm () -> frameHeightForFrame (currentAspect ()) - 
				 (currentForm () -> frameWidthForFrame (currentAspect ()) >> 1))));

	// Keep the changes in the configuration...
	((TestPlatformGame::Game*) game ()) -> updateThingStatus (oldD, _description);

	reStartAllCounters ();
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
void TestPlatformGame::Meal::setDescription (const TestPlatformGame::MealLocation& mL)
{ 
	_description = mL; 

	setCurrentForm (__GAMETEST_MEALFORM__);
	setCurrentAspect (mL._mealType);
	setVisible (true);
	
	setPosition (mL._position - // The position kept is always with no z coordinate...
		QGAMES::Vector (__BD 0, __BD 0, __BD 32) -
		QGAMES::Vector (__BD 0, __BD 0, 
			__BD (currentForm () -> frameHeightForFrame (currentAspect ()) - 
				 (currentForm () -> frameWidthForFrame (currentAspect ()) >> 1))));
}

// ---
void TestPlatformGame::Meal::setNoDescription ()
{
	_description = TestPlatformGame::MealLocation ();

	setVisible (false);
}

// ---
void TestPlatformGame::Meal::initialize ()
{
	QGAMES::PlatformArtist::initialize ();

	// The aspect by default...it never changes!!
	setCurrentForm (__GAMETEST_MEALFORM__);
	setCurrentAspect (0);
}

// ---
void TestPlatformGame::Meal::drawOn (QGAMES::Screen* s, const QGAMES::Position& p)
{
	if (!isVisible ())
		return;

	// Draws the shadow...
	game () -> form (__GAMETEST_MEALSHADOWFORM__) -> drawOn (s, 0, position (), _alphaLevel);
	//Draws the artist...
	QGAMES::PlatformArtist::drawOn (s, p);
}

// ---
void TestPlatformGame::Meal::whenCollisionWith (QGAMES::Entity* e)
{
	if (dynamic_cast <TestPlatformGame::Knight*> (e))
	{
		// ...and the meal disappears from the maze once it has been used...
		((TestPlatformGame::Game*) game ()) -> removeMeal (description ());
		// ...and from this visualization...
		setVisible (false);
	}
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
QGAMES::Entity* TestPlatformGame::EntityBuilder::createEntity (const QGAMES::EntityBuilder::EntityDefinition& def)
{
	QGAMES::Entity* result = NULL;

	if (def._id == __GAMETEST_MAINCHARACTERID__)
		result = new TestPlatformGame::Knight (def._id);
	else if (def._id >= __GAMETEST_VILLANERCHARACTERBASEID__ && 
			def._id < (__GAMETEST_VILLANERCHARACTERBASEID__ + __GAMETEST_NUMBERVILLANERS__))
		result = new TestPlatformGame::Villaner (def._id);
	else if (def._id >= __GAMETEST_THINGTOCATCHBASEID__ && 
			def._id < (__GAMETEST_THINGTOCATCHBASEID__ + __GAMETEST_MAXNUMBERTHINGSINTHEMAZEPERROOM__))
		result = new TestPlatformGame::ThingToCatch (def._id);
	else if (def._id >= __GAMETEST_MEALBASEID__ && 
			def._id < (__GAMETEST_MEALBASEID__ + __GAMETEST_MAXNUMBEROFMEALSINMAZEPERROOM__))
		result = new TestPlatformGame::Meal (def._id);
	else
		result = QGAMES::AdvancedEntityBuilder::createEntity (def);

	return (result);
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
QGAMES::Movement* TestPlatformGame::MovementBuilder::createMovement (const QGAMES::MovementBuilder::MovementDefinition& def)
{
	QGAMES::Movement* result = NULL;
	
	if (def._id == __GAMETEST_KNIGHTSTAYMOVEMENTID__)
		result = new QGAMES::NoMovement (def._id, def._variables);
	else if (def._id == __GAMETEST_KNIGHTWALKMOVEMENTID__)
		result = new QGAMES::SimpleLinearMovement (def._id, def._variables);
	else if (def._id >= __GAMETEST_VILLANERSTAYMOVEMENTID__ &&
			def._id < (__GAMETEST_VILLANERSTAYMOVEMENTID__ + __GAMETEST_NUMBERVILLANERS__))
		result = new QGAMES::NoMovement (def._id, def._variables);
	else if (def._id >= __GAMETEST_VILLANERWALKMOVEMENTID__ && 
			def._id < (__GAMETEST_VILLANERWALKMOVEMENTID__ + __GAMETEST_NUMBERVILLANERS__))
		result = new QGAMES::SimpleLinearMovement (def._id, def._variables);
	else if (def._id >= __GAMETEST_THINGTOCATCHMOVEMENTID__ && 
			def._id < (__GAMETEST_THINGTOCATCHMOVEMENTID__ + __GAMETEST_MAXNUMBERTHINGSINTHEMAZEPERROOM__))
		result = new QGAMES::NoMovement (def._id, def._variables);
	else if (def._id >= __GAMETEST_MEALMOVEMENTID__ && 
			def._id < (__GAMETEST_MEALMOVEMENTID__ + __GAMETEST_MAXNUMBEROFMEALSINMAZEPERROOM__))
		result = new QGAMES::NoMovement (def._id, def._variables);
	else
		result = QGAMES::AdvancedMovementBuilder::createMovement (def);

	return (result);
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
void TestPlatformGame::World::setMazeScene (int nM)
{
	assert (nM < __GAMETEST_NUMBEROFSCENESINTHEMAZE__);

	((TestPlatformGame::Game*) game ()) -> setMazeScene (nM);
	((TestPlatformGame::Scene*) activeScene ()) -> setSceneType (_MAZESCENES [nM]);

	// Active the villaners in the room, and desactive the rest...
	int nV = 0;
	std::vector <TestPlatformGame::VillanerLocation> vll = ((TestPlatformGame::Game*) game ()) -> villanersInMazeRoom (nM);
	for (int i = 0; i < (int) vll.size (); i++, nV++)
		((TestPlatformGame::Villaner*) (__AGM game ()) -> artist (__GAMETEST_VILLANERCHARACTERBASEID__ + i)) -> setDescription (vll [i]);
	for (int i = nV; i < __GAMETEST_NUMBERVILLANERS__; i++)
		((TestPlatformGame::Villaner*) (__AGM game ()) -> artist (__GAMETEST_VILLANERCHARACTERBASEID__ + i)) -> setNoDescription ();
	// but all of them will update his position...

	// Active the things that can be caught...
	int aT = 0;
	std::vector <TestPlatformGame::ThingToCatchLocation> thgs = ((TestPlatformGame::Game*) game ()) -> thingsInMaze () [nM];
	for (int i = 0; i < (int) thgs.size (); i++, aT++) // A vector with all elements is always got back...
	{
		if (thgs [i]._thingType != -1)
			((TestPlatformGame::ThingToCatch*) 
				(__AGM game ()) -> artist (__GAMETEST_THINGTOCATCHBASEID__ + aT)) -> setDescription (thgs [i]);
		else 
			((TestPlatformGame::ThingToCatch*) 
				(__AGM game ()) -> artist (__GAMETEST_THINGTOCATCHBASEID__ + aT)) -> setNoDescription ();
	}

	// Active the meal in the room...
	int aM = 0;
	std::vector <TestPlatformGame::MealLocation> ml = ((TestPlatformGame::Game*) game ()) -> mealInMaze () [nM];
	for (int i = 0; i < (int) ml.size (); i++, aM++) // A vector with all the meal is always got back...
	{
		if (ml [i]._mealType != -1)
			((TestPlatformGame::Meal*) 
				(__AGM game ()) -> artist (__GAMETEST_MEALBASEID__ + aM)) -> setDescription (ml [i]);
		else
			((TestPlatformGame::Meal*) 
				(__AGM game ()) -> artist (__GAMETEST_MEALBASEID__ + aM)) -> setNoDescription ();
	}
}

// ---
int TestPlatformGame::World::typeMazeScene (int nM)
{
	assert (nM < __GAMETEST_NUMBEROFSCENESINTHEMAZE__);
	return (_MAZESCENES [nM]);
}

// ---
bool TestPlatformGame::World::setNextMazeSceneInDirection (const QGAMES::Vector& dr)
{
	bool result = false;

	int cMS = ((TestPlatformGame::Game*) game ()) -> mazeScene ();
	int cRMS = cMS / __GAMETEST_HORIZONTALSCENESINMAZE__; // Row an column in the maze where the scene is now located...
	int cCMS = cMS % __GAMETEST_HORIZONTALSCENESINMAZE__;

	// First of all, checks whether we are in the last scene
	// and the user is wanting to go to the exit
	if (cMS == 0 && dr.posX () == __BD -1)
		return (true);

	int nMS = cMS + (int) dr.posX () + (int) (dr.posY () * __GAMETEST_HORIZONTALSCENESINMAZE__);
	int nRMS = nMS / __GAMETEST_HORIZONTALSCENESINMAZE__; // Same in the potential next position...
	int nCMS = nMS % __GAMETEST_HORIZONTALSCENESINMAZE__;
	int pnRMS = cRMS + (int) dr.posY ();
	int pnCMS = cCMS + (int) dr.posX ();
	assert (nRMS == pnRMS && nCMS == pnCMS); 
	// If they are not the same, then the position is not possible with in the maze...
	// so an alert is shown in debug mode!

	// Finally sets the new maze scene
	setMazeScene (nMS);
	// And changes the position of the knight with in the maze...
	TestPlatformGame::Knight* k = 
		((TestPlatformGame::Knight*) (__AGM game ()) -> artist (__GAMETEST_MAINCHARACTERID__));
	k -> setPosition (k -> position () - 
		QGAMES::Vector (dr.posX () * __BD (k -> map () -> width () >> 1),
						dr.posY () * __BD (k -> map () -> height ()),
						__BD 0) + (__BD 10 * dr));

	return (false);
}

void TestPlatformGame::World::initialize ()
{
	setScene (__GAMETEST_SCENEID__);

	QGAMES::World::initialize ();

	(__AT (__AGM game ()) -> artist (__GAMETEST_MAINCHARACTERID__)) -> setMap (activeScene () -> activeMap ());
	for (int i = 0; i < __GAMETEST_NUMBERVILLANERS__; i++)
		(__AT (__AGM game ()) -> artist (__GAMETEST_VILLANERCHARACTERBASEID__ + i)) -> setMap (activeScene () -> activeMap ());
	for (int i = 0; i < __GAMETEST_MAXNUMBERTHINGSINTHEMAZEPERROOM__; i++)
		(__AT (__AGM game ()) -> artist (__GAMETEST_THINGTOCATCHBASEID__ + i)) -> setMap (activeScene () -> activeMap ());
	for (int i = 0; i < __GAMETEST_MAXNUMBEROFMEALSINMAZEPERROOM__; i++)
		(__AT (__AGM game ()) -> artist (__GAMETEST_MEALBASEID__ + i)) -> setMap (activeScene () -> activeMap ());
}

void TestPlatformGame::World::drawOn (QGAMES::Screen* s, const QGAMES::Position& p)
{
	// Draws the background...
	QGAMES::Camera::ProjectionType pt = s -> currentCamera () -> projectionType ();
	s -> currentCamera () -> setProjectType (QGAMES::Camera::ProjectionType::_ORTHOGONAL);
	s -> drawRectangle (
		QGAMES::Rectangle (s -> position (), 
			s -> position () + QGAMES::Vector (__BD __GAMETEST_SCREENWIDTH__, __BD __GAMETEST_SCREENHEIGHT__, __BD 0)),
		__QGAMES_LIGHTBLUECOLOR__, true);
	s -> currentCamera () -> setProjectType (pt);
	
	QGAMES::World::drawOn (s, p);

#ifndef NDEBUG
	// Draws the reference lines in real diedric 
	// Remember that a diedric tile map is a simplification to speed up the calculation of the positions
	QGAMES::TiledMap* tM = __TM activeScene () -> activeMap ();
	int xM = tM -> width () >> 1; int yM = tM -> height ();
	int tW = tM -> tileWidth (); int tH = tM -> tileHeight ();

	// The Base...
	for (int i = 0; i < 25; i++)
	{
		int h = -tH;
		int iPos = i * tW >> 1; 
		s -> drawLine (QGAMES::Position (__BD 0, __BD iPos, __BD h),
			QGAMES::Position (__BD xM, __BD iPos, __BD h), __QGAMES_TRANSPARENTBLUECOLOR__);
		s -> drawLine (QGAMES::Position (__BD iPos, __BD 0, __BD h),
			QGAMES::Position (__BD iPos, __BD yM, __BD h), __QGAMES_TRANSPARENTBLUECOLOR__);
	}

	// The limits in Z axis...
	for (int i = 0; i < 10; i++)
	{
		int h = -tH - (i * 5);
		s -> drawLine (QGAMES::Position (__BD 0, __BD 0, __BD h), 
					   QGAMES::Position (__BD xM, __BD 0, __BD h), __QGAMES_TRANSPARENTREDCOLOR__);
		s -> drawLine (QGAMES::Position (__BD 0, __BD 0, __BD h), 
					   QGAMES::Position (__BD 0, __BD yM, __BD h), __QGAMES_TRANSPARENTREDCOLOR__);
		s -> drawLine (QGAMES::Position (__BD xM, __BD 0, __BD h), 
					   QGAMES::Position (__BD xM, __BD yM, __BD h), __QGAMES_TRANSPARENTREDCOLOR__);
		s -> drawLine (QGAMES::Position (__BD 0, __BD yM, __BD h), 
					   QGAMES::Position (__BD xM, __BD yM, __BD h), __QGAMES_TRANSPARENTREDCOLOR__);
	}

	// Draws a cross just inthe middle of the world...
	int h = -tH;
	xM >>= 1; yM >>= 1;
	s -> drawLine (QGAMES::Position (__BD (xM - 25), __BD yM, __BD h),
				   QGAMES::Position (__BD (xM + 25), __BD yM, __BD h), __QGAMES_BLACKCOLOR__);
	s -> drawLine (QGAMES::Position (__BD xM, __BD (yM - 25), __BD h),
				   QGAMES::Position (__BD xM, __BD (yM + 25), __BD h), __QGAMES_BLACKCOLOR__);
#endif
}

// ---
void TestPlatformGame::World::processEvent (const QGAMES::Event& e)
{
	if (e.code () == __GAMETEST_KNIGHTREACHEDLEFTLIMIT__ ||
		e.code () == __GAMETEST_KNIGHTREACHEDRIGHTLIMIT__ ||
		e.code () == __GAMETEST_KNIGHTREACHEDUPLIMIT__ ||
		e.code () == __GAMETEST_KNIGHTREACHEDDOWNLIMIT__)
	{
		QGAMES::Vector dr = QGAMES::Vector::_cero;
		switch (e.code ())
		{
			// Comming from the knight...
			case __GAMETEST_KNIGHTREACHEDLEFTLIMIT__:
				dr = QGAMES::Vector (__BD -1, __BD 0, __BD 0); // Previous maze scene
				break;
			case __GAMETEST_KNIGHTREACHEDRIGHTLIMIT__:
				dr = QGAMES::Vector (__BD 1, __BD 0, __BD 0); // Next maze scene
				break;
			case __GAMETEST_KNIGHTREACHEDUPLIMIT__:
				dr = QGAMES::Vector (__BD 0, __BD -1, __BD 0);
				break;
			case __GAMETEST_KNIGHTREACHEDDOWNLIMIT__:
				dr = QGAMES::Vector (__BD 0, __BD 1, __BD 0);
				break;

			default:
				break; // Not needed really...
		}

		// Changes the scene and verifies whether the exit scene has been already reached
		if (setNextMazeSceneInDirection (dr))
			notify (QGAMES::Event (__GAMETEST_KNIGHTREACHEDENDOFMAZE__, this));
	}
	else if (e.code () == __GAMETEST_SCENETIMEFINISHES__)
		notify (e); // There is nothing special to do so far with this event...
	else if (e.code () == __GAMETEST_TOREBUILDTHESCENE__)
		setMazeScene (((TestPlatformGame::Game*) game ()) -> mazeScene ());
	else
		QGAMES::World::processEvent (e);
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::Scene::Scene (int c, const QGAMES::Maps& m, const QGAMES::Scene::Connections& cn, 
					const QGAMES::SceneProperties& p, const QGAMES::EntitiesPerLayer& ePL)
				: QGAMES::Scene (c, m, cn, p, ePL)
{ 
	game () -> createLoopCounter (__GAMETEST_SCENELOOPACTIONID__, 
		game () -> framesPerSecond () /* every second */, true, new LoopCounterAction (this)); 
	// It will be destroyed by the game main loop
}

// ---
void TestPlatformGame::Scene::setSceneType (int sT)
{
	assert (sT < __GAMETEST_NUMBEROFTYPESOFSCENES__);

	((TestPlatformGame::Map*) activeMap ()) -> setMapType (_SCENESTYPE [sT]);
}

// ---
void TestPlatformGame::Scene::toAddMoreTime (int nS)
{
	int aS = counter (_COUNTERSECONDS) -> value (); aS += nS; 
	if (aS > _DIFFICULTYRELATEDDATA [((TestPlatformGame::Game::Conf*) game () -> configuration ()) -> difficulty ()][0])
		aS = _DIFFICULTYRELATEDDATA [((TestPlatformGame::Game::Conf*) game () -> configuration ()) -> difficulty ()][0]; 
	counter (_COUNTERSECONDS) -> initialize (0, aS, false, false);
}

// ---
void TestPlatformGame::Scene::updatePositions ()
{
	if (onOffSwitch (_SWITCHTOCOUNTSECONDS) -> isOn ())
	{
		onOffSwitch (_SWITCHTOCOUNTSECONDS) -> set (false);
		if (counter (_COUNTERSECONDS) -> isEnd ())
		{
			((TestPlatformGame::Knight*) entity (__GAMETEST_MAINCHARACTERID__)) -> toDie (); // The player also dies...
			notify (QGAMES::Event (__GAMETEST_SCENETIMEFINISHES__, this)); // No more time in the scene...
		}
	}

	QGAMES::Scene::updatePositions ();
}

// ---
void TestPlatformGame::Scene::initialize ()
{
	addEntity (__AT (__AGM game ()) -> artist (__GAMETEST_MAINCHARACTERID__));
	for (int i = 0; i < __GAMETEST_NUMBERVILLANERS__; i++)
		addEntity (__AT (__AGM game ()) -> artist (__GAMETEST_VILLANERCHARACTERBASEID__ + i));
	for (int i = 0; i < __GAMETEST_MAXNUMBERTHINGSINTHEMAZEPERROOM__; i++)
		addEntity (__AT (__AGM game ()) -> artist (__GAMETEST_THINGTOCATCHBASEID__ + i));
	for (int i = 0; i < __GAMETEST_MAXNUMBEROFMEALSINMAZEPERROOM__; i++)
		addEntity (__AT (__AGM game ()) -> artist (__GAMETEST_MEALBASEID__ + i));

	setMap (__GAMETEST_MAPID__);

	counter (_COUNTERSECONDS) -> 
		initialize (0, _DIFFICULTYRELATEDDATA [((TestPlatformGame::Game::Conf*) game () ->
			configuration ()) -> difficulty ()][0], false, false);

	reStartAllCounters ();
	reStartAllOnOffSwitches ();

	QGAMES::Scene::initialize ();
}

// ---
void TestPlatformGame::Scene::processEvent (const QGAMES::Event& e)
{
	// Comming from the knight...
	if (e.code () == __GAMETEST_KNIGHTREACHEDLEFTLIMIT__  ||
		e.code () == __GAMETEST_KNIGHTREACHEDRIGHTLIMIT__ ||
		e.code () == __GAMETEST_KNIGHTREACHEDUPLIMIT__ ||
		e.code () == __GAMETEST_KNIGHTREACHEDDOWNLIMIT__)
		notify (e); // and send it upper to the world...
	// From the knight too when it eat something proving more time...
	else if (e.code () == __GAMETEST_SCENETIMETOBEINCREMENTED__)
		toAddMoreTime (20); // 20 more...
	else if (e.code () ==  __GAMETEST_TOREBUILDTHESCENE__, this)
		notify (e); // To be processed by the World...
	else
		QGAMES::Scene::processEvent (e);
}

// ---
void TestPlatformGame::Scene::LoopCounterAction::doAction ()
{
	_sceneOwner -> onOffSwitch (TestPlatformGame::Scene::_SWITCHTOCOUNTSECONDS) -> set (true);
}

// ---
__IMPLEMENTCOUNTERS__ (TestPlatformGame::Scene::Counters)
{
	// The user jas always just 120 seconds by default (it could depend on the difficulty level) to add more time...
	// It counts backwards...
	addCounter (new QGAMES::Counter 
		(TestPlatformGame::Scene::_COUNTERSECONDS, 0, 120, false, false));
}

// ---
__IMPLEMENTONOFFSWITCHES__ (TestPlatformGame::Scene::OnOffSwitches)
{
	addOnOffSwitch (new QGAMES::OnOffSwitch (TestPlatformGame::Scene::_SWITCHTOCOUNTSECONDS, false));
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
void TestPlatformGame::Map::setMapType (const int* lI)
{
	for (int i = 0; i < __GAMETEST_NUMBEROFLAYERSPOSSIBLEPERSCENE__; i++)
		layer (std::string (_LAYERNAME [i])) -> setVisible ((lI [i] == 1) ? true : false);
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
QGAMES::World* TestPlatformGame::WorldBuilder::createWorldObject (int no, const QGAMES::Scenes& s, 
		const QGAMES::WorldProperties& p)
{
	QGAMES::World* result = NULL;
	if (no == __GAMETEST_WORLDID__)
		result = new TestPlatformGame::World (no, s, p);
	return (result);
}

// ---
QGAMES::Scene* TestPlatformGame::WorldBuilder::createSceneObject (int ns, const QGAMES::Maps& m, 
		const QGAMES::Scene::Connections& cn, 
		const QGAMES::SceneProperties& p, const QGAMES::EntitiesPerLayer& ePL)
{
	QGAMES::Scene* result = NULL;
	if (ns == __GAMETEST_SCENEID__)
		result = new TestPlatformGame::Scene (ns, m, cn, p, ePL);
	return (result);
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::ControllingGameStates::ControllingGameStates (QGAMES::GameState* psSt)
	: QGAMES::StandardGameStateControl (__GAMETEST_CONTROLGAMESTATE__, psSt, QGAMES::GameStates ())
{
	QGAMES::GameStates sts;
	sts [std::string (__QGAMES_GAMESTATELOADINGNAME__)] =
		game () -> state (std::string (__QGAMES_GAMESTATELOADINGNAME__));
	sts [std::string (__GAMETEST_INTROGAMESTATENAME__)] = 
		game () -> state (std::string (__GAMETEST_INTROGAMESTATENAME__));
	sts [std::string (__GAMETEST_PRESENTATIONGAMESTATENAME__)] = 
		game () -> state (std::string (__GAMETEST_PRESENTATIONGAMESTATENAME__));
	sts [std::string (__GAMETEST_SELECTIONGAMESTATENAME__)] = 
		game () -> state (std::string (__GAMETEST_SELECTIONGAMESTATENAME__));
	sts [std::string (__GAMETEST_HALLOFFAMEGAMESTATENAME__)] =
		game () -> state (std::string (__GAMETEST_HALLOFFAMEGAMESTATENAME__));
	sts [std::string (__GAMETEST_ABOUTTOSTARTGAMESTATENAME__)] =
		game () -> state (std::string (__GAMETEST_ABOUTTOSTARTGAMESTATENAME__));
	sts [std::string (__GAMETEST_PLAYINGGAMESTATENAME__)] =
		game () -> state (std::string (__GAMETEST_PLAYINGGAMESTATENAME__));
	sts [std::string (__GAMETEST_LEVELHASCOMPLETEDGAMESTATENAME__)] =
		game () -> state (std::string (__GAMETEST_LEVELHASCOMPLETEDGAMESTATENAME__));
	sts [std::string (__GAMETEST_PLAYERHASDIEDGAMESTATENAME__)] =
		game () -> state (std::string (__GAMETEST_PLAYERHASDIEDGAMESTATENAME__));
	sts [std::string (__GAMETEST_GAMEHASENDEDGAMESTATENAME__)] =
		game () -> state (std::string (__GAMETEST_GAMEHASENDEDGAMESTATENAME__));
	sts [std::string (__GAMETEST_PLAYERHASWONSTATENAME__)] =
		game () -> state (std::string (__GAMETEST_PLAYERHASWONSTATENAME__));
	sts [std::string (__QGAMES_EXITGAMEGAMESTATENAME__)] =
		game () -> state (std::string (__QGAMES_EXITGAMEGAMESTATENAME__));
	setStates (sts);
}

// ---
std::string TestPlatformGame::ControllingGameStates::nextGameState (QGAMES::GameState* st, const QGAMES::Event& evnt) const
{
	assert (st);

	std::string result (__NULL_STRING__);
	switch (st -> type ())
	{
		// From loading to show "hello world"...
		case __QGAMES_GAMESTATELOADING__:
			result = std::string (__GAMETEST_INTROGAMESTATENAME__);
			break;

		// From intro to presentation...
		case __GAMETEST_INTROGAMESTATE__:
			result = std::string (__GAMETEST_PRESENTATIONGAMESTATENAME__);
			break;

		// From the presentation game state to the selection game state...
		case __GAMETEST_PRESENTATIONGAMESTATE__:
			result = std::string (__GAMETEST_SELECTIONGAMESTATENAME__);
			break;

		// From the selection to...depends on the option selected
		case __GAMETEST_SELECTIONGAMESTATE__:
			{
				TestPlatformGame::SelectionGameState* gS = (TestPlatformGame::SelectionGameState*) st;
				switch (gS -> lastOptionSelected ())
				{
					case __QGAMES_STDGUISYSTEMSTARTGAMEWDT__:
						result = std::string (__GAMETEST_ABOUTTOSTARTGAMESTATENAME__); // To start the game
						break;

					case __QGAMES_STDGUISYSTEMHALLOFFAMEWDT__:
						result = std::string (__GAMETEST_HALLOFFAMEGAMESTATENAME__); // To the hall of fame
						break;

					case __QGAMES_STDGUISYSTEMEXITWDT__:
						result = std::string (__QGAMES_EXITGAMEGAMESTATENAME__); // To exit
						break;

					default:
						assert (0); // It shouldn't be here...
						break;
				}
			}

			break;

		// After the hall of fame, then the selection is back...
		case __GAMETEST_HALLOFFAMEGAMESTATE__:
			result = std::string (__GAMETEST_SELECTIONGAMESTATENAME__);
			break;

		// After saying the game is about to start...it starts eventually!
		case __GAMETEST_ABOUTTOSTARTGAMESTATE__:
			result = std::string (__GAMETEST_PLAYINGGAMESTATENAME__);
			break;

		// Playing finishes just because only two reasons:
		// either the player has reached his goal and it is time to finishes the game for him...
		// or the player has died, then it is time the next player to play or (if there is no more lives)
		// the game ended (so probably here we have the most complex logic in terms of game state flow of the game)
		case __GAMETEST_PLAYINGGAMESTATE__:
			{
				TestPlatformGame::Playing* pS = (TestPlatformGame::Playing*) st;
				switch (pS -> result ())
				{
					case TestPlatformGame::Playing::Result::_PLAYERDIED:
						result = std::string (__GAMETEST_PLAYERHASDIEDGAMESTATENAME__);
						break;

					case TestPlatformGame::Playing::Result::_LEVELACHIEVED:
						result = std::string (__GAMETEST_LEVELHASCOMPLETEDGAMESTATENAME__);
						break;

					default:
						assert (0); // It shouldn't be here even when the current result is still playing...
						break;
				}
			}

			break;

		// From player has died to game has ended or start back!
		// depending on the number of lives still to consume
		case __GAMETEST_PLAYERHASDIEDGAMESTATE__:
			{
				const TestPlatformGame::Game* g = (const TestPlatformGame::Game*) game ();
				const TestPlatformGame::Game::Conf* cfg = (const TestPlatformGame::Game::Conf*) g -> configuration ();
				if (g -> lives () == 1 && g -> currentPlayer () == cfg -> numberPlayers ())
					result = std::string (__GAMETEST_GAMEHASENDEDGAMESTATENAME__);
				else
					result = std::string (__GAMETEST_ABOUTTOSTARTGAMESTATENAME__);
			}

			break;

		// From player has completed the level so the game finishes!
		case __GAMETEST_LEVELHASCOMPLETEDGAMESTATE__:
			result = std::string (__GAMETEST_PLAYERHASWONSTATENAME__);
			break;

		// From game has ended to the hall of fame, with the results!!
		case __GAMETEST_GAMEHASENDEDGAMESTATE__:
			result = std::string (__GAMETEST_HALLOFFAMEGAMESTATENAME__);
			break;

		// From player has reached the goal to the hall of the fame, with the result!!
		case __GAMETEST_PLAYERHASWONSTATE__:
			result = std::string (__GAMETEST_HALLOFFAMEGAMESTATENAME__);
			break;

		default:
			assert (0); // It shouldn't be here...
			break;
	}

	return (result);
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
bool TestPlatformGame::UntilReturnKeyOrMouseLeftButtonIsPressed::StopStateHandler::stopWhen (const QGAMES::Event& evnt)
{
	bool result = false;
	if (evnt.code () == __QGAMES_KEYBOARDEVENT__)
	{
		QGAMES::KeyBoardEventData* dt = (QGAMES::KeyBoardEventData*) evnt.data ();
		if (!dt -> _on && dt ->_internalCode == QGAMES::KeyCode::QGAMES_RETURN)
			result = true;
	}
	else
	if (evnt.code () == __QGAMES_MOUSEBUTTONEVENT__)
	{
		QGAMES::MouseButtonEventData* dt = (QGAMES::MouseButtonEventData*) evnt.data ();
		if (!dt -> _on && dt ->_internalCode == QGAMES::MouseButtonCode::QGAMES_BUTTONLEFT)
			result = true;
	}

	return (result);
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
TestPlatformGame::ShowingPartOfPresentationGameState::ShowingPartOfPresentationGameState 
		(int id, int frmId, const std::string& txt, int t)
	: QGAMES::ShowingTextGameState (id + 1, QGAMES::ShowingTextGameState::Properties (),
		new QGAMES::ShowingFixFormGameState (id + 2, 
					QGAMES::ShowingFixFormGameState::Properties (frmId, 
					255, //Totally shown
					QGAMES::Position (__BD 0, __BD 0, __BD 0), // Top left
					QGAMES::ShowingFixFormGameState::Properties::PosReference::_TOPLEFT))),
			  _text (txt),
			  _textType (t)
{ 
	_form = (QGAMES::ShowingFixFormGameState*) nestedState (); 

	// Set the properties of the text...
	setProperties (QGAMES::ShowingTextGameState::Properties (
		255, // Totally shown
		QGAMES::Position (__BD 0.5, __BD 0.5,__BD 0), // In the middle
		QGAMES::ShowingTextGameState::Properties::PosReference::_CENTER, // In the middle
		QGAMES::Vector (__BD 1, __BD 0, __BD 0)));

	// ...abd also of the form...
	_form -> setProperties (QGAMES::ShowingFixFormGameState::Properties (
		frmId, 
		255, //Totally shown
		QGAMES::Position (__BD 0, __BD 0, __BD 0), // Top left
		QGAMES::ShowingFixFormGameState::Properties::PosReference::_TOPLEFT));
}

// ---
TestPlatformGame::ShowingPartOfPresentationGameState::~ShowingPartOfPresentationGameState ()
{ 
	delete (_form); 
	_form = NULL; 
}

// ---
QGAMES::ScoreObjectText* TestPlatformGame::ShowingPartOfPresentationGameState::createText ()
{
	QGAMES::ScoreObjectText* result = NULL;

	if (_textType == 2)
		result = new QGAMES::Ravie36OrangeFont (_text);
	else if (_textType == 1)
		result = new QGAMES::Ravie24BlueFont (_text);
	else
		result = new QGAMES::Ravie24GreenFont (_text);

	return (result);
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::PresentationGameState::PresentationGameState ()
	: TestPlatformGame::UntilReturnKeyOrMouseLeftButtonIsPressed (__GAMETEST_PRESENTATIONGAMESTATE__,
		new QGAMES::RollingStatesGameState (__GAMETEST_PRESENTATIONGAMESTATE__ + 1, 
			TestPlatformGame::PresentationGameState::separationStates (), 
		new QGAMES::PlayingASoundGameState (__GAMETEST_PRESENTATIONGAMESTATE__ + 2))),
	  _rollingStates (NULL),
	  _playingSound (NULL)
{
	_rollingStates = (QGAMES::RollingStatesGameState*) nestedState ();
	_playingSound = (QGAMES::PlayingASoundGameState*) _rollingStates -> nestedState ();

	// Stablish the properties of the sound...
	_playingSound -> setProperties (QGAMES::PlayingASoundGameState::Properties
		(__GAMETEST_PRESENTATIONBACKGROUNDSOUNDID__,
		 __GAMETEST_GAMESTATESOUNDCHANNEL__
		));
}

// ---
TestPlatformGame::PresentationGameState::~PresentationGameState ()
{
	delete (_rollingStates);
	_rollingStates = NULL;
	delete (_playingSound);
	_playingSound = NULL;
}

// ---
QGAMES::RollingStatesGameState::SeparationStates TestPlatformGame::PresentationGameState::separationStates ()
{
	QGAMES::RollingStatesGameState::SeparationStates result;
	
	QGAMES::RollingStatesGameState::SeparationState s1 (
		QGAMES::RollingStatesGameState::_CIRCLEGROWINGFROMCENTER, 
			std::to_string (__GAMETEST_SHOWINGPRESENTATIONSTATEBASE__), __BD 4, 1, 2);
	result.push_back (s1);
	QGAMES::RollingStatesGameState::SeparationState s2 (
		QGAMES::RollingStatesGameState::_CIRCLEDECRESINGTOCENTER, 
			std::to_string (__GAMETEST_SHOWINGPRESENTATIONSTATEBASE__ + 1), __BD 4, 1, 2);
	result.push_back (s2);
	QGAMES::RollingStatesGameState::SeparationState s3 (
		QGAMES::RollingStatesGameState::_VERTICALLINEFROMLEFTTORIGHT, 
			std::to_string (__GAMETEST_SHOWINGPRESENTATIONSTATEBASE__ + 2), __BD 4, 1, 2);
	result.push_back (s3);
	QGAMES::RollingStatesGameState::SeparationState s4 (
		QGAMES::RollingStatesGameState::_VERTICALLINEFROMRIGHTTOLEFT, 
			std::to_string (__GAMETEST_SHOWINGPRESENTATIONSTATEBASE__ + 3), __BD 4, 1, 2);
	result.push_back (s4);
	QGAMES::RollingStatesGameState::SeparationState s5 (
		QGAMES::RollingStatesGameState::_HORIZONTALLINEFROMTOPTODOWN, 
			std::to_string (__GAMETEST_SHOWINGPRESENTATIONSTATEBASE__ + 4), __BD 4, 1, 2);
	result.push_back (s5);
	// This last state it is not really needed for anything...
	QGAMES::RollingStatesGameState::SeparationState s6 (
		QGAMES::RollingStatesGameState::_HORIZONTALLINEFROMDOWNTOTOP, 
			std::to_string (__GAMETEST_SHOWINGPRESENTATIONSTATEBASE__ + 5), __BD 4, 1, 2);
	result.push_back (s6);

	return (result);
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::SelectionGameState::SelectionGameState ()
	: QGAMES::StandardInitialSelectionOptionsGameState (__GAMETEST_SELECTIONGAMESTATE__,
		new QGAMES::ShowingFixFormGameState (__GAMETEST_SELECTIONGAMESTATE__ + 1, 
			QGAMES::ShowingFixFormGameState::Properties (),
		new QGAMES::ShowingFixFormGameState (__GAMETEST_SELECTIONGAMESTATE__ + 2,
			QGAMES::ShowingFixFormGameState::Properties (),
		new QGAMES::PlayingASoundGameState (__GAMETEST_SELECTIONGAMESTATE__ + 3)))),
	  _logoForm (NULL),
	  _backgroundForm (NULL),
	  _playingSound (NULL)
{
	_logoForm = (QGAMES::ShowingFixFormGameState*) nestedState ();
	_backgroundForm = (QGAMES::ShowingFixFormGameState*) _logoForm -> nestedState ();
	_playingSound =  (QGAMES::PlayingASoundGameState*) _backgroundForm -> nestedState ();
		
	// Properties for the state showing the logo of comuunity...
	_logoForm -> setProperties (QGAMES::ShowingFixFormGameState::Properties
		(__QGAMES_COMMTYNETSSMALLLOGO__,
		 255, /** Fade */ 
		 QGAMES::Position (__BD (__GAMETEST_SCREENWIDTH__ - 110) / __BD __GAMETEST_SCREENWIDTH__, 
						   __BD (__GAMETEST_SCREENHEIGHT__ - 44) / __BD __GAMETEST_SCREENHEIGHT__, 
						   __BD 0), /** Reference position in the screen */
		 QGAMES::ShowingFixFormGameState::Properties::PosReference::_TOPLEFT /** Reference position of the form */
		));

	// Properties of the background...
	_backgroundForm -> setProperties (QGAMES::ShowingFixFormGameState::Properties
		(__GAMETEST_MEDIEVALBACKGROUND__,
		 255, /** Fade **/
		 QGAMES::Position::_cero, /** Top left corner. **/
		 QGAMES::ShowingFixFormGameState::Properties::PosReference::_TOPLEFT
		));

	// ...and for the sound...
	_playingSound -> setProperties (QGAMES::PlayingASoundGameState::Properties
		(__GAMETEST_MENUBACKGROUNDSOUNDID__,
		 __GAMETEST_GAMESTATESOUNDCHANNEL__
		));
}

// ---
TestPlatformGame::SelectionGameState::~SelectionGameState ()
{
	delete (_playingSound);
	_playingSound = NULL;
	delete (_backgroundForm);
	_backgroundForm = NULL;
	delete (_logoForm);
	_logoForm = NULL;
}

// ---
void TestPlatformGame::SelectionGameState::onEnter ()
{
	QGAMES::StandardInitialSelectionOptionsGameState::onEnter ();

	// Always the selection menu is open, the current player is back to the first...
	((TestPlatformGame::Game*) game ()) -> setCurrentPlayer (1);
}

// ---
void TestPlatformGame::SelectionGameState::initializeGUISystem ()
{
	QGAMES::StandardInitialSelectionOptionsGameState::initializeGUISystem ();

	QGAMES::CompositeWidget* mWdt = 
		dynamic_cast <QGAMES::CompositeWidget*> (currentGUISystem () -> mainWidget ());
	// It is not needed to assert it as it has been already done at parent level
	if (mWdt -> id () == __QGAMES_STDGUISYSTEMMAINCONFWIDGET__)
	{
		std::vector <int> controlKeys = _configuration -> controlKeys ();
		// It is not needed to assert the size at it has already been done at parent level...
		((QGAMES::IteratingLabelWidget*) 
			(mWdt -> widget (__GAMETEST_GUISYSTEMCONFKEYITERATEWDT__))) -> 
				setCurrentValue (_keyCodesConverterInv [(QGAMES::KeyCode) controlKeys [__GAMETEST_KEYITERATE__]]);
		((QGAMES::IteratingLabelWidget*) 
			(mWdt -> widget (__GAMETEST_GUISYSTEMCONFKEYCATCHLEAVEWDT__))) -> 
				setCurrentValue (_keyCodesConverterInv [(QGAMES::KeyCode) controlKeys [__GAMETEST_KEYCATCHLEAVE__]]);
	}
}

// ---
void TestPlatformGame::SelectionGameState::finalizeGUISystem ()
{
	QGAMES::StandardInitialSelectionOptionsGameState::finalizeGUISystem ();

	QGAMES::CompositeWidget* mWdt = 
		dynamic_cast <QGAMES::CompositeWidget*> (currentGUISystem () -> mainWidget ());
	// It is not needed to assert it as it has been already done at parent level
	if (mWdt -> id () == __QGAMES_STDGUISYSTEMMAINCONFWIDGET__)
	{
		std::vector <int> controlKeys (9);
		controlKeys = _configuration -> controlKeys ();
		controlKeys [__GAMETEST_KEYITERATE__] = 
			_keycodesConverter [((QGAMES::IteratingLabelWidget*) 
				(mWdt -> widget (__GAMETEST_GUISYSTEMCONFKEYITERATEWDT__))) -> currentValue ()];
		controlKeys [__GAMETEST_KEYCATCHLEAVE__] = 
			_keycodesConverter [((QGAMES::IteratingLabelWidget*) 
				(mWdt -> widget (__GAMETEST_GUISYSTEMCONFKEYCATCHLEAVEWDT__))) -> currentValue ()];
		_configuration -> setControlKeys (controlKeys);
	}
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
void TestPlatformGame::ShowingPlayerAboutToStart::actualizeText ()
{
	QGAMES::ShowingTextGameState::actualizeText ();
	std::string str = std::string ("Player ") + 
		std::to_string (((TestPlatformGame::Game*) game ()) -> currentPlayer ());
	_text -> setMaxTextLength (str.length ()); _text -> setText (str);
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::GameAboutToStart::GameAboutToStart ()
	: QGAMES::SpendingTimeGameState (__GAMETEST_ABOUTTOSTARTGAMESTATE__, __BD 3 /** Seconds maximum. */, 
		new TestPlatformGame::UntilReturnKeyOrMouseLeftButtonIsPressed (__GAMETEST_ABOUTTOSTARTGAMESTATE__ + 1,
		new TestPlatformGame::ShowingPlayerAboutToStart (__GAMETEST_ABOUTTOSTARTGAMESTATE__ + 2,
		new QGAMES::ShowingFixFormGameState (__GAMETEST_ABOUTTOSTARTGAMESTATE__ + 3,
			QGAMES::ShowingFixFormGameState::Properties ())))),
	  _untilPressed (NULL),
	  _numberPlayer (NULL),
	  _background (NULL),
	  _guiSystem (NULL),
	  _nameIntroduced (false)
{
	_untilPressed = (TestPlatformGame::UntilReturnKeyOrMouseLeftButtonIsPressed*) nestedState ();
	_numberPlayer = (TestPlatformGame::ShowingPlayerAboutToStart*) _untilPressed -> nestedState ();
	_background = (QGAMES::ShowingFixFormGameState*) _numberPlayer -> nestedState ();

	_background -> setProperties (QGAMES::ShowingFixFormGameState::Properties 
		(__GAMETEST_MEDIEVALCASTLE2__, 
		 255, /** Fade */ 
		 QGAMES::Position (__BD 0.5, __BD 0.5, __BD 0), /** Reference position in the screen */
		 QGAMES::ShowingFixFormGameState::Properties::PosReference::_CENTER /** Reference position of the form */
		));
}

// ---
TestPlatformGame::GameAboutToStart::~GameAboutToStart ()
{
	delete(_background);
	_background = NULL;
	delete (_numberPlayer);
	_numberPlayer = NULL;
	delete (_untilPressed);
	_untilPressed = NULL;

	deleteGUISystem ();
}

// ---
void TestPlatformGame::GameAboutToStart::onEnter ()
{
	QGAMES::SpendingTimeGameState::onEnter ();

	if (!(_nameIntroduced = 
			(((TestPlatformGame::Game*) game ()) -> playerName () != std::string (__NULL_STRING__))))
	{
		_guiSystem = game () -> guiSystemBuilder () -> system (__GAMETEST_PLAYERNAMEGUISYSTEM__);
		assert (dynamic_cast <QGAMES::CompositeWidget*> (_guiSystem -> mainWidget ())); // Just in case...
		observe (_guiSystem -> mainWidget ());
	}
	else
	{
		QGAMES::ShowingTextGameState::Properties prps = _numberPlayer -> properties ();
		prps._position = QGAMES::Position (__BD 0.5, __BD 0.5, __BD 0);
		_numberPlayer -> setProperties (prps); 
		// Put the text in the middle, because to request the name is no longer needed for this player at least!
	}
}

// ---
void TestPlatformGame::GameAboutToStart::updatePositions ()
{
	// The parent method is not invoked because its behaviour has been changed...
	// Now the state finishes not only when the count down finishes but also when the name has already been introduced...
	QGAMES::GameState::updatePositions ();

	if (counter (_COUNTERTIME) -> isEnd () && _nameIntroduced)
		notify (QGAMES::Event (__QGAMES_GAMENESTEDSTATEFINISHED__, this));
}

// ---
void TestPlatformGame::GameAboutToStart::onExit ()
{
	QGAMES::SpendingTimeGameState::onExit ();

	// When exiting the state, the name has been introduced,
	// or it was previously...
	if (_guiSystem)
		((TestPlatformGame::Game*) game ()) -> setPlayerName (((QGAMES::IntroWidget*) 
			((QGAMES::CompositeWidget*) _guiSystem -> mainWidget ()) -> widget (__GAMETEST_PLAYERNAMEOPTIONID__)) -> text ());

	deleteGUISystem ();
}

// ---
void TestPlatformGame::GameAboutToStart::processEvent (const QGAMES::Event& evnt)
{
	// The events from the nested states are only propagated when the name has already been introduced...or it was!
	if (_nameIntroduced ||
		(evnt.code () == __QGAMES_GAMENESTEDSTATEFINISHED__ &&
		((QGAMES::IntroWidget*) ((QGAMES::CompositeWidget*) _guiSystem -> mainWidget ()) -> 
			widget (__GAMETEST_PLAYERNAMEOPTIONID__)) -> text () != std::string (__NULL_STRING__)))
	{
		_nameIntroduced = true;
		notify (evnt);
	}
}

// ---
void TestPlatformGame::GameAboutToStart::deleteGUISystem ()
{
	if (_guiSystem)
		unObserve (_guiSystem -> mainWidget ());

	delete (_guiSystem);
	_guiSystem = NULL;
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::Playing::Playing ()
	: QGAMES::GameState (__GAMETEST_PLAYINGGAMESTATE__, QGAMES::Game::game (), 
		new QGAMES::PlayingASoundGameState (__GAMETEST_PLAYINGGAMESTATE__ + 1)),
	  _result (TestPlatformGame::Playing::Result::_STILLPLAYING),
	  _playingSound (NULL)
{
	_playingSound =  (QGAMES::PlayingASoundGameState*) nestedState ();

	// Sets the configuration for the background sound...
	_playingSound -> setProperties (QGAMES::PlayingASoundGameState::Properties
		(__GAMETEST_PLAYINGBACKGROUNDSOUNDID__,
		 __GAMETEST_GAMESTATESOUNDCHANNEL__
		));
}

// ---
TestPlatformGame::Playing::~Playing ()
{
	delete (_playingSound);
	_playingSound = NULL;
}

// ---
void TestPlatformGame::Playing::onEnter ()
{
	QGAMES::GameState::onEnter ();

	game () -> addScoreObjects ();

#ifndef NDEBUG
	// Observe the input handler to move the visualization...
	observe (game () -> inputHandler ());
#endif

	(__AGM game ()) -> setWorld (__GAMETEST_WORLDID__);
	QGAMES::World* wrld = (__AGM game ()) -> world (__GAMETEST_WORLDID__);
	wrld -> initialize ();
	observe (wrld);

	observe (game () -> entity (__GAMETEST_MAINCHARACTERID__));

	// The initial maze scene is kept in the configuration...
	((TestPlatformGame::World*) wrld) -> setMazeScene (((TestPlatformGame::Game*) game ()) -> mazeScene ());

	// The position of the screen is set to guaranttee that up corner of any part of the maze if at the top
	// ...and the main of the full playing zone is visible!
	QGAMES::TiledMap* tM = __TM wrld -> activeScene () -> activeMap ();
	int mCX = tM -> width () >> 2; int mCY = tM -> height () >> 1;
	game () -> mainScreen () -> setPosition (
		QGAMES::Position (
			__BD (((mCX - mCY) >> 1) - (__GAMETEST_SCREENWIDTH__ >> 1)), // (mCX - mCY) >> 1 is the middle of the maze in diedric...
			__BD ((((mCX + mCY) >> 1) - (__GAMETEST_SCREENHEIGHT__ >> 1)) - 32), 
			__BD 0));

	// The type of projection in this game is diedric (the map is diediric too)...
	game () -> mainScreen () -> currentCamera () -> setProjectType (QGAMES::Camera::ProjectionType::_QUICKDIEDRIC);

	// Sets the initial position of the entity...
	QGAMES::Entity* ety = game () -> entity (__GAMETEST_MAINCHARACTERID__);
	QGAMES::Position lP = ((TestPlatformGame::Game*) game ()) -> lastPosition ();
	ety -> setPosition (lP - 
		QGAMES::Vector (__BD 0, __BD 0, __BD 32) -
		QGAMES::Vector (__BD 0, __BD 0, 
			__BD (ety -> currentForm () -> frameHeightForFrame (ety -> currentAspect ()) - 
				 (ety -> currentForm () -> frameWidthForFrame (ety -> currentAspect ()) >> 1))));
	
	// Stops music if the configuration says so...
	if (!((TestPlatformGame::Game::Conf*) game () -> configuration ()) -> musicOn ())
		_playingSound -> stopSound ();

	// This state keeps trace about what is happening...
	_result = TestPlatformGame::Playing::Result::_STILLPLAYING;
}

// ---
void TestPlatformGame::Playing::updatePositions ()
{
	QGAMES::GameState::updatePositions ();

	if (_result != TestPlatformGame::Playing::Result::_STILLPLAYING)
		notify (QGAMES::Event (__QGAMES_GAMENESTEDSTATEFINISHED__, this));
}

// ---
void TestPlatformGame::Playing::onExit ()
{
	QGAMES::GameState::onExit ();

	// Back to orthogonal...
	game () -> mainScreen () -> currentCamera () -> setProjectType (QGAMES::Camera::ProjectionType::_ORTHOGONAL);
	// ...and put the screen at the cero position...
	game () -> mainScreen () -> setPosition (QGAMES::Position::_cero);

	unObserve (game () -> entity (__GAMETEST_MAINCHARACTERID__));

	(__AGM game ()) -> world (__GAMETEST_WORLDID__) -> finalize ();
	(__AGM game ()) -> setWorld (__MININT__);

	unObserve ((__AGM game ()) -> world (__GAMETEST_WORLDID__));

#ifndef NDEBUG
	unObserve (game () -> inputHandler ());
#endif

	game () -> removeScoreObjects ();

	// Puts back the position of the entity into the configuration file, 
	// Because it can be needed later...
	QGAMES::Entity* ety = game () -> entity (__GAMETEST_MAINCHARACTERID__);
	((TestPlatformGame::Game*) game ()) -> setLastPosition (ety -> position () - 
		QGAMES::Vector (__BD 0, __BD 0, ety -> position ().posZ ())); // 
	// With no Z ccordinate (it will be set when the game starts back)
	
	// It is supossed that the result variable has been changed along the game...
}

// ---
void TestPlatformGame::Playing::processEvent (const QGAMES::Event& evnt)
{
#ifndef _NDEBUG
	// Only in debug mode to control what is being visualized!
	if (evnt.code () == __QGAMES_MOUSEMOVEMENTEVENT__)
	{
		QGAMES::MouseStateData mSt = game () -> inputHandler () -> mouseState ();
		game () -> mainScreen () -> setPosition (
			QGAMES::Position (__BD (-1500 + (mSt._X._rel << 1)), __BD (-900 + (mSt._Y._rel << 1)), __BD 0));
	}
	else
	if (evnt.code () == __QGAMES_KEYBOARDEVENT__)
	{
		QGAMES::KeyBoardEventData* dt = (QGAMES::KeyBoardEventData*) evnt.data ();
		if (!dt -> _on)
		{
			int toAdd = 0;
			switch (dt -> _internalCode)
			{
				case QGAMES::KeyCode::QGAMES_RIGHT:
					toAdd = 1;   break;
				case QGAMES::KeyCode::QGAMES_LEFT:
					toAdd = -1;  break;
				case QGAMES::KeyCode::QGAMES_UP:
					toAdd = -__GAMETEST_HORIZONTALSCENESINMAZE__; break;
				case QGAMES::KeyCode::QGAMES_DOWN:
					toAdd = __GAMETEST_HORIZONTALSCENESINMAZE__;  break;
				case QGAMES::KeyCode::QGAMES_ESCAPE:
					game () -> exitGame (); break;
				default:
					break;
			}

			int nMS = ((TestPlatformGame::Game*) game ()) -> mazeScene () + toAdd;
			if (nMS >= 0 && nMS < __GAMETEST_NUMBEROFSCENESINTHEMAZE__)
				((TestPlatformGame::World*) ((__AGM game ()) -> activeWorld ())) -> setMazeScene (nMS);
		}
	}
	else
#endif 

	// Comming from the knight...
	if (evnt.code () == __GAMETEST_KNIGHTDIED__)
		_result = TestPlatformGame::Playing::Result::_PLAYERDIED;
	else if (evnt.code () == __GAMETEST_KNIGHTREACHEDENDOFMAZE__)
		_result = TestPlatformGame::Playing::Result::_LEVELACHIEVED;
	else
		QGAMES::GameState::processEvent (evnt);
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::PlayerHasDied::PlayerHasDied ()
	: QGAMES::SpendingTimeGameState (__GAMETEST_PLAYERHASDIEDGAMESTATE__, __BD 3 /** Seconds before the state finishes */,
		new QGAMES::PlayingASoundGameState (__GAMETEST_PLAYERHASDIEDGAMESTATE__ + 1)),
	  _playingSound (NULL)
{
	_playingSound = (QGAMES::PlayingASoundGameState*) nestedState ();

	// Properties of the sound...
	_playingSound -> setProperties (QGAMES::PlayingASoundGameState::Properties
		(__GAMETEST_PLAYERHASDIEDBACKGROUNDSOUNDID__,
		 __GAMETEST_GAMESTATESOUNDCHANNEL__
		));
}

// ---
TestPlatformGame::PlayerHasDied::~PlayerHasDied ()
{
	delete (_playingSound);
	_playingSound = NULL;
}

// ---
void TestPlatformGame::PlayerHasDied::onExit ()
{
	QGAMES::SpendingTimeGameState::onExit ();

	((TestPlatformGame::Game*) game ()) -> addLives (-1); // One life less for the current player
	((TestPlatformGame::Game*) game ()) -> setNextPlayer (); // Change the player...if any
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::LevelCompleted::LevelCompleted ()
	: QGAMES::SpendingTimeGameState (__GAMETEST_LEVELHASCOMPLETEDGAMESTATE__, __BD 10 /** Seconds */, 
		new QGAMES::PlayingASoundGameState (__GAMETEST_LEVELHASCOMPLETEDGAMESTATE__ + 2)),
	  _playingSound (NULL)
{
	_playingSound = (QGAMES::PlayingASoundGameState*) nestedState ();

	// Set the properties for the sound...
	_playingSound -> setProperties (QGAMES::PlayingASoundGameState::Properties
		(__GAMETEST_PLAYERWINSBACKGROUNDSOUNDID__,
		 __GAMETEST_GAMESTATESOUNDCHANNEL__
		));
}

// ---
TestPlatformGame::LevelCompleted::~LevelCompleted ()
{
	delete (_playingSound);
	_playingSound = NULL;
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::GameHasEnded::GameHasEnded ()
	: QGAMES::SpendingTimeGameState (__GAMETEST_GAMEHASENDEDGAMESTATE__, __BD 10 /** seconds */,
		new TestPlatformGame::ShowingGameHasEnded (__GAMETEST_GAMEHASENDEDGAMESTATE__ + 1,
		new QGAMES::ShowingFixFormGameState (__GAMETEST_GAMEHASENDEDGAMESTATE__ + 2,
			QGAMES::ShowingFixFormGameState::Properties (),
		new QGAMES::PlayingASoundGameState (__GAMETEST_GAMEHASENDEDGAMESTATE__ + 3)))),
	  _text (NULL),
	  _background (NULL),
	  _playingSound (NULL)
{
	_text = (TestPlatformGame::ShowingGameHasEnded*) nestedState ();
	_background = (QGAMES::ShowingFixFormGameState*) _text -> nestedState ();
	_playingSound = (QGAMES::PlayingASoundGameState*) _background -> nestedState ();

	// Sets the behaviour of the background...
	_background -> setProperties (QGAMES::ShowingFixFormGameState::Properties
		(__GAMETEST_MEDIEVALCASTLE3__,
		 255, /** Fade */
		 QGAMES::Position (__BD 0.5, __BD 0.5, __BD 0), /** Reference position in the screen */
		 QGAMES::ShowingFixFormGameState::Properties::PosReference::_CENTER /** Reference position of the form */
		));
	
	// Same for the playing sound at the background...
	_playingSound -> setProperties (QGAMES::PlayingASoundGameState::Properties
		(__GAMETEST_GAMEFINISHEDBACKGROUNDSOUNDID__,
		 __GAMETEST_GAMESTATESOUNDCHANNEL__
		));
}

// ---
TestPlatformGame::GameHasEnded::~GameHasEnded ()
{
	delete (_playingSound);
	_playingSound = NULL;
	delete (_background);
	_background = NULL;
	delete (_text);
	_text = NULL;
}

// ---
void TestPlatformGame::GameHasEnded::onExit ()
{
	QGAMES::SpendingTimeGameState::onExit ();

	// Copy the results of the game into the hall of fame...
	((TestPlatformGame::Game*) game ()) -> actualizeHallOfFame ();
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::PlayerHasWon::PlayerHasWon ()
	: QGAMES::SpendingTimeGameState (__GAMETEST_PLAYERHASWONSTATE__, __BD 10 /** seconds */,
		new TestPlatformGame::ShowingPlayerHasWon (__GAMETEST_PLAYERHASWONSTATE__ + 1,
		new QGAMES::ShowingFixFormGameState (__GAMETEST_PLAYERHASWONSTATE__ + 2,
			QGAMES::ShowingFixFormGameState::Properties (),
		new QGAMES::PlayingASoundGameState (__GAMETEST_PLAYERHASWONSTATE__ + 3)))),
	  _text (NULL),
	  _background (NULL),
	  _playingSound (NULL)
{
	_text = (TestPlatformGame::ShowingGameHasEnded*) nestedState ();
	_background = (QGAMES::ShowingFixFormGameState*) _text -> nestedState ();
	_playingSound = (QGAMES::PlayingASoundGameState*) _background -> nestedState ();

	// Sets the behaviour of the background...
	_background -> setProperties (QGAMES::ShowingFixFormGameState::Properties
		(__GAMETEST_MEDIEVALCASTLE3__,
		 255, /** Fade */
		 QGAMES::Position (__BD 0.5, __BD 0.5, __BD 0), /** Reference position in the screen */
		 QGAMES::ShowingFixFormGameState::Properties::PosReference::_CENTER /** Reference position of the form */
		));
	
	// Same for the playing sound at the background...
	_playingSound -> setProperties (QGAMES::PlayingASoundGameState::Properties
		(__GAMETEST_GAMEFINISHEDBACKGROUNDSOUNDID__,
		 __GAMETEST_GAMESTATESOUNDCHANNEL__
		));
}

// ---
TestPlatformGame::PlayerHasWon::~PlayerHasWon ()
{
	delete (_playingSound);
	_playingSound = NULL;
	delete (_background);
	_background = NULL;
	delete (_text);
	_text = NULL;
}

// ---
void TestPlatformGame::PlayerHasWon::onExit ()
{
	QGAMES::SpendingTimeGameState::onExit ();

	// Copy the results of the game into the hall of fame...
	((TestPlatformGame::Game*) game ()) -> actualizeHallOfFame ();
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
std::map <std::string, QGAMES::SetOfOpenValues> TestPlatformGame::ShowingEntriesHallOfFame::orderHallOfFame 
	(const std::map <std::string, QGAMES::SetOfOpenValues>& hF)
{
	// list of scores, and per score which is the original entry...
	std::vector <int> scores;
	std::multimap <int, int> scoresPlayer;
	for (std::multimap <std::string, QGAMES::SetOfOpenValues>::const_iterator i = hF.begin ();
			i != hF.end (); i++)
	{
		scores.push_back ((*i).second.openValue (__GAMETEST_SCOREPLAYEROPENVALUE__).intValue ());
		scoresPlayer.insert (std::multimap <int, int>::value_type ((*i).second.openValue (__GAMETEST_SCOREPLAYEROPENVALUE__).intValue (),
			std::atoi ((*i).first.c_str ())));
	}

	// Order the scores...
	std::sort (scores.begin (), scores.end ());

	// Generates the new order...
	// But take care, if the are many equal scores, then the name of player can be repeated...
	std::map <std::string, QGAMES::SetOfOpenValues> result;
	for (std::vector <int>::const_iterator i = scores.begin (); i != scores.end (); i++)
		result.insert (std::map <std::string, QGAMES::SetOfOpenValues>::value_type (std::to_string ((*i)), 
			(*hF.find (std::to_string ((*scoresPlayer.find ((*i))).second))).second));
	return (result);
}

// ---
QGAMES::ScoreObjectText* TestPlatformGame::ShowingEntriesHallOfFame::createTextFor 
	(const std::string& eK, const QGAMES::SetOfOpenValues& sV)
{
	std::string scr = std::to_string (sV.openValue (__GAMETEST_SCOREPLAYEROPENVALUE__).intValue ());
	if (scr.length () < 5)  // The score...
		scr = std::string ("00000").substr (0, 5 - scr.length ()) + scr;
	return (new QGAMES::Ravie24OrangeFont (scr + std::string ("  ") +
		sV.openValue (__GAMETEST_NAMEPLAYEROPENVALUE__).strValue ()));
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::HallOfFameGameState::HallOfFameGameState ()
	: TestPlatformGame::UntilReturnKeyOrMouseLeftButtonIsPressed (__GAMETEST_HALLOFFAMEGAMESTATE__,
		new TestPlatformGame::ShowingEntriesHallOfFame (__GAMETEST_HALLOFFAMEGAMESTATE__ + 1,
		new QGAMES::ShowingFixFormGameState (__GAMETEST_HALLOFFAMEGAMESTATE__ + 1, 
			QGAMES::ShowingFixFormGameState::Properties (), 
		new QGAMES::PlayingASoundGameState (__GAMETEST_HALLOFFAMEGAMESTATE__ + 2)))),
	  _hallOfFame (NULL),
	  _logoForm (NULL),
	  _playingSound (NULL)
{
	_hallOfFame = (TestPlatformGame::UntilReturnKeyOrMouseLeftButtonIsPressed*) nestedState ();
	_logoForm = (QGAMES::ShowingFixFormGameState*) _hallOfFame -> nestedState ();
	_playingSound =  (QGAMES::PlayingASoundGameState*) _logoForm -> nestedState ();
		
	// Properties for the state showing the logo of comuunity...
	_logoForm -> setProperties (QGAMES::ShowingFixFormGameState::Properties (
		__QGAMES_COMMTYNETSSMALLLOGO__, 255 /** Fade */, 
			QGAMES::Position (__BD (__GAMETEST_SCREENWIDTH__ - 110) / __BD __GAMETEST_SCREENWIDTH__, 
							  __BD (__GAMETEST_SCREENHEIGHT__ - 44) / __BD __GAMETEST_SCREENHEIGHT__, 
							  __BD 0), QGAMES::ShowingFixFormGameState::Properties::PosReference::_TOPLEFT));

	// ...and for the sound...
	_playingSound -> setProperties (QGAMES::PlayingASoundGameState::Properties (
		__GAMETEST_HALLOFFAMEBACKGROUNDSOUNDID__, __GAMETEST_GAMESTATESOUNDCHANNEL__));
}

// ---
TestPlatformGame::HallOfFameGameState::~HallOfFameGameState ()
{
	delete (_playingSound);
	_playingSound = NULL;
	delete (_logoForm);
	_logoForm = NULL;
	delete (_hallOfFame);
	_hallOfFame = NULL;
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::Game::Conf::Conf (int nP)
	: QGAMES::AdvancedArcadeGame::Conf (nP),
	  _score (), _weaponsLevel (), _level (), _playerName (), _lastPositions (), _mazeScene (), 
	  _thingsCarried (), _thingsInMaze (), _mealInMaze ()
{ 
	assert (_numberPlayers > 0 && _numberPlayers < __GAMETEST_MAXNUMBERPLAYERS__);

	// Add two keys to the default one managed in the parent class...
	_keys.resize (9);
	_keys [__GAMETEST_KEYITERATE__] = QGAMES::KeyCode::QGAMES_Q; // Iterate
	_keys [__GAMETEST_KEYCATCHLEAVE__] = QGAMES::KeyCode::QGAMES_W; // Catch / Leave

	adjustToPlayers (nP);
}

// ---
void TestPlatformGame::Game::Conf::removeLastThingCarried (int nP)
{
	assert (nP > 0 && nP <= (int) _thingsCarried.size ());

	for (int i = (__GAMETEST_MAXNUMBERTHINGSCARRIED__ - 2); i >= 0 ; i--)
		_thingsCarried [nP - 1][i + 1] = _thingsCarried [nP - 1][i];
	_thingsCarried [nP - 1][0] = -1; // Empty...
}

// ---
void TestPlatformGame::Game::Conf::iterateThingsCarried (int nP)
{ 
	assert (nP > 0 && nP <= (int) _thingsCarried.size ());
	
	int lT = _thingsCarried [nP - 1][__GAMETEST_MAXNUMBERTHINGSCARRIED__ - 1];
	for (int i = (__GAMETEST_MAXNUMBERTHINGSCARRIED__ - 2); i >= 0; i--)
		_thingsCarried [nP - 1][i + 1] = _thingsCarried [nP - 1][i];
	_thingsCarried [nP - 1][0] = lT;
}

// ---
int TestPlatformGame::Game::Conf::carryThing (int nP, int tC)
{ 
	assert (nP > 0 && nP <= (int) _thingsCarried.size ());
	
	int lT = _thingsCarried [nP - 1][__GAMETEST_MAXNUMBERTHINGSCARRIED__ - 1];
	for (int i = (__GAMETEST_MAXNUMBERTHINGSCARRIED__ - 2); i >= 0; i--)
		_thingsCarried [nP - 1][i + 1] = _thingsCarried [nP - 1][i];
	_thingsCarried [nP - 1][0] = tC;

	return (lT);
}

// ---
const std::vector <TestPlatformGame::VillanerLocation> TestPlatformGame::Game::Conf::villanersInMaze (int nP, int nR) const
{
	std::vector <TestPlatformGame::VillanerLocation> result;

	const std::vector <TestPlatformGame::VillanerLocation>& vlls = villanersInMaze (nP);
	for (std::vector <TestPlatformGame::VillanerLocation>::const_iterator i = vlls.begin (); i != vlls.end (); i++)
		if ((*i)._roomNumber == nR) result.push_back ((*i));

	return (result);
}

// ---
void TestPlatformGame::Game::Conf::leaveThing (int nP, const TestPlatformGame::ThingToCatchLocation& tL)
{
	assert (nP > 0 && nP <= (int) _thingsInMaze.size ());

	// Look for the first place to leave the thing...
	bool left = false;
	std::vector <TestPlatformGame::ThingToCatchLocation>& thgs = _thingsInMaze [nP - 1][tL._roomNumber];
	for (int i = 0; i < (int) thgs.size () && !left; i++)
		if (thgs [i]._thingType == -1) { thgs [i] = tL; left = true; }

	assert (left); // It should be left...
}

// ---
void TestPlatformGame::Game::Conf::removeThing (int nP, const TestPlatformGame::ThingToCatchLocation& tL)
{
	assert (nP > 0 && nP <= (int) _thingsInMaze.size ());

	bool found = false;
	std::vector <TestPlatformGame::ThingToCatchLocation>& thgs = _thingsInMaze [nP - 1][tL._roomNumber];
	for (int i = 0; i < (int) thgs.size () && !found; i++)
		if (thgs [i] == tL) 
			{ thgs [i] = TestPlatformGame::ThingToCatchLocation (); found = true; }
}

// ---
void TestPlatformGame::Game::Conf::updateThingStatus (int nP, 
		const ThingToCatchLocation& oTL, const TestPlatformGame::ThingToCatchLocation& tL)
{
	assert (nP > 0 && nP <= (int) _thingsInMaze.size ());

	bool found = false;
	std::vector <TestPlatformGame::ThingToCatchLocation>& thgs = _thingsInMaze [nP - 1][oTL._roomNumber];
	for (int i = 0; i < (int) thgs.size () && !found; i++)
		if (thgs [i] == oTL) // It doesn't take into account the status (take care)
			{ thgs [i] = tL; found = true; }
}

// ---
void TestPlatformGame::Game::Conf::removeMeal (int nP, const TestPlatformGame::MealLocation& mL)
{
	assert (nP > 0 && nP <= (int) _mealInMaze.size ());

	bool found = false;
	std::vector <TestPlatformGame::MealLocation>& mls = _mealInMaze [nP - 1][mL._roomNumber];
	for (int i = 0; i < (int) mls.size () && !found; i++)
		if (mls [i] == mL) 
			{ mls [i] = TestPlatformGame::MealLocation (); found = true; }
}

// ---
void TestPlatformGame::Game::Conf::actualizeHallOfFame ()
{
	int iS = (int) _hallOfFame.size ();
	for (int i = 0; i < _numberPlayers; i++)
	{
		QGAMES::OpenValues vals; 
		vals.insert (QGAMES::OpenValues::value_type (__GAMETEST_SCOREPLAYEROPENVALUE__, QGAMES::OpenValue (_score [i])));
		vals.insert (QGAMES::OpenValues::value_type (__GAMETEST_NAMEPLAYEROPENVALUE__, QGAMES::OpenValue (_playerName [i])));
		_hallOfFame.insert (std::map <std::string, QGAMES::SetOfOpenValues>::value_type 
			(std::to_string (i + iS), QGAMES::SetOfOpenValues (vals)));
	}
}

// ---
void TestPlatformGame::Game::Conf::adjustToPlayers (int nP)
{
	QGAMES::AdvancedArcadeGame::Conf::adjustToPlayers (nP);

	_score			= std::vector <int> (_numberPlayers, 0);
	_weaponsLevel	= std::vector <int> (_numberPlayers, 0);
	_level			= std::vector <int> (_numberPlayers, 1); // The first level is 1
	_playerName		= std::vector <std::string> (_numberPlayers, std::string (__NULL_STRING__));
	_lastPositions  = std::vector <QGAMES::Position> (_numberPlayers, __GAMETEST_FIRSTPOSITIONINMAZE__); // It will be modified later...
	_mazeScene		= std::vector <int> (_numberPlayers, __GAMETEST_FIRSTSCENEINTHEMAZE__);
	_thingsCarried	= std::vector <std::vector <int>> 
		(_numberPlayers, std::vector <int> (__GAMETEST_MAXNUMBERTHINGSCARRIED__, -1 /** Nothing by default. */));

	// ...and the villaners distributed across the maze...
	_villanersInMaze = std::vector <std::vector <TestPlatformGame::VillanerLocation>>
		(_numberPlayers, std::vector <TestPlatformGame::VillanerLocation> (__GAMETEST_NUMBERVILLANERS__, 
			TestPlatformGame::VillanerLocation ()));
	for (int i = 0; i < _numberPlayers; i++)
		distributeVillanersInTheMaze (i);

	// ...and now the things are distributed across the maze...
	_thingsInMaze	= std::vector <std::vector <std::vector <TestPlatformGame::ThingToCatchLocation>>>
		(_numberPlayers, std::vector <std::vector <TestPlatformGame::ThingToCatchLocation>> 
			(__GAMETEST_NUMBEROFSCENESINTHEMAZE__, std::vector <TestPlatformGame::ThingToCatchLocation> 
				(__GAMETEST_MAXNUMBERTHINGSINTHEMAZEPERROOM__, TestPlatformGame::ThingToCatchLocation ())));
	for (int i = 0; i < _numberPlayers; i++)
		distributeElementsInTheMaze (i);

	// ...and now the meal is distributed across the maze...
	_mealInMaze	= std::vector <std::vector <std::vector <TestPlatformGame::MealLocation>>>
		(_numberPlayers, std::vector <std::vector <TestPlatformGame::MealLocation>> 
			(__GAMETEST_NUMBEROFSCENESINTHEMAZE__, std::vector <TestPlatformGame::MealLocation> 
				(__GAMETEST_MAXNUMBEROFMEALSINMAZEPERROOM__, TestPlatformGame::MealLocation ())));
	for (int i = 0; i < _numberPlayers; i++)
		distributeMaleInTheMaze (i);

	// Adjust the previous loaded elements to this game...
	_currentWorld = std::vector <int> (_numberPlayers, __GAMETEST_WORLDID__);
	_currentScene = std::vector <int> (_numberPlayers, __GAMETEST_SCENEID__);
	_numberLives = std::vector <int> (_numberPlayers, __GAMETEST_MAXNUMBERLIVES__);
}

// ---
void TestPlatformGame::Game::Conf::cfgToStream (std::ostringstream& oS)
{
	QGAMES::AdvancedArcadeGame::Conf::cfgToStream (oS);

	for (int i = 0; i < _numberPlayers; i++)
		oS << _score [i] << std::endl;

	for (int i = 0; i < _numberPlayers; i++)
		oS << _weaponsLevel [i] << std::endl;

	for (int i = 0; i < _numberPlayers; i++)
		oS << _level [i] << std::endl;

	for (int i = 0; i < _numberPlayers; i++)
		oS << _playerName [i] << std::endl;

	for (int i = 0; i < _numberPlayers; i++)
		oS << _lastPositions [i] << std::endl;

	for (int i = 0; i < _numberPlayers; i++)
		oS << _mazeScene [i] << std::endl;

	for (int i = 0; i < _numberPlayers; i++)
	{
		oS << _thingsCarried [i].size () << std::endl;
		for (int j = 0; j < (int) _thingsCarried [i].size (); j++)
			oS << _thingsCarried [i][j] << std::endl;
	}

	for (int i = 0; i < _numberPlayers; i++)
	{
		oS << _villanersInMaze [i].size () << std::endl;
		for (int j = 0; j < (int) _villanersInMaze [i].size (); j++)
			oS << _villanersInMaze [i][j] << std::endl;
	}

	for (int i = 0; i < _numberPlayers; i++)
	{
		oS << _thingsInMaze [i].size () << std::endl;
		for (int j = 0; j < (int) _thingsInMaze [i].size (); j++)
		{
			oS << _thingsInMaze [i][j].size () << std::endl;
			for (int k = 0; k < (int) _thingsInMaze [i][j].size (); k++)
				oS << _thingsInMaze [i][j][k] << std::endl;
		}
	}

	for (int i = 0; i < _numberPlayers; i++)
	{
		oS << _mealInMaze [i].size () << std::endl;
		for (int j = 0; j < (int) _mealInMaze [i].size (); j++)
		{
			oS << _mealInMaze [i][j].size () << std::endl;
			for (int k = 0; k < (int) _mealInMaze [i][j].size (); k++)
				oS << _mealInMaze [i][j][k] << std::endl;
		}
	}
}

// ---
void TestPlatformGame::Game::Conf::cfgFromStream (std::istringstream& iS)
{
	QGAMES::AdvancedArcadeGame::Conf::cfgFromStream (iS);

	_score.resize (_numberPlayers);
	for (int i = 0; i < _numberPlayers; i++)
		iS >> _score [i];

	_weaponsLevel.resize (_numberPlayers);
	for (int i = 0; i < _numberPlayers; i++)
		iS >> _weaponsLevel [i];

	_level.resize (_numberPlayers);
	for (int i = 0; i < _numberPlayers; i++)
		iS >> _level [i];

	_playerName.resize (_numberPlayers);
	for (int i = 0; i < _numberPlayers; i++)
		iS >> _playerName [i];

	_lastPositions.resize (_numberPlayers);
	for (int i = 0; i < _numberPlayers; i++)
		iS >> _lastPositions [i];

	_mazeScene.resize (_numberPlayers);
	for (int i = 0; i < _numberPlayers; i++)
		iS >> _mazeScene [i];

	_thingsCarried.resize (_numberPlayers);
	for (int i = 0; i < _numberPlayers; i++)
	{
		int nT = 0; iS >> nT; // First the size (it should be the same for any player)
		_thingsCarried [i].resize (nT);
		for (int j = 0; j < nT; j++)
			iS >> _thingsCarried [i][j];
	}

	_villanersInMaze.resize (_numberPlayers);
	for (int i = 0; i < _numberPlayers; i++)
	{
		int nV = 0; iS >> nV; // First the size (it should be the same for every player anycase)
		_villanersInMaze [i].resize (nV);
		for (int j = 0; j < nV; j++)
			iS >> _villanersInMaze [i][j];
	}

	_thingsInMaze.resize (_numberPlayers);
	for (int i = 0; i < _numberPlayers; i++)
	{
		int nR = 0; iS >> nR; // First the number of rooms
		_thingsInMaze [i].resize (nR);
		for (int j = 0; j < nR; j++)
		{
			int nT = 0; iS >> nT; // Then the number of things in the room...
			_thingsInMaze [i][j].resize (nT);
			for (int k = 0; k <nT; k++)
				iS >> _thingsInMaze [i][j][k];
		}
	}

	_mealInMaze.resize (_numberPlayers);
	for (int i = 0; i < _numberPlayers; i++)
	{
		int nR = 0; iS >> nR; // First the number of rooms
		_mealInMaze [i].resize (nR);
		for (int j = 0; j < nR; j++)
		{
			int nT = 0; iS >> nT; // Then the number of things to eat in the room...
			_mealInMaze [i][j].resize (nT);
			for (int k = 0; k <nT; k++)
				iS >> _mealInMaze [i][j][k];
		}
	}
}

// ---
void TestPlatformGame::Game::Conf::distributeVillanersInTheMaze (int nP)
{
	QGAMES::Position posLeft (__BD (01 * 32), __BD (14 * 32), __BD 0);
	QGAMES::Position posMiddle (__BD (10 * 32), __BD (14 * 32), __BD 0);
	
	std::vector <int> rUsed (__GAMETEST_NUMBERVILLANERS__, -1);
	// The first and the central room have always a villaner...
	rUsed [0] = __GAMETEST_EXITSCENEINTHEMAZE__; rUsed [1] = __GAMETEST_CENTERSCENEINTHEMAZE__; 

	std::vector <TestPlatformGame::VillanerLocation> vll = _villanersInMaze [nP];
	vll [0] = TestPlatformGame::VillanerLocation (0, rUsed [0], posLeft, QGAMES::Vector (__BD 1, __BD 0, __BD 0));
	vll [1] = TestPlatformGame::VillanerLocation (0, rUsed [1], posMiddle, QGAMES::Vector (__BD 1, __BD 0, __BD 0));
	for (int i = 0; i < (__GAMETEST_NUMBERVILLANERS__ - 2); i++) // All of them except two are placed aleatory...
	{
		int nM = -1;
		do
		{
			nM = rand () % __GAMETEST_NUMBEROFSCENESINTHEMAZE__; // In which room?
		} while (std::find (rUsed.begin (), rUsed.end (), nM) != rUsed.end ());

		rUsed [i + 2] = nM;
		vll [i + 2] = TestPlatformGame::VillanerLocation (0, nM, posMiddle, QGAMES::Vector (__BD 1, __BD 0, __BD 0));
	}

	_villanersInMaze [nP] = vll;
}

// ---
void TestPlatformGame::Game::Conf::distributeElementsInTheMaze (int nP)
{
	static std::vector <QGAMES::Position> _POSIBBLEPOSITIONS 
		(__GAMETEST_NUMBERPOSSIBLEPOSITIONSINAROOM__, QGAMES::Position::_cero);
	_POSIBBLEPOSITIONS [0] = QGAMES::Position (__BD (11 * 32), __BD (06 * 32), __BD 0);
	_POSIBBLEPOSITIONS [1] = QGAMES::Position (__BD (06 * 32), __BD (13 * 32), __BD 0);
	_POSIBBLEPOSITIONS [2] = QGAMES::Position (__BD (11 * 32), __BD (12 * 32), __BD 0);
	_POSIBBLEPOSITIONS [3] = QGAMES::Position (__BD (18 * 32), __BD (12 * 32), __BD 0);
	_POSIBBLEPOSITIONS [4] = QGAMES::Position (__BD (11 * 32), __BD (18 * 32), __BD 0);

	std::vector <std::vector <TestPlatformGame::ThingToCatchLocation>> tLR = _thingsInMaze [nP];
	for (int j = 0; j < __GAMETEST_MAXNUMBEROFTHINGSINTHEMAZE__; j++)
	{
		int nM = rand () % __GAMETEST_NUMBEROFSCENESINTHEMAZE__; // In which room?
		int tR = TestPlatformGame::World::typeMazeScene (nM); // Which type is it?
		std::vector <TestPlatformGame::ThingToCatchLocation> tL = tLR [nM];
		int pF = -1;
		for (int k = 0; k < (int) tL.size () && pF == -1; k++)
			if (tL [k]._thingType == -1) pF = k;
		if (pF != -1)
		{
			int nII = 0;
			int tM = rand () % __GAMETEST_MAXTYPESOFTHINGSTOBECARRIED__; // Which type thing to be carried ?
			int pM = rand () % __GAMETEST_NUMBERPOSSIBLEPOSITIONSINAROOM__; // In which position?
			while (_POSITIONSPERSCENETYPE [tR][pM] == -1 && nII < 2) 
				{ pM++; if (pM >= __GAMETEST_NUMBERPOSSIBLEPOSITIONSINAROOM__) { pM = 0; nII++; } }
			if (nII < 2) 
				tL [pF] = TestPlatformGame::ThingToCatchLocation 
					(tM, nM, _POSIBBLEPOSITIONS [_POSITIONSPERSCENETYPE [tR][pM]]);
		}

		tLR [nM] = tL;
	}

	_thingsInMaze [nP] = tLR;
}

// ---
void TestPlatformGame::Game::Conf::distributeMaleInTheMaze (int nP)
{
	static std::vector <QGAMES::Position> _POSIBBLEPOSITIONS 
		(__GAMETEST_NUMBERPOSSIBLEPOSITIONSINAROOM__, QGAMES::Position::_cero);
	_POSIBBLEPOSITIONS [0] = QGAMES::Position (__BD (12 * 32), __BD (04 * 32), __BD 0);
	_POSIBBLEPOSITIONS [1] = QGAMES::Position (__BD (04 * 32), __BD (13 * 32), __BD 0);
	_POSIBBLEPOSITIONS [2] = QGAMES::Position (__BD (12 * 32), __BD (13 * 32), __BD 0);
	_POSIBBLEPOSITIONS [3] = QGAMES::Position (__BD (20 * 32), __BD (13 * 32), __BD 0);
	_POSIBBLEPOSITIONS [4] = QGAMES::Position (__BD (12 * 32), __BD (20 * 32), __BD 0);

	std::vector <std::vector <TestPlatformGame::MealLocation>> mLR = _mealInMaze [nP];
	for (int j = 0; j < __GAMETEST_MAXNUMBEROFMEALINTHEMAZE__; j++)
	{
		int nM = rand () % __GAMETEST_NUMBEROFSCENESINTHEMAZE__;
		int tR = TestPlatformGame::World::typeMazeScene (nM); 
		std::vector <TestPlatformGame::MealLocation> mL = mLR [nM];
		int pF = -1;
		for (int k = 0; k < (int) mL.size () && pF == -1; k++)
			if (mL [k]._mealType == -1) pF = k;
		if (pF != -1)
		{
			int nII = 0;
			int tM = rand () % __GAMETEST_MAXTYPESOFMEAL__;
			int pM = rand () % __GAMETEST_NUMBERPOSSIBLEPOSITIONSINAROOM__;
			while (_POSITIONSPERSCENETYPE [tR][pM] == -1 && nII < 2) 
				{ pM++; if (pM >= __GAMETEST_NUMBERPOSSIBLEPOSITIONSINAROOM__) { pM = 0; nII++; } }
			if (nII < 2) // Hole is possible... 
				mL [pF] = TestPlatformGame::MealLocation (tM, nM, _POSIBBLEPOSITIONS [_POSITIONSPERSCENETYPE [tR][pM]]);
		}

		mLR [nM] = mL;
	}

	_mealInMaze [nP] = mLR;
}

// ---
void TestPlatformGame::Game::addScoreObjects ()
{
	addScoreObject (new TestPlatformGame::NumberOfLivesScoreObject ());
	addScoreObject (new TestPlatformGame::NumberOfPointsScoreObject ());
	addScoreObject (new TestPlatformGame::ThingsBeingCarriedScoreObject ());
	addScoreObject (new TestPlatformGame::RemainingTimeScoreObject ());
	addScoreObject (new TestPlatformGame::KnightEnergyLevelScoreObject ());
}

// ---
void TestPlatformGame::Game::removeScoreObjects ()
{
	const QGAMES::Entities scObjs = scoreObjects ();
	for (QGAMES::Entities::const_iterator i = scObjs.begin (); i != scObjs.end (); i++)
		delete ((*i).second);

	QGAMES::AdvancedArcadeGame::removeScoreObjects ();
}

// ---
void TestPlatformGame::Game::initialize ()
{
	if (isInitialized ())
		return;

	QGAMES::AdvancedArcadeGame::initialize ();

	mainScreen () -> windowAtCenter ();
	mainScreen () -> setIcon (form (__GAMETEST_MEDIEVALGAMEICON__));

	// Change the form used in the default loading state...
	((QGAMES::LoadingGameState*) state (__QGAMES_GAMESTATELOADINGNAME__)) ->
		setProperties (QGAMES::ShowingFormBeingFulfilledGameState::Properties
			(__GAMETEST_MEDIEVALCASTLE__,
			 255, /** Fade */ 
			 QGAMES::Position (__BD 0.5, __BD 0.5, __BD 0), /** Releative position in the screen */
			 QGAMES::LoadingGameState::Properties::PosReference::_CENTER, /** Reference point of the form */
			 false, /** Direct fullfilment */
			 true /** Horizontal fullfilment */
			));

	// Adds all states used in the game...
	addState (new TestPlatformGame::IntroLogoGameState, std::string (__GAMETEST_INTROGAMESTATENAME__));
	addState (new TestPlatformGame::PresentationGameState, std::string (__GAMETEST_PRESENTATIONGAMESTATENAME__));
	addState (new TestPlatformGame::SelectionGameState, std::string (__GAMETEST_SELECTIONGAMESTATENAME__));
	addState (new TestPlatformGame::HallOfFameGameState, std::string (__GAMETEST_HALLOFFAMEGAMESTATENAME__));
	addState (new TestPlatformGame::GameAboutToStart, std::string (__GAMETEST_ABOUTTOSTARTGAMESTATENAME__));
	addState (new TestPlatformGame::Playing, std::string (__GAMETEST_PLAYINGGAMESTATENAME__));
	addState (new TestPlatformGame::LevelCompleted, std::string (__GAMETEST_LEVELHASCOMPLETEDGAMESTATENAME__));
	addState (new TestPlatformGame::PlayerHasDied, std::string (__GAMETEST_PLAYERHASDIEDGAMESTATENAME__));
	addState (new TestPlatformGame::GameHasEnded, std::string (__GAMETEST_GAMEHASENDEDGAMESTATENAME__));
	addState (new TestPlatformGame::PlayerHasWon, std::string (__GAMETEST_PLAYERHASWONSTATENAME__));
	addState (new TestPlatformGame::ControllingGameStates (state (std::string (__QGAMES_GAMESTATESHOWENTITYNAME__))), 
		std::string (__GAMETEST_CONTROLGAMESTATENAME__));

	// Adds other internal states used in presentation state...
	std::string presentationTexts [6] =
		{ "Community Networks Presents", 
		  "a new Adventure by Ignacio Cea",
		  "Medieval Adventure",
		  "From our internal factories",
		  "and using many other internet resources",
		  "Press any key to continue" };
	int presentationForms [6] = { 104, 105, 106, 107, 108, 109 };
	for (int i = 0; i < 6; i++)
		addState (
			new TestPlatformGame::ShowingPartOfPresentationGameState (
				__GAMETEST_SHOWINGPRESENTATIONSTATEBASE__,
				presentationForms [i], 
				presentationTexts [i], (i == 2) ? 2 : ((i == 5) ? 1 : 0)), // The type of font used is different depending on the text itself...
			std::to_string (__GAMETEST_SHOWINGPRESENTATIONSTATEBASE__ + i));

	addWorld (__GAMETEST_WORLDID__);

	addArtist (__GAMETEST_MAINCHARACTERID__); // Main character...
	for (int i = 0; i < __GAMETEST_NUMBERVILLANERS__; i++)
		addArtist (__GAMETEST_VILLANERCHARACTERBASEID__ + i); // Villaners...
	for (int i = 0; i < __GAMETEST_MAXNUMBERTHINGSINTHEMAZEPERROOM__; i++)
		addArtist (__GAMETEST_THINGTOCATCHBASEID__ + i); // Things to catch...
	for (int i = 0; i < __GAMETEST_MAXNUMBEROFMEALSINMAZEPERROOM__; i++)
		addArtist (__GAMETEST_MEALBASEID__ + i); // Things to eat...

	// The base state is one to control all...
	setState (__GAMETEST_CONTROLGAMESTATENAME__);
}

// ---
void TestPlatformGame::Game::additionalTasks ()
{
	if (activeState () -> nestedState () -> type () == __GAMETEST_PLAYINGGAMESTATE__)
		QGAMES::AdvancedArcadeGame::additionalTasks ();
}
// --------------------------------------------------------------------------------
