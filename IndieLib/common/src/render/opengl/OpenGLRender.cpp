/*****************************************************************************************
 * File: OpenGLRender.cpp
 * Desc: Initialization / Destruction using OpenGL
 *****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier L�pez L�pez (info@pixelartgames.com)
THIS FILE IS AN ADDITIONAL FILE ADDED BY Miguel Angel Qui�ones (2011) (mail:m.quinones.garcia@gmail.com / mikeskywalker007@gmail.com), BUT HAS THE
SAME LICENSE AS THE WHOLE LIBRARY TO RESPECT ORIGINAL AUTHOR OF LIBRARY

This library is free software; you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place,
Suite 330, Boston, MA 02111-1307 USA
*/
#include "Defines.h"

#ifdef INDIERENDER_OPENGL

// ----- Includes -----

#include "Global.h"
#include "Tools.h"
#include "OpenGLRender.h"
#include "IND_Window.h"
#include "IND_FontManager.h"
#include "IND_SurfaceManager.h"
#include "IND_AnimationManager.h"
#include "IND_FontManager.h"
#include "IND_Timer.h"
#include "IND_Font.h"
#include "IND_Animation.h"
#include "IND_Camera2d.h"

#if defined (PLATFORM_WIN32) || defined(PLATFORM_LINUX) || defined (PLATFORM_OSX)
#include "platform/OSOpenGLManager.h"
#endif

//Constants
#define MINIMUM_OPENGL_VERSION_STRING "GL_VERSION_1_5"  //The minimum GL version supported by this renderer  

// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pWindow                 Pointer to an object initializing IND_Window

\b Operation:

This function returns 1 (true) if the render is initialized sucessfully,
0 (false) otherwise.
*/
IND_Window* OpenGLRender::initRenderAndWindow(IND_WindowProperties& props) {
	if(props._bpp <= 0 || props._height <= 0 || props._width <= 0) {
		g_debug->header("Error initializing window: Invalid parameters provided", 2);
		return 0;
	}
	end();
	initVars();

	// Window creation
	_window = new IND_Window();
	if (!_window) {
		freeVars();
		return NULL;
	}
	
	if(!_osOpenGLMgr) {
		_osOpenGLMgr = new OSOpenGLManager(_window);  
	}

	//TODO: Give option to configure double buffer, depth and stencil buffer bits
	_doubleBuffer = true;  //May be configured in future
	
	//Initialize OpenGL parameters for SDL before creating the window for OpenGL
	_osOpenGLMgr->setOpenGLContextParams(IND_RGBA, //Color format
										 props._bpp/4, //Color depth (Bpp) /  num colors
	                                     8, //Depth Buffer bits
	                                     1,  //Stencil Buffer bits
	                                     _doubleBuffer //Double buffering
	                                    );

	if(!_window->create(props)) {
		/*TODO: recreate window with different params?
		_osOpenGLMgr->setOpenGLContextParams(IND_RGBA, //Color format
										 props._bpp/4, //Color depth (Bpp) /  num colors
	                                     8, //Depth Buffer bits
	                                     1,  //Stencil Buffer bits
	                                     _doubleBuffer //Double buffering
	                                    );*/
		freeVars();
		return NULL;
	}

	g_debug->header("Creating OpenGL Render", 5);
	_ok = initializeOpenGLRender();
	if (!_ok) {
		g_debug->header("Finalizing OpenGL", 5);
		freeVars();
		g_debug->header("OpenGL finalized", 6);
		return false;
	}

	g_debug->header("Checking created OpenGL context pixel format", 5);
	if(!_osOpenGLMgr->checkOpenGLSDLContextProps()) {
		g_debug->header("Different GL context pixel format used", 6);
	} else {
		g_debug->header("Same GL context pixel format used", 6);
	}

	writeInfo();

	g_debug->header("OpenGL Render Created", 6);
	return _window;
}

