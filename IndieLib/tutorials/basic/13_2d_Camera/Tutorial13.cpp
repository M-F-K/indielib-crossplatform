/*****************************************************************************************
 * Desc: Tutorials a) 13 2d Camera
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
#include "IND_Surface.h"
#include "IND_Font.h"
#include "IND_Entity2d.h"
#include "IND_Camera2d.h"
#include "../../WorkingPath.h"
#include <cstring>

static const float K_ZOOMSPEED = 0.001f;

/*
==================
Main
==================
*/
Indielib_Main			
{
    //Sets the working path at the resources directory. Resources paths are relative to that directory
	if (!WorkingPathSetup::setWorkingPath(WorkingPathSetup::resourcesDirectory())) {
		std::cout<<"\nUnable to Set the working path !";
	}
	
	// ----- IndieLib intialization -----

	CIndieLib *mI = CIndieLib::instance();
	if (!mI->init()) return 0;			

	// ----- Surface loading -----

	// Loading tile for the terrain
	IND_Surface *mSurfaceBack = IND_Surface::newSurface();
	if (!mI->_surfaceManager->add(mSurfaceBack, "twist.jpg", IND_OPAQUE, IND_32)) return 0;
	
	// Loading beetle
	IND_Surface *mSurfaceBeetle = IND_Surface::newSurface();
	if (!mI->_surfaceManager->add(mSurfaceBeetle, "beetleship.png", IND_ALPHA, IND_32)) return 0;
	
	// Loading octopus
	IND_Surface *mSurfaceOctopus = IND_Surface::newSurface();
	if (!mI->_surfaceManager->add(mSurfaceOctopus, "octopus.png", IND_ALPHA, IND_32)) return 0;

	// Loading bug
	IND_Surface *mSurfaceBug = IND_Surface::newSurface();
	if (!mI->_surfaceManager->add(mSurfaceBug, "Enemy Bug.png", IND_ALPHA, IND_32)) return 0;

	// Font
	IND_Font *mFontSmall = IND_Font::newFont();
	if (!mI->_fontManager->add(mFontSmall, "font_small.png", "font_small.xml", IND_ALPHA, IND_32)) return 0;

	// ----- Font creation -----

	IND_Entity2d *mTextSmallWhite = IND_Entity2d::newEntity2d();					
	mI->_entity2dManager->add(mTextSmallWhite);			// Entity adding
	mTextSmallWhite->setFont(mFontSmall);				// Set the font into the entity
	mTextSmallWhite->setLineSpacing(18);
	mTextSmallWhite->setCharSpacing(-8);
	mTextSmallWhite->setPosition(5, 5, 1);
	mTextSmallWhite->setAlign(IND_LEFT);

	// ----- Entities -----

	// Terrain
	IND_Entity2d *mTerrain = IND_Entity2d::newEntity2d();
	mI->_entity2dManager->add(mTerrain);
	mTerrain->setSurface(mSurfaceBack);

	// Beetle
	IND_Entity2d *mBeetle = IND_Entity2d::newEntity2d();
	mI->_entity2dManager->add(mBeetle);
	mBeetle->setSurface(mSurfaceBeetle);
	mBeetle->setHotSpot(0.5f, 0.5f);
	mBeetle->setPosition(150, 300, 2);

	// Octopus
	IND_Entity2d *mOctopus = IND_Entity2d::newEntity2d();
	mI->_entity2dManager->add(mOctopus);
	mOctopus->setSurface(mSurfaceOctopus);
	mOctopus->setHotSpot(0.5f, 0.5f);
	mOctopus->setPosition(450, 300, 2);

	// Bug
	IND_Entity2d *mBug = IND_Entity2d::newEntity2d();
	mI->_entity2dManager->add(mBug);
	mBug->setSurface(mSurfaceBug);
	mBug->setHotSpot(0.5f, 0.5f);
	mBug->setPosition(700, 300, 2);

	// ----- Camera -----

	float mCameraX = (float) mI->_window->getWidth () / 2;
	float mCameraY = (float) mI->_window->getHeight () / 2;
	IND_Camera2d *mCamera = new IND_Camera2d(mCameraX, mCameraY);

	// ----- Main Loop -----

	float mZoom = 1.0f;
	float mCameraAngle = 0;
	float mBugAngle = 0;
	char mText [2048];
	mText [0] = 0;
	int mSpeedMoveCamera = 200;
	int mSpeedRotation = 50;
	float mDelta;

	while (!mI->_input->onKeyPress(IND_ESCAPE) && !mI->_input->quit())
	{
		// ----- Input update ----

		mI->_input->update();

		// ----- Text -----

		strcpy (mText, "Use WASD keys for translating the camera\n");
		strcat (mText, "Use mouse wheel for zooming in/out\n");
		strcat (mText, "Use mouse buttons for rotating the camera");
		mTextSmallWhite->setText(mText);	

		// ----- Input ----

		mDelta = mI->_render->getFrameTime() / 1000.0f;

		// Camera translation
		if (mI->_input->isKeyPressed(IND_W)){
			mCameraY -= mSpeedMoveCamera * mDelta;
		}
		if (mI->_input->isKeyPressed(IND_S)){
			mCameraY += mSpeedMoveCamera * mDelta;
		}
		if (mI->_input->isKeyPressed(IND_A)){
			mCameraX -= mSpeedMoveCamera * mDelta;
		}
		if (mI->_input->isKeyPressed(IND_D)){
			mCameraX += mSpeedMoveCamera * mDelta;
		}

		// Camera Zoom in / out
		if (mI->_input->isMouseScroll()) {
			mZoom += mI->_input->getMouseScrollY() * K_ZOOMSPEED;
		}

		// Camera angle
		if (mI->_input->isMouseButtonPressed(IND_MBUTTON_LEFT)){
			mCameraAngle += mSpeedRotation * mDelta;
		}
		if (mI->_input->isMouseButtonPressed(IND_MBUTTON_RIGHT)){
			mCameraAngle -= mSpeedRotation * mDelta;
		}

		// ----- Updating entities attributes  -----

		// Rotation of the beetle and bug
		mBugAngle += mSpeedRotation * mDelta;
		mBeetle->setAngleXYZ(0, 0, mBugAngle);
		mBeetle->setAngleXYZ(0, 0, mBugAngle);
		mBug->setAngleXYZ(0, 0, -mBugAngle);
		mBug->setAngleXYZ(0, 0, -mBugAngle);

		// Tranlasting, zooming and rotating the camera
		if (mZoom < 0.1f){
			mZoom =  0.1f;
		}
		mCamera->setPosition(mCameraX, mCameraY);
		mCamera->setAngle(mCameraAngle);
		mCamera->setZoom(mZoom);

		// ----- Render  -----

		mI->_render->beginScene();
		mI->_render->clearViewPort(60, 60, 60);
		mI->_render->setViewPort2d(0, 0, mI->_window->getWidth(), mI->_window->getHeight());
		mI->_render->setCamera2d(mCamera);
		mI->_entity2dManager->renderEntities2d();
		mI->_render->endScene();
	}

	// ----- Free -----

	mI->end();

	return 0;
}
