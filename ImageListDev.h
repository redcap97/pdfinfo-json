/* This program is based on poppler/utils/ImageOutputDev.h (GPL) */

/*
Copyright (C) 2016 Akira Midorikawa

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef IMAGELISTDEV_H
#define IMAGELISTDEV_H

#include <stdio.h>
#include <list>
#include "goo/gtypes.h"
#include "goo/ImgWriter.h"
#include "OutputDev.h"

class GfxState;

//------------------------------------------------------------------------
// ImageInfo
//------------------------------------------------------------------------

struct ImageInfo {
  int pageNum;
  int imgNum;
  const char *type;
  int width;
  int height;
  const char *colorspace;
  int components;
  const char *colorspace2;
  int components2;
  double xppi;
  double yppi;

  ImageInfo() : pageNum(0), imgNum(0), type(NULL), width(0), height(0),
    colorspace(NULL), components(0), colorspace2(NULL), components2(0), xppi(0), yppi(0) {}
};

typedef std::list<ImageInfo> ImageInfoList;
typedef ImageInfoList::iterator ImageInfoListIterator;

//------------------------------------------------------------------------
// ImageListDev
//------------------------------------------------------------------------

class ImageListDev: public OutputDev {
public:
  enum ImageType {
    imgImage,
    imgStencil,
    imgMask,
    imgSmask
  };
  enum ImageFormat {
    imgRGB,
    imgGray,
    imgMonochrome,
    imgCMYK
  };

  // Constructor
  ImageListDev();

  // Destructor.
  virtual ~ImageListDev();

  // Check if file was successfully created.
  virtual GBool isOk() { return ok; }

  // Does this device use tilingPatternFill()?  If this returns false,
  // tiling pattern fills will be reduced to a series of other drawing
  // operations.
  virtual GBool useTilingPatternFill() { return gTrue; }

  // Does this device use beginType3Char/endType3Char?  Otherwise,
  // text in Type 3 fonts will be drawn with drawChar/drawString.
  virtual GBool interpretType3Chars() { return gFalse; }

  // Does this device need non-text content?
  virtual GBool needNonText() { return gTrue; }

  // Start a page
  virtual void startPage(int pageNumA, GfxState *state, XRef *xref) 
			{ pageNum = pageNumA; }
 
  //---- get info about output device

  // Does this device use upside-down coordinates?
  // (Upside-down means (0,0) is the top left corner of the page.)
  virtual GBool upsideDown() { return gTrue; }

  // Does this device use drawChar() or drawString()?
  virtual GBool useDrawChar() { return gFalse; }

  //----- path painting
  virtual GBool tilingPatternFill(GfxState *state, Gfx *gfx, Catalog *cat, Object *str,
				  double *pmat, int paintType, int tilingType, Dict *resDict,
				  double *mat, double *bbox,
				  int x0, int y0, int x1, int y1,
				  double xStep, double yStep);

  //----- image drawing
  virtual void drawImageMask(GfxState *state, Object *ref, Stream *str,
			     int width, int height, GBool invert,
			     GBool interpolate, GBool inlineImg);
  virtual void drawImage(GfxState *state, Object *ref, Stream *str,
			 int width, int height, GfxImageColorMap *colorMap,
			 GBool interpolate, int *maskColors, GBool inlineImg);
  virtual void drawMaskedImage(GfxState *state, Object *ref, Stream *str,
			       int width, int height,
			       GfxImageColorMap *colorMap,
			       GBool interpolate,
			       Stream *maskStr, int maskWidth, int maskHeight,
			       GBool maskInvert, GBool maskInterpolate);
  virtual void drawSoftMaskedImage(GfxState *state, Object *ref, Stream *str,
				   int width, int height,
				   GfxImageColorMap *colorMap,
				   GBool interpolate,
				   Stream *maskStr,
				   int maskWidth, int maskHeight,
				   GfxImageColorMap *maskColorMap,
				   GBool maskInterpolate);

  ImageInfoListIterator getListBegin() { return list.begin(); }
  ImageInfoListIterator getListEnd() { return list.end(); }

private:
  void listImage(GfxState *state, Object *ref, Stream *str,
		 int width, int height,
		 GfxImageColorMap *colorMap,
		 GBool interpolate, GBool inlineImg,
		 ImageType imageType);
  int pageNum;			// current page number
  int imgNum;			// current image number
  GBool ok;			// set up ok?
  ImageInfoList list;		// list of ImageInfo
};

#endif
