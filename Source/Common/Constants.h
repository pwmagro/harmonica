/*
  ==============================================================================

    Constants.h
    Created: 11 Sep 2022 9:21:26pm
    Author:  pwmag

  ==============================================================================
*/

#pragma once

// Parameter tree IDs
#define MIX_ID          "MIX"
#define WINDOW_LEN_ID   "WINDOW_LENGTH"
#define ABOUT_ID        "ABOUT_WINDOW"

// Parameter names
#define MIX_NAME        "Mix"
#define WINDOW_LEN_NAME "Window Length"
#define ABOUT_NAME      "About"

// Fonts
#define FONT_MAIN       BinaryData::RalewayMedium_ttf
#define FONT_MAIN_SIZE  BinaryData::RalewayMedium_ttfSize
#define FONT_BOLD       BinaryData::RalewayRegular_ttf
#define FONT_BOLD_SIZE  BinaryData::RalewayRegular_ttfSize
#define FONT_LIGHT      BinaryData::RalewayLight_ttf
#define FONT_LIGHT_SIZE BinaryData::RalewayLight_ttfSize

// Images
#define IMAGE_LOGO      BinaryData::diodine_logo_svg
#define IMAGE_LOGO_SIZE BinaryData::diodine_logo_svgSize
#define IMAGE_BG        BinaryData::diodine_bg_svg
#define IMAGE_BG_SIZE   BinaryData::diodine_bg_svgSize

// Window properties
#define WIDTH               720u
#define HEIGHT              520u
#define MAX_SCALE           2.f
#define LOCAL_WIDTH_ID      "WIDTH"
#define LOCAL_HEIGHT_ID     "HEIGHT"