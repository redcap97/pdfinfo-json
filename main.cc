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

#include <cstdio>
#include <cstdarg>
#include <cassert>

#include <PDFDoc.h>
#include <PDFDocFactory.h>
#include <Annot.h>
#include <FontInfo.h>
#include <GlobalParams.h>
#include <Error.h>

#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include "JSInfo.h"
#include "ImageListDev.h"
#include "parseargs.h"

extern "C" {
#include "is_utf8.h"
}

#define PACKAGE_VERSION "0.2.1"

using rapidjson::PrettyWriter;
using rapidjson::StringBuffer;

namespace {
  char ownerPassword[33] = "\001";
  GBool ignoreEncodingError = gFalse;
  GBool printVersion = gFalse;
  GBool printHelp = gFalse;

  const ArgDesc argDesc[] = {
    {"--owner-password",        argString, ownerPassword,        sizeof(ownerPassword), "owner password (for encrypted files)"},
    {"--ignore-encoding-error", argFlag,   &ignoreEncodingError, 0,                     "ignore encoding errror"},
    {"-v",                      argFlag,   &printVersion,        0,                     "print copyright and version info"},
    {"--version",               argFlag,   &printVersion,        0,                     "print copyright and version info"},
    {"-h",                      argFlag,   &printHelp,           0,                     "print usage information"},
    {"--help",                  argFlag,   &printHelp,           0,                     "print usage information"},
    {NULL}
  };

  const char *fontTypeNames[] = {
    "unknown",
    "Type 1",
    "Type 1C",
    "Type 1C (OT)",
    "Type 3",
    "TrueType",
    "TrueType (OT)",
    "CID Type 0",
    "CID Type 0C",
    "CID Type 0C (OT)",
    "CID TrueType",
    "CID TrueType (OT)"
  };

  const size_t fontTypeSize = (sizeof(fontTypeNames) / sizeof(const char *));

  const char *errorMessages[] = {
    "No error",
    "Couldn't open the PDF file",
    "Couldn't read the page catalog",
    "PDF file was damaged and couldn't be repaired",
    "File was encrypted and password was incorrect or not supplied",
    "Nonexistent or invalid highlight file",
    "Invalid printer",
    "Error during printing",
    "PDF file doesn't allow that operation",
    "Invalid page number",
    "File I/O error"
  };

  const size_t errorMessageSize = (sizeof(errorMessages) / sizeof(const char *));

  bool is_utf8_string(const char *str) {
    char *message;
    int faulty_bytes;
    return !is_utf8((unsigned char *)str, strlen(str), &message, &faulty_bytes);
  }

  GooList *scanFonts(PDFDoc *doc) {
    FontInfoScanner scanner(doc, 0);
    GooList *list = scanner.scan(doc->getNumPages());
    assert(list);
    return list;
  }

  void cleanupFonts(GooList *fonts) {
    for (int i = 0, length = fonts->getLength(); i < length; ++i) {
      delete (FontInfo*)fonts->get(i);
    }
    delete fonts;
  }

  template <typename JSON>
  void write_string(JSON &json, const char *str) {
    if (ignoreEncodingError) {
      json.String(str);
    } else {
      json.String(is_utf8_string(str) ? str : "[unknown encoding]");
    }
  }

  template <typename JSON>
  void write_formatted_string(JSON &json, const char *str, ...) {
    va_list ap;

    va_start(ap, str);
    int length = vsnprintf(NULL, 0, str, ap);
    va_end(ap);

    assert(length >= 0);
    char *buffer = new char[length + 1];

    va_start(ap, str);
    vsnprintf(buffer, length + 1, str, ap);
    va_end(ap);

    write_string(json, buffer);
    delete [] buffer;
  }

  template <typename JSON>
  void write_box(JSON &json, PDFRectangle *box) {
    json.StartObject();

    json.Key("offset_x");
    assert(json.Double(box->x1));

    json.Key("offset_y");
    assert(json.Double(box->y1));

    json.Key("width");
    assert(json.Double(box->x2 - box->x1));

    json.Key("height");
    assert(json.Double(box->y2 - box->y1));

    json.EndObject();
  }

  template <typename JSON, typename ImageInfoPointer>
  void write_image(JSON &json, ImageInfoPointer image) {
    json.StartObject();

    json.Key("type");
    if (image->type) {
      write_string(json, image->type);
    } else {
      json.Null();
    }

    json.Key("width");
    json.Int(image->width);

    json.Key("height");
    json.Int(image->height);

    json.Key("ppi_x");
    assert(json.Double(image->xppi));

    json.Key("ppi_y");
    assert(json.Double(image->yppi));

    if (image->has_colorspace) {
      json.Key("color_space");
      json.StartObject();

      json.Key("type");
      write_string(json, image->colorspace);

      json.Key("number_of_components");
      json.Int(image->components);

      json.EndObject();
    }

    if (image->has_colorspace2) {
      json.Key("base_color_space");
      json.StartObject();

      json.Key("type");
      write_string(json, image->colorspace2);

      json.Key("number_of_components");
      json.Int(image->components2);

      json.EndObject();
    }

    json.EndObject();
  }

