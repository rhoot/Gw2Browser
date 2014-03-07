/* \file       Readers/ImageReader.h
*  \brief      Contains the declaration of the image reader class.
*  \author     Rhoot
*/

/*
Copyright (C) 2012 Rhoot <https://github.com/rhoot>

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

#ifndef READERS_IMAGEREADER_H_INCLUDED
#define READERS_IMAGEREADER_H_INCLUDED

#include "FileReader.h"

#ifdef RGB
#  undef RGB    // GOD DAMN MICROSOFT WITH YOUR GOD DAMN MACROS
#endif

namespace gw2b {

#pragma pack(push, 1)

	union BGRA {
		struct {
			uint8 b;
			uint8 g;
			uint8 r;
			uint8 a;
		};
		uint8 parts[4];
		uint32 color;
	};

	union RGBA {
		struct {
			uint8 r;
			uint8 g;
			uint8 b;
			uint8 a;
		};
		uint8 parts[4];
		uint32 color;
	};

	struct BGR {
		uint8   b;
		uint8   g;
		uint8   r;
	};

	struct RGB {
		uint8   r;
		uint8   g;
		uint8   b;
	};

	union DXTColor {
		struct {
			uint16 red1 : 5;
			uint16 green1 : 6;
			uint16 blue1 : 5;
			uint16 red2 : 5;
			uint16 green2 : 6;
			uint16 blue2 : 5;
		};
		struct {
			uint16 color1;
			uint16 color2;
		};
	};

	struct DXT1Block {
		DXTColor colors;
		uint32   indices;
	};

	struct DXT3Block {
		uint64   alpha;
		DXTColor colors;
		uint32   indices;
	};

	struct DCXBlock     // Should be 3DCXBlock, but names can't start with a number D:
	{
		uint64  green;
		uint64  red;
	};

	struct DDSPixelFormat {
		uint32          size;                   /**< Structure size; set to 32 (bytes). */
		uint32          flags;                  /**< Values which indicate what type of data is in the surface. */
		uint32          fourCC;                 /**< Four-character codes for specifying compressed or custom formats. */
		uint32          rgbBitCount;            /**< Number of bits in an RGB (possibly including alpha) format. */
		uint32          rBitMask;               /**< Red (or lumiannce or Y) mask for reading color data. */
		uint32          gBitMask;               /**< Green (or U) mask for reading color data. */
		uint32          bBitMask;               /**< Blue (or V) mask for reading color data. */
		uint32          aBitMask;               /**< Alpha mask for reading alpha data. */
	};

	struct DDSHeader {
		uint32          magic;                  /**< Identifies a DDS file. This member must be set to 0x20534444. */
		uint32          size;                   /**< Size of structure. This member must be set to 124. */
		uint32          flags;                  /**< Flags to indicate which members contain valid data. */
		uint32          height;                 /**< Surface height (in pixels). */
		uint32          width;                  /**< Surface width (in pixels). */
		uint32          pitchOrLinearSize;      /**< The pitch or number of bytes per scan line in an uncompressed texture; the total number of bytes in the top level texture for a compressed texture. */
		uint32          depth;                  /**< Depth of a volume texture (in pixels), otherwise unused. */
		uint32          mipMapCount;            /**< Number of mipmap levels, otherwise unused. */
		uint32          reserved1[11];          /**< Unused. */
		DDSPixelFormat  pixelFormat;            /**< The pixel format. */
		uint32          caps;                   /**< Specifies the complexity of the surfaces stored. */
		uint32          caps2;                  /**< Additional detail about the surfaces stored. */
		uint32          caps3;                  /**< Unused. */
		uint32          caps4;                  /**< Unused. */
		uint32          reserved2;              /**< Unused. */
	};

#pragma pack(pop)

	class ImageReader : public FileReader {
	public:
		/** Constructor.
		*  \param[in]  p_data       Data to be handled by this reader.
		*  \param[in]  p_fileType   File type of the given data. */
		ImageReader( const Array<byte>& p_data, ANetFileType p_fileType );
		/** Destructor. Clears all data. */
		virtual ~ImageReader( );

		/** Gets the type of data contained in this file. Not to be confused with
		*  file type.
		*  \return DataType    type of data. */
		virtual DataType dataType( ) const override {
			return DT_Image;
		}
		/** Gets an appropriate file extension for the contents of this reader.
		*  \return wxString    File extension. */
		virtual const wxChar* extension( ) const override {
			return wxT( ".png" );
		}
		/** Converts the data associated with this file into PNG.
		*  \return Array<byte> converted data. */
		virtual Array<byte> convertData( ) const;
		/** Gets the image contained in the data owned by this reader.
		*  \return wxImage     Newly created image. */
		wxImage getImage( ) const;
		/** Determines whether the header of this image is valid.
		*  \return bool    true if valid, false if not. */
		static bool isValidHeader( const byte* p_data, uint p_size );
	private:
		bool readDDS( wxSize& po_size, BGR*& po_colors, uint8*& po_alphas ) const;
		bool readATEX( wxSize& po_size, BGR*& po_colors, uint8*& po_alphas ) const;

		bool processLuminanceDDS( const DDSHeader* p_header, RGB*& po_colors ) const;
		bool processUncompressedDDS( const DDSHeader* p_header, RGB*& po_colors, uint8*& po_alphas ) const;

		void processDXTColor( BGR* p_colors, uint8* p_alphas, const DXTColor& p_blockColor, bool p_isDXT1 ) const;
		void processDXT1( const BGRA* p_data, uint p_width, uint p_height, BGR*& po_colors, uint8*& po_alphas ) const;
		void processDXT1Block( BGR* p_colors, uint8* p_alphas, const DXT1Block& p_block, uint p_blockX, uint p_blockY, uint p_width ) const;
		void processDXT3( const BGRA* p_data, uint p_width, uint p_height, BGR*& po_colors, uint8*& po_alphas ) const;
		void processDXT3Block( BGR* p_colors, uint8* p_alphas, const DXT3Block& p_block, uint p_blockX, uint p_blockY, uint p_width ) const;
		void processDXT5( const BGRA* p_data, uint p_width, uint p_height, BGR*& po_colors, uint8*& po_alphas ) const;
		void processDXT5Block( BGR* p_colors, uint8* p_alphas, const DXT3Block& p_block, uint p_blockX, uint p_blockY, uint p_width ) const;
		void processDXTA( const uint64* p_data, uint p_width, uint p_height, BGR*& po_colors ) const;
		void processDXTABlock( BGR* p_colors, uint64 p_block, uint p_blockX, uint p_blockY, uint p_width ) const;
		void process3DCX( const RGBA* p_data, uint p_width, uint p_height, BGR*& po_colors, uint8*& po_alphas ) const;
		void process3DCXBlock( RGB* p_colors, const DCXBlock& p_block, uint p_blockX, uint p_blockY, uint p_width ) const;
	}; // class ImageReader

}; // namespace gw2b

#endif // READERS_IMAGEREADER_H_INCLUDED
