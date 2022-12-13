# Utility for working with BMP

## Cut and rotate
The application `./hw-01_bmp` is implemented, which cuts a rectangle from a BMP image,
rotates this rectangle 90 degrees clockwise and saves the result in a separate
file.

All images are stored in the given format:

* The general format is [BMP](https://en.wikipedia.org/wiki/BMP).
* The BMP format uses the *DIB* format with the `BITMAPINFOHEADER` header (version 3).
* The value of the `biHeight` field (image height) is strictly greater than zero.
* Uses 24 bits of color per pixel (one byte per color channel).
* Palette (color table) is not used.
* Compression is not used.

The utility works for little endian systems and only for one specific type of BMP files.

### Console application
The application is launched with the following command:

```
./hw-01_bmp crop-rotate ‹in-bmp› ‹out-bmp› ‹x› ‹y› ‹w› ‹h›
```

Used parameters:

* `crop-rotate` is a required parameter, indicating the action to be performed.
* `in-bmp` is the name of the input image file.
* `out-bmp` is the name of the output image file.
* `x`, `y` are the coordinates of the upper left corner of the area to be cut and rotated.
   Coordinates start at zero, so *(0, 0)* is the top left corner.
* `w`, `h` - respectively, the width and height of the area before rotation.

Thus, if we denote the width and height of the original image as `W` and `H`, respectively,
for correct arguments, the following inequalities are true:

* `0 <= x < x + w <= W`
* `0 <= y < y + h <= H`

## Shorthand
In addition to the `crop-rotate` command, the `insert` and `extract` commands are implemented,
allowing you to hide a message inside the image
([Shorthand](https://en.wikipedia.org/wiki/Shorthand)).
The `insert` command saves the message to the image, and `extract` extracts it from there.

The idea is as follows: if you change only the least significant bits in the color components,
then an outside observer will not notice the distortion.
This distortion can carry information.

### Coding method
The initial message consists only of capital Latin letters, a space, a period and a comma.
Each character is converted to a number from 0 to 28, respectively (29 different values in total),
and the number is in five bits, written from the lowest to the highest.
A total message of `N` characters is encoded using `5N` Bits.

To send a message, in addition to the carrier image, you will need __key__ - a text file,
describing in which pixels the message bits are encoded.
In this file, on separate lines, are written:

* The `x` and `y` coordinates (`0 <= x < W`, `0 <= y < H`) of the pixel to save to
   corresponding bit.
* The letter `R`/`G`/`B` denoting the color channel in the least significant bit of which you want to write a bit
   messages.

If the key writes more bits than the message needs, the last lines are ignored.

### Console application
To save the secret stitch to an image, the application is launched with the following command:
```
./hw-01_bmp insert ‹in-bmp› ‹out-bmp› ‹key-txt› ‹msg-txt›
```

To extract the secret line from an image, the application is launched with the following command:
```
./hw-01_bmp extract ‹in-bmp› ‹key-txt› ‹msg-txt›
```

Used parameters:

* `in-bmp` is the name of the input image file.
* `out-bmp` is the name of the output image file.
* `key-txt` is a test file with a key.
* `msg-txt` is a text file with a secret message.