/*!
\b Parameters:

\arg \b IND_WindowProperties new properties of window after reset

\b Operation:

This function returns 1 (true) if the application window resets to the attributes passed as parameters. This method
is useful when you want to change the application window on runtime, 0 (false) if it is not possible to create
the new window.
*/
bool OpenGLRender::reset(IND_WindowProperties& props) {
	if(props._bpp <= 0 || props._height <= 0 || props._width <= 0) {
		g_debug->header("Error resetting window: Invalid parameters provided", 2);
		return 0;
	}

	if (!_window->reset(props)) {
		g_debug->header("Error resetting SDL window", 2);
		return 0;
	}

	return resetViewport();
}


/*!
\b Operation:

This function returns 1 (true) if the application window toggles to fullscreen or windowed, 0 (false) if it is not possible
to create the new window.
*/
bool OpenGLRender::toggleFullScreen() {

	g_debug->header("Changing To/From Full Screen", 5);

	if (!_window->toggleFullScreen()) return false;

	return true;
}

void OpenGLRender::beginScene() {

	if (!_ok)
		return;

	//TODO // ----- Recovering device (after ALT+TAB) -----

	//Clear buffers
	glClearColor(0, 0, 0, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}


/*!
\b Operation:

Finish the scene. This function must be called after drawing all the graphical objects.
*/
void OpenGLRender::endScene() {
	if (!_ok)
		return;

	//Swap memory buffers (OS-dependant)
	_osOpenGLMgr->presentBuffer();

#ifdef _DEBUG
    GLenum glerror = glGetError();
	if (glerror) {
        g_debug->header("OpenGLRenderer::endScene() OpenGL error flag!", 2);
		printf("OpenGLRenderer::endScene() Error at end of scene! :%i\n",glerror);
	}
#endif	
}


/*!
\b Operation:

This function shows the fps (frames per second) as the title of the window.

NOTE: The updating of the window title is quite time-consuming, so this is not the correct method for
checking the FPS. It's better to use the methods IND_Render::getFpsInt() or IND_Render::getFpsString() and drawing
the result using an ::IND_Font object.
*/
void OpenGLRender::showFpsInWindowTitle(char *pFPSString) {
	if (!_ok)   return;

	_window->setTitle(pFPSString);
}

void OpenGLRender::getNumrenderedObjectsString(char *pBuffer)      {
	Tools::itoa(_numrenderedObjects, pBuffer);
}

void OpenGLRender::getNumDiscardedObjectsString(char *pBuffer)      {
	Tools::itoa(_numDiscardedObjects, pBuffer);
}
/*!
\b Operation:

This function frees the manager and all the objects that it contains.
*/
void OpenGLRender::end() {
	if (_ok) {
		g_debug->header("Finalizing OpenGL", 5);
		_osOpenGLMgr->endOpenGLContext();
		freeVars();
		g_debug->header("OpenGL finalized ", 6);
		_ok = false;
	}
}

// --------------------------------------------------------------------------------
//							        Private methods
// --------------------------------------------------------------------------------

/*
==================
Init vars
==================
*/
void OpenGLRender::initVars() {
	_numrenderedObjects = 0;
	_numDiscardedObjects = 0;
	_window = NULL;
	_math.init();
	_osOpenGLMgr = NULL;

}

/*
==================
Init OpenGL
==================
*/
bool OpenGLRender::initializeOpenGLRender() {

	//Check dependency of window initialization
	if (!_window->isOK()) {
		// Window error
		g_debug->header("This operation can not be done:", 3);
		g_debug->dataChar("", 1);
		g_debug->header("Invalid Id or IND_Window not correctly initialized.", 2);

		return false;
	}

	//OpenGL context creation
	_osOpenGLMgr->createOpenGLSDLContext();

	//Check for GL extensions
	if (!checkGLExtensions())
		return false;

	//Get all graphics device information
	getInfo();

	// ViewPort initialization
	return resetViewport();
}

/*
==================
Check OpenGL extensions
==================
*/
bool OpenGLRender::checkGLExtensions() {

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		g_debug->header("Extensions loading (GLEW) failed!", 2);
		return false;
	}

	//Check system support for minimum targeted version of library
	if (!glewIsSupported(MINIMUM_OPENGL_VERSION_STRING)) {
		g_debug->header("Minimum OPENGL version is not available!", 2);
		return false;
	}

	strcpy(_info._version, MINIMUM_OPENGL_VERSION_STRING);

	//TODO: Other extensions

	return true;
}

