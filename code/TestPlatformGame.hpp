/** \addtogroup AdArcadeGame */
/*@{*/

/**	
 *	@file	
 *	File: TestPlatformGame.hpp \n
 *	Framework: Commty Game Library (CGL) \n
 *	Author: Ignacio Cea Forniés (Community Networks) \n
 *	Creation Date: 08/06/2018 \n
 *	Description: Testing how the platform game library works. \n
 *	Versions: 1.0 Initial
 */

#ifndef __QGAMES_TESTPLATFORMGAME__
#define __QGAMES_TESTPLATFORMGAME__

#include "Defs.hpp"
#include <Platform/ptartist.hpp>
#include <Platform/ptbuilders.hpp>

namespace TestPlatformGame
{
	// -------The GUI System-------------------------------------------------------------------------
	/** Defines a decorator to show the element when focused. */
	class WhenFocusDecorator : public QGAMES::Widget::ComplexDecorator
	{
		public:
		WhenFocusDecorator ();

		/** @see parent. */
		bool isBefore () { return (true); }
	};

	/** Defines a widget to control the menus. */
	class ControlWidget : public QGAMES::SelectionOptionsGameState::MenuControlWidget
	{
		public:
		ControlWidget (int id, Decorator* d = NULL, 
			const QGAMES::ListOfWidgets& wdts = QGAMES::ListOfWidgets (), int wF = -1)
			: QGAMES::SelectionOptionsGameState::MenuControlWidget (id, d, wdts, wF)
							{ }

		/** @see parent. */
		virtual void processEvent (const QGAMES::Event& e);
	};

	/** The standard GUI System builder.
		It is used by most of the implementations. */
	class GUISystemBuilder : public QGAMES::StandardGUISystemBuilder
	{
		public:
		GUISystemBuilder (const std::string& fDef, QGAMES::FormBuilder* fB)
			:  QGAMES::StandardGUISystemBuilder (fDef, fB)
							{ }

		protected:
		/** @see parent. */
		virtual QGAMES::Widget* createWidget (const QGAMES::GUISystemBuilder::WidgetDefinition& def);
	};
	// --------------------------------------------------------------------------------

	// -------The input handler-------------------------------------------------------------------------
	class InputHandlerBehaviour : public QGAMES::InputHandler::StandardBehavior
	{
		public:
		InputHandlerBehaviour ()
			: QGAMES::InputHandler::StandardBehavior ()
							{ }

		/** @see parent. */
		virtual void onKeyboardStatus (const std::vector <bool>& kS);
		virtual void manageKeyOnState (QGAMES::GameState* st, int kP);
		virtual void manageJoystickMovementOnState (QGAMES::GameState* st, int nJ, const QGAMES::Vector& dr);
		virtual void manageJoystickButtonOnState (QGAMES::GameState* st, int nJ, bool p, int nB, int bF);
	};
	// --------------------------------------------------------------------------------

	// -------The score objects-------------------------------------------------------------------------
	/** The score object representing the left number of lives of the current player. */
	class NumberOfLivesScoreObject : public QGAMES::ScoreObject
	{
		public:
		NumberOfLivesScoreObject ();

		/** @see parent. */
		virtual QGAMES::Entity* clone ()
							{ return (new NumberOfLivesScoreObject ()); }

		/** @see parent. */
		virtual void drawOn (QGAMES::Screen* scr, const QGAMES::Position& p);
	};

	/** The score object to indicate the number of pointt of the current player. */
	class NumberOfPointsScoreObject : public QGAMES::ScoreObjectNumber
	{
		public:
		NumberOfPointsScoreObject ();

		/** @see parent. */
		virtual QGAMES::Entity* clone ()
							{ return (new NumberOfPointsScoreObject ()); }

		/** @see parent. */
		virtual void updatePositions ();

		protected:
		/** @see parent */
		virtual int formToDrawDigits ()
							{ return (__QGAMES_RAVIE24BLUELETTERS__); }
	};

	/** This score object is to indicate the things the current character is carrying with him / her. */
	class ThingsBeingCarriedScoreObject : public QGAMES::ScoreObject
	{
		public:
		ThingsBeingCarriedScoreObject ();

		/** @see parent. */
		virtual QGAMES::Entity* clone ()
							{ return (new ThingsBeingCarriedScoreObject ()); }

		/** @see parent. */
		virtual void drawOn (QGAMES::Screen* scr, const QGAMES::Position& p);
	};

	/** This score object is to indicate remaining time. */
	class RemainingTimeScoreObject : public QGAMES::ScoreObjectNumber
	{
		public:
		RemainingTimeScoreObject ();

		/** @see parent. */
		virtual QGAMES::Entity* clone ()
							{ return (new RemainingTimeScoreObject ()); }

		/** @see parent. */
		virtual void updatePositions ();

		protected:
		/** @see parent */
		virtual int formToDrawDigits ()
							{ return (__QGAMES_RAVIE24ORANGELETTERS__); }
	};

	/** The level of the energy. */
	class KnightEnergyLevelScoreObject : public QGAMES::StandardEnergyBarScoreObject
	{
		public:
		KnightEnergyLevelScoreObject ();

		/* @see parent. */
		virtual void updatePositions ();
	};
	// --------------------------------------------------------------------------------

	// -------The characters-------------------------------------------------------------------------
	/** Defines the main character of the game. */
	class Knight : public QGAMES::PlatformArtist
	{
		public:
		Knight (int id, 
				const QGAMES::Forms& frms = QGAMES::Forms (), const QGAMES::Entity::Data& dt = QGAMES::Entity::Data ())
			: QGAMES::PlatformArtist (id, frms, dt),
			  _energyLevel (100) // By default...
							{ }

		/** @see parent. */
		virtual Entity* clone ()
							{ return (new Knight (_id, _forms, _data)); }

		/** Energy level. */
		int energyLevel () const
							{ return (_energyLevel); }
		void setEnergyLevel (int eL)
							{ _energyLevel = eL; 
							  if (_energyLevel < 0) _energyLevel = 0;
							  if (_energyLevel > 100) _energyLevel = 100; }

		// Different actions
		void toIterateThings ();
		void toCatchLeave ();
		void toStay (const QGAMES::Vector& o = QGAMES::Vector::_noPoint);
		bool staying () const;
		void toWalk (const QGAMES::Vector& o = QGAMES::Vector::_noPoint);
		bool walking () const;
		void toAttack (const QGAMES::Vector& o = QGAMES::Vector::_noPoint);
		bool attacking () const;
		bool endOfAttack () const;
		void toDie (const QGAMES::Vector& o = QGAMES::Vector::_noPoint); 
		bool dieing () const;
		bool died () const;
		bool alive () const
							{ return (!dieing () && !died ()); }

