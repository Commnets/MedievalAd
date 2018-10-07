/**	
 *	@file	
 *	File: main.cpp \n
 *	Framework: Commty Game Library (CGL) \n
 *	Author: Ignacio Cea Forniés (Community Networks) \n
 *	Creation Date: 08/06/2018 \n
 *	Description: Main programm for testing Platform Games
 *	Versions: 1.0 Initial
 */

#include "stdafx.h"

#include "TestPlatformGame.hpp"

using namespace TestPlatformGame;

#ifndef _CONSOLE
#include <SDL.h>
#ifdef __cplusplus
#define C_LINKAGE "C"
#else
#define C_LINKAGE
#endif /* __cplusplus */
extern C_LINKAGE int main (int argc, char *argv [])
#else
int _tmain (int argc, char *argv [])
#endif /* _CONSOLE */
{
#ifdef NDEBUG
	// In the release version, the resources are in a zip file
	// This instruction reads them and store in temporaly files when needed!
	// The resource reader is deleted at the end, and all the temporal file with them!
	QGAMES::InZipResourceReader rR 
		(std::string (".") + __PATH_SEPARATOR__ + std::string ("temp"));
#endif

	Game game;
	game.setLinkDrawToFrameRate (true);
	/** Forms are not preloaded. */
	game.exec ();

	return (0);
}
