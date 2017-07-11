[![Build Status](https://travis-ci.org/redcap97/pdfinfo-json.svg?branch=master)](https://travis-ci.org/redcap97/pdfinfo-json)

# pdfinfo-json

pdfinfo-json is a PDF document information extractor which outputs data using JSON.

## Usage

```
pdfinfo-json /path/to/pdf-file
```

`pdfinfo-json` outputs like the following JSON data:

```
{
    "version": "1.5",
    "is_tagged": false,
    "contains_form": false,
    "contains_javascript": false,
    "number_of_pages": 1,
    "is_encrypted": false,
    "pages": [
        {
            "page_number": 1,
            "media_box": {
                "offset_x": 0.0,
                "offset_y": 0.0,
                "width": 436.54,
                "height": 612.2800000000001
            },
            "crop_box": {
                "offset_x": 0.0,
                "offset_y": 0.0,
                "width": 436.54,
                "height": 612.2800000000001
            },
            "images": [
                {
                    "type": "Image",
                    "width": 924,
                    "height": 924,
                    "ppi_x": 468.50702509692595,
                    "ppi_y": 468.50702509692595,
                    "color_space": {
                        "type": "CMYK",
                        "number_of_components": 4
                    }
                }
            ],
            "number_of_annotations": 0
        }
    ],
    "fonts": [
        {
            "name": "KQSFZR+Arial-Black",
            "type": "TrueType",
            "encoding": "WinAnsi",
            "is_embedded": true,
            "is_subset": true
        }
    ]
}
```

## Build Dependencies

* g++ (or clang++)
* Poppler (with `--enable-xpdf-headers`)
* GNU Make
* pkg-config

## Build and Install

```
# Build and install to /usr/local
make
make install

# Build and install to /usr
make PREFIX=/usr
make install PREFIX=/usr
```

## License

Licensed under the GPLv3: http://www.gnu.org/licenses/gpl-3.0.txt