		/** @see parent. */
		virtual bool canMove (const QGAMES::Vector& d, const QGAMES::Vector& a);

		/** @see parent. */
		virtual void initialize ();
		virtual void updatePositions ();
		virtual void drawOn (QGAMES::Screen* s, const QGAMES::Position& p = QGAMES::Position::_noPoint);

		/** @see parent. */
		virtual void whenCollisionWith (QGAMES::Entity* e);

		private:
		/** To know wich is the current state orientation... */
		QGAMES::Vector currentStateOrientation () const;

		__DECLARECOUNTERS__ (Counters);
		virtual QGAMES::Counters* createCounters ()
							{ return (new Counters ()); }

		private:
		/** The energy level. 
			It is not persistent. */
		int _energyLevel; // Between 0 and 100. When becomes 0, the knight dies...

		// Implementation
		/** The last orientation when moving.
			It is useful to set states like attacking or dieing. */
		QGAMES::Vector _lastOrientation;

		// Implementation
		static const int _COUNTERDECREMENTENERGY = 0;
	};

	/** This structure is to keep where the villaners are in the maze and
		the status (satying, walking,...) of each. 
		Remember that the game moves the villaber across the maze.
		The different status can be so far just two: 
		0 Staying,
		1 Walking towards the next point of decision.
		2 Dieding. */
	struct VillanerLocation
	{
		VillanerLocation ()
			: _status (0), // Staying...
			  _roomNumber (-1), // Not defined
			  _position (QGAMES::Position::_cero), // None
			  _orientation (QGAMES::Vector (__BD 1, __BD 0, __BD 0)) // Looking to the right...
							{ }

		VillanerLocation (int s, int r, const QGAMES::Position& p, const QGAMES::Vector& v)
			: _status (s), 
			  _roomNumber (r),
			  _position (p),
			  _orientation (v)
							{ assert (s < 2 &&
									  _roomNumber >= 0 && _roomNumber < __GAMETEST_NUMBEROFSCENESINTHEMAZE__); }

		friend std::ostream& operator << (std::ostream& oS, const VillanerLocation& vL)
							{ oS << vL._status << std::endl << vL._roomNumber << std::endl 
								 << vL._position << std::endl << vL._orientation; return (oS); }
		friend std::istream& operator >> (std::istream& iS, VillanerLocation& vL)
							{ iS >> vL._roomNumber 
								 >> vL._position >> vL._orientation; return (iS); }

		int _status;
		int _roomNumber;
		QGAMES::Position _position;
		QGAMES::Vector _orientation;
	};

	/** Defines the villaner of the game. */
	class Villaner : public QGAMES::PlatformArtist
	{
		public:
		Villaner (int id, 
				const QGAMES::Forms& frms = QGAMES::Forms (), const QGAMES::Entity::Data& dt = QGAMES::Entity::Data ())
			: QGAMES::PlatformArtist (id, frms, dt),
			  _description (),
			  _energyLevel (100) // By default...
							{ }

		/** @see parent. */
		virtual Entity* clone ()
							{ return (new Villaner (_id, _forms, _data)); }

		// Manage the aspect
		const VillanerLocation& description () const
							{ return (_description); }
		void setDescription (const VillanerLocation& vL);
		void setNoDescription ();

		/** Energy level. */
		int energyLevel () const
							{ return (_energyLevel); }
		void setEnergyLevel (int eL)
							{ _energyLevel = eL; 
							  if (_energyLevel < 0) _energyLevel = 0;
							  if (_energyLevel > 100) _energyLevel = 100; }

		/** @see parent. */
		virtual bool canMove (const QGAMES::Vector& d, const QGAMES::Vector& a);

		/** @see parent. */
		virtual void initialize ();
		virtual void updatePositions ();
		virtual void drawOn (QGAMES::Screen* s, const QGAMES::Position& p = QGAMES::Position::_noPoint);

		/** @see parent. */
		virtual void whenCollisionWith (QGAMES::Entity* e);

		private:
		// Different actions are managed in a private way...
		void toStay (const QGAMES::Vector& o = QGAMES::Vector::_noPoint);
		bool staying () const;
		void toWalk (const QGAMES::Vector& o = QGAMES::Vector::_noPoint);
		bool walking () const;
		void toDie (const QGAMES::Vector& o = QGAMES::Vector::_noPoint); 
		bool dieing () const;
		bool died () const;
		bool alive () const
							{ return (!dieing () && !died ()); }

		/** To know wich is the current state orientation... */
		QGAMES::Vector currentStateOrientation () const;

		private:
		/** The description. */
		VillanerLocation _description;
		/** The energy level. 
			It is not persistent. */
		int _energyLevel; // Between 0 and 100. When becomes 0, the villaner dies...

		// Implementation
		/** The last orientation when moving.
			It is useful to set states like dieing. */
		QGAMES::Vector _lastOrientation;
	};

	/** This structure is used to keep information about the things distributed across the maze. */
	struct ThingToCatchLocation
	{
		ThingToCatchLocation ()
			: _thingType (-1),
			  _roomNumber (-1),
			  _canBeDestroyed (true), _canBeCaught (false),
			  _position (QGAMES::Position::_noPoint)
							{ }
		ThingToCatchLocation (int tP, int nR, const QGAMES::Position& p)
			: _thingType (tP),
			  _roomNumber (nR),
			  _canBeDestroyed (true), _canBeCaught (false), // By default the things can be destroyed first and then destroyed...
			  _position (p)
							{ assert (_thingType >= 0 && _thingType < __GAMETEST_MAXTYPESOFTHINGSTOBECARRIED__ &&
									  _roomNumber >= 0 && _roomNumber < __GAMETEST_NUMBEROFSCENESINTHEMAZE__); }

		bool operator == (const ThingToCatchLocation& tL) const // It doesn't take into account the status!!
							{ return (_thingType == tL._thingType && 
									  _roomNumber == tL._roomNumber && _position == tL._position); }

		friend std::ostream& operator << (std::ostream& oS, const ThingToCatchLocation& tL)
							{ oS << tL._thingType << std::endl << tL._roomNumber << std::endl 
								 << tL._canBeDestroyed << std::endl << tL._canBeCaught << std::endl << tL._position; return (oS); }
		friend std::istream& operator >> (std::istream& iS, ThingToCatchLocation& tL)
							{ iS >> tL._thingType >> tL._roomNumber 
								 >> tL._canBeDestroyed >> tL._canBeCaught >> tL._position; return (iS); }

