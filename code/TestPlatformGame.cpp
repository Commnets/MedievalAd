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
	if (!p && (nB == 0 || nB == 10)) // In some joysticks (nintendo like) this should be the main button...
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
	_forms.insert (QGAMES::Forms::value_type (__QGAMES_SCARDGOTHIC32BLUELETTERS__, 
		QGAMES::Game::game () -> form (__QGAMES_SCARDGOTHIC32BLUELETTERS__)));
	setCurrentForm (__QGAMES_SCARDGOTHIC32BLUELETTERS__); 
	setNumberLength (5);
	setSpace (0);
	setPosition (QGAMES::Position (__BD 20, __BD 110, __BD 0));
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
	std::vector <TestPlatformGame::ThingToCatchLocation> tC = ((TestPlatformGame::Game*) game ()) -> thingsCarried ();

	for (int i = 0; i < (int) tC.size (); i++)
		if (tC [i] != TestPlatformGame::ThingToCatchLocation ()) // If there is something being carried at that position...
			currentForm () -> frame (tC [i]._thingType) -> 
				drawOn (scr, p + QGAMES::Position (
					__BD (__GAMETEST_SCREENWIDTH__ - 10 - (__GAMETEST_MAXNUMBERTHINGSCARRIED__ * 64 ) + (i * 64)), 
					__BD 20, __BD 0));

	// Round the first one, just if it exists...
	if (tC [__GAMETEST_MAXNUMBERTHINGSCARRIED__ - 1]._thingType != -1)
		scr -> drawCircle (p + QGAMES::Position (__BD (__GAMETEST_SCREENWIDTH__ - 10 - 36), __BD 10 + 32, __BD 10), 
			QGAMES::Vector::_zNormal, 32, 32, __QGAMES_REDCOLOR__, false);
}

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::RemainingTimeScoreObject::RemainingTimeScoreObject ()
	: QGAMES::ScoreObjectNumber (__GAMETEST_TIMETOLEFTSCOREOBJECTID__, 0, QGAMES::Forms (), 16)
{ 
	_forms.insert (QGAMES::Forms::value_type (__QGAMES_SCARDGOTHIC32YELLOWLETTERS__, 
		QGAMES::Game::game () -> form (__QGAMES_SCARDGOTHIC32YELLOWLETTERS__)));
	setCurrentForm (__QGAMES_SCARDGOTHIC32YELLOWLETTERS__); 
	setNumberLength (5);
	setSpace (0);
	setPosition (QGAMES::Position (__BD 20, __BD (__GAMETEST_SCREENHEIGHT__ - 70), __BD 0));
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
QGAMES::Entity* TestPlatformGame::Knight::clone () const
{ 
	QGAMES::Entity* result = new TestPlatformGame::Knight (_id, _forms, _data);

	result -> setMovements (movements ());
	result -> setAnimations (cloneAnimations ());
	result -> setStates (cloneStates ());

	return (result);
}

// ---
void TestPlatformGame::Knight::toIterateThings ()
{
	((TestPlatformGame::Game*) game ()) -> iterateThingsCarried (); 
}

// ---
void TestPlatformGame::Knight::toCatchLeave ()
{
	// If there will be something to leave, but thre won't be enought space in the room to do so, make no sense to progress
	// Additionally a sound is emitted...
	if ((((TestPlatformGame::Game*) game ()) -> lastThingCarried ()._thingId != -1) && !
		((TestPlatformGame::Game*) game ()) -> spaceToLeave (((TestPlatformGame::Game*) game ()) -> mazeScene ()))
	{
		game () -> sound (__GAMETEST_NOACTIONPOSIBLESOUNDID__) -> play (__GAMETEST_KNIGHTSOUNDCHANNEL__);
		return;
	}

	// Something to catch?
	TestPlatformGame::ThingToCatch* thg = NULL;
	const QGAMES::Entities& eties = map () -> scene () -> entities ();
	for (QGAMES::Entities::const_iterator i = eties.begin (); i != eties.end () && thg == NULL; i++)
		if ((*i).second != this && // Not me,
			(*i).second -> isVisible () && // Visible?,
			dynamic_cast <TestPlatformGame::ThingToCatch*> ((*i).second) && // Something to catch?,
			isNearOf ((*i).second, 55) && // Being near of the knight?
			((TestPlatformGame::ThingToCatch*) (*i).second) -> canBeCaught ())  // and can it be caught?
				thg = ((TestPlatformGame::ThingToCatch*) (*i).second); 

	TestPlatformGame::ThingToCatchLocation toLeave;
	// What to leave (calculated depending on whether there is something to catch)
	// If there is something to catch...
	if (thg)
	{
		toLeave = ((TestPlatformGame::Game*) game ()) -> carryThing (thg -> description ());
		thg -> setVisible (false); // Not visible now...
		((TestPlatformGame::Game*) game ()) -> removeThing (thg -> description ()); // No longer in the maze!!...
		game () -> sound (__QGAMES_EATWAVSOUND__) -> play (__GAMETEST_KNIGHTSOUNDCHANNEL__);
	}
	else
		toLeave = ((TestPlatformGame::Game*) game ()) -> carryThing (TestPlatformGame::ThingToCatchLocation ());

	// What to leave, is left near the knight...
	if (toLeave._thingId != -1)
	{
		QGAMES::Position pC = baseZone ().toBase ().center () -
			game () -> form (__GAMETEST_THINGSCANBECAUGHTFORM__) -> baseZone (0).toBase ().center ();
		toLeave._position = pC + (30 * _lastOrientation); // in the direction of the last movement...
		toLeave._roomNumber = ((TestPlatformGame::Game*) game ()) -> mazeScene (); // Where the knight is now...
		((TestPlatformGame::Game*) game ()) -> leaveThing (toLeave);

		// If the thing left belongs to the type used to unblock the villan in the exit place...
		// A special notification is thrown!
		if (toLeave._thingType >= __GAMETEST_TYPEOFTHINGFORVILLANTOMOVE1__ &&
			toLeave._thingType <= __GAMETEST_TYPEOFTHINGFORVILLANTOMOVE2__)
			notify (QGAMES::Event (__GAMETEST_THINGTOMOVEVILLANERLEFT__, NULL)); 

		// Any case, the scene has to be rebuilt...
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
		// Stop walking sound if any...
		if (walking ())
			game () -> sound (__GAMETEST_FOOTSTEPSSOUNDID__) -> stop ();

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

		game () -> sound (__GAMETEST_FOOTSTEPSSOUNDID__) -> play (__GAMETEST_KNIGHTSOUNDCHANNEL__);
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
		// Stop walking sound if any...
		if (walking ())
			game () -> sound (__GAMETEST_FOOTSTEPSSOUNDID__) -> stop ();

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
		// Stop walking sound if any...
		if (walking ())
			game () -> sound (__GAMETEST_FOOTSTEPSSOUNDID__) -> stop ();

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
	// Take into account that collisions are not detected with meal!
	bool result = true;
	const QGAMES::Entities& eties = map () -> scene () -> entities ();
	for (QGAMES::Entities::const_iterator i = eties.begin (); i != eties.end () && result; i++)
		if ((*i).second != this && // Not me...
			(*i).second -> isVisible () && dynamic_cast <TestPlatformGame::ThingToCatch*> ((*i).second) && // Something visible to catch
			hasCollisionWith ((*i).second)) // ...and with collision
			result = false; // No movement possible...

	// If the movement is still possible, then
	// consider that the entity is or not over a base...
	if (result)
		result = isOnABase ();

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
		// The more difficult the game is the quicker the energy is reduced!
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
		__BD ((__AGM game ()) -> activeWorld () -> activeScene () -> activeMap () -> width ()))
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
	else
	// When collisions whith a villan the energy is lost quicker...if he is not fighting...
	if (dynamic_cast <TestPlatformGame::Villaner*> (e) && !attacking ())
		setEnergyLevel (energyLevel () - 10); 
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
TestPlatformGame::VillanerLocation::VillanerLocation ()
	: _villanerId (-1), // No id by default...
	  _type (0), // The basic one...
	  _status (0), // Staying...
	  _roomNumber (-1), // Not defined
	  _position (QGAMES::Position::_cero), // None
	  _orientation (QGAMES::Vector (__BD 1, __BD 0, __BD 0)), // Looking to the right...
	  _speed (1), // By default...
	  _initialMazeRoom (),
	  _finalMazeRoom (),
	  _currentPathPosition (0)
{
	// Nothing else to do so far...
}

// ---
TestPlatformGame::VillanerLocation::VillanerLocation 
		(int vId, int tp, int s, int r, const QGAMES::Position& p, const QGAMES::Vector& v, int spd,
		 const QGAMES::MazeModel::PositionInMaze& iP, const QGAMES::MazeModel::PositionInMaze& fP,
		 int cPP)
	: _villanerId (vId),
	  _type (tp),
	  _status (s), 
	  _roomNumber (r),
	  _position (p),
	  _orientation (v),
	  _speed (spd),
	  _initialMazeRoom (iP),
	  _finalMazeRoom (fP),
	  _currentPathPosition (cPP)
{ 
	assert (_villanerId >= 0 && _villanerId < __GAMETEST_NUMBERVILLANERS__ && 
			_type >= 0 && _type < 3 &&
			_status < 2 &&
			_speed > 0 && _speed <= 3 &&
			_roomNumber >= 0 && _roomNumber < __GAMETEST_NUMBEROFSCENESINTHEMAZE__ &&
			_currentPathPosition >= 0); 
}

// ---
std::ostream& TestPlatformGame::operator << (std::ostream& oS, const TestPlatformGame::VillanerLocation& vL)
{ 
	oS << vL._villanerId << std::endl 
	   << vL._type << std::endl 
	   << vL._status << std::endl 
	   << vL._roomNumber << std::endl 
	   << vL._position << std::endl << vL._orientation << std::endl << vL._speed << std::endl
	   << vL._initialMazeRoom << std::endl << vL._finalMazeRoom << std::endl << vL._currentPathPosition; 
	return (oS); 
}

// ---
std::istream& TestPlatformGame::operator >> (std::istream& iS, TestPlatformGame::VillanerLocation& vL)
{ 
	iS >> vL._villanerId 
	   >> vL._type 
	   >> vL._status 
	   >> vL._roomNumber 
	   >> vL._position >> vL._orientation >> vL._speed
	   >> vL._initialMazeRoom >> vL._finalMazeRoom >> vL._currentPathPosition; 
	return (iS); 
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
QGAMES::Entity* TestPlatformGame::Villaner::clone () const
{ 
	QGAMES::Entity* result = new TestPlatformGame::Villaner (_id, _forms, _data);

	result -> setMovements (movements ());
	result -> setAnimations (cloneAnimations ());
	result -> setStates (cloneStates ());

	return (result);
}

// ---
bool TestPlatformGame::Villaner::isPossiblePosInMap (const QGAMES::Position& v)
{
	if (map () == NULL)	return (true); // If there is no map, the movement is possible...(no sense but...)

	return (v.posX () > __BD -100 && v.posX () < __BD (map () -> width () + 100) &&
			v.posY () > __BD -100 && v.posY () < __BD (map () -> height () + 100)); // To be out a little bit...
}

// ---
void TestPlatformGame::Villaner::setDescription (const TestPlatformGame::VillanerLocation& vL)
{
	_description = vL;
	switch (_description._status)
	{
		case 0: toStay (_description._orientation); break;
		case 1: toWalk (_description._orientation, _description._speed); break;
		case 2: toDie (_description._orientation); break;
		default:
			assert (0); // It shouldn't be here...
	};

	// Sets the position...
	setPosition (_description._position - 
		__GAMETEST_REFERENCEALTITUDOFBASE__ - QGAMES::Vector (__BD 0, __BD 0, __BD visualHeight ()));
	setFixBasePosition (); 
	setFixBasePosition 
		(QGAMES::Position (__MINBDATA__, __MINBDATA__, basePosition ().posZ ())); // anchor it to the floor...

	// ...and a movement if any...
	// the villan has to be alived...otherwise it stays...
	if (_description._initialMazeRoom != _description._finalMazeRoom && alive ()) 
		moveFollowingPath (((TestPlatformGame::World*) (__AGM game ()) -> activeWorld ()) -> 
			worldModel ().shortestWayToGo (_description._initialMazeRoom, _description._finalMazeRoom));
	else
		addControlStepsMonitor (NULL); // No monitor needed at all...

	// By default the entity is not visible...
	// Each time it is update, the visible attribute will be updated...
	setVisible (true);
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

	// When to change the path...
	if (_changePath)
	{
		_changePath = false;

		_description._currentPathPosition++;
		// When the last position is reached, the villan moves back...
		if (_description._currentPathPosition >= ((int) _path.size () - 1)) 
		{ 
			QGAMES::MazeModel::PositionInMaze tP = _description._initialMazeRoom; 
			_description._initialMazeRoom = _description._finalMazeRoom; _description._finalMazeRoom = tP;
			_description._currentPathPosition = 0; 
			moveFollowingPath (((TestPlatformGame::World*) (__AGM game ()) -> activeWorld ()) -> 
				worldModel ().shortestWayToGo (_description._initialMazeRoom, _description._finalMazeRoom));
		}
		// Otherwise the next movement is selected...
		else 
			setControlMonitorToRoom (_path [_description._currentPathPosition + 1]);
	}

	setVisible (((TestPlatformGame::Game*) game ()) -> mazeScene () == _description._roomNumber);

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
		__BD ((__AGM game ()) -> activeWorld () -> activeScene () -> activeMap () -> width ()))
		notify (QGAMES::Event (__GAMETEST_VILLANERREACHEDRIGHTLIMIT__, this));
	else if (basePosition ().posY () >=
		__BD ((__AGM game ()) -> activeWorld () -> activeScene () -> activeMap () -> height ()))
		notify (QGAMES::Event (__GAMETEST_VILLANERREACHEDDOWNLIMIT__, this));

	// Actualize the position and the orientation in the description object...
	// The z coordinate is not kept (is just the position of the base)
	_description._position = QGAMES::Position (position ().posX (), position ().posY (), __BD 0);
	_description._orientation = orientation ();
	// Both can be changed during the excution of this method (even after notifying an event)

	// The information has to be updated also in the configuration entities
	// just in case the game is saved...
	((TestPlatformGame::Game*) game ()) -> actualizeVillanersInfo (_description);
}

// ---
void TestPlatformGame::Villaner::drawOn (QGAMES::Screen* s, const QGAMES::Position& p)
{
	if (!isVisible ())
		return; // The villaner can be in another room scene so it won't be drawn!
				// The attribute is set in the updatePositions method...

	QGAMES::PlatformArtist::drawOn (s, p);
}

// ---
void TestPlatformGame::Villaner::whenCollisionWith (QGAMES::Entity* e)
{
	if (dynamic_cast <TestPlatformGame::Knight*> (e) &&
		((TestPlatformGame::Knight*) e) -> attacking () && !inmortal ())
		setEnergyLevel (energyLevel () - 10); // The knight can hit him...
	// If the villaner is in collision with a thingToCatch, it has to roundeded it...
	// Bu the position of the thing has to be in the direction of the movement...
	else
	if (dynamic_cast <TestPlatformGame::ThingToCatch*> (e))
	{
		QGAMES::Vector cV = e -> basePosition () - basePosition ();
		cV = QGAMES::Vector (cV.posX (), cV.posY (), __BD 0); // Plane...
		cV = QGAMES::Vector ((abs (cV.posX ()) >= abs (cV.posY ())) ? cV.posX () : __BD 0,
							 (abs (cV.posY ()) >= abs (cV.posX ())) ? cV.posY () : __BD 0,
							 __BD 0); // The relevant direction...
		if (_description._orientation.posX () > 0 && cV.posX () > 0 ||
			_description._orientation.posX () < 0 && cV.posX () < 0 ||
			_description._orientation.posY () > 0 && cV.posY () < 0 ||
			_description._orientation.posY () < 0 && cV.posY () < 0)
		{
			setControlMonitorToAvoidObstacle (e);
			stepsMonitor () -> run (); // To change defininetively...
		}
	}
}

// ---
void TestPlatformGame::Villaner::processEvent (const QGAMES::Event& e)
{
	// The end of the movement has been reached
	// So it is time to add a new one, of to finish the movement!
	if (e.code () == __QGAMES_CHRSTEPSMONITORLASTSTEP__)
		_changePath = true;
		// The monitor can't be changed in this moment, because the notification comes 
		// from the monitor itself... 
	else
		QGAMES::PlatformArtist::processEvent (e);
}

// ---
void TestPlatformGame::Villaner::moveToMazeScene (const QGAMES::Vector& dir)
{
	// Change the maze scene...
	int rR = (_description._roomNumber / __GAMETEST_HORIZONTALSCENESINMAZE__) + (int) dir.posY ();
	int rC = (_description._roomNumber % __GAMETEST_HORIZONTALSCENESINMAZE__) + (int) dir.posX ();
	assert (rR < __GAMETEST_VERTICALSCENESINMAZE__ && rC < __GAMETEST_HORIZONTALSCENESINMAZE__); // Just to be sure...
	_description._roomNumber = (rR * __GAMETEST_HORIZONTALSCENESINMAZE__) + rC;

	// ...and the position too...
	setPosition (position () - 
		QGAMES::Vector (dir.posX () * __BD (map () -> width ()),
						dir.posY () * __BD (map () -> height ()),
						__BD 0) + (__BD 10 * dir));

	// just in case...
	// Never by default is visible unless the updatePositions method determinates the opposite.
	setVisible (false);
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
			else setCurrentState (__GAMETEST_VILLANERIDLESTATERIGHTDOWN__);
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

		// Changes a little bit the form attending the type of villan...
		int cA = currentAspect (); // The aspect is lost when changing the form, so...
		setCurrentForm (currentForm () -> id () + _description._type); setCurrentAspect (cA);
		_description._status = 0; // To stay...

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
int TestPlatformGame::Villaner::standingStateForOrientation (const QGAMES::Vector& o) const
{
	int result = -1;

	if (o.posX () == 1)
	{
		if (o.posY () == 1) result = __GAMETEST_VILLANERIDLESTATEDOWN__;
		else if (o.posY () == -1) result = __GAMETEST_VILLANERIDLESTATERIGHT__;
		else result = __GAMETEST_VILLANERIDLESTATERIGHTDOWN__;
	}
	else if (o.posX () == -1)
	{
		if (o.posY () == 1) result = __GAMETEST_VILLANERIDLESTATELEFT__;
		else if (o.posY () == -1) result = __GAMETEST_VILLANERIDLESTATEUP__;
		else result = __GAMETEST_VILLANERIDLESTATELEFTUP__;
	}
	else
	{
		if (o.posY () == 1) result = __GAMETEST_VILLANERIDLESTATELEFTDOWN__;
		else if (o.posY () == -1) result = __GAMETEST_VILLANERIDLESTATERIGHTUP__;
	}

	assert (result != -1);
	return (result);
}

// ---
void TestPlatformGame::Villaner::toWalk (const QGAMES::Vector& o, int spd)
{
	QGAMES::Vector to = (o != QGAMES::Vector::_noPoint) ? o : _lastOrientation;

	// @see toStay comments for further understanding about how the method works
	if (alive () && 
		(!walking () || 
		 (walking () && currentStateOrientation () != to) ||
		 (walking () && currentStateOrientation () == to && currentSpeed () != spd)))
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

		assert (dynamic_cast <QGAMES::SimpleLinearMovement*> (currentMovement ())); // Just in case...
		((QGAMES::SimpleLinearMovement*) currentMovement ()) -> setSpeed (__BD spd);

		int cA = currentAspect (); 
		setCurrentForm (currentForm () -> id () + _description._type); setCurrentAspect (cA);
		_description._status = 1; // To walk...

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
int TestPlatformGame::Villaner::walkingStateForOrientation (const QGAMES::Vector& o) const
{
	int result = -1;

	if (o.posX () == 1)
	{
		if (o.posY () == 1) result = __GAMETEST_VILLANERWALKSTATEDOWN__;
		else if (o.posY () == -1) result = __GAMETEST_VILLANERWALKSTATERIGHT__;
		else result = __GAMETEST_VILLANERWALKSTATERIGHTDOWN__;
	}
	else if (o.posX () == -1)
	{
		if (o.posY () == 1) result = __GAMETEST_VILLANERWALKSTATELEFT__;
		else if (o.posY () == -1) result = __GAMETEST_VILLANERWALKSTATEUP__;
		else result = __GAMETEST_VILLANERWALKSTATELEFTUP__;
	}
	else
	{
		if (o.posY () == 1) result = __GAMETEST_VILLANERWALKSTATELEFTDOWN__;
		else if (o.posY () == -1) result = __GAMETEST_VILLANERWALKSTATERIGHTUP__;
	}

	assert (result != -1);
	return (result);
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

		// When the form is changed, the aspect is set back to 0...
		int cA = currentAspect ();
		setCurrentForm (currentForm () -> id () + _description._type); setCurrentAspect (cA);
		_description._status = 2;

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

// ---
int TestPlatformGame::Villaner::currentSpeed () const
{
	int result = 0;
	if (walking ())
	{
		assert (dynamic_cast <const QGAMES::SimpleLinearMovement*> (currentMovement ())); // It shouldn't but just in case...
		result = (int) ((QGAMES::SimpleLinearMovement*) currentMovement ()) -> speed ();
	}

	return (result);
}

// ---
void TestPlatformGame::Villaner::moveFollowingPath (const std::vector <QGAMES::MazeModel::PositionInMaze>& pth)
{
	_path = pth;
	if (_path.empty ())
		return;

	setControlMonitorToRoom (_description._currentPathPosition < ((int) _path.size () - 1) 
		? _path [_description._currentPathPosition + 1] : _path [_description._currentPathPosition]);
}

// ---
void TestPlatformGame::Villaner::setControlMonitorToRoom (const QGAMES::MazeModel::PositionInMaze& p)
{
	QGAMES::CharacterControlStep* stpNR = NULL;
	QGAMES::Vector dir = _description._orientation;

	// First of all, calculate the direction to follow to get the next room (ir the center of the current one)
	if (((p._positionY * __GAMETEST_HORIZONTALSCENESINMAZE__) + p._positionX) != _description._roomNumber)
	{
		// Across maze scenes...
		int cRR = _description._roomNumber / __GAMETEST_HORIZONTALSCENESINMAZE__;
		int cRC = _description._roomNumber % __GAMETEST_HORIZONTALSCENESINMAZE__;
		dir = QGAMES::Vector (__BD ((p._positionX > cRC) ? 1 : ((p._positionX < cRC) ? -1 : 0)),
							  __BD ((p._positionY > cRR) ? 1 : ((p._positionY < cRR) ? -1 : 0)),
							  __BD 0);
		// There will be step to get the next room...
		stpNR = new TestPlatformGame::Villaner::ControlStep (dir, _description._speed);
	}
	// If the "next room" to get is where the villan is already could be due to two reasons:
	// Either the path to follow is a one-step only path.
	// Or the game was restarted (after e.g the knight died) when the villan was in te next room.
	
	// Now, does the villan to be center before moving (or not) to the next room?
	// If the villan has not not move to any other room, this piece of code will be useful to get the center...
	QGAMES::CharacterControlSteps stps;

	// Now if the position is not close to the center
	// Add a set of steps to get close the villan to the center...
	QGAMES::Vector tCtr = QGAMES::Position (__BD (11 * 32), __BD (13 * 32),__BD 0) -
						  QGAMES::Position (position ().posX (), position ().posY (), __BD 0);
	if (tCtr.module () > 5)
	{
		if (std::abs (tCtr.posX ()) > __BD 3)
		{
			QGAMES::Vector drX (__BD ((tCtr.posX () > 0) ? 1 : -1), __BD 0, __BD 0);
			if (drX.posX () == dir.posX () || dir.posX () == __BD 0)
				stps.push_back (new TestPlatformGame::Villaner::ControlStep (drX, _description._speed));
		}

		if (std::abs (tCtr.posY ()) > __BD 3)
		{
			QGAMES::Vector drY (__BD 0, __BD ((tCtr.posY () > 0) ? 1 : -1), __BD 0);
			if (drY.posY () == dir.posY () || dir.posY () == __BD 0)
				stps.push_back (new TestPlatformGame::Villaner::ControlStep (drY, _description._speed));
		}
	}

	// Then the movement to the next room, if any...
	if (stpNR) stps.push_back (stpNR);
	// If there is no movements to do, it would mean the villan is exactly at the end
	// But an step to force the end would be event neccessary...
	if (stps.empty ()) stps.push_back (new TestPlatformGame::Villaner::ControlStep (dir, _description._speed));

	// ...and link everything in the monitor...
	addControlStepsMonitor (new QGAMES::CharacterControlStepsMonitor 
		(__GAMETEST_STEPSMONITORTOROOM__, stps, this, false)); // It is observed...not cyclical...
}

// ---
void TestPlatformGame::Villaner::setControlMonitorToAvoidObstacle (QGAMES::Entity* e)
{
	QGAMES::CharacterControlSteps stps;

	// How to avoid the obstacle will depend on the size of the obstacle itself,
	// and on where the villan is in relation with the obstacle to avoid...
	// Take into account that the villan neves moves in diagonal, he only moves following the x or y axis
	QGAMES::bdata ang = __BD 0;
	int nStA = -1; int nStB = -1;
	if (_description._orientation.posY () != 0) // Moving in the y axis...
	{
		// The number of steps walking down or up to avoid the obstacle are the visual height of the object...
		nStA = e -> currentForm () -> visualDepth (e -> currentAspect ()) + 
			currentForm () -> visualDepth (currentAspect ()) + 10 /** To avoid the collision. */;
		if (_description._orientation.posY () == 1) // Going down...
		{
			if (centerPosition ().posX () > e -> centerPosition ().posX ()) // To the left first
			{
				ang = -(__PI / __BD 2);
				nStB = (int) (e -> baseZone ().pos2 ().posX () - baseZone ().pos1 ().posX ()) + 5;
			}
			else // To the right first...
			{
				ang = (__PI / __BD 2);
				nStB = (int) (baseZone ().pos2 ().posX () - e -> baseZone ().pos1 ().posX ()) + 5;
			}
		}
		else // Going up...
		{
			if (centerPosition ().posX () > e -> centerPosition ().posX ()) // To the right first
			{
				ang = (__PI / __BD 2);
				nStB = (int) (e -> baseZone ().pos2 ().posX () - baseZone ().pos1 ().posX ()) + 5;
			}
			else // To the left first...
			{
				ang = -(__PI / __BD 2);
				nStB = (int) (baseZone ().pos2 ().posX () - e -> baseZone ().pos1 ().posX ()) + 5;
			}
		}
	}
	else // Moving in the x axis...
	{
		// The number of steps walking down or up to avoid the obstacle are the visual width of the object...
		nStA = e -> currentForm () -> visualLength (e -> currentAspect ()) +
			currentForm () -> visualLength (currentAspect ());
		if (_description._orientation.posX () == 1) // Going right...
		{
			if (centerPosition ().posY () > e -> centerPosition ().posY ()) // To the right first
			{
				ang = (__PI / __BD 2);
				nStB = (int) (e -> baseZone ().pos2 ().posY () - baseZone ().pos1 ().posY ());
			}
			else // To the left first...
			{
				ang = -(__PI / __BD 2);
				nStB = (int) (baseZone ().pos2 ().posY () - e -> baseZone ().pos1 ().posY ());
			}
		}
		else // Going left...
		{
			if (centerPosition ().posY () > e -> centerPosition ().posY ()) // To the left first
			{
				ang = -(__PI / __BD 2);
				nStB = (int) (e -> baseZone ().pos2 ().posY () - baseZone ().pos1 ().posY ());
			}
			else // To the right first...
			{
				ang = (__PI / __BD 2);
				nStB = (int) (baseZone ().pos2 ().posY () - e -> baseZone ().pos1 ().posY ());
			}
		}
	}

	assert (ang != __BD 0); // It shouldn't, but just in case!!

	// Add some steps to avoid the obstacle...
	QGAMES::Vector iDir = _description._orientation;
	iDir = iDir.rotate (QGAMES::Position::_cero, QGAMES::Position (__BD 0, __BD 0, __BD 1), ang);
	stps.push_back (new QGAMES::CharacterControlChangeStateStep (walkingStateForOrientation (iDir)));
	stps.push_back (new QGAMES::CharacterControlJustMoveStep (iDir, QGAMES::Vector::_cero));
	stps.push_back (new QGAMES::CharacterControlDistanceStep (__BD nStB));
	iDir = iDir.rotate (QGAMES::Position::_cero, QGAMES::Position (__BD 0, __BD 0, __BD 1), -ang);
	stps.push_back (new QGAMES::CharacterControlChangeStateStep (walkingStateForOrientation (iDir)));
	stps.push_back (new QGAMES::CharacterControlJustMoveStep (iDir, QGAMES::Vector::_cero));
	stps.push_back (new QGAMES::CharacterControlDistanceStep (__BD nStA));
	iDir = iDir.rotate (QGAMES::Position::_cero, QGAMES::Position (__BD 0, __BD 0, __BD 1), -ang);
	stps.push_back (new QGAMES::CharacterControlChangeStateStep (walkingStateForOrientation (iDir)));
	stps.push_back (new QGAMES::CharacterControlJustMoveStep (iDir, QGAMES::Vector::_cero));
	stps.push_back (new QGAMES::CharacterControlDistanceStep (__BD nStB));
	iDir = iDir.rotate (QGAMES::Position::_cero, QGAMES::Position (__BD 0, __BD 0, __BD 1), ang);
	stps.push_back (new QGAMES::CharacterControlChangeStateStep (walkingStateForOrientation (iDir))); 
	stps.push_back (new QGAMES::CharacterControlJustMoveStep (iDir, QGAMES::Vector::_cero));
	// Walk just a pixel to leave the orientation as at the beginning...
	stps.push_back (new QGAMES::CharacterControlDistanceStep (__BD 1)); 

	// The new steps monitor is made up of those steps...
	QGAMES::CharacterControlStepsMonitor* nSM = new QGAMES::CharacterControlStepsMonitor 
		(__GAMETEST_STEPSMONITORTOAVOIDOBSTACLE__, stps, this, false);
	// and what left from the previous one (if any)
	if (stepsMonitor () != NULL)
		nSM -> addStepsLeftFrom (stepsMonitor ());

	// Finally, adds the new steps monitor...
	addControlStepsMonitor (nSM); // It is observed, not ciclycal...
}

// ---
void TestPlatformGame::Villaner::ControlStep::initializeOn (QGAMES::Character* a)
{
	_startingMove = 0;
	assert (dynamic_cast <TestPlatformGame::Villaner*> (a)); // It has to be a villaner...
	((TestPlatformGame::Villaner*) a) -> toWalk (_direction, _speed); // Starts to wals...
}

// ---
bool TestPlatformGame::Villaner::ControlStep::isEndOn (QGAMES::Character* a)
{
	if (++_startingMove < 5)
		return (false);
	// During the first 5 steps after starting the monitor the position is not checked...
	// Just to avoid that false notifications are launched...

	bool result = false;

	// The maximum desviation of the final position will depend on the speed...
	assert (dynamic_cast <QGAMES::SimpleLinearMovement*> (a -> currentMovement ())); // Just in case...
	int mD = (int) ((QGAMES::SimpleLinearMovement*) a -> currentMovement ()) -> speed () + 1;

	// Depending on the diredction of the movement
	// The villan will get the end of the controlStep reaching whether the x or the y coordinate of central position
	if (_direction == QGAMES::Vector (__BD 1, __BD 0, __BD 0) ||
		_direction == QGAMES::Vector (__BD -1, __BD 0, __BD 0)) // That's it, he's moving in the x axis...
		result = (a -> position ().posX () >= __BD (11 * 32) && a -> position ().posX () <= __BD ((11 * 32) + mD));
	else 
		result = (a -> position ().posY () >= __BD (13 * 32) && a -> position ().posY () <= __BD ((13 * 32) + mD));

	return (result);
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
const int TestPlatformGame::ThingToCatch::_INTENSITIES [6] = { 255, 200, 155, 100, 55, 0 };

// ---
QGAMES::Entity* TestPlatformGame::ThingToCatch::clone () const
{ 
	QGAMES::Entity* result = new TestPlatformGame::ThingToCatch (_id, _forms, _data);

	result -> setMovements (movements ());
	result -> setAnimations (cloneAnimations ());
	result -> setStates (cloneStates ());

	return (result);
}

// ---
void TestPlatformGame::ThingToCatch::setDescription (const TestPlatformGame::ThingToCatchLocation& tL)
{ 
	_description = tL; 

	setCurrentForm (_description._canBeCaught ? __GAMETEST_THINGSCANBECAUGHTFORM__ : __GAMETEST_THINGSBEINGCARRIEDFORM__);
	setCurrentAspect (_description._thingType);
	setVisible (true);
	
	setPosition (_description._position - 
		__GAMETEST_REFERENCEALTITUDOFBASE__ - QGAMES::Vector (__BD 0, __BD 0, __BD visualHeight ()));
	setFixBasePosition (); 
	setFixBasePosition 
		(QGAMES::Position (__MINBDATA__, __MINBDATA__, basePosition ().posZ ())); // anchor it to the floor...

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
	if (!isVisible ())
		return;

	QGAMES::PlatformArtist::updatePositions ();

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
		__GAMETEST_REFERENCEALTITUDOFBASE__ - QGAMES::Vector (__BD 0, __BD 0, __BD visualHeight ()));
	setFixBasePosition (); 
	setFixBasePosition 
		(QGAMES::Position (__MINBDATA__, __MINBDATA__, basePosition ().posZ ())); // anchor it to the floor...

	// Keep the changes in the configuration...
	((TestPlatformGame::Game*) game ()) -> updateThingStatus (oldD, _description);

	reStartAllCounters ();
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
QGAMES::Entity* TestPlatformGame::Meal::clone () const
{ 
	QGAMES::Entity* result = new TestPlatformGame::Meal (_id, _forms, _data);

	result -> setMovements (movements ());
	result -> setAnimations (cloneAnimations ());
	result -> setStates (cloneStates ());

	return (result);
}

// ---
void TestPlatformGame::Meal::setDescription (const TestPlatformGame::MealLocation& mL)
{ 
	_description = mL; 

	setCurrentForm (__GAMETEST_MEALFORM__);
	setCurrentAspect (mL._mealType);
	setVisible (true);
	
	setPosition (_description._position - 
		__GAMETEST_REFERENCEALTITUDOFBASE__ - QGAMES::Vector (__BD 0, __BD 0, __BD visualHeight ()));
	setFixBasePosition (); 
	setFixBasePosition 
		(QGAMES::Position (__MINBDATA__, __MINBDATA__, basePosition ().posZ ())); // anchor it to the floor...
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
void TestPlatformGame::Meal::updatePositions ()
{
	if (!isVisible ())
		return;

	QGAMES::PlatformArtist::updatePositions ();
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
TestPlatformGame::WorldModel::WorldModel ()
	: QGAMES::MazeModel (__GAMETEST_HORIZONTALSCENESINMAZE__,  __GAMETEST_VERTICALSCENESINMAZE__, 
						 std::vector <QGAMES::SetOfOpenValues> 
							(__GAMETEST_HORIZONTALSCENESINMAZE__ * __GAMETEST_VERTICALSCENESINMAZE__))
{
	assert (_mazeModel.size () == (sizeof (TestPlatformGame::World::_MAZESCENES) / sizeof (int)));
	for (int i = 0; i < (int) _mazeModel.size (); i++)
	{
		QGAMES::SetOfOpenValues oV;
		oV.addOpenValue (0, QGAMES::OpenValue (TestPlatformGame::World::_MAZESCENES [i]));
		_mazeModel [i] = oV; // only the basic value is stored (the type of moze scene)
		// Now both will be the same...
		// What is kept in the maze model vector is the type of scene...
	}
}

// ---
std::vector <bool> TestPlatformGame::WorldModel::possibleConnectionsAt (const QGAMES::MazeModel::PositionInMaze& p)
{
	std::vector<bool> result (8, false); 
	// possible maximum connections, by default all of them are not possible...
	
	bool r = false; bool l = false; bool u = false; bool d = false;
	switch (mazeInfoAt (QGAMES::MazeModel::PositionInMaze 
		(p._positionX, p._positionY)).openValue (0).intValue ())
	{
		case  1: u = d =			true; break;
		case  2: r = l =			true; break;
		case  3: r = d =			true; break;
		case  4: l = d =			true; break;
		case  5: l = u =			true; break;
		case  6: r = u =			true; break;
		case  7: l = r = u =		true; break;
		case  8: l = r = d =		true; break;
		case  9: r = u = d =		true; break;
		case 10: l = u = d =		true; break;
		case 11: l =				true; break;
		case 12: d =				true; break;
		case 13: r =				true; break;
		case 14: u =				true; break;
		case 15: 
		case  0: r = l = u = d =	true; break;
		default: assert (0); // It shouldn't be here...
	}

	// 0:RIGHT, 1:RIGHT-DOWN, 2:DOWN, 3:DOWN_LEFT, 4:LEFT, 5:LEFT-UP, 6:UP, 7:RIGHT-UP 
	result [0] = r; result [2] = d; result [4] = l; result [6] = u;
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
bool TestPlatformGame::World::moveToMazeSceneInDirection (const QGAMES::Vector& dr)
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
		QGAMES::Vector (dr.posX () * __BD (k -> map () -> width ()),
						dr.posY () * __BD (k -> map () -> height ()),
						__BD 0) + (__BD 10 * dr));

	return (false);
}

// ---
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

	// The villaners are set at the early beginning...
	// Because they are visible not depending on the room the knight is in.
	const std::vector <TestPlatformGame::VillanerLocation>& vll = ((TestPlatformGame::Game*) game ()) -> villanersInMaze ();
	assert ((int) vll.size () == __GAMETEST_NUMBERVILLANERS__); // To be sure...
	for (int i = 0; i < (int) vll.size (); i++)
		((TestPlatformGame::Villaner*) (__AGM game ()) -> 
			artist (__GAMETEST_VILLANERCHARACTERBASEID__ + i)) -> setDescription (vll [i]);

	reStartAllOnOffSwitches ();
}

// ---
void TestPlatformGame::World::updatePositions ()
{
	QGAMES::World::updatePositions ();

	// Verify whether there are 5 (in conf) or more boxes of the type 8 (int conf) in the central scene....
	// If they are, then the villan at the exit position starts to move towards the center...
	// leaving the exit and allowing the knight to exit and finishs the game!
	const std::vector <TestPlatformGame::ThingToCatchLocation>& thgs =
		((TestPlatformGame::Game*) game ()) -> thingsInARoom (__GAMETEST_CENTERSCENEINTHEMAZE__);
	
	// Verifies (if it wasn't done previously) whether the central room has or not enought
	// elements to attract the villan at the exit room...
	if (!onOffSwitch (_SWITCHEXITVILLANTOMOVE) -> isOn ())
	{
		int nYStns = 0;
		for (int i = 0; i < (int) thgs.size (); i++)
			if (thgs [i]._canBeCaught && 
				(thgs [i]._thingType >= __GAMETEST_TYPEOFTHINGFORVILLANTOMOVE1__ && 
				 thgs [i]._thingType <= __GAMETEST_TYPEOFTHINGFORVILLANTOMOVE2__)) // Including dark blue, light blue, green, orange, violet and purple
				nYStns++;
		if (nYStns >= __GAMETEST_NUMBEROFTHINGSFORVILLANTOMOVE__)
		{
			onOffSwitch (_SWITCHEXITVILLANTOMOVE) -> set (true); // No more...
			game () -> sound (__GAMETEST_DONESOUNDID__) -> play (__GAMETEST_WORLDSOUNDCHANNEL__); // A sound plays...

			// The villan starts to move...
			((TestPlatformGame::Villaner*) (__AGM game ()) -> artist (__GAMETEST_VILLANERCHARACTERBASEID__)) -> 
				setDescription (TestPlatformGame::VillanerLocation (0, 2, 0, 0, 
					QGAMES::Position (__BD (01 * 32), __BD (13 * 32), __BD 0), 
					QGAMES::Vector (__BD 1, __BD 0, __BD 0), 3 /** Speed */, 
					QGAMES::MazeModel::PositionInMaze (0, 0), QGAMES::MazeModel::PositionInMaze (5, 4), 0));
		}
	}
}

// ---
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
	int xM = tM -> width (); int yM = tM -> height ();
	int tW = tM -> visualTileWidth (); int tH = tM -> visualTileHeight ();

	// The Base...
	for (int i = 0; i < 25; i++)
	{
		int h = -tH;
		int iPos = i * tW; 
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
	// Comming from the knight or from the villaner, but with similar sense...
	if (e.code () == __GAMETEST_KNIGHTREACHEDLEFTLIMIT__ || 
		e.code () == __GAMETEST_VILLANERREACHEDLEFTLIMIT__ ||
		e.code () == __GAMETEST_KNIGHTREACHEDRIGHTLIMIT__ || 
		e.code () == __GAMETEST_VILLANERREACHEDRIGHTLIMIT__ ||
		e.code () == __GAMETEST_KNIGHTREACHEDUPLIMIT__ || 
		e.code () == __GAMETEST_VILLANERREACHEDUPLIMIT__ ||
		e.code () == __GAMETEST_KNIGHTREACHEDDOWNLIMIT__ || 
		e.code () == __GAMETEST_VILLANERREACHEDDOWNLIMIT__)
	{
		// Is the knight who notifies?
		bool knight = (e.code () == __GAMETEST_KNIGHTREACHEDLEFTLIMIT__ ||
					   e.code () == __GAMETEST_KNIGHTREACHEDRIGHTLIMIT__ ||
					   e.code () == __GAMETEST_KNIGHTREACHEDUPLIMIT__ ||
					   e.code () == __GAMETEST_KNIGHTREACHEDDOWNLIMIT__);

		QGAMES::Vector dr = QGAMES::Vector::_cero;
		switch (e.code ())
		{
			// Comming from the knight or the villaner...
			case __GAMETEST_KNIGHTREACHEDLEFTLIMIT__:
			case __GAMETEST_VILLANERREACHEDLEFTLIMIT__:
				dr = QGAMES::Vector (__BD -1, __BD 0, __BD 0); // Previous maze scene
				break;
			case __GAMETEST_KNIGHTREACHEDRIGHTLIMIT__:
			case __GAMETEST_VILLANERREACHEDRIGHTLIMIT__:
				dr = QGAMES::Vector (__BD 1, __BD 0, __BD 0); // Next maze scene
				break;
			case __GAMETEST_KNIGHTREACHEDUPLIMIT__:
			case __GAMETEST_VILLANERREACHEDUPLIMIT__:
				dr = QGAMES::Vector (__BD 0, __BD -1, __BD 0); // Previous row of scenes...
				break;
			case __GAMETEST_KNIGHTREACHEDDOWNLIMIT__:
			case __GAMETEST_VILLANERREACHEDDOWNLIMIT__:
				dr = QGAMES::Vector (__BD 0, __BD 1, __BD 0); // Next row of scenes...
				break;

			default:
				break; // Not needed really...
		}

		// Only when the notifier is the knight, the scene has to be changed
		// Otherwise what it has to be updated is only
		// its position in the maze, but not to change also the room visualized.
		if (knight)
		{
			// Changes the room visualized, unless the exit was reached
			// In that case a notification to finalize the game would be launched
			if (moveToMazeSceneInDirection (dr))
				notify (QGAMES::Event (__GAMETEST_KNIGHTREACHEDENDOFMAZE__, this));
		}
		else
			((TestPlatformGame::Villaner*) e.data ()) -> moveToMazeScene (dr);
	}
	else if (e.code () == __GAMETEST_SCENETIMEFINISHES__)
		notify (e); // There is nothing special to do so far with this event...
	else if (e.code () == __GAMETEST_TOREBUILDTHESCENE__)
		setMazeScene (((TestPlatformGame::Game*) game ()) -> mazeScene ());
	else if (e.code () == __GAMETEST_THINGTOMOVEVILLANERLEFT__)
	{
		// If something important has been left in the main scene...
		if (((TestPlatformGame::Game*) game ()) -> mazeScene () == __GAMETEST_CENTERSCENEINTHEMAZE__)
			game () -> sound (__GAMETEST_SOMETHINGLEFTSOUNDID__) -> play (__GAMETEST_WORLDSOUNDCHANNEL__); // A sound plays...
	}
	else
		QGAMES::World::processEvent (e);
}

// ---
__IMPLEMENTONOFFSWITCHES__ (TestPlatformGame::World::OnOffSwitches)
{
	addOnOffSwitch (new QGAMES::OnOffSwitch (TestPlatformGame::World::_SWITCHEXITVILLANTOMOVE, false));
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
void TestPlatformGame::Scene::finalize ()
{
	QGAMES::Scene::finalize ();

	removeEntity (__AT (__AGM game ()) -> artist (__GAMETEST_MAINCHARACTERID__));
	for (int i = 0; i < __GAMETEST_NUMBERVILLANERS__; i++)
		removeEntity (__AT (__AGM game ()) -> artist (__GAMETEST_VILLANERCHARACTERBASEID__ + i));
	for (int i = 0; i < __GAMETEST_MAXNUMBERTHINGSINTHEMAZEPERROOM__; i++)
		removeEntity (__AT (__AGM game ()) -> artist (__GAMETEST_THINGTOCATCHBASEID__ + i));
	for (int i = 0; i < __GAMETEST_MAXNUMBEROFMEALSINMAZEPERROOM__; i++)
		removeEntity (__AT (__AGM game ()) -> artist (__GAMETEST_MEALBASEID__ + i));

	setMap (__MININT__);
}

// ---
void TestPlatformGame::Scene::processEvent (const QGAMES::Event& e)
{
	// Comming from the knight...or from the villaner...
	if (e.code () == __GAMETEST_KNIGHTREACHEDLEFTLIMIT__ || 
		e.code () == __GAMETEST_VILLANERREACHEDLEFTLIMIT__ ||
		e.code () == __GAMETEST_KNIGHTREACHEDRIGHTLIMIT__ || 
		e.code () == __GAMETEST_VILLANERREACHEDRIGHTLIMIT__ ||
		e.code () == __GAMETEST_KNIGHTREACHEDUPLIMIT__ || 
		e.code () == __GAMETEST_VILLANERREACHEDUPLIMIT__ ||
		e.code () == __GAMETEST_KNIGHTREACHEDDOWNLIMIT__ || 
		e.code () == __GAMETEST_VILLANERREACHEDDOWNLIMIT__)
		notify (e); // and send it upper to the world...
	// From the knight too when it eat something proving more time...
	else if (e.code () == __GAMETEST_SCENETIMETOBEINCREMENTED__)
		toAddMoreTime (20); // 20 more...
	else if (e.code () ==  __GAMETEST_TOREBUILDTHESCENE__)
		notify (e); // To be processed by the World...
	else if (e.code () == __GAMETEST_THINGTOMOVEVILLANERLEFT__)
		notify (e);
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
QGAMES::Map* TestPlatformGame::TMXMapBuilder::createMapObject (int id, 
	const QGAMES::Layers& l, int w, int h, int d,
	int tW, int tH, int tD, const QGAMES::MapProperties& p)
{
	return (new TestPlatformGame::Map (id, l, w, h, tW, tH, p)); 
}
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// ---
TestPlatformGame::ControllingGameStates::ControllingGameStates (QGAMES::GameState* psSt)
	: QGAMES::AdvancedGameStateControl (__GAMETEST_CONTROLGAMESTATE__, psSt, 
		QGAMES::GameStates (), QGAMES::GameStateControl::FlowMap ())
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
std::string TestPlatformGame::ControllingGameStates::nextGameState
	(QGAMES::GameState* st, const QGAMES::Event& evnt)
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
		QGAMES::Position::_cero, // Top left
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
		result = new QGAMES::ShowcardGothic64YellowFont (_text);
	else if (_textType == 1)
		result = new QGAMES::ShowcardGothic36BlueFont (_text);
	else
		result = new QGAMES::ShowcardGothic32GreenFont (_text);

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
	  _nameIntroduced (false),
	  _initialPlayerProperties ()
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

	_initialPlayerProperties = _numberPlayer -> properties ();
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
		_numberPlayer -> setProperties (_initialPlayerProperties); // At the initial position...
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
	int mCX = tM -> width () >> 1; int mCY = tM -> height () >> 1;
	game () -> mainScreen () -> setPosition (QGAMES::Position (__BD -320, __BD 200, __BD 0));

	// The type of projection in this game is diedric (the map is diediric too)...
	game () -> mainScreen () -> currentCamera () -> setProjectType (QGAMES::Camera::ProjectionType::_QUICKDIEDRIC);

	// Sets the initial position of the entity...
	QGAMES::Entity* ety = game () -> entity (__GAMETEST_MAINCHARACTERID__);
	QGAMES::Position lP = ((TestPlatformGame::Game*) game ()) -> lastPosition ();
	ety -> setPosition (lP - __GAMETEST_REFERENCEALTITUDOFBASE__ - QGAMES::Vector (__BD 0, __BD 0, __BD ety -> visualHeight ()));
	((TestPlatformGame::Knight*) ety) -> setFixBasePosition ();
	((TestPlatformGame::Knight*) ety) -> setFixBasePosition 
		(QGAMES::Position (__MINBDATA__, __MINBDATA__, ety -> basePosition ().posZ ())); // anchor it to the floor...
	
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

	/*
	It shouldn't happen here, but it's better the game continue with entities moving...
	// Back to orthogonal...
	game () -> mainScreen () -> currentCamera () -> setProjectType (QGAMES::Camera::ProjectionType::_ORTHOGONAL);
	// ...and put the screen at the zero position...
	game () -> mainScreen () -> setPosition (QGAMES::Position::_cero);

	(__AGM game ()) -> world (__GAMETEST_WORLDID__) -> finalize ();
	(__AGM game ()) -> setWorld (__MININT__);
	*/

	unObserve (game () -> entity (__GAMETEST_MAINCHARACTERID__));

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
	// With no Z coordinate (it will be set when the game starts back)
	
	// It is supposed that the result variable has been changed along the game...
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
				case QGAMES::KeyCode::QGAMES_0: // To force to die...
					((TestPlatformGame::Knight*) (game () -> entity (__GAMETEST_MAINCHARACTERID__))) -> toDie ();
					break;
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

	// Coming from the knight...
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
	: QGAMES::SpendingTimeGameState (__GAMETEST_PLAYERHASDIEDGAMESTATE__, __BD 5 /** Seconds before the state finishes */,
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

	// Instead of finishing the world in the playing state
	// that is done here. It means that the world keeps moving after music after dead has finished!
	// Back to orthogonal...
	game () -> mainScreen () -> currentCamera () -> setProjectType (QGAMES::Camera::ProjectionType::_ORTHOGONAL);
	// ...and put the screen at the cero position...
	game () -> mainScreen () -> setPosition (QGAMES::Position::_cero);

	(__AGM game ()) -> world (__GAMETEST_WORLDID__) -> finalize ();
	(__AGM game ()) -> setWorld (__MININT__);
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

// ---
void TestPlatformGame::LevelCompleted::onExit ()
{
	// Instead of finishing the world in the playing state
	// that is done here. It means that the world keeps moving after music after dead has finished!
	// Back to orthogonal...
	game () -> mainScreen () -> currentCamera () -> setProjectType (QGAMES::Camera::ProjectionType::_ORTHOGONAL);
	// ...and put the screen at the cero position...
	game () -> mainScreen () -> setPosition (QGAMES::Position::_cero);

	(__AGM game ()) -> world (__GAMETEST_WORLDID__) -> finalize ();
	(__AGM game ()) -> setWorld (__MININT__);
}
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// ---
TestPlatformGame::GameHasEnded::GameHasEnded ()
	: QGAMES::SpendingTimeGameState (__GAMETEST_GAMEHASENDEDGAMESTATE__, __BD 12 /** seconds */,
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
	return (new QGAMES::ShowcardGothic32BrownFont (scr + std::string ("  ") +
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
	_thingsCarried [nP - 1][0] = TestPlatformGame::ThingToCatchLocation (); // Empty...
}

// ---
void TestPlatformGame::Game::Conf::iterateThingsCarried (int nP)
{ 
	assert (nP > 0 && nP <= (int) _thingsCarried.size ());
	
	TestPlatformGame::ThingToCatchLocation lT = _thingsCarried [nP - 1][__GAMETEST_MAXNUMBERTHINGSCARRIED__ - 1];
	for (int i = (__GAMETEST_MAXNUMBERTHINGSCARRIED__ - 2); i >= 0; i--)
		_thingsCarried [nP - 1][i + 1] = _thingsCarried [nP - 1][i];
	_thingsCarried [nP - 1][0] = lT;
}

// ---
TestPlatformGame::ThingToCatchLocation TestPlatformGame::Game::Conf::carryThing 
	(int nP, const TestPlatformGame::ThingToCatchLocation& tC)
{ 
	assert (nP > 0 && nP <= (int) _thingsCarried.size ());
	
	TestPlatformGame::ThingToCatchLocation lT = _thingsCarried [nP - 1][__GAMETEST_MAXNUMBERTHINGSCARRIED__ - 1];
	for (int i = (__GAMETEST_MAXNUMBERTHINGSCARRIED__ - 2); i >= 0; i--)
		_thingsCarried [nP - 1][i + 1] = _thingsCarried [nP - 1][i];
	_thingsCarried [nP - 1][0] = tC;

	return (lT); // Can be -1...
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
bool TestPlatformGame::Game::Conf::spaceToLeave (int nP,int nR)
{
	assert (nP > 0 && nP <= (int) _thingsInMaze.size ());

	bool result = false;
	for (std::vector <TestPlatformGame::ThingToCatchLocation>::const_iterator i = _thingsInMaze [nP - 1][nR].begin ();
			i != _thingsInMaze [nP - 1][nR].end () && !result; i++)
		if ((*i)._thingId == -1)
			result = true;
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

	_score			= std::vector <int> (nP, 0);
	_weaponsLevel	= std::vector <int> (nP, 0);
	_level			= std::vector <int> (nP, 1); // The first level is 1
	_playerName		= std::vector <std::string> (nP, std::string (__NULL_STRING__));
	_lastPositions  = std::vector <QGAMES::Position> (nP, __GAMETEST_FIRSTPOSITIONINMAZE__); // It will be modified later...
	_mazeScene		= std::vector <int> (nP, __GAMETEST_FIRSTSCENEINTHEMAZE__);
	_thingsCarried	= std::vector <std::vector <TestPlatformGame::ThingToCatchLocation>> 
		(nP, std::vector <TestPlatformGame::ThingToCatchLocation> 
			(__GAMETEST_MAXNUMBERTHINGSCARRIED__, TestPlatformGame::ThingToCatchLocation () /** Nothing by default. */));

	// ...and the villaners distributed across the maze...
	_villanersInMaze = std::vector <std::vector <TestPlatformGame::VillanerLocation>>
		(nP, std::vector <TestPlatformGame::VillanerLocation> (__GAMETEST_NUMBERVILLANERS__, 
			TestPlatformGame::VillanerLocation ()));
	for (int i = 0; i < nP; i++)
		distributeVillanersInTheMaze (i);

	// ...and now the things are distributed across the maze...
	_thingsInMaze	= std::vector <std::vector <std::vector <TestPlatformGame::ThingToCatchLocation>>>
		(nP, std::vector <std::vector <TestPlatformGame::ThingToCatchLocation>> 
			(__GAMETEST_NUMBEROFSCENESINTHEMAZE__, std::vector <TestPlatformGame::ThingToCatchLocation> 
				(__GAMETEST_MAXNUMBERTHINGSINTHEMAZEPERROOM__, TestPlatformGame::ThingToCatchLocation ())));
	for (int i = 0; i < nP; i++)
		distributeElementsInTheMaze (i);

	// ...and now the meal is distributed across the maze...
	_mealInMaze	= std::vector <std::vector <std::vector <TestPlatformGame::MealLocation>>>
		(nP, std::vector <std::vector <TestPlatformGame::MealLocation>> 
			(__GAMETEST_NUMBEROFSCENESINTHEMAZE__, std::vector <TestPlatformGame::MealLocation> 
				(__GAMETEST_MAXNUMBEROFMEALSINMAZEPERROOM__, TestPlatformGame::MealLocation ())));
	for (int i = 0; i < nP; i++)
		distributeMealInTheMaze (i);

	// Adjust the previous loaded elements to this game...
	_currentWorld = std::vector <int> (nP, __GAMETEST_WORLDID__);
	_currentScene = std::vector <int> (nP, __GAMETEST_SCENEID__);
	_numberLives = std::vector <int> (nP, __GAMETEST_MAXNUMBERLIVES__);
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
	QGAMES::Position posWatching (__BD (01 * 32), __BD (14 * 32), __BD 0); // Only occuppied by the villaner looking at the ener...
	QGAMES::Position posMiddle (__BD (11 * 32), __BD (13 * 32), __BD 0);

	// Posible destinations of the villan moving...
	std::vector <QGAMES::MazeModel::PositionInMaze> tPM (4);
	tPM [0] = QGAMES::MazeModel::PositionInMaze (5, 4); // The four corners big...
	tPM [1] = QGAMES::MazeModel::PositionInMaze (7, 7); // The four corners little...
	tPM [2] = QGAMES::MazeModel::PositionInMaze (9, 0); // The top limit of the maze...
	tPM [3] = QGAMES::MazeModel::PositionInMaze (0, 3); // A limit in the middle...

	// A vector with scenes already used and where another villaner can't be located...
	std::vector <int> rUsed;
	// The first and the central room have always a villaner...
	rUsed.push_back (__GAMETEST_EXITSCENEINTHEMAZE__); 
	rUsed.push_back (__GAMETEST_CENTERSCENEINTHEMAZE__);
	// The central villaner always move to the same position...
	rUsed.push_back (63); 

	std::vector <TestPlatformGame::VillanerLocation> vll = _villanersInMaze [nP];
	// The first villan is at the exit scene...
	// This villan doesn't move until the knight put a collection of objects at the center scene
	vll [0] = TestPlatformGame::VillanerLocation 
		(0, 2 /* type BRIGHT */, 0 /* Idle */, rUsed [0], posWatching, QGAMES::Vector (__BD 1, __BD 0, __BD 0), 1 /* normal speed */,
		 QGAMES::MazeModel::PositionInMaze (0, 0), QGAMES::MazeModel::PositionInMaze (0, 0), 0);
	// There is a villan at the center of the maze moving to a very specific scene room...
	vll [1] = TestPlatformGame::VillanerLocation 
		(1, 1 /* type DARK */, 0 /* Idle */, rUsed [1], posMiddle, QGAMES::Vector (__BD 1, __BD 0, __BD 0), 2 /* double speed */,
		 tPM [0], QGAMES::MazeModel::PositionInMaze (3, 6) /* 63 = rUsed [2]. */, 0); // This villan moves in a very special way...

	// Now the rest of the villan ar located ramdomly...
	for (int i = 0; i < (__GAMETEST_NUMBERVILLANERS__ - 2); i++) 
	{
		int nM = -1;
		do
		{
			nM = rand () % __GAMETEST_NUMBEROFSCENESINTHEMAZE__; // In which room?
		} while (std::find (rUsed.begin (), rUsed.end (), nM) != rUsed.end ()); // Until finding a room not ocuppied already...

		rUsed.push_back (nM);
		QGAMES::MazeModel::PositionInMaze pM (nM % __GAMETEST_HORIZONTALSCENESINMAZE__, nM / __GAMETEST_HORIZONTALSCENESINMAZE__);
		vll [i + 2] = 
			TestPlatformGame::VillanerLocation 
			(i + 2, (rand () % 2) /* type NORMAL or DARK. */, 0 /* Idle */, nM, posMiddle, 
			 QGAMES::Vector (__BD 1, __BD 0, __BD 0), (rand () % 2) + 1 /* normal speed or double */, pM, tPM [rand () % 4] /** The target */, 0); 
		// Two possible targets...
	}

	_villanersInMaze [nP] = vll;
}

// ---
void TestPlatformGame::Game::Conf::distributeElementsInTheMaze (int nP)
{
	static std::vector <QGAMES::Position> _POSIBBLEPOSITIONS 
		(__GAMETEST_NUMBERPOSSIBLEPOSITIONSINAROOM__, QGAMES::Position::_cero);
	_POSIBBLEPOSITIONS [0] = QGAMES::Position (__BD (11 * 32), __BD (05 * 32), __BD 0);
	_POSIBBLEPOSITIONS [1] = QGAMES::Position (__BD (05 * 32), __BD (13 * 32), __BD 0);
	_POSIBBLEPOSITIONS [2] = QGAMES::Position (__BD (11 * 32), __BD (13 * 32), __BD 0);
	_POSIBBLEPOSITIONS [3] = QGAMES::Position (__BD (17 * 32), __BD (13 * 32), __BD 0);
	_POSIBBLEPOSITIONS [4] = QGAMES::Position (__BD (11 * 32), __BD (17 * 32), __BD 0);

	std::vector <std::vector <TestPlatformGame::ThingToCatchLocation>> tLR = _thingsInMaze [nP];

	// The first two stones are located in the exist room
	// behind the villaner looking after that position...
	// Those two stones are to avoid you can avoid the villaner by one side and scape...
	std::vector <TestPlatformGame::ThingToCatchLocation> tL0 = tLR [0];
	tL0 [0] = TestPlatformGame::ThingToCatchLocation (0, 0, 0, QGAMES::Position (__BD (0 * 32), __BD (11.5 * 32), __BD 0));
	tL0 [1] = TestPlatformGame::ThingToCatchLocation (1, 0, 0, QGAMES::Position (__BD (0 * 32), __BD (13 * 32), __BD 0));
	tLR [0] = tL0;

	for (int j = 2 /** Two has already been located at room o. */; j < __GAMETEST_MAXNUMBEROFTHINGSINTHEMAZE__; j++)
	{
		int nM = rand () % __GAMETEST_NUMBEROFSCENESINTHEMAZE__; // In which room?
		int tR = TestPlatformGame::World::typeMazeScene (nM); // Which type is it?
		std::vector <TestPlatformGame::ThingToCatchLocation> tL = tLR [nM]; // Get all objects already in the room selected
		int pF = -1;
		for (int k = 0; k < (int) tL.size () && pF == -1; k++)
			if (tL [k]._thingType == -1) pF = k; // The element to set (there is a maximum per room) is one not used before...
		if (pF != -1) // unless there is none free, then nothing else can be done...
		{
			// Two elements can't be in the same place,
			// so it is necessary to know the positions already used in the room selected...
			QGAMES::Positions pUsed;
			for (int m = 0; m < (int) tL.size (); m++)
				if (tL [m]._thingId != -1)
					pUsed.push_back (tL [m]._position);

			int nII = 0;
			int tM = rand () % __GAMETEST_MAXTYPESOFTHINGSTOBECARRIED__; // Which type thing to be carried ?
			int pM = rand () % __GAMETEST_NUMBERPOSSIBLEPOSITIONSINAROOM__; // In which position? 
			while ((_POSITIONSPERSCENETYPE [tR][pM] == -1 /** while not valid for this room */ || 
					_POSITIONSPERSCENETYPE [tR][pM] == 2 /** not in the center. */ ||
					std::find (pUsed.begin (), pUsed.end (), _POSIBBLEPOSITIONS [_POSITIONSPERSCENETYPE [tR][pM]]) != pUsed.end () /** Not used before. */) && 
					nII < 2 /** to avoid infinite time. */) 
				{ pM++; if (pM >= __GAMETEST_NUMBERPOSSIBLEPOSITIONSINAROOM__) { pM = 0; nII++; } }
			if (nII < 2) // The thing to catch can be set...
				tL [pF] = TestPlatformGame::ThingToCatchLocation 
					(j, tM, nM, _POSIBBLEPOSITIONS [_POSITIONSPERSCENETYPE [tR][pM]]);
		}

		tLR [nM] = tL;
	}

	_thingsInMaze [nP] = tLR;
}

// ---
void TestPlatformGame::Game::Conf::distributeMealInTheMaze (int nP)
{
	static std::vector <QGAMES::Position> _POSIBBLEPOSITIONS 
		(__GAMETEST_NUMBERPOSSIBLEPOSITIONSINAROOM__, QGAMES::Position::_cero);
	_POSIBBLEPOSITIONS [0] = QGAMES::Position (__BD (11 * 32), __BD (03 * 32), __BD 0);
	_POSIBBLEPOSITIONS [1] = QGAMES::Position (__BD (03 * 32), __BD (12 * 32), __BD 0);
	_POSIBBLEPOSITIONS [2] = QGAMES::Position (__BD (11 * 32), __BD (12 * 32), __BD 0);
	_POSIBBLEPOSITIONS [3] = QGAMES::Position (__BD (19 * 32), __BD (12 * 32), __BD 0);
	_POSIBBLEPOSITIONS [4] = QGAMES::Position (__BD (11 * 32), __BD (19 * 32), __BD 0);

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
			// Two elements can't be in the same place,
			// so it is necessary to know the positions already used in the room selected...
			QGAMES::Positions pUsed;
			for (int m = 0; m < (int) mL.size (); m++)
				if (mL [m]._mealType != -1)
					pUsed.push_back (mL [m]._position);

			int nII = 0;
			int tM = rand () % __GAMETEST_MAXTYPESOFMEAL__;
			int pM = rand () % __GAMETEST_NUMBERPOSSIBLEPOSITIONSINAROOM__;
			while ((_POSITIONSPERSCENETYPE [tR][pM] == -1 /** while not valid for this room */ || 
					std::find (pUsed.begin (), pUsed.end (), _POSIBBLEPOSITIONS [_POSITIONSPERSCENETYPE [tR][pM]]) != pUsed.end () /** Not used before. */) && 
					nII < 2 /** to avoid infinite time. */) 
				{ pM++; if (pM >= __GAMETEST_NUMBERPOSSIBLEPOSITIONSINAROOM__) { pM = 0; nII++; } }
			if (nII < 2) // The location of the meal is possible... 
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
QGAMES::MapBuilder* TestPlatformGame::Game::createMapBuilder ()
{ 
	// In the form representing the base tiles, the ones maked with 0 represent bases, 
	// the others are decorate...
	static int eE [64] =
		{ 1,1,0,1,1,1,1,1,
		  1,1,1,1,0,0,0,0,
		  1,1,1,1,1,1,1,1,
		  1,1,1,1,1,1,1,1,
		  1,1,1,1,1,1,1,1,
		  1,1,1,1,1,1,1,1,
		  1,1,1,1,1,1,1,1,
		  1,1,1,1,1,1,1,1 };
	 
	QGAMES::MapBuilder* result = new QGAMES::MapBuilder (parameter (__GAME_MAPSOBJECTSFILE__));
	result -> addAddsOn (new QGAMES::ObjectMapBuilderAddsOn (objectBuilder ()));
	result -> addAddsOn (new QGAMES::BackgroundObjectMapBuilder (objectBuilder ()));
	TestPlatformGame::TMXMapBuilder* mAdd = 
		new TestPlatformGame::TMXMapBuilder ((QGAMES::Sprite2DBuilder*) formBuilder ());
	std::vector <int> e; for (int i = 0; i < 64; i++) if (eE [i] == 1) e.push_back (i);
	mAdd -> setBaseTilesExcluded (e);
	result -> addAddsOn (mAdd);
	
	return (result);
}

// ---
void TestPlatformGame::Game::initialize ()
{
	if (isInitialized ())
		return; // Only once...

	QGAMES::AdvancedArcadeGame::initialize ();
	if (!isInitialized ())
		return;

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
