# Image Editor – C CLI Application - Dobrinoiu Bogda Mihai


## Overview

This project is a command-line image editor implemented in C, designed to process grayscale and RGB images in the 
PGM/PPM formats (P2, P3, P5, P6). The application supports loading, saving, selecting, cropping, rotating, filtering,
equalizing, and generating histograms using efficient matrix-based operations. 
The program reads commands interactively from standard input and applies pixel-level transformations such as 
convolution filters and geometric processing.

## Supported Image Formats

- **Grayscale:**
    - P2 (ASCII)
    - P5 (Binary)

- **Color (RGB):**
    - P3 (ASCII)
    - P6 (Binary)

---

## Features

### Image Management
- `LOAD <file>` — Load a PGM/PPM image
- `SAVE <file> [ascii]` — Save the current image (optionally convert to ASCII)
- `SELECT x1 y1 x2 y2 | ALL` — Select a rectangular region
- `CROP` — Crop the image to the selected region
- `ROTATE <angle>` — Rotate a square selection by **90°, 180°, or 270°**

---

### Image Processing Filters (RGB only)

Applied using convolution kernels:

- `APPLY SHARPEN`
- `APPLY EDGE`
- `APPLY BLUR`
- `APPLY GAUSSIAN_BLUR`

---

### Grayscale Operations

- `EQUALIZE` — Perform histogram equalization for contrast enhancement
- `HISTOGRAM <max_stars> <bins>` — Generate a scaled textual histogram of pixel intensity ranges

---

## Implementation Details

- Dynamic memory allocation for 2D image matrices (grayscale and RGB).
- Convolution-based image filtering with pixel-value clamping to `[0..255]`.
- Safe validation and normalization of image selection coordinates.
- Automatic handling of ASCII ↔ binary format conversions.
- Efficient histogram creation and cumulative distribution computation.
- Support for rotation of square image selections only.
- Robust command parsing and error handling.

---

## Project Structure

    ├── main.c           # Application loop & command handling
    ├── image_editor.c  # Image processing implementation
    ├── image_editor.h  # Function declarations and data structures
    ├── Makefile        # Build instructions & packaging
    └── README.md

## Build Instructions

    Compile using the provided Makefile:
    
    ```bash
    make

    This will produce the image_editor executable.

## Run
    
    ./image_editor

    The program runs in interactive mode and accepts commands from standard input.

## Example of usage

    LOAD image.ppm
    SELECT ALL
    APPLY SHARPEN
    SAVE result.ppm
    EXIT

## Notes

    - Filters via APPLY are available only for color images (P3 / P6).
    - EQUALIZE and HISTOGRAM commands are supported only on grayscale images (P2 / P5).
    - ROTATE requires a square selection area and angles that are multiples of 90°.
    - Pixel values are automatically clamped to remain within valid image intensity limits.
    - Invalid commands or unsupported operations are handled gracefully with runtime messages.