		int _thingType;
		int _roomNumber;
		bool _canBeDestroyed, _canBeCaught;
		QGAMES::Position _position;
	};
	
	/** Defines the basic thing distributed across the maze. */
	class ThingToCatch : public QGAMES::PlatformArtist
	{
		public:
		ThingToCatch (int id, 
				const QGAMES::Forms& frms = QGAMES::Forms (), const QGAMES::Entity::Data& dt = QGAMES::Entity::Data ())
			: QGAMES::PlatformArtist (id, frms, dt),
			  _description (ThingToCatchLocation ()) // Empty...
							{ setVisible (false); } 

		/** @see parent. */
		virtual Entity* clone ()
							{ return (new ThingToCatch (_id, _forms, _data)); }

		// Manage the aspect
		const ThingToCatchLocation& description () const
							{ return (_description); }
		void setDescription (const ThingToCatchLocation& tL);
		void setNoDescription ();

		// Different actions
		void toBeDestroyed ();
		bool canBeDestroyed () const
							{ return (_description._canBeDestroyed); }
		bool canBeCaught () const
							{ return (_description._canBeCaught); }

		/** @see parent. */
		virtual void initialize ();
		virtual void updatePositions ();
		virtual void drawOn (QGAMES::Screen* s, const QGAMES::Position& p = QGAMES::Position::_noPoint);

		private:
		__DECLARECOUNTERS__ (Counters);
		virtual QGAMES::Counters* createCounters ()
							{ return (new Counters ()); }
		__DECLAREONOFFSWITCHES__ (OnOffSwitches);
		virtual QGAMES::OnOffSwitches* createOnOffSwitches ()
							{ return (new OnOffSwitches ()); }

		void toBeCaught ();

		private:
		ThingToCatchLocation _description;

		// Implementation
		/** Counters used to create the feeling the box is disappearing. */
		static const int _COUNTERDECREMENTINTENSITY = 0;
		static const int _COUNTERINTENSITY = 1;
		static const int _INTENSITIES [6]; // 255, 200, 155, 100, 55, 0...
		/** Switch on when the container is getting destroyed. */
		static const int _SWITCHCONTAINERBEINGDESTROYED = 0;
	};

	/** This structrure is used to keep information about the meal distributed across the maze. */
	struct MealLocation
	{
		MealLocation ()
			: _mealType (-1),
			  _roomNumber (-1),
			  _position (QGAMES::Position::_noPoint)
							{ }
		MealLocation (int mT, int nR, const QGAMES::Position& p)
			: _mealType (mT),
			  _roomNumber (nR),
			  _position (p)
							{ assert (_mealType >= 0 && _mealType < __GAMETEST_MAXTYPESOFMEAL__ &&
									  _roomNumber >= 0 && _roomNumber < __GAMETEST_NUMBEROFSCENESINTHEMAZE__); }

		bool operator == (const MealLocation& mL) const
							{ return (_mealType == mL._mealType && 
									  _roomNumber == mL._roomNumber && mL._position == mL._position); }

		friend std::ostream& operator << (std::ostream& oS, const MealLocation& tL)
							{ oS << tL._mealType << std::endl << tL._roomNumber << std::endl << tL._position; return (oS); }
		friend std::istream& operator >> (std::istream& iS, MealLocation& tL)
							{ iS >> tL._mealType >> tL._roomNumber >> tL._position; return (iS); }

		int _mealType;
		int _roomNumber;
		QGAMES::Position _position;
	};
	
	/** Defines the basic meal distributed across the maze. */
	class Meal : public QGAMES::PlatformArtist
	{
		public:
		Meal (int id, 
				const QGAMES::Forms& frms = QGAMES::Forms (), const QGAMES::Entity::Data& dt = QGAMES::Entity::Data ())
			: QGAMES::PlatformArtist (id, frms, dt),
			  _description (MealLocation ()) // Empty...
							{ setVisible (false); } 

		/** @see parent. */
		virtual Entity* clone ()
							{ return (new Meal (_id, _forms, _data)); }

		// Manage the aspect
		const MealLocation& description () const
							{ return (_description); }
		void setDescription (const MealLocation& tL);
		void setNoDescription ();

		bool isPoisson () const
							{ return (_description._mealType == 8); }
		bool isForIncrementingTime () const
							{ return (_description._mealType == 0 || _description._mealType == 1); }
		bool isForIncrementingEnergy () const
							{ return (_description._mealType >= 2 && _description._mealType < 8); }

		/** @see parent. */
		virtual void initialize ();
		virtual void drawOn (QGAMES::Screen* s, const QGAMES::Position& p = QGAMES::Position::_noPoint);

		/** @see parent. */
		virtual void whenCollisionWith (QGAMES::Entity* e);

		private:
		MealLocation _description;
	};

	/** To create basic entities. */
	class EntityBuilder : public QGAMES::AdvancedEntityBuilder
	{
		public:
		EntityBuilder (const std::string& fDef, QGAMES::FormBuilder* fB, QGAMES::MovementBuilder* mB)
			: QGAMES::AdvancedEntityBuilder (fDef, fB, mB)
							{ }
	
		private:
		virtual QGAMES::Entity* createEntity (const EntityDefinition& def);
	};
	// --------------------------------------------------------------------------------

	// -------The movements-------------------------------------------------------------------------
	/** To create basic movements. */
	class MovementBuilder : public QGAMES::AdvancedMovementBuilder
	{
		public:
		MovementBuilder (const std::string& fDef)
			: QGAMES::AdvancedMovementBuilder (fDef)
							{ }
	
		private:
		virtual QGAMES::Movement* createMovement (const MovementDefinition& def);
	};
	// --------------------------------------------------------------------------------

	// -------The world, scenes and maps-------------------------------------------------------------------------
	/** The class defining the head world. */
	class World : public QGAMES::World
	{
		public:
		World (int c, const QGAMES::Scenes& s, 
				const QGAMES::WorldProperties& p = QGAMES::WorldProperties ())
			: QGAMES::World (c, s, p)
							{ }

		/** To set the maze represented by the scene. */
		void setMazeScene (int nM);
		/** To know which is the type of a certain room in the maze. */
		static int typeMazeScene (int nM);
		/** To set that scene attending to a direction.
			The method returns true when the exit has been reached. */
		bool setNextMazeSceneInDirection (const QGAMES::Vector& dr);

		/** @see parent. */
		virtual void initialize ();
		virtual void drawOn (QGAMES::Screen* s, const QGAMES::Position& p);

