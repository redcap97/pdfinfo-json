# pdfinfo-json

pdfinfo-json is a PDF document information extractor which outputs data using JSON.

## How to Run

Please type following command:

```
$ pdfinfo-json /path/to/pdf-file
{
    "version": "1.5",
    "is_tagged": true,
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
                "width": 516.0,
                "height": 728.64
            },
            "crop_box": {
                "offset_x": 0.0,
                "offset_y": 0.0,
                "width": 516.0,
                "height": 728.64
            },
            "number_of_annotations": 0
        }
    ],
    "fonts": [
        {
            "name": "ABCDEE+MS-Mincho",
            "type": "TrueType",
            "encoding": "WinAnsi",
            "is_embedded": true,
            "is_subset": true
        },
        {
            "name": "ABCDEE+MS-Mincho",
            "type": "CID TrueType",
            "encoding": "Identity-H",
            "is_embedded": true,
            "is_subset": true
        },
        {
            "name": "ABCDEE+Century",
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
# Build
make

# Install to /usr/local
make install

# Install to /usr
make install PREFIX=/usr
```

## License

Licensed under the GPLv3: http://www.gnu.org/licenses/gpl-3.0.txt