/*
==================
Free memory
==================
*/
void OpenGLRender::freeVars() {
	DISPOSE(_osOpenGLMgr);
	DISPOSE(_window);
	_ok = false;
}

/*
==================
Hardware information
==================
*/
void OpenGLRender::getInfo() {

    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE,&maxTextureSize);
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &_info._textureUnits);
    _info._maxTextureSize = maxTextureSize;

}

/*
==================
Write hardware information to debug log
==================
*/
void OpenGLRender::writeInfo() {
	g_debug->header("Hardware information" , 5);

	// ----- D3D version -----

	g_debug->header("OpenGL version:" , 3);
	g_debug->dataChar(_info._version, 1);

	// ----- Vendor -----

	g_debug->header("Mark:" , 3);
	g_debug->dataChar(_info._vendor, 1);

	// ----- Renderer -----

	g_debug->header("Chip:" , 3);
	g_debug->dataChar(_info._renderer, 1);

	// ----- Antialiasing -----

	g_debug->header("Primitive antialising:", 3);
	if (_info._antialiasing)
		g_debug->dataChar("Yes", 1);
	else
		g_debug->dataChar("No", 1);

	// ----- Max texture size -----

	g_debug->header("Maximum texture size:" , 3);
	g_debug->dataInt(_info._maxTextureSize, 0);
	g_debug->dataChar("x", 0);
	g_debug->dataInt(_info._maxTextureSize, 1);
	g_debug->header("Texture units:" , 3);
	g_debug->dataInt(_info._textureUnits, 0);


	// ----- Vertex Shader version  -----

	/*g_debug->Header ("Vertex Shader:" , 3);
	g_debug->DataInt (D3DSHADER_VERSION_MAJOR (_info.mVertexShaderVersion), 0);
	g_debug->DataChar (".", 0);
	g_debug->DataInt (D3DSHADER_VERSION_MINOR (_info.mVertexShaderVersion), 0);

	if (_info.mSoftwareVertexProcessing)
	    g_debug->DataChar ("(Software)", 1);
	else
	    g_debug->DataChar ("", 1);*/

	// ----- Pixel Shader version -----

	/*g_debug->Header ("Pixel Shader:" , 3);
	g_debug->DataInt (D3DSHADER_VERSION_MAJOR (_info.mPixelShaderVersion), 0);
	g_debug->DataChar (".", 0);
	g_debug->DataInt (D3DSHADER_VERSION_MINOR (_info.mPixelShaderVersion), 1);*/

	g_debug->header("Hardware Ok" , 6);
}

/*
==================
Resets the viewport to all window width/height
==================
*/
bool OpenGLRender::resetViewport() {
	int defaultwidth(_window->getWidth());
	int defaultheight(_window->getHeight());
	_info._fbWidth = defaultwidth;
	_info._fbHeight = defaultheight;

	if (!setViewPort2d(0, 0, defaultwidth, defaultheight))
		return false;

	IND_Camera2d mCamera2d(static_cast<float>(_window->getWidth() / 2), 
						   static_cast<float>(_window->getHeight() / 2));   //Default 2D camera in center of viewport
	setCamera2d(&mCamera2d);
	clearViewPort(0, 0, 0);

	return true;
}

#endif //INDIERENDER_OPENGL