		/** @see parent. */
		virtual void processEvent (const QGAMES::Event& e);

		private:
		/** Definition of the maze (defines wich kind of scene is every part of the maze) */
		static const int _MAZESCENES [__GAMETEST_NUMBEROFSCENESINTHEMAZE__];
	};

	/** The class defining the scene inside the world above used in the test. */
	class Scene : public QGAMES::Scene
	{
		public:
		friend class LoopCounterAction;

		Scene (int c, const QGAMES::Maps& m, const QGAMES::Scene::Connections& cn = QGAMES::Scene::Connections (), 
					const QGAMES::SceneProperties& p = QGAMES::SceneProperties (), 
					const QGAMES::EntitiesPerLayer& ePL = QGAMES::EntitiesPerLayer ());

		/** To change the aspect of the scene. */
		void setSceneType (int sT);

		/** To add more time. */
		int remainingTime ()
							{ return (counter (_COUNTERSECONDS) -> value ()); }
		void toAddMoreTime (int nS);

		/** @see parent. */
		virtual void updatePositions ();
		virtual void initialize ();

		/** @see parent. */
		virtual void processEvent (const QGAMES::Event& e);

		private:
		__DECLARECOUNTERS__ (Counters);
		virtual QGAMES::Counters* createCounters ()
								{ return (new Counters ()); }
		__DECLAREONOFFSWITCHES__ (OnOffSwitches);
		virtual QGAMES::OnOffSwitches* createOnOffSwitches ()
								{ return (new OnOffSwitches ()); }

		/** The action related with the loop created in the game. */
		class LoopCounterAction : public QGAMES::LoopCounter::Action
		{
			public:
			LoopCounterAction (Scene* scn)
				: QGAMES::LoopCounter::Action (),
				  _sceneOwner (scn)
							{ }

			/** @see parent. */
			virtual void doAction ();
			
			private:
			Scene* _sceneOwner;
		};

		private:
		// Implementation
		/** Definition of the types of scenes (defines whether every layer is or not active) */
		static const int _SCENESTYPE [__GAMETEST_NUMBEROFTYPESOFSCENES__][__GAMETEST_NUMBEROFLAYERSPOSSIBLEPERSCENE__];

		// Implementation
		/** To count the number of seconds from the beginning of the scene. */
		static const int _COUNTERSECONDS = 0;
		/** Switch to control whether, it is time to incremente the number of seconds. */
		static const int _SWITCHTOCOUNTSECONDS = 0;

		// Implementation
		// Data related with the level of difficulty
		static const int _DIFFICULTYRELATEDDATA [__GAMETEST_MAXDIFFICULTYLEVEL__][4];
	};

	/** The class defining the type of map used in the system. */
	class Map : public QGAMES::TiledMap
	{
		public:
		Map (int c, const QGAMES::Layers& l, int w, int h, int d, int tW, int tH, int tD,
			const QGAMES::MapProperties& p = QGAMES::MapProperties ())
			: QGAMES::TiledMap (c, l, w, h, d, tW, tH, tD, p)
							{ }

		/** To change the form of the map,, activating and desativating layers. */
		void setMapType (const int* lI);

		private:
		/** The name of all potential layers a scene can have */
		static const std::string _LAYERNAME [__GAMETEST_NUMBEROFLAYERSPOSSIBLEPERSCENE__];
	};

	/** The extension to create worlds. */
	class WorldBuilder : public QGAMES::WorldBuilder
	{
		public: 
		WorldBuilder (const std::string& eDef, QGAMES::MapBuilder* mB)
			: QGAMES::WorldBuilder (eDef, mB)
							{ }

		private:
		virtual QGAMES::World* createWorldObject (int no, const QGAMES::Scenes& s, 
			const QGAMES::WorldProperties& p);
		virtual QGAMES::Scene* createSceneObject (int ns, const QGAMES::Maps& m, 
			const QGAMES::Scene::Connections& cn, 
			const QGAMES::SceneProperties& p, const QGAMES::EntitiesPerLayer& ePL);
	};
	// --------------------------------------------------------------------------------

	// -------The Game States-------------------------------------------------------------------------
	/** 
	  *	PRINCIPAL GAME STATE:
	  *	The state to control the rest. 
	  *	What is important here is to define the first game state in the process and the rules to move to the next ones.
	  */
	class ControllingGameStates : public QGAMES::StandardGameStateControl
	{
		public:
		ControllingGameStates (QGAMES::GameState* psSt);

		protected:
		/** @see parent. */
		virtual std::string firstGameState () const
							{ return (std::string (__QGAMES_GAMESTATELOADINGNAME__)); }
		virtual std::string nextGameState (QGAMES::GameState* st, const QGAMES::Event& evnt) const;
	};

	/** A state that finishes when the return key or the mouse left button is pressed. */
	class UntilReturnKeyOrMouseLeftButtonIsPressed : public QGAMES::ForEverUntilInputHandlerEventGameState
	{
		public:
		UntilReturnKeyOrMouseLeftButtonIsPressed (int id, GameState* nS = NULL)
			: QGAMES::ForEverUntilInputHandlerEventGameState (id, new StopStateHandler, nS)
							{ }

		private:
		class StopStateHandler : public QGAMES::ForEverUntilInputHandlerEventGameState::Handler
		{
			public:
			StopStateHandler ()
				: QGAMES::ForEverUntilInputHandlerEventGameState::Handler ()
							{ }

			/** @see parent. */
			virtual bool stopWhen (const QGAMES::Event& evnt);
		};
	};

	/** 
	  * PRINCIPAL GAME STATE:
	  * The game starts with a Commty Logo appearing little by little. 
	  */
	class IntroLogoGameState : public QGAMES::SpendingTimeGameState
	{
		public:
		IntroLogoGameState ()
			: QGAMES::SpendingTimeGameState (__GAMETEST_INTROGAMESTATE__, __BD 10 /** Seconds on the screen after appearing */, 
				new QGAMES::ShowingAppearingFormGameState (__GAMETEST_INTROGAMESTATE__ + 1, 
					QGAMES::ShowingAppearingFormGameState::Properties 
						(__QGAMES_COMMTYNETSBIGLOGO__, 
						 0, /** Initial fade */ 
						 255, /** Final fade */
						 10, /** Fade increment */
						 1, /** loops before increment the fade */ 
						 QGAMES::Vector (__BD 0.5, __BD 0.5, __BD 0), /** Where. Relative position on the screen */ 
						 QGAMES::ShowingAppearingFormGameState::Properties::PosReference::_CENTER /** Position refered to the center of the logo */
						)))
							{ }
	};

