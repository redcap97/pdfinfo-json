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
  const char *enc;
  int components, bpc;

  ImageInfo info;
  info.pageNum = pageNum;
  info.imgNum = imgNum;

  type = NULL;
  switch (imageType) {
  case imgImage:
    type = "image";
    break;
  case imgStencil:
    type = "stencil";
    break;
  case imgMask:
    type = "mask";
    break;
  case imgSmask:
    type = "smask";
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
        colorspace = "gray";
        break;
      case csDeviceRGB:
      case csCalRGB:
        colorspace = "rgb";
        break;
      case csDeviceCMYK:
        colorspace = "cmyk";
        break;
      case csLab:
        colorspace = "lab";
        break;
      case csICCBased:
        colorspace = "icc";
        break;
      case csIndexed:
        colorspace = "index";
        break;
      case csSeparation:
        colorspace = "sep";
        break;
      case csDeviceN:
        colorspace = "devn";
        break;
      case csPattern:
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

  switch (str->getKind()) {
  case strCCITTFax:
    enc = "ccitt";
    break;
  case strDCT:
    enc = "jpeg";
    break;
  case strJPX:
    enc = "jpx";
    break;
  case strJBIG2:
    enc = "jbig2";
    break;
  case strFile:
  case strFlate:
  case strCachedFile:
  case strASCIIHex:
  case strASCII85:
  case strLZW:
  case strRunLength:
  case strWeird:
  default:
    enc = "image";
    break;
  }
  info.enc = enc;

  double *mat = state->getCTM();
  double width2 = mat[0] + mat[2];
  double height2 = mat[1] + mat[3];
  double xppi = fabs(width*72.0/width2) + 0.5;
  double yppi = fabs(height*72.0/height2) + 0.5;
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