  template <typename JSON>
  void write_page(JSON &json, Page *page) {
    json.StartObject();

    json.Key("page_number");
    json.Int(page->getNum());

    json.Key("media_box");
    write_box(json, page->getMediaBox());

    json.Key("crop_box");
    write_box(json, page->getCropBox());

    {
      ImageListDev dev;
      page->display(&dev, 72, 72, 0, gTrue, gFalse, gFalse);

      json.Key("images");
      json.StartArray();
      for (ImageInfoListIterator it = dev.getListBegin(); it != dev.getListEnd(); ++it) {
        write_image(json, it);
      }
      json.EndArray();
    }

    json.Key("number_of_annotations");
    json.Int(page->getAnnots()->getNumAnnots());

    json.EndObject();
  }

  template <typename JSON>
  void write_font(JSON &json, FontInfo *font) {
    json.StartObject();

    json.Key("name");
    if (font->getName()) {
      write_string(json, font->getName()->getCString());
    } else {
      json.Null();
    }

    json.Key("type");
    if (font->getType() > 0 && font->getType() < fontTypeSize) {
      write_string(json, fontTypeNames[font->getType()]);
    } else {
      json.Null();
    }

    json.Key("encoding");
    if (font->getEncoding()) {
      write_string(json, font->getEncoding()->getCString());
    } else {
      json.Null();
    }

    json.Key("is_embedded");
    json.Bool(font->getEmbedded() ? true : false);

    json.Key("is_subset");
    json.Bool(font->getSubset() ? true : false);

    json.EndObject();
  }

  template <typename JSON>
  void write_json(JSON &json, PDFDoc *doc) {
    // Prefetch FontInfo objects because ImageListDev modifies data for fonts depending on the file
    GooList *fonts = scanFonts(doc);

    json.StartObject();

    json.Key("version");
    write_formatted_string(json, "%d.%d", doc->getPDFMajorVersion(), doc->getPDFMinorVersion());

    json.Key("is_tagged");
    json.Bool((doc->getCatalog()->getMarkInfo() & Catalog::markInfoMarked) ? true : false);

    json.Key("contains_form");
    json.Bool((doc->getCatalog()->getFormType() != Catalog::NoForm) ? true : false);

    json.Key("contains_javascript");
    {
      JSInfo jsInfo(doc, 0);
      jsInfo.scanJS(doc->getNumPages());
      json.Bool(jsInfo.containsJS() ? true : false);
    }

    json.Key("number_of_pages");
    json.Int(doc->getNumPages());

    json.Key("is_encrypted");
    json.Bool(doc->isEncrypted() ? true : false);

    json.Key("pages");
    json.StartArray();
    for (int i = 1, last = doc->getNumPages(); i <= last; ++i) {
      write_page(json, doc->getPage(i));
    }
    json.EndArray();

    json.Key("fonts");
    json.StartArray();
    for (int i = 0, length = fonts->getLength(); i < length; ++i) {
      write_font(json, (FontInfo*)fonts->get(i));
    }
    json.EndArray();

    json.EndObject();

    cleanupFonts(fonts);
  }

  void output_json(PDFDoc *doc) {
    StringBuffer s;
    PrettyWriter<StringBuffer> writer(s);
    write_json(writer, doc);
    fprintf(stdout, "%s\n", s.GetString());
    fflush(stdout);
  }

  template <typename JSON>
  void write_error(JSON &json, int errorCode) {
    json.StartObject();

    json.Key("error");
    json.StartObject();

    json.Key("code");
    json.Int(errorCode);

    json.Key("message");
    write_string(json, errorCode < errorMessageSize ? errorMessages[errorCode] : "Unknown errror");

    json.EndObject();

    json.EndObject();
  }

  void output_error(int errorCode) {
    StringBuffer s;
    PrettyWriter<StringBuffer> writer(s);
    write_error(writer, errorCode);
    fprintf(stdout, "%s\n", s.GetString());
    fflush(stdout);
  }
}

int main(int argc, char *argv[]) {
  PDFDoc *doc = NULL;
  int exitCode = 0;

  GBool ok = parseArgs(argDesc, &argc, argv);
  if (!ok || argc != 2 || printVersion || printHelp) {
    fprintf(stderr, "pdfinfo-json version %s\n", PACKAGE_VERSION);

    if (!printVersion) {
      printUsage("pdfinfo-json", "<PDF-file>", argDesc);
    }

    exitCode = (printVersion || printHelp) ? 0 : 99;
    goto fail;
  }

  globalParams = new GlobalParams();

  {
    GooString path(argv[1]), *ownerPW = NULL;

    if (ownerPassword[0] != '\001') {
      ownerPW = new GooString(ownerPassword);
    }

    doc = PDFDocFactory().createPDFDoc(path, ownerPW, NULL);

    if (ownerPW) {
      delete ownerPW;
    }
  }

  if (!doc->isOk()) {
    output_error(doc->getErrorCode());
    exitCode = 1;
    goto fail;
  }
  output_json(doc);

fail:
  if (doc) {
    delete doc;
  }

  if (globalParams) {
    delete globalParams;
  }

  return exitCode;
}