	/**
	  * Game State part of the presentation.
	  * Every state of the presentation is made of the same things:
	  * A very beautifll background with a text in the middle of the screen.
	  * What the text represents will depend on the part of the presentation.
	  * Several states of this type will be set at the beginning of the project, but they will be used
	  * only as part of the next game state.
	  */
	class ShowingPartOfPresentationGameState : public QGAMES::ShowingTextGameState
	{
		public:
		ShowingPartOfPresentationGameState (int id, int frmId, const std::string& txt, int t);
		~ShowingPartOfPresentationGameState ();

		private:
		/** @see parent. */
		virtual QGAMES::ScoreObjectText* createText ();

		private:
		QGAMES::ShowingFixFormGameState* _form;

		// Implementation
		/** The text being shown. */
		std::string _text;
		/** The type of test. **/
		int _textType;
	};

	/** 
	 * PRINCIPAL GAME STATE:
	 * The presentation of the game rolls among different backgrounds with different titles.
	 * Every state dissapear and the next one appears back.
	 * A music plays in the background, and the state is on until a key is pressed. 
	 */
	class PresentationGameState : public UntilReturnKeyOrMouseLeftButtonIsPressed
	{
		public:
		PresentationGameState ();
		~PresentationGameState ();

		private:
		static QGAMES::RollingStatesGameState::SeparationStates separationStates ();

		private:
		QGAMES::RollingStatesGameState* _rollingStates;
		QGAMES::PlayingASoundGameState* _playingSound;
	};

	/** 
	  * PRINCIPAL GAME STATE:
	  * This state shows the standard menu, but with some additionals.
	  * The state finishes when a valid option is selected.
	  * The state creates the nested stated it needs, and destroy them when it is destroyed as well.
	  */
	class SelectionGameState : public QGAMES::StandardInitialSelectionOptionsGameState
	{
		public:
		SelectionGameState ();
		~SelectionGameState ();

		/** @see parent. */
		virtual void onEnter ();

		private:
		/** @see parent. */
		virtual void initializeGUISystem ();
		virtual void finalizeGUISystem ();

		private:
		QGAMES::ShowingFixFormGameState* _logoForm;
		QGAMES::ShowingFixFormGameState* _backgroundForm;
		QGAMES::PlayingASoundGameState* _playingSound;
	};

	/** 
	  * This game state shows the number of the player that is about to start to play.
	  */
	class ShowingPlayerAboutToStart : public QGAMES::ShowingTextGameState
	{
		public:
		ShowingPlayerAboutToStart (int id, QGAMES::GameState* nS = NULL)
			: QGAMES::ShowingTextGameState (id, 
				QGAMES::ShowingTextGameState::Properties 
					(255, /** Fade */
					 QGAMES::Position (__BD 0.5, __BD 0.4, __BD 0), /** Relative position of the letters in the screen */ 
					 QGAMES::ShowingTextGameState::Properties::PosReference::_CENTER, /** Reference position of the text (center) */
					 QGAMES::Vector (__BD 1, __BD 0, __BD 0) /** Orientation of the letters of the text (horizontal) */
					), nS)
							{ }

		private:
		/** @see parent. */
		virtual void actualizeText ();
		virtual QGAMES::ScoreObjectText* createText ()
							{ return (new QGAMES::Ravie36BlueFont (std::string ("---"))); }
	};

	/**
	 * PRINCIPAL GAME STATE:
	 * This game state is shown when everything is about to start. 
	 * The player takes a short breathe before the battle starts.
	 * The game state crates what it needs and destroy the nested states when finishes.
	 * If RETURN key or left mouse button are pressed or certain time of time goes fly the game starts.
	 */
	class GameAboutToStart : public QGAMES::SpendingTimeGameState
	{
		public:
		GameAboutToStart ();
		~GameAboutToStart ();

		bool nameIntroduced () const
							{ return (_nameIntroduced); }

		/** @see parent. */
		virtual void onEnter ();
		virtual void updatePositions ();
		virtual void onExit ();

		/** @see parent. */
		virtual void processEvent (const QGAMES::Event& evnt);

		private:
		void deleteGUISystem ();

		private:
		UntilReturnKeyOrMouseLeftButtonIsPressed* _untilPressed;
		ShowingPlayerAboutToStart* _numberPlayer;
		QGAMES::ShowingFixFormGameState* _background;

		// Implementation
		QGAMES::GUISystem* _guiSystem;
		bool _nameIntroduced;
	};

	/** 
	  * PRINCIPAL GAME STATE:
	  * When the player is playing.
	  * This is the central game state.
	  * The knight fights against the monsters in the maze trying to get the exit before time expires.
	  * An emotional music plays (if it has been allowed at configuration time) also as background.
	  * The game finishes whether the player reaches the exit at time or when there are no more lives left.
	  */
	class Playing : public QGAMES::GameState
	{
		public:
		typedef enum { 
			_STILLPLAYING = 0, _LEVELACHIEVED, _PLAYERDIED } Result;

		Playing ();
		~Playing ();

		/** To know which was the result of the game state when it finishes. */
		Result result () const
							{ return (_result); }

		/** @see parent. */
		virtual void onEnter ();
		virtual void updatePositions ();
		virtual void onExit ();

		/** @see parent. */
		virtual void processEvent (const QGAMES::Event& evnt);

		private:
		Result _result;

		// Implementation
		QGAMES::PlayingASoundGameState* _playingSound;
	};

	/** 
	 * PRINCIPAL GAME STATE:
	 * The player has been killed. This is the state to indicate it. 
	 * Who control what is next is the Control Game State over this one. 
	 * It could be to start with the next player if any,
	 * or even to finishes the game if there is no more players.
	 * In this state the world is still visible and just a simple sound sounds on the bckground.
	 * What it is important here is that when the game state finished the control is moved to the next player.
	 */
	class PlayerHasDied : public QGAMES::SpendingTimeGameState
	{
		public:
		PlayerHasDied ();
		~PlayerHasDied ();

		/** @see parent. */
		virtual void onExit ();

		private:
		QGAMES::PlayingASoundGameState* _playingSound;
	};

	/**
	 * PRINCIPAL GAME STATE:
	 * When the level has completed, and it is time to go finish the game for that player at least.
	 */
	class LevelCompleted : public QGAMES::SpendingTimeGameState
	{
		public:
		LevelCompleted ();
		~LevelCompleted ();

