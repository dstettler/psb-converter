# `psb-converter`

This repo is the home of psb-converter, a program that converts raw image arrays (which can be generated from common formats with `rbm-converter.py`) into my `psb` image format.
It's mostly just a way for me to play around with concepts behind image compression, and isn't anything too serious. 
I'll update this README as I work out a, well, working version.

<hr>

### `rbm-converter.py`

Pip Install Requirements:
- `Pillow`
- `numpy`

### `.rbm files`
`.rbm` (Raw BitMap) files are just raw csv arrays of RGB(A) information, created really for just the sake of convenience on my part. 
The first row is the width and height of the image.
Each row after contains, in order: red, green, blue, and, if there is an alpha channel (which there may not be), alpha.

### `.psb files`
`.psb` (Pretty Simple Bitmap) files are the main purpose of this project's development.
I wanted to learn a bit more about image compression, and this project/format is my little playground.
If you're able to decipher it, the contents of [psb_format_brainstorming.txt](psb_format_brainstorming.txt) are a general target I am using.

The general intent behind the format *currently* is to be a palette-based format.
Instead of using raw RGB values, each pixel in an image will be analyzed, and its color added to a frequency dictionary. 
The colors with the highest frequencies will be assigned a "shorthand."
The way I see it, this shorthand has a few requirements: 
- Must be no larger than 2 bytes
    - I want this because if we have an image of *just* shorthands, and each shorthand is 3 bytes, 
    this will produce a file the exact same size as just a raw bitmap,
    completely negating the purpose of compression.
- <s> Must not begin with a reserved byte
    - The format has a few "reserved bytes."
    This may not be an issue for 1 byte shorthands, but if the algorithm
    creates enough shorthands to necessitate a length of 2 bytes, 
    we need to ensure there is no overlap between the start of a shorthand and 
    a reserved byte </s>
    - This is no longer required 
    (and was quite honestly ill-advised in the first place)
    since I have decided to introduce a flag byte to indicate the following
    bytes will be shorthand content.

The format operates sort of like RTF where the a "flag byte" is used to indicate
what type of content the following byte(s) will be.
For the time being, this is the list of flag bytes:
- 0: Section separator
- 1 + pixel content: Alpha modify pixel
- 2 + [alpha] + pixel content: Repeat pixel
- 3 + char + char + char: Single use pixel
- 4 + char*: Following byte(s) will be cTable pixel shorthands 
(number dependent on what was defined in the header)
- 5: End cTable

For more details, check [the brainstorming text file](psb_format_brainstorming.txt).
Likely details will change and that file will already be somewhat out of date, 
but I will update this README as the definitive location for reference.