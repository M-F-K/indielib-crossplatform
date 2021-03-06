/*****************************************************************************************
 * Desc: EntityTests_animateRotations class
 *****************************************************************************************/

/*********************************** The zlib License ************************************
 *
 * Copyright (c) 2013 Indielib-crossplatform Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 *
 *****************************************************************************************/

#include "CIndieLib.h"
#include "EntityTests_animateRotations.h"
#include "IND_Animation.h"
#include "IND_Entity2d.h"
#include "IND_Font.h"
#include "IND_Surface.h"
#include "IndiePlatforms.h"

#if defined(PLATFORM_LINUX)
#include <cstring>  // INFO : since this test uses strcat and strcopy we need this on linux ( basic tutorial 08, uses another solution). DartMike, if this ok then remove this comment. 
#endif


void EntityTests_animateRotations::prepareTests() {
	// ----- Surface loading -----
    CIndieLib* iLib = CIndieLib::instance();

	// Loading Rocket
	if (!iLib->_surfaceManager->add(_surfaces[0], const_cast<char *>("rocket.png"), IND_ALPHA, IND_32)) return;

	// Loading Beetleship
	if (!iLib->_surfaceManager->add(_surfaces[1], const_cast<char *>("beetleship.png"), IND_ALPHA, IND_32)) return;
	
	// Loading star
	if (!iLib->_surfaceManager->add(_surfaces[2], const_cast<char *>("star.png"), IND_ALPHA, IND_32)) return;

	// Sword Master animation, we apply a color key of (0, 255, 0)
	if (!iLib->_animationManager->addToSurface(_animations[0], const_cast<char *>("animations/sword_master.xml"), IND_ALPHA, IND_16, 0, 255, 0)) return;

	// ----- Font loading -----

	// Font
	if (!iLib->_fontManager->add(_fonts[0], const_cast<char *>("font_small.png"), const_cast<char *>("font_small.xml"), IND_ALPHA, IND_32)) return;

	// Rocket
	_entities[0]->setSurface(_surfaces[0]);
	_entities[0]->setHotSpot(0.5f, 0.5f);
	_entities[0]->setScale(2.0f,2.0f);
	_entities[0]->setPosition(200, 450, 1);

	// Beetle
	_entities[1]->setSurface(_surfaces[1]);
	_entities[1]->setHotSpot(0.5f, 0.5f);
	_entities[1]->setMirrorX(1);
	_entities[1]->setPosition(300,250,1);

	// Sword Master Animation
	_entities[2]->setAnimation(_animations[0]);
	_entities[2]->setHotSpot(0.5f, 0.5f);
	_entities[2]->setPosition(500, 220, 3);

	//Star
	_entities[3]->setSurface(_surfaces[2]);
	_entities[3]->setHotSpot(0.5f, 0.5f);
	_entities[3]->setPosition(350, 50, 1);
}


void EntityTests_animateRotations::performTests(float dt) {
	//IF - Check if test is active
    if(!_active)
        return;

	CIndieLib *iLib = CIndieLib::instance();
	
	//Toggling of entity border lines in entities
	if(iLib->_input->onKeyPress(IND_F1) && _active) {
		for (int i = 0; i < _testedEntities; ++i) {
			_entities[i]->showGridAreas(!_entities[i]->isShowGridAreas());
			_entities[i]->showCollisionAreas(!_entities[i]->isShowCollisionAreas());
		}
	}

	// ----- Input ----

	float mDelta = iLib->_render->getFrameTime() / 1000.0f;

	_animationValue += _animationSpeed * mDelta;
	
	if (_animationValue < 0){
		_animationValue = 0;
	}

	// ----- Updating entities attributes  -----

	_entities[0]->setAngleXYZ (0, 0, _animationValue*2);
	_entities[1]->setAngleXYZ (0, 0, _animationValue);	
	_entities[2]->setAngleXYZ (0, 0, _animationValue*20);
}

bool EntityTests_animateRotations::isActive(){
    return (ManualTests::isActive());
}
    
void EntityTests_animateRotations::setActive(bool active){
    ManualTests::setActive(active);

    CIndieLib *iLib = CIndieLib::instance();
    //IF - Active
    if(active){
		for (int i = 0; i < _testedEntities; ++i) {
			iLib->_entity2dManager->add(_entities[i]);
		}

    } else { //Inactive
        //Release all variables from indieLib before exiting
		for (int i = 0; i < _testedEntities; ++i) {
			iLib->_entity2dManager->remove(_entities[i]);
		}
    }
}

//-----------------------------------PRIVATE METHODS----------------------------

void EntityTests_animateRotations::init() {
	_testedEntities = 4;
	_animations = new IND_Animation*[_testedEntities];
	_entities = new IND_Entity2d*[_testedEntities];
	_fonts = new IND_Font*[_testedEntities];
	_surfaces = new IND_Surface*[_testedEntities];

	//Create underlying entities
	for (int i = 0; i < _testedEntities; ++i) {
		_animations[i] = IND_Animation::newAnimation();
		_entities[i] = IND_Entity2d::newEntity2d();
		_fonts[i] = IND_Font::newFont();
		_surfaces[i] = IND_Surface::newSurface();
	}
}

void EntityTests_animateRotations::release() {
    CIndieLib* iLib = CIndieLib::instance();
    //Release all variables from indieLib before exiting
	for (int i = 0; i < _testedEntities; ++i) {
		iLib->_animationManager->remove(_animations[i]);
		iLib->_entity2dManager->remove(_entities[i]);
		iLib->_fontManager->remove(_fonts[i]);
		iLib->_surfaceManager->remove(_surfaces[i]);
	}

	DISPOSEARRAY(_animations);
	DISPOSEARRAY(_entities);
	DISPOSEARRAY(_fonts);
	DISPOSEARRAY(_surfaces);
}