		private:
		QGAMES::PlayingASoundGameState* _playingSound;
	};

	/** The game state that shows the game has ended. */
	class ShowingGameHasEnded: public QGAMES::ShowingTextGameState
	{
		public:
		ShowingGameHasEnded (int id, QGAMES::GameState* nS = NULL)
			: QGAMES::ShowingTextGameState (id,
				QGAMES::ShowingTextGameState::Properties
					(255, /** Fade */
					 QGAMES::Position (__BD 0.5, __BD 0.5, __BD 0), /** Relative position in the screen */ 
					 QGAMES::ShowingTextGameState::Properties::PosReference::_CENTER, /** Reference position of the text (center) */
					 QGAMES::Vector (__BD 1, __BD 0, __BD 0) /** Orientation of the text (horizontal) */
					), nS)
							{ }

		private:
		/** @see parent. */
		virtual QGAMES::ScoreObjectText* createText ()
							{ return (new QGAMES::Ravie72OrangeFont (std::string ("Game Over"))); }
	};

	/** 
	 * PRINCIPAL GAME STATE:
	 * The game has finishes as nobody has enough lives to continue.
	 * Who control what is next, is the Control Game State over this one. In this case is to go back to initial menu.
	 * What happen in this game is as simple as a text is shown to indicate the situation with a image and stars in the background.
	 * A simple music is also indicating that situation.
	 */
	class GameHasEnded : public QGAMES::SpendingTimeGameState
	{
		public:
		GameHasEnded ();
		~GameHasEnded ();
		
		/** @see parent. */
		virtual void onExit ();

		private:
		ShowingGameHasEnded* _text;
		QGAMES::ShowingFixFormGameState* _background;
		QGAMES::PlayingASoundGameState* _playingSound;
	};

	/** The game state that shows that the player has won. */
	class ShowingPlayerHasWon : public QGAMES::ShowingTextGameState
	{
		public:
		ShowingPlayerHasWon (int id, QGAMES::GameState* nS = NULL)
			: QGAMES::ShowingTextGameState (id,
				QGAMES::ShowingTextGameState::Properties
					(255, /** Fade */
					 QGAMES::Position (__BD 0.5, __BD 0.5, __BD 0), /** Relative position in the screen */ 
					 QGAMES::ShowingTextGameState::Properties::PosReference::_CENTER, /** Reference position of the text (center) */
					 QGAMES::Vector (__BD 1, __BD 0, __BD 0) /** Orientation of the text (horizontal) */
					), nS)
							{ }

		private:
		/** @see parent. */
		virtual QGAMES::ScoreObjectText* createText ()
							{ return (new QGAMES::Ravie72OrangeFont (std::string ("Player wins"))); }
	};

	/**
	 * PRINCIPAL GAME STATE:
	 * The game has finishes because one of the players has reached the goal.
	 */
	class PlayerHasWon : public QGAMES::SpendingTimeGameState
	{
		public:
		PlayerHasWon ();
		~PlayerHasWon ();
		
		/** @see parent. */
		virtual void onExit ();

		private:
		ShowingGameHasEnded* _text;
		QGAMES::ShowingFixFormGameState* _background;
		QGAMES::PlayingASoundGameState* _playingSound;
	};

	/** 
	  * The specific class to show the different entries of the hall of fame. 
	  *	The class inherits from the ome defined at AdvancedArcade library redefining how entries are created. 
	  */
	class ShowingEntriesHallOfFame : public QGAMES::ShowingHallOfFameGameState 
	{
		public:
		ShowingEntriesHallOfFame (int id, GameState* nS = NULL)
			: QGAMES::ShowingHallOfFameGameState (id, 
				QGAMES::ShowingHallOfFameGameState::Properties 
					(255, /** Fade */
					 QGAMES::Position (__BD 10 / __BD __GAMETEST_SCREENWIDTH__, 
									   __BD 10 / __BD __GAMETEST_SCREENHEIGHT__, 
									   __BD 0), /** Relative position of the first letter of the first entry*/
					 10 /** Separation between entries */
					), nS)
							{ }

		private:
		/** @see parent. */
		virtual std::map <std::string, QGAMES::SetOfOpenValues> orderHallOfFame (const std::map <std::string, QGAMES::SetOfOpenValues>& hF);
		// The text created is just with the score adjusted to 5 digits and the name of the player. */
		virtual QGAMES::ScoreObjectText* createTextFor (const std::string& eK, const QGAMES::SetOfOpenValues& sV);
	};

	/**
	  * PRINCIPAL GAME STATE:
	  * The hall of fame. The entries are show with a background of start rolling behind. 
	  * Everything until a return key or a mouse left button is pressed. 
	  */
	class HallOfFameGameState : public UntilReturnKeyOrMouseLeftButtonIsPressed
	{
		public:
		HallOfFameGameState ();
		~HallOfFameGameState ();

		UntilReturnKeyOrMouseLeftButtonIsPressed* _hallOfFame;
		QGAMES::ShowingFixFormGameState* _logoForm;
		QGAMES::PlayingASoundGameState* _playingSound;
	};
	// --------------------------------------------------------------------------------

	// -------The Game-------------------------------------------------------------------------
	class Game : public QGAMES::AdvancedArcadeGame
	{
		public:
		/** The configuration adds things important for every player. */
		class Conf : public QGAMES::AdvancedArcadeGame::Conf
		{
			public:
			Conf (int nP);

			/** @see parent. */
			virtual Configuration* clone ()
							{ return (new Conf (*this)); }

