/*****************************************************************************************
 * File: IND_Frame.h
 * Desc: Frame object
 *****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier L�pez L�pez (info@pixelartgames.com)
MODIFIED BY Miguel Angel Qui�ones (2011) (mail:m.quinones.garcia@gmail.com / mikeskywalker007@gmail.com)

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

#ifndef _IND_FRAME_
#define _IND_FRAME_

#include <list>
#include "IND_Image.h"
#include "IND_Surface.h"
#include "Defines.h"

// --------------------------------------------------------------------------------
//									 IND_Frame
// --------------------------------------------------------------------------------

class IND_Frame {
private:

	// ----- Structures ------

	// Frame
	struct structFrame {
		char *_name;
		IND_Image *_image;
		IND_Surface *_surface;
		int _offsetX;
		int _offsetY;
		list <BOUNDING_COLLISION *> *_listBoundingCollision;
		structFrame() {
			_name       = 0;
			_image      = 0;
			_surface    = 0;
			_offsetX = _offsetY = 0;
		}
	};
	typedef struct structFrame A_FRAME;

	A_FRAME _frame;

	// ----- Private sets ------

	void setName(char *pName)       {
		_frame._name = pName;
	}
	void setImage(IND_Image *pIm)    {
		_frame._image = pIm;
	}
	void setSurface(IND_Surface *pSu)  {
		_frame._surface = pSu;
	}
	void SetOffsetX(int pOffsetX)      {
		_frame._offsetX = pOffsetX;
	}
	void SetOffsetY(int pOffsetY)      {
		_frame._offsetY = pOffsetY;
	}

	// ----- Private gets ------

	char        *getName()  {
		return _frame._name;
	}
	IND_Image   *getImage()  {
		return _frame._image;
	}
	IND_Surface *getSurface()  {
		return _frame._surface;
	}
	int         GetOffsetX()  {
		return _frame._offsetX;
	}
	int         GetOffsetY()  {
		return _frame._offsetY;
	}
	list <BOUNDING_COLLISION *> *GetListBoundingCollision() {
		return _frame._listBoundingCollision;
	}

	// ----- Friends -----

	friend class IND_Animation;
	friend class IND_AnimationManager;
	friend class IND_Entity2dManager;
};

#endif // _IND_SEQUENCE_
