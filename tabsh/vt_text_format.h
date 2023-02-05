#ifndef VT_TEXT_FORMAT_H
#define VT_TEXT_FORMAT_H

#define VT_Reset           0 // Returns all attributes to the default state prior to modification
#define VT_Bold            1 // Applies brightness/intensity flag to foreground color
#define VT_Italic          3 // Italic (does not work with dos nor powershell)
#define VT_Underline       4 // Adds underline
#define VT_Negative        7 // Swaps foreground and background colors
#define VT_No_underline   24 // Removes underline
#define VT_No_bold        22 // Removes brightness/intensity flag from foreground color
#define VT_Positive       27 // Returns foreground/background to normal
#define VT_Black          30 // Applies non-bold/bright black to foreground
#define VT_Red            31 // Applies non-bold/bright red to foreground
#define VT_Green          32 // Applies non-bold/bright green to foreground
#define VT_Yellow         33 // Applies non-bold/bright yellow to foreground
#define VT_Blue           34 // Applies non-bold/bright blue to foreground
#define VT_Magenta        35 // Applies non-bold/bright magenta to foreground
#define VT_Cyan           36 // Applies non-bold/bright cyan to foreground
#define VT_White          37 // Applies non-bold/bright white to foreground
#define VT_Extended       38 // Applies extended color value to the foreground (see details below)
#define VT_Default        39 // Applies only the foreground portion of the defaults (see 0)
#define VT_BG_Black       40 // Applies non-bold/bright black to background
#define VT_BG_Red         41 // Applies non-bold/bright red to background
#define VT_BG_Green       42 // Applies non-bold/bright green to background
#define VT_BG_Yellow      43 // Applies non-bold/bright yellow to background
#define VT_BG_Blue        44 // Applies non-bold/bright blue to background
#define VT_BG_Magenta     45 // Applies non-bold/bright magenta to background
#define VT_BG_Cyan        46 // Applies non-bold/bright cyan to background
#define VT_BG_White       47 // Applies non-bold/bright white to background
#define VT_BG_Extended    48 // Applies extended color value to the background (see details below)
#define VT_BG_Default     49 // Applies only the background portion of the defaults (see 0)
#define VT_Br_Black       90 // Applies bold/bright black to foreground
#define VT_Br_Red         91 // Applies bold/bright red to foreground
#define VT_Br_Green       92 // Applies bold/bright green to foreground
#define VT_Br_Yellow      93 // Applies bold/bright yellow to foreground
#define VT_Br_Blue        94 // Applies bold/bright blue to foreground
#define VT_Br_Magenta     95 // Applies bold/bright magenta to foreground
#define VT_Br_Cyan        96 // Applies bold/bright cyan to foreground
#define VT_Br_White       97 // Applies bold/bright white to foreground
#define VT_Br_BG_Black   100 // Applies bold/bright black to background
#define VT_Br_BG_Red     101 // Applies bold/bright red to background
#define VT_Br_BG_Green   102 // Applies bold/bright green to background
#define VT_Br_BG_Yellow  103 // Applies bold/bright yellow to background
#define VT_Br_BG_Blue    104 // Applies bold/bright blue to background
#define VT_Br_BG_Magenta 105 // Applies bold/bright magenta to background
#define VT_Br_BG_Cyan    106 // Applies bold/bright cyan to background
#define VT_Br_BG_White   107 // Applies bold/bright white to background

#endif // VT_TEXT_FORMAT_H

