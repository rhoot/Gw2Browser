/** \file       Data.h
 *  \brief      Embeds for the various files in the data folder. This is to
 *              make the exe self-sufficient, without platform dependency.
 *  \author     Rhoot
 */

/*	Copyright (C) 2012 Rhoot <https://github.com/rhoot>

    This file is part of Gw2Browser.

    Gw2Browser is free software: you can redistribute it and/or modify
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

#pragma once

#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

namespace gw2b
{

namespace data
{

/** Loads a PNG from memory and returns a new wxBitmap containing it.
 *  \param[in]  p_data   PNG data to load.
 *  \param[in]  p_size   Size of the PNG data.
 *  \return wxBitmap    Bitmap containing the loaded PNG. */
wxBitmap loadPNG(const byte* p_data, uint p_size);
/** Loads a JPG from memory and returns a new wxBitmap containing it.
 *  \param[in]  p_data   PNG data to load.
 *  \param[in]  p_size   Size of the PNG data.
 *  \return wxBitmap    Bitmap containing the loaded JPG. */
wxBitmap loadJPG(const byte* p_data, uint p_size);

// File: data/about_box.jpg
extern const uint about_box_jpg_size;
extern const byte about_box_jpg[];
// File: data/checkers.png
extern const uint checkers_png_size;
extern const byte checkers_png[];
// File: data/closed_folder.png
extern const uint closed_folder_png_size;
extern const byte closed_folder_png[];
// File: data/dll.png
extern const uint dll_png_size;
extern const byte dll_png[];
// File: data/exe.png
extern const uint exe_png_size;
extern const byte exe_png[];
// File: data/image.png
extern const uint image_png_size;
extern const byte image_png[];
// File: data/model_view.hlsl
extern const uint model_view_hlsl_size;
extern const byte model_view_hlsl[];
// File: data/open_folder.png
extern const uint open_folder_png_size;
extern const byte open_folder_png[];
// File: data/text.png
extern const uint text_png_size;
extern const byte text_png[];
// File: data/toggle_alpha.png
extern const uint toggle_alpha_png_size;
extern const byte toggle_alpha_png[];
// File: data/toggle_blue.png
extern const uint toggle_blue_png_size;
extern const byte toggle_blue_png[];
// File: data/toggle_green.png
extern const uint toggle_green_png_size;
extern const byte toggle_green_png[];
// File: data/toggle_red.png
extern const uint toggle_red_png_size;
extern const byte toggle_red_png[];
// File: data/unknown.png
extern const uint unknown_png_size;
extern const byte unknown_png[];

}; // namespace data

}; // namespace gw2b

#endif // DATA_H_INCLUDED
