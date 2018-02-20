<?php

const NAME_APP        = 'qiitedon';
const DIR_SEP         = DIRECTORY_SEPARATOR;
const INDENT          = '    ';
const NAME_DIR_CURNT  = '.';
const NAME_DIR_PARNT  = '..';
const REQUIRE_OS      = 'DARWIN'; //Required OS to compare with PHP_OS
const MARGIN_LEFT_MAX = 20;
const MARKER_TRIM     = '...';
const NAME_FILE_CONF  = NAME_APP . '.json';
const PATH_DIR_CURNT  = NAME_DIR_CURNT . DIR_SEP;
const PATH_DIR_PARNT  = NAME_DIR_PARNT . DIR_SEP;
const DO_NOT_ECHO     = false;
const DO_NOT_RETURN   = false;

// Bash color constants
// https://misc.flogisoft.com/bash/tip_colors_and_formatting

//Text attributes
const ATTRB_OFF = 0; // All attributes off
const BOLD_ON   = 1; // Bold on
const GRAYOUT   = 2; // Gray out
const ITALIC    = 3; // Italic
const UNDERLINE = 4; // Underscore (on monochrome display adapter only)
const BLINK_ON  = 5; // Blink on
const REV_VIDEO = 7; // Reverse video on
const CONCLD_ON = 8; // Concealed on(Password)

//Foreground colors
const FONT_BLACK   = 30; // Black
const FONT_RED     = 31; // Red
const FONT_YELLOW  = 32; // Yellow
const FONT_ORANGE  = 33; // Orange
const FONT_BLUE    = 34; // Blue
const FONT_PURPLE  = 35; // Purple
const FONT_CYAN    = 36; // Cyan
const FONT_WHITE   = 37; // White

//Background colors
const BKGD_BLACK   =  40; // Black
const BKGD_RED     =  41; // Red
const BKGD_GREEN   =  42; // Green
const BKGD_ORANGE  =  43; // Orange
const BKGD_BLUE    =  44; // Blue
const BKGD_Purple  =  45; // Purple
const BKGD_CYAN    =  46; // Cyan
const BKGD_GRAY    =  47; // Gray
const BKGD_WHITE   = 107; // White

define('WIDTH_INDENT', mb_strlen(INDENT));