			// To Read & Set the internal attributes
			// nP from 1 to the max number of users
			const std::string& playerName (int nP) const 
							{ assert (nP > 0 && nP <= (int) _playerName.size ());
							  return (_playerName [nP - 1]); }
			void setPlayerName (int nP, const std::string& n)
							{ assert (nP > 0 && nP <= (int) _playerName.size ());
							  _playerName [nP - 1] = n; }
			const QGAMES::Position& lastPosition (int nP) const
							{ assert (nP > 0 && nP <= (int) _lastPositions.size ());
							  return (_lastPositions [nP - 1]); }
			void setLastPosition (int nP, const QGAMES::Position& p)
							{ assert (nP > 0 && nP <= (int) _lastPositions.size ());
							  _lastPositions [nP - 1] = p; }
			int level (int nP) const 
							{ assert (nP > 0 && nP <= (int) _level.size ());
							  return (_level [nP - 1]); }
			void setLevel (int nP, int l)
							{ assert (nP > 0 && nP <= (int) _level.size ());
							  _level [nP - 1] = (l >= 0) ? l : 0; }
			int score (int nP) const 
							{ assert (nP > 0 && nP <= (int) _score.size ());
							  return (_score [nP - 1]); }
			void setScore (int nP, int s)
							{ assert (nP > 0 && nP <= (int) _score.size ());
							  _score [nP - 1] = (s >= 0) ? s : 0; }
			int weaponsLevel (int nP) const 
							{ assert (nP > 0 && nP <= (int) _weaponsLevel.size ());
							  return (_weaponsLevel [nP - 1]); }
			void setWeaponsLevel (int nP, int wL)
							{ assert (nP > 0 && nP <= (int) _weaponsLevel.size ());
							  _weaponsLevel [nP - 1] = (wL >= 0) ? wL : 0; }
			int mazeScene (int nP) const 
							{ assert (nP > 0 && nP <= (int) _mazeScene.size ());
							  return (_mazeScene [nP - 1]); }
			void setMazeScene (int nP, int mS)
							{ assert (nP > 0 && nP <= (int) _mazeScene.size ());
							  _mazeScene [nP - 1] = (mS >= 0) ? mS : 0; }
			const std::vector <int>& thingsCarried (int nP) const
							{ assert (nP > 0 && nP <= (int) _thingsCarried.size ());
							  return (_thingsCarried [nP -1]); }
			int firstThingCarried (int nP) const
							{ assert (nP > 0 && nP <= (int) _thingsCarried.size ());
							  return (_thingsCarried [nP -1][0]); }
			void removeLastThingCarried (int nP);
			void setThingsCarried (int nP, const std::vector <int>& tC)
							{ assert (nP > 0 && nP <= (int) _thingsCarried.size ());
							  _thingsCarried [nP - 1] = tC; }
			void iterateThingsCarried (int nP);
			int carryThing (int nP, int tC);

			const std::vector <VillanerLocation>& villanersInMaze (int nP) const 
							{ assert (nP > 0 && nP <= (int) _villanersInMaze.size ());
							  return (_villanersInMaze [nP -1]); }
			const std::vector <VillanerLocation> villanersInMaze (int nP, int nR) const;

			const std::vector <std::vector <ThingToCatchLocation>>& thingsInMaze (int nP) const 
							{ assert (nP > 0 && nP <= (int) _thingsInMaze.size ());
							  return (_thingsInMaze [nP - 1]); }
			/** nR is the number of the room in the maze. */
			void leaveThing (int nP, const ThingToCatchLocation& tL);
			void removeThing (int nP, const ThingToCatchLocation& tL);
			void updateThingStatus (int nP, const ThingToCatchLocation& oTL, const ThingToCatchLocation& tL);

			const std::vector <std::vector <MealLocation>>& mealInMaze (int nP) const 
							{ assert (nP > 0 && nP <= (int) _mealInMaze.size ());
							  return (_mealInMaze [nP -1]); }
			void removeMeal (int nP, const MealLocation& mL);

			/** To change the hall of fame. */
			void actualizeHallOfFame ();

			private:
			/** @see parent. */
			virtual void adjustToPlayers (int nP);

			/** @see parent. */
			virtual void cfgToStream (std::ostringstream& oS);
			virtual void cfgFromStream (std::istringstream& iS);

			// Implementation
			void distributeVillanersInTheMaze (int nP);
			void distributeElementsInTheMaze (int nP);
			void distributeMaleInTheMaze (int nP);

			private:
			std::vector <int> _score;
			std::vector <int> _weaponsLevel;
			std::vector <int> _level;
			std::vector <std::string> _playerName;
			std::vector <QGAMES::Position> _lastPositions;
			std::vector <int> _mazeScene;
			std::vector <std::vector <int>> _thingsCarried;
			std::vector <std::vector <VillanerLocation>> _villanersInMaze;
			std::vector <std::vector <std::vector <ThingToCatchLocation>>> _thingsInMaze;
			std::vector <std::vector <std::vector <MealLocation>>> _mealInMaze;

			private:
			static const int _POSITIONSPERSCENETYPE 
				[__GAMETEST_NUMBEROFTYPESOFSCENES__][__GAMETEST_NUMBERPOSSIBLEPOSITIONSINAROOM__];
		};

		Game ()
			: QGAMES::AdvancedArcadeGame ()
							{ }

