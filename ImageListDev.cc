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
  const char *type;
  const char *colorspace;
  int components, bpc;

  ImageInfo info;
  info.pageNum = pageNum;
  info.imgNum = imgNum;

  type = NULL;
  switch (imageType) {
  case imgImage:
    type = "Image";
    break;
  case imgStencil:
    type = "Stencil";
    break;
  case imgMask:
    type = "Mask";
    break;
  case imgSmask:
    type = "Smask";
    break;
  }
  info.type = type;
  info.width = width;
  info.height = height;

  colorspace = NULL;
  /* masks and stencils default to ncomps = 1 and bpc = 1 */
  components = 1;
  bpc = 1;
  if (colorMap && colorMap->isOk()) {
    switch (colorMap->getColorSpace()->getMode()) {
      case csDeviceGray:
      case csCalGray:
        colorspace = "Gray";
        break;
      case csDeviceRGB:
      case csCalRGB:
        colorspace = "RGB";
        break;
      case csDeviceCMYK:
        colorspace = "CMYK";
        break;
      case csLab:
        colorspace = "Lab";
        break;
      case csICCBased:
        colorspace = "ICC-Based";
        break;
      case csIndexed:
        colorspace = "Indexed";
        break;
      case csSeparation:
        colorspace = "Separation";
        break;
      case csDeviceN:
        colorspace = "DeviceN";
        break;
      case csPattern:
        colorspace = "Pattern";
        break;
      default:
        colorspace = NULL;
        break;
    }
    components = colorMap->getNumPixelComps();
    bpc = colorMap->getBits();
  }
  info.colorspace = colorspace;
  info.components = components;
  info.bpc = bpc;

  double *mat = state->getCTM();
  double width2 = mat[0] + mat[2];
  double height2 = mat[1] + mat[3];
  double xppi = fabs(width*72.0/width2);
  double yppi = fabs(height*72.0/height2);
  info.xppi = xppi;
  info.yppi = yppi;

  list.push_back(info);
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
