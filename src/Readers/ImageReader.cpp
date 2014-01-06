/* \file       Readers/ImageReader.cpp
*  \brief      Contains the definition of the image reader class.
*  \author     Rhoot
*/

/*
Copyright (C) 2012 Rhoot <https://github.com/rhoot>
Copyright (C) 2014 Khral Steelforge <https://github.com/kytulendu>

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

#include "stdafx.h"
#include <wx/mstream.h>

#include "Imported/AtexAsm.h"
#include "ImageReader.h"

#ifdef RGB
#  undef RGB
#endif

namespace gw2b {

	enum FourCC {
		FCC_ATEX = 0x58455441,
		FCC_ATTX = 0x58545441,
		FCC_ATEP = 0x50455441,
		FCC_ATEU = 0x55455441,
		FCC_ATEC = 0x43455441,
		FCC_ATET = 0x54455441,
		FCC_3DCX = 0x58434433,
		FCC_DXT = 0x00545844,
		FCC_DDS = 0x20534444,

		FCC_DXT1 = 0x31545844,
		FCC_DXT2 = 0x32545844,
		FCC_DXT3 = 0x33545844,
		FCC_DXT4 = 0x34545844,
		FCC_DXT5 = 0x35545844,
		FCC_DXTN = 0x4e545844,
		FCC_DXTL = 0x4c545844,
		FCC_DXTA = 0x41545844,
	};

	ImageReader::ImageReader( const Array<byte>& p_data, ANetFileType p_fileType )
		: FileReader( p_data, p_fileType ) {
	}

	ImageReader::~ImageReader( ) {
	}

	wxImage ImageReader::getImage( ) const {
		Assert( m_data.GetSize( ) >= 4 );

		wxSize size;
		BGR* colors = nullptr;
		uint8* alphas = nullptr;

		// Read the correct type of data
		auto fourcc = *reinterpret_cast<const uint32*>( m_data.GetPointer( ) );
		if ( fourcc != FCC_DDS ) {
			if ( !this->readATEX( size, colors, alphas ) ) {
				return wxImage( );
			}
		} else {
			if ( !this->readDDS( size, colors, alphas ) ) {
				return wxImage( );
			}
		}

		// Create image and fill it with color data
		wxImage image( size.x, size.y );
		image.SetData( reinterpret_cast<unsigned char*>( colors ) );

		// Set alpha if the format has any
		if ( alphas ) {
			image.SetAlpha( alphas );
		}

		return image;
	}

	Array<byte> ImageReader::convertData( ) const {
		auto image = this->getImage( );

		// Bail if invalid
		if ( !image.IsOk( ) ) {
			return Array<byte>( );
		}

		// Write the png to memory
		wxMemoryOutputStream stream;
		if ( !image.SaveFile( stream, wxBITMAP_TYPE_PNG ) ) {
			return Array<byte>( );
		}

		// Reset the position of the stream
		auto buffer = stream.GetOutputStreamBuffer( );
		buffer->Seek( 0, wxFromStart );

		// Read the data from the stream and into a buffer
		Array<byte> data( buffer->GetBytesLeft( ) );
		buffer->Read( data.GetPointer( ), data.GetSize( ) );

		// Return the PNG data
		return data;
	}

	bool ImageReader::readDDS( wxSize& po_size, BGR*& po_colors, uint8*& po_alphas ) const {
		// Get header
		if ( m_data.GetSize( ) < sizeof( DDSHeader ) ) {
			return false;
		}
		auto header = reinterpret_cast<const DDSHeader*>( m_data.GetPointer( ) );

		// Ensure some of the values are correct
		if ( header->magic != FCC_DDS ||
			header->size != sizeof( DDSHeader ) -4 ||
			header->pixelFormat.size != sizeof( DDSPixelFormat ) ) {
			return false;
		}

		// Determine the pixel format
		if ( header->pixelFormat.flags & 0x40 ) {               // 0x40 = DDPF_RGB, uncompressed data
			if ( !this->processUncompressedDDS( header, reinterpret_cast<RGB*&>( po_colors ), po_alphas ) ) {
				return false;
			}
		} else if ( header->pixelFormat.flags & 0x4 ) {         // 0x4 = DDPF_FOURCC, compressed
			const BGRA* data = reinterpret_cast<const BGRA*>( &m_data[sizeof( *header )] );
			switch ( header->pixelFormat.fourCC ) {
			case FCC_DXT1:
				this->processDXT1( data, header->width, header->height, po_colors, po_alphas );
				break;
			case FCC_DXT2:
			case FCC_DXT3:
				this->processDXT3( data, header->width, header->height, po_colors, po_alphas );
				break;
			case FCC_DXT4:
			case FCC_DXT5:
				this->processDXT5( data, header->width, header->height, po_colors, po_alphas );
				break;
			}
		} else if ( header->pixelFormat.flags & 0x20000 ) {     // 0x20000 = DDPF_LUMINANCE, single-byte color
			if ( !this->processLuminanceDDS( header, reinterpret_cast<RGB*&>( po_colors ) ) ) {
				return false;
			}
		}

		if ( !!po_colors ) {
			po_size.Set( header->width, header->height );
		}

		return !!po_colors;
	}

	bool ImageReader::processLuminanceDDS( const DDSHeader* p_header, RGB*& po_colors ) const {
		// Ensure the image is 8-bit
		if ( p_header->pixelFormat.rgbBitCount != 8 ) {
			return false;
		}

		// Determine the size of the pixel data
		uint numPixels = p_header->width * p_header->height;
		uint dataSize = ( numPixels * p_header->pixelFormat.rgbBitCount ) >> 3;

		// Image data buffer too small?
		if ( m_data.GetSize( ) < ( sizeof( *p_header ) + dataSize ) ) {
			return false;
		}

		// Read the data (we've already determined that the data is 8bpp above)
		auto pixelData = static_cast<const uint8*>( &m_data[sizeof( *p_header )] );

#pragma omp parallel for
		for ( int y = 0; y < static_cast<int>( p_header->height ); y++ ) {
			uint32 curPixel = ( y * p_header->width );

			for ( uint x = 0; x < p_header->width; x++ ) {
				::memset( &po_colors[curPixel], pixelData[curPixel], sizeof( po_colors[curPixel] ) );
				curPixel++;
			}
		}

		return true;
	}

	bool ImageReader::processUncompressedDDS( const DDSHeader* p_header, RGB*& po_colors, uint8*& po_alphas ) const {
		// Ensure the image is 32-bit. Until a non-32 bit texture is found,
		// there's no point adding support for it
		if ( p_header->pixelFormat.rgbBitCount != 32 ) {
			return false;
		}

		// Determine the size of the pixel data
		uint numPixels = p_header->width * p_header->height;
		uint dataSize = ( numPixels * p_header->pixelFormat.rgbBitCount ) >> 3;

		// Image data buffer too small?
		if ( m_data.GetSize( ) < ( sizeof( *p_header ) + dataSize ) ) {
			return false;
		}

		// Color data
		RGBA shift;
		po_colors = allocate<RGB>( numPixels );
		shift.r = lowestSetBit( p_header->pixelFormat.rBitMask );
		shift.g = lowestSetBit( p_header->pixelFormat.gBitMask );
		shift.b = lowestSetBit( p_header->pixelFormat.bBitMask );

		// Alpha data
		bool hasAlpha = ( p_header->pixelFormat.flags & 0x1 );    // 0x1 = DDPF_ALPHAPIXELS, alpha is present
		if ( hasAlpha ) {
			po_alphas = allocate<uint8>( numPixels );
			shift.a = lowestSetBit( p_header->pixelFormat.aBitMask );
		}

		// Read the data (we've already determined that the data is 32bpp)
		auto pixelData = reinterpret_cast<const uint32*>( &m_data[sizeof( *p_header )] );

#pragma omp parallel for
		for ( int y = 0; y < static_cast<int>( p_header->height ); y++ ) {
			uint32 curPixel = ( y * p_header->width );

			for ( uint x = 0; x < p_header->width; x++ ) {
				po_colors[curPixel].r = ( pixelData[curPixel] & p_header->pixelFormat.rBitMask ) >> shift.r;
				po_colors[curPixel].g = ( pixelData[curPixel] & p_header->pixelFormat.gBitMask ) >> shift.g;
				po_colors[curPixel].b = ( pixelData[curPixel] & p_header->pixelFormat.bBitMask ) >> shift.b;

				if ( hasAlpha ) {
					po_alphas[curPixel] = ( pixelData[curPixel] & p_header->pixelFormat.aBitMask ) >> shift.a;
				}

				curPixel++;
			}
		}

		return true;
	}

	bool ImageReader::readATEX( wxSize& po_size, BGR*& po_colors, uint8*& po_alphas ) const {
		Assert( isValidHeader( m_data.GetPointer( ), m_data.GetSize( ) ) );
		auto atex = reinterpret_cast<const ANetAtexHeader*>( m_data.GetPointer( ) );

		// Determine mipmap0 size and bail if the file is too small
		if ( m_data.GetSize( ) >= sizeof( ANetAtexHeader ) +sizeof( uint32 ) ) {
			auto mipMap0Size = *reinterpret_cast<const uint32*>( &m_data[sizeof( ANetAtexHeader )] );

			if ( mipMap0Size + sizeof( ANetAtexHeader ) > m_data.GetSize( ) ) {
				po_size.Set( 0, 0 );
				return false;
			}
		} else {
			po_size.Set( 0, 0 );
			return false;
		}

		// Create and init
		::SImageDescriptor descriptor;
		descriptor.xres = atex->width;
		descriptor.yres = atex->height;
		descriptor.Data = m_data.GetPointer( );
		descriptor.imageformat = 0xf;
		descriptor.a = m_data.GetSize( );
		descriptor.b = 6;
		descriptor.c = 0;

		// Init some fields
		//auto data = reinterpret_cast<const uint*>( m_data.GetPointer( ) );
		po_colors = nullptr;
		po_alphas = nullptr;

		// Allocate output
		auto output = allocate<BGRA>( atex->width * atex->height );
		descriptor.image = reinterpret_cast<unsigned char*>( output );

		// Fix me
		uint32_t aOutBufferSize = 1024 * 1024 * 30; // We make the assumption that no file is bigger than 30 Mb

		// Uncompress
		switch ( atex->formatInteger ) {
		case FCC_DXT1:
			gw2dt::compression::inflateTextureFileBuffer( m_data.GetSize( ), descriptor.Data, aOutBufferSize, descriptor.image );
			this->processDXT1( output, atex->width, atex->height, po_colors, po_alphas );
			break;
		case FCC_DXT2:
		case FCC_DXT3:
		case FCC_DXTN:
			gw2dt::compression::inflateTextureFileBuffer( m_data.GetSize( ), descriptor.Data, aOutBufferSize, descriptor.image );
			this->processDXT3( output, atex->width, atex->height, po_colors, po_alphas );
			break;
		case FCC_DXT4:
		case FCC_DXT5:
			gw2dt::compression::inflateTextureFileBuffer( m_data.GetSize( ), descriptor.Data, aOutBufferSize, descriptor.image );
			this->processDXT5( output, atex->width, atex->height, po_colors, po_alphas );
			break;
		case FCC_DXTA:
			gw2dt::compression::inflateTextureFileBuffer( m_data.GetSize( ), descriptor.Data, aOutBufferSize, descriptor.image );
			this->processDXTA( reinterpret_cast<uint64*>( output ), atex->width, atex->height, po_colors );
			break;
		case FCC_DXTL:
			gw2dt::compression::inflateTextureFileBuffer( m_data.GetSize( ), descriptor.Data, aOutBufferSize, descriptor.image );
			this->processDXT5( output, atex->width, atex->height, po_colors, po_alphas );

			for ( uint i = 0; i < ( static_cast<uint>( atex->width ) * static_cast<uint>( atex->height ) ); i++ ) {
				po_colors[i].r = ( po_colors[i].r * po_alphas[i] ) / 0xff;
				po_colors[i].g = ( po_colors[i].g * po_alphas[i] ) / 0xff;
				po_colors[i].b = ( po_colors[i].b * po_alphas[i] ) / 0xff;
			}
			break;
		case FCC_3DCX:
			gw2dt::compression::inflateTextureFileBuffer( m_data.GetSize( ), descriptor.Data, aOutBufferSize, descriptor.image );
				this->process3DCX( reinterpret_cast<RGBA*>( output ), atex->width, atex->height, po_colors, po_alphas );
			break;
		default:
			freePointer( output );
			return false;
		}

		freePointer( output );

		if ( po_colors ) {
			po_size.Set( atex->width, atex->height );
			return true;
		}

		return false;
	}

	bool ImageReader::isValidHeader( const byte* p_data, uint p_size ) {
		if ( p_size < 0x10 ) {
			return false;
		}
		auto fourcc = *reinterpret_cast<const uint32*>( p_data );

		// Is this a DDS file?
		if ( fourcc == FCC_DDS ) {
			if ( p_size < sizeof( DDSHeader ) ) {
				return false;
			}
			auto dds = reinterpret_cast<const DDSHeader*>( p_data );

			bool isUncompressedRgb = !!( dds->pixelFormat.flags & 0x40 );
			bool isCompressedRgb = !!( dds->pixelFormat.flags & 0x4 );
			bool isLuminanceTexture = !!( dds->pixelFormat.flags & 0x20000 );
			bool is8Bit = ( dds->pixelFormat.rgbBitCount == 8 );
			bool is32Bit = ( dds->pixelFormat.rgbBitCount == 32 );

			return ( isCompressedRgb || ( isLuminanceTexture && is8Bit ) || ( isUncompressedRgb && is32Bit ) );
		}

		auto atex = reinterpret_cast<const ANetAtexHeader*>( p_data );
		auto compression = atex->formatInteger;

		// The compression algorithm for non-power-of-two textures is unknown
		if ( !isPowerOfTwo( atex->width ) || !isPowerOfTwo( atex->height ) ) {
			return false;
		}

		if ( ( fourcc == FCC_ATEX ) || ( fourcc == FCC_ATTX ) || ( fourcc == FCC_ATEP ) ||
			( fourcc == FCC_ATEU ) || ( fourcc == FCC_ATEC ) || ( fourcc == FCC_ATET ) ) {
			return ( compression == FCC_DXT1 ) ||
				( compression == FCC_DXT2 ) ||
				( compression == FCC_DXT3 ) ||
				( compression == FCC_DXT4 ) ||
				( compression == FCC_DXT5 ) ||
				( compression == FCC_DXTN ) ||
				( compression == FCC_DXTL ) ||
				( compression == FCC_DXTA ) ||
				( compression == FCC_3DCX );
		}

		return false;
	}

	void ImageReader::processDXTColor( BGR* p_colors, uint8* p_alphas, const DXTColor& p_blockColor, bool p_isDXT1 ) const {
		static XMFLOAT4 unpackedColorScale( 255.f / 31.f, 255.f / 63.f, 255.f / 31.f, 1.f );

		XMU565 color1Src( p_blockColor.color1 );
		XMU565 color2Src( p_blockColor.color2 );
		XMVECTOR colors[4];

		XMVECTOR colorScale = ::XMLoadFloat4( &unpackedColorScale );

		colors[0] = ::XMVectorSetW( ::XMLoadU565( &color1Src ), 255.0f );
		colors[0] = ::XMVectorSwizzle( colors[0], 2, 1, 0, 3 );
		colors[0] = ::XMVectorMultiply( colors[0], colorScale );
		colors[1] = ::XMVectorSetW( ::XMLoadU565( &color2Src ), 255.0f );
		colors[1] = ::XMVectorSwizzle( colors[1], 2, 1, 0, 3 );
		colors[1] = ::XMVectorMultiply( colors[1], colorScale );

		if ( !p_isDXT1 || p_blockColor.color1 > p_blockColor.color2 ) {
			colors[2] = ::XMVectorLerp( colors[0], colors[1], ( 1.0f / 3.0f ) );
			colors[2] = ::XMVectorSetW( colors[2], 255.0f );
			colors[3] = ::XMVectorLerp( colors[0], colors[1], ( 2.0f / 3.0f ) );
			colors[3] = ::XMVectorSetW( colors[3], 255.0f );
		} else {
			colors[2] = ::XMVectorLerp( colors[0], colors[1], 0.5f );
			colors[2] = ::XMVectorSetW( colors[2], 255.0f );
			colors[3] = ::XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
		}

		for ( uint i = 0; i < ArraySize( colors ); i++ ) {
			XMUBYTE4 color;
			::XMStoreUByte4( &color, colors[i] );
			::memcpy( &p_colors[i], &color, sizeof( p_colors[i] ) );
			if ( p_alphas ) {
				p_alphas[i] = color.w;
			}
		}
	}

	void ImageReader::processDXT1( const BGRA* p_data, uint p_width, uint p_height, BGR*& po_colors, uint8*& po_alphas ) const {
		uint numPixels = ( p_width * p_height );
		uint numBlocks = numPixels >> 4;
		const DXT1Block* blocks = reinterpret_cast<const DXT1Block*>( p_data );

		po_colors = allocate<BGR>( numPixels );
		po_alphas = allocate<uint8>( numPixels );

		const uint numHorizBlocks = p_width >> 2;
		const uint numVertBlocks = p_height >> 2;

#pragma omp parallel for
		for ( int y = 0; y < static_cast<int>( numVertBlocks ); y++ ) {
			for ( uint x = 0; x < numHorizBlocks; x++ ) {
				const DXT1Block& block = blocks[( y * numHorizBlocks ) + x];
				this->processDXT1Block( po_colors, po_alphas, block, x * 4, y * 4, p_width );
			}
		}
	}

	void ImageReader::processDXT1Block( BGR* p_colors, uint8* p_alphas, const DXT1Block& p_block, uint p_blockX, uint p_blockY, uint p_width ) const {
		uint32 indices = p_block.indices;
		BGR colors[4];
		uint8 alphas[4];

		this->processDXTColor( colors, alphas, p_block.colors, true );

		for ( uint y = 0; y < 4; y++ ) {
			uint curPixel = ( p_blockY + y ) * p_width + p_blockX;

			for ( uint x = 0; x < 4; x++ ) {
				BGR& color = p_colors[curPixel];
				uint8& alpha = p_alphas[curPixel];

				uint index = ( indices & 3 );
				::memcpy( &color, &colors[index], sizeof( color ) );
				alpha = alphas[index];

				curPixel++;
				indices >>= 2;
			}
		}
	}

	void ImageReader::processDXTA( const uint64* p_data, uint p_width, uint p_height, BGR*& po_colors ) const {
		uint numPixels = ( p_width * p_height );
		uint numBlocks = numPixels >> 4;

		po_colors = allocate<BGR>( numPixels );

		const uint numHorizBlocks = p_width >> 2;
		const uint numVertBlocks = p_height >> 2;

#pragma omp parallel for
		for ( int y = 0; y < static_cast<int>( numVertBlocks ); y++ ) {
			for ( uint x = 0; x < numHorizBlocks; x++ ) {
				uint64 block = p_data[( y * numHorizBlocks ) + x];
				this->processDXTABlock( po_colors, block, x * 4, y * 4, p_width );
			}
		}
	}

	void ImageReader::processDXTABlock( BGR* p_colors, uint64 p_block, uint p_blockX, uint p_blockY, uint p_width ) const {
		uint8  alphas[8];

		// Alpha 1 and 2
		alphas[0] = ( p_block & 0xff );
		alphas[1] = ( p_block & 0xff00 ) >> 8;
		p_block >>= 16;
		// Alpha 3 to 8
		if ( alphas[0] > alphas[1] ) {
			for ( uint i = 2; i < 8; i++ ) {
				alphas[i] = ( ( 8 - i ) * alphas[0] + ( i - 1 ) * alphas[1] ) / 7;
			}
		} else {
			for ( uint i = 2; i < 6; i++ ) {
				alphas[i] = ( ( 6 - i ) * alphas[0] + ( i - 1 ) * alphas[1] ) / 5;
			}
			alphas[6] = 0x00;
			alphas[7] = 0xff;
		}

		for ( uint y = 0; y < 4; y++ ) {
			uint curPixel = ( p_blockY + y ) * p_width + p_blockX;

			for ( uint x = 0; x < 4; x++ ) {
				::memset( &p_colors[curPixel], alphas[p_block & 0x7], sizeof( p_colors[curPixel] ) );
				curPixel++;
				p_block >>= 3;
			}
		}
	}

	void ImageReader::processDXT3( const BGRA* p_data, uint p_width, uint p_height, BGR*& po_colors, uint8*& po_alphas ) const {
		uint numPixels = ( p_width * p_height );
		uint numBlocks = numPixels >> 4;
		const DXT3Block* blocks = reinterpret_cast<const DXT3Block*>( p_data );

		po_colors = allocate<BGR>( numPixels );
		po_alphas = allocate<uint8>( numPixels );

		const uint numHorizBlocks = p_width >> 2;
		const uint numVertBlocks = p_height >> 2;

#pragma omp parallel for
		for ( int y = 0; y < static_cast<int>( numVertBlocks ); y++ ) {
			for ( uint x = 0; x < numHorizBlocks; x++ ) {
				const DXT3Block& block = blocks[( y * numHorizBlocks ) + x];
				this->processDXT3Block( po_colors, po_alphas, block, x * 4, y * 4, p_width );
			}
		}
	}

	void ImageReader::processDXT3Block( BGR* p_colors, uint8* p_alphas, const DXT3Block& p_block, uint p_blockX, uint p_blockY, uint p_width ) const {
		uint32 indices = p_block.indices;
		uint64 blockAlpha = p_block.alpha;
		BGR colors[4];

		this->processDXTColor( colors, nullptr, p_block.colors, false );

		for ( uint y = 0; y < 4; y++ ) {
			uint curPixel = ( p_blockY + y ) * p_width + p_blockX;

			for ( uint x = 0; x < 4; x++ ) {
				BGR& color = p_colors[curPixel];
				uint8& alpha = p_alphas[curPixel];

				uint index = ( indices & 3 );
				::memcpy( &color, &colors[index], sizeof( color ) );
				alpha = ( ( blockAlpha & 0xf ) << 4 ) | ( blockAlpha & 0xf );

				curPixel++;
				indices >>= 2;
				blockAlpha >>= 4;
			}
		}
	}

	void ImageReader::processDXT5( const BGRA* p_data, uint p_width, uint p_height, BGR*& po_colors, uint8*& po_alphas ) const {
		uint numPixels = ( p_width * p_height );
		uint numBlocks = numPixels >> 4;
		const DXT3Block* blocks = reinterpret_cast<const DXT3Block*>( p_data );

		po_colors = allocate<BGR>( numPixels );
		po_alphas = allocate<uint8>( numPixels );

		const uint numHorizBlocks = p_width >> 2;
		const uint numVertBlocks = p_height >> 2;

#pragma omp parallel for
		for ( int y = 0; y < static_cast<int>( numVertBlocks ); y++ ) {
			for ( uint x = 0; x < numHorizBlocks; x++ ) {
				const DXT3Block& block = blocks[( y * numHorizBlocks ) + x];
				this->processDXT5Block( po_colors, po_alphas, block, x * 4, y * 4, p_width );
			}
		}
	}

	void ImageReader::processDXT5Block( BGR* p_colors, uint8* p_alphas, const DXT3Block& p_block, uint p_blockX, uint p_blockY, uint p_width ) const {
		uint32 indices = p_block.indices;
		uint64 blockAlpha = p_block.alpha;
		BGR    colors[4];
		uint8  alphas[8];

		this->processDXTColor( colors, nullptr, p_block.colors, false );

		// Alpha 1 and 2
		alphas[0] = ( blockAlpha & 0xff );
		alphas[1] = ( blockAlpha & 0xff00 ) >> 8;
		blockAlpha >>= 16;
		// Alpha 3 to 8
		if ( alphas[0] > alphas[1] ) {
			for ( uint i = 2; i < 8; i++ ) {
				alphas[i] = ( ( 8 - i ) * alphas[0] + ( i - 1 ) * alphas[1] ) / 7;
			}
		} else {
			for ( uint i = 2; i < 6; i++ ) {
				alphas[i] = ( ( 6 - i ) * alphas[0] + ( i - 1 ) * alphas[1] ) / 5;
			}
			alphas[6] = 0x00;
			alphas[7] = 0xff;
		}

		for ( uint y = 0; y < 4; y++ ) {
			uint curPixel = ( p_blockY + y ) * p_width + p_blockX;

			for ( uint x = 0; x < 4; x++ ) {
				BGR& color = p_colors[curPixel];
				uint8& alpha = p_alphas[curPixel];

				uint index = ( indices & 3 );
				::memcpy( &color, &colors[index], sizeof( color ) );
				alpha = alphas[blockAlpha & 7];

				curPixel++;
				indices >>= 2;
				blockAlpha >>= 3;
			}
		}
	}

	void ImageReader::process3DCX( const RGBA* p_data, uint p_width, uint p_height, BGR*& po_colors, uint8*& po_alphas ) const {
		uint numPixels = ( p_width * p_height );
		uint numBlocks = numPixels >> 4;
		const DCXBlock* blocks = reinterpret_cast<const DCXBlock*>( p_data );

		po_colors = allocate<BGR>( numPixels );
		po_alphas = nullptr; // 3DCX does not use alpha

		const uint numHorizBlocks = p_width >> 2;
		const uint numVertBlocks = p_height >> 2;

#pragma omp parallel for
		for ( int y = 0; y < static_cast<int>( numVertBlocks ); y++ ) {
			for ( uint x = 0; x < numHorizBlocks; x++ ) {
				const DCXBlock& block = blocks[( y * numHorizBlocks ) + x];
				// 3DCX actually uses RGB and not BGR, so *pretend* that's what the output is
				this->process3DCXBlock( reinterpret_cast<RGB*>( po_colors ), block, x * 4, y * 4, p_width );
			}
		}
	}

	void ImageReader::process3DCXBlock( RGB* p_colors, const DCXBlock& p_block, uint p_blockX, uint p_blockY, uint p_width ) const {
		const float floatToByte = 127.5f;
		const float byteToFloat = ( 1.0f / floatToByte );

		uint64 red = p_block.red;
		uint64 green = p_block.green;
		uint8 reds[8];
		uint8 greens[8];

		// Reds 1 and 2
		reds[0] = ( red & 0xff );
		reds[1] = ( red & 0xff00 ) >> 8;
		red >>= 16;
		// Reds 3 to 8
		if ( reds[0] > reds[1] ) {
			for ( uint i = 2; i < 8; i++ ) {
				reds[i] = ( ( 8 - i ) * reds[0] + ( i - 1 ) * reds[1] ) / 7;
			}
		} else {
			for ( uint i = 2; i < 6; i++ ) {
				reds[i] = ( ( 6 - i ) * reds[0] + ( i - 1 ) * reds[1] ) / 5;
			}
			reds[6] = 0x00;
			reds[7] = 0xff;
		}
		// Greens 1 and 2
		greens[0] = ( green & 0xff );
		greens[1] = ( green & 0xff00 ) >> 8;
		green >>= 16;
		//Greens 3 to 8
		if ( greens[0] > greens[1] ) {
			for ( uint i = 2; i < 8; i++ ) {
				greens[i] = ( ( 8 - i ) * greens[0] + ( i - 1 ) * greens[1] ) / 7;
			}
		} else {
			for ( uint i = 2; i < 6; i++ ) {
				greens[i] = ( ( 6 - i ) * greens[0] + ( i - 1 ) * greens[1] ) / 5;
			}
			greens[6] = 0x00;
			greens[7] = 0xff;
		}

		struct {
			float r; float g; float b;
		} normal;
		for ( uint y = 0; y < 4; y++ ) {
			uint curPixel = ( p_blockY + y ) * p_width + p_blockX;

			for ( uint x = 0; x < 4; x++ ) {
				RGB& color = p_colors[curPixel];

				// Get normal
				normal.r = ( ( float ) reds[red & 7] * byteToFloat ) - 1.0f;
				normal.g = ( ( float ) greens[green & 7] * byteToFloat ) - 1.0f;

				// Compute blue, based on red/green
				normal.b = ::sqrt( 1.0f - normal.r * normal.r - normal.g * normal.g );

				// Store normal
				color.r = ( ( normal.r + 1.0f ) * floatToByte );
				color.g = ( ( normal.g + 1.0f ) * floatToByte );
				color.b = ( ( normal.b + 1.0f ) * floatToByte );

				// Invert green as that seems to be the more common format
				color.g = 0xff - color.g;

				curPixel++;
				red >>= 3;
				green >>= 3;
			}
		}
	}

}; // namespace gw2b