		// Information about the players
		// nP = -1 will is equivalent to current player...
		/** The name of the player. */
		const std::string& playerName (int nP = -1) const
							{ return (((Game::Conf*) configuration ()) -> 
								playerName ((nP == -1) ? currentPlayer () : nP)); }
		void setPlayerName (const std::string& pN, int nP = -1)
							{ ((Game::Conf*) configuration ()) -> 
								setPlayerName ((nP == -1) ? currentPlayer () : nP, pN); }
		/** The last position in the maze. */
		const QGAMES::Position lastPosition (int nP = -1) const
							{ return (((Game::Conf*) configuration ()) -> 
								lastPosition ((nP == -1) ? currentPlayer () : nP)); }
		void setLastPosition (const QGAMES::Position& p, int nP = -1)
							{ ((Game::Conf*) configuration ()) -> 
								setLastPosition ((nP == -1) ? currentPlayer () : nP, p); }
		/** The manage the score. */
		int score (int nP = -1) const
							{ return (((Game::Conf*) configuration ()) -> 
								score ((nP == -1) ? currentPlayer () : nP)); }
		void setScore (int s, int nP = -1)
							{ ((Game::Conf*) configuration ()) -> 
								setScore ((nP == -1) ? currentPlayer () : nP, s); }
		void addScore (int a, int nP = -1)
							{ setScore (score (nP) + a, nP); }
		/** The level. */
		int level (int nP = -1) const
							{ return (((Game::Conf*) configuration ()) -> 
								level ((nP == -1) ? currentPlayer () : nP)); }
		void setLevel (int l, int nP = -1)
							{ ((Game::Conf*) configuration ()) -> 
								setLevel ((nP == -1) ? currentPlayer () : nP, l); }
		void setNextLevel (int nP = -1)
							{ setLevel (level (nP) + 1, nP); }
		/** The manage the level of weapons managed. */
		int weaponsLevel (int nP = -1) const
							{ return (((Game::Conf*) configuration ()) -> 
								weaponsLevel ((nP == -1) ? currentPlayer () : nP)); }
		void setweaponsLevel (int wL, int nP = -1)
							{ ((Game::Conf*) configuration ()) -> 
								setWeaponsLevel ((nP == -1) ? currentPlayer () : nP, wL); }
		void setNextWeaponsLevel (int nP = -1)
							{ setweaponsLevel (weaponsLevel (nP) + 1, nP); }
		/** The maze scene where the action is taking place. */
		int mazeScene (int nP = -1) const
							{ return (((Game::Conf*) configuration ()) -> 
								mazeScene ((nP == -1) ? currentPlayer () : nP)); }
		void setMazeScene (int l, int nP = -1)
							{ ((Game::Conf*) configuration ()) -> 
								setMazeScene ((nP == -1) ? currentPlayer () : nP, l); }
		/** To manage the things being varried by a player. */
		std::vector <int> thingsCarried (int nP = -1)
							{ return (((Game::Conf*) configuration ()) -> 
								thingsCarried ((nP == -1) ? currentPlayer () : nP)); }
		int firstThingCarried (int nP = -1)
							{ return (((Game::Conf*) configuration ()) -> 
								firstThingCarried ((nP == -1) ? currentPlayer () : nP)); }
		void removeLastThingCarried (int nP = -1)
							{ return (((Game::Conf*) configuration ()) -> 
								removeLastThingCarried ((nP == -1) ? currentPlayer () : nP)); }
		void setThingsCarried (const std::vector <int>& tC,int nP = -1)
							{ ((Game::Conf*) configuration ()) -> 
								setThingsCarried ((nP == -1) ? currentPlayer () : nP, tC); }
		void iterateThingsCarried (int nP = -1)
							{ ((Game::Conf*) configuration ()) -> 
								iterateThingsCarried ((nP == -1) ? currentPlayer () : nP); }
		int carryThing (int t, int nP = -1)
							{ return (((Game::Conf*) configuration ()) -> 
								carryThing ((nP == -1) ? currentPlayer () : nP, t)); }
		void leaveThing (const ThingToCatchLocation& tL, int nP = -1)
							{ return (((Game::Conf*) configuration ()) -> 
								leaveThing ((nP == -1) ? currentPlayer () : nP, tL)); }
		const std::vector <VillanerLocation>& villanersInMaze (int nP = -1) const 
							{ return (((Game::Conf*) configuration ()) -> 
								villanersInMaze ((nP == -1) ? currentPlayer () : nP)); }
		const std::vector <VillanerLocation> villanersInMazeRoom (int nR, int nP = -1) const 
							{ return (((Game::Conf*) configuration ()) -> 
								villanersInMaze ((nP == -1) ? currentPlayer () : nP, nR)); }
		const std::vector <std::vector <ThingToCatchLocation>>& thingsInMaze (int nP = -1) const 
							{ return (((Game::Conf*) configuration ()) -> 
								thingsInMaze ((nP == -1) ? currentPlayer () : nP)); }
		void removeThing (const ThingToCatchLocation& tL, int nP = -1)
							{ ((Game::Conf*) configuration ()) -> removeThing ((nP == -1) ? currentPlayer () : nP, tL); }
		void updateThingStatus (const ThingToCatchLocation& oTL, const ThingToCatchLocation& tL, int nP = -1)
							{ ((Game::Conf*) configuration ()) -> updateThingStatus ((nP == -1) ? currentPlayer () : nP, oTL, tL); }
		/** To manage the meal in the maze. */
		const std::vector <std::vector <MealLocation>>& mealInMaze (int nP = -1) const 
							{ return (((Game::Conf*) configuration ()) -> 
								mealInMaze ((nP == -1) ? currentPlayer () : nP)); }
		void removeMeal (const MealLocation& mL, int nP = -1)
							{ ((Game::Conf*) configuration ()) -> removeMeal ((nP == -1) ? currentPlayer () : nP, mL); }

		/** Actualize the hall of fame, with the last results. */
		void actualizeHallOfFame ()
							{ ((Game::Conf*) configuration ()) -> actualizeHallOfFame (); }

		// The rest of the configuration attributes are not directly accesible

		/** @see parent. */
		virtual void addScoreObjects ();
		virtual void removeScoreObjects ();

		private:
		/** see parent. */
		virtual QGAMES::EntityBuilder* createEntityBuilder ()
							{ return (new EntityBuilder (parameter (__GAME_PROPERTYENTITIESFILE__), 
								formBuilder (), movementBuilder ())); }
		virtual QGAMES::GUISystemBuilder* createGUISystemBuilder ()
							{ return (new GUISystemBuilder (game () -> parameter (__GAME_PROPERTYGUISYSTEMFILE__), 
								game () -> formBuilder ())); }
		virtual QGAMES::MovementBuilder* createMovementBuilder () 
							{ return (new MovementBuilder (parameter (__GAME_PROPERTYMOVEMENTSFILE__))); }
		virtual QGAMES::InputHandler* createInputHandler ()
							{ return (implementation () -> createInputHandler (new InputHandlerBehaviour ())); }
		virtual QGAMES::Screens createScreens ()
							{ QGAMES::Screens result;
								result.insert (QGAMES::Screens::value_type (__QGAMES_MAINSCREEN__, 
									implementation () -> createScreen 
										(std::string ("Medieval Adventure"),
										QGAMES::Position::_cero, __GAMETEST_SCREENWIDTH__, __GAMETEST_SCREENHEIGHT__, 100, 100)));
								return (result); }
		virtual QGAMES::WorldBuilder* createWorldBuilder ()
							{ return (new WorldBuilder (parameter (__GAME_PROPERTYWORLDSFILE__), 
								mapBuilder ())); }
		virtual QGAMES::MapBuilder* createMapBuilder ()
							{ QGAMES::MapBuilder* result = new QGAMES::MapBuilder (parameter (__GAME_MAPSOBJECTSFILE__));
								result -> addAddsOn (new QGAMES::ObjectMapBuilderAddsOn (objectBuilder ()));
								result -> addAddsOn (new QGAMES::PlatformTMXMapBuilder ((QGAMES::Sprite2DBuilder*) formBuilder ()));
								return (result); }
		virtual QGAMES::CharacterControlStepsMonitorBuilder* createCharacterMonitorBuilder ()
							{ return (new QGAMES::CharacterControlStepsMonitorBuilder 
								(parameter (__GAME_PROPERTUCHARACTERMONITORFILE__))); }

		/** @see parent. */
		virtual Configuration* createConfiguration ()
							{ return (new Conf (1)); } // Just one player in this game...

		/** @see parent. */
		virtual void initialize ();
		virtual void additionalTasks ();

		/** @see parent. */
		virtual bool drawGameBeforeState () const
							{ return (false); }
	};
	// --------------------------------------------------------------------------------
}

#endif
  
// End of the file
/*@}*/