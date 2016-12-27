/* This program is based on poppler/utils/ImageOutputDev.cc (GPL) */

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

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <math.h>
#include "goo/gmem.h"
#include "GfxState.h"
#include "Object.h"
#include "Stream.h"
#include "JBIG2Stream.h"
#include "ImageListDev.h"

namespace {
  const char *getColorSpaceName(GfxColorSpace *colorSpace) {
    switch (colorSpace->getMode()) {
      case csDeviceGray:
      case csCalGray:
        return "Gray";
      case csDeviceRGB:
      case csCalRGB:
        return "RGB";
      case csDeviceCMYK:
        return "CMYK";
      case csLab:
        return "Lab";
      case csICCBased:
        return "ICC-Based";
      case csIndexed:
        return "Indexed";
      case csSeparation:
        return "Separation";
      case csDeviceN:
        return "DeviceN";
      case csPattern:
        return "Pattern";
      default:
        return NULL;
    }
  }

  const char *getImageTypeName(ImageListDev::ImageType imageType) {
    switch (imageType) {
    case ImageListDev::imgImage:
      return "Image";
    case ImageListDev::imgStencil:
      return "Stencil";
    case ImageListDev::imgMask:
      return "Mask";
    case ImageListDev::imgSmask:
      return "Smask";
    default:
      return NULL;
    }
  }
}

ImageListDev::ImageListDev() {
  imgNum = 0;
  pageNum = 0;
  ok = gTrue;
}

ImageListDev::~ImageListDev() {}

void ImageListDev::listImage(GfxState *state, Object *ref, Stream *str,
			       int width, int height,
			       GfxImageColorMap *colorMap,
			       GBool interpolate, GBool inlineImg,
			       ImageType imageType) {
  ImageInfo data;

  data.pageNum = pageNum;
  data.imgNum = imgNum;

  data.type = getImageTypeName(imageType);
  data.width = width;
  data.height = height;

  if (colorMap && colorMap->isOk()) {
    GfxColorSpace *colorSpace = colorMap->getColorSpace();

    data.colorspace = getColorSpaceName(colorSpace);
    data.components = colorSpace->getNComps();
    data.has_colorspace = true;

    if (colorSpace->getMode() == csIndexed) {
      GfxColorSpace *colorSpace2 = ((GfxIndexedColorSpace*)colorSpace)->getBase();

      data.colorspace2 = getColorSpaceName(colorSpace2);
      data.components2 = colorSpace2->getNComps();
      data.has_colorspace2 = true;
    }
  }

  double *mat = state->getCTM();

  double width2 = mat[0] + mat[2];
  double height2 = mat[1] + mat[3];

  data.xppi = fabs(width*72.0/width2);
  data.yppi = fabs(height*72.0/height2);

  list.push_back(data);
  ++imgNum;
}

GBool ImageListDev::tilingPatternFill(GfxState *state, Gfx *gfx, Catalog *cat, Object *str,
				  double *pmat, int paintType, int tilingType, Dict *resDict,
				  double *mat, double *bbox,
				  int x0, int y0, int x1, int y1,
				  double xStep, double yStep) {
  return gTrue;
  // do nothing -- this avoids the potentially slow loop in Gfx.cc
}

void ImageListDev::drawImageMask(GfxState *state, Object *ref, Stream *str,
				   int width, int height, GBool invert,
				   GBool interpolate, GBool inlineImg) {
  listImage(state, ref, str, width, height, NULL, interpolate, inlineImg, imgStencil);
}

void ImageListDev::drawImage(GfxState *state, Object *ref, Stream *str,
			       int width, int height,
			       GfxImageColorMap *colorMap,
			       GBool interpolate, int *maskColors, GBool inlineImg) {
  listImage(state, ref, str, width, height, colorMap, interpolate, inlineImg, imgImage);
}

void ImageListDev::drawMaskedImage(
  GfxState *state, Object *ref, Stream *str,
  int width, int height, GfxImageColorMap *colorMap, GBool interpolate,
  Stream *maskStr, int maskWidth, int maskHeight, GBool maskInvert, GBool maskInterpolate) {
  listImage(state, ref, str, width, height, colorMap, interpolate, gFalse, imgImage);
  listImage(state, ref, str, maskWidth, maskHeight, NULL, maskInterpolate, gFalse, imgMask);
}

void ImageListDev::drawSoftMaskedImage(
  GfxState *state, Object *ref, Stream *str,
  int width, int height, GfxImageColorMap *colorMap, GBool interpolate,
  Stream *maskStr, int maskWidth, int maskHeight,
  GfxImageColorMap *maskColorMap, GBool maskInterpolate) {
  listImage(state, ref, str, width, height, colorMap, interpolate, gFalse, imgImage);
  listImage(state, ref, maskStr, maskWidth, maskHeight, maskColorMap, maskInterpolate, gFalse, imgSmask);
}
