#include "image.hpp"

#include <memory>
#include <algorithm>

#include <cstdio>
#include <cstring>
#include <cassert>

#include <stb_image.h>

#include "surface.hpp"

#include "../support/error.hpp"

namespace
{
	struct STBImageRGBA_ : public ImageRGBA
	{
		STBImageRGBA_( Index, Index, std::uint8_t* );
		virtual ~STBImageRGBA_();
	};
}

ImageRGBA::ImageRGBA()
	: mWidth( 0 )
	, mHeight( 0 )
	, mData( nullptr )
{}

ImageRGBA::~ImageRGBA() = default;


std::unique_ptr<ImageRGBA> load_image( char const* aPath )
{
	assert( aPath );

	stbi_set_flip_vertically_on_load( true );

	int w, h, channels;
	stbi_uc* ptr = stbi_load( aPath, &w, &h, &channels, 4 );
	if( !ptr )
		throw Error( "Unable to load image \"{}\"", aPath );

	return std::make_unique<STBImageRGBA_>(
		ImageRGBA::Index(w),
		ImageRGBA::Index(h),
		ptr
	);
}

void blit_masked( Surface& aSurface, ImageRGBA const& aImage, Vec2f aPosition )
{
	// Get image dimensions
	ImageRGBA::Index imageWidth = aImage.get_width();
	ImageRGBA::Index imageHeight = aImage.get_height();
	
	// Calculate the top-left corner of the image (position is relative to center)
	float startX = aPosition.x - static_cast<float>(imageWidth) * 0.5f;
	float startY = aPosition.y - static_cast<float>(imageHeight) * 0.5f;
	
	// Convert to integer coordinates for pixel-level operations
	int intStartX = static_cast<int>(std::floor(startX));
	int intStartY = static_cast<int>(std::floor(startY));
	
	// Get surface dimensions
	Surface::Index surfaceWidth = aSurface.get_width();
	Surface::Index surfaceHeight = aSurface.get_height();
	
	// Calculate the visible region (clipping bounds)
	int visibleStartX = std::max(0, intStartX);
	int visibleEndX = std::min(static_cast<int>(surfaceWidth), intStartX + static_cast<int>(imageWidth));
	int visibleStartY = std::max(0, intStartY);
	int visibleEndY = std::min(static_cast<int>(surfaceHeight), intStartY + static_cast<int>(imageHeight));
	
	// Early exit if no visible region
	if (visibleStartX >= visibleEndX || visibleStartY >= visibleEndY)
		return;
	
	// Calculate the corresponding source image region
	int sourceStartX = visibleStartX - intStartX;
	int sourceStartY = visibleStartY - intStartY;
	int sourceEndX = sourceStartX + (visibleEndX - visibleStartX);
	int sourceEndY = sourceStartY + (visibleEndY - visibleStartY);
	
	// Iterate only through the visible region
	for (int y = sourceStartY; y < sourceEndY; ++y)
	{
		int surfaceY = visibleStartY + (y - sourceStartY);
		
		for (int x = sourceStartX; x < sourceEndX; ++x)
		{
			int surfaceX = visibleStartX + (x - sourceStartX);
			
			// Get the source pixel color
			ColorU8_sRGB_Alpha sourcePixel = aImage.get_pixel(
				static_cast<ImageRGBA::Index>(x), 
				static_cast<ImageRGBA::Index>(y)
			);
			
			// Skip pixels with alpha < 128 (discard transparent pixels)
			if (sourcePixel.a < 128)
				continue;
			
			// Convert to ColorU8_sRGB (without alpha) for the surface
			ColorU8_sRGB targetColor = { sourcePixel.r, sourcePixel.g, sourcePixel.b };
			
			// Set the pixel on the surface
			aSurface.set_pixel_srgb(
				static_cast<Surface::Index>(surfaceX), 
				static_cast<Surface::Index>(surfaceY), 
				targetColor
			);
		}
	}
}

namespace
{
	STBImageRGBA_::STBImageRGBA_( Index aWidth, Index aHeight, std::uint8_t* aPtr )
	{
		mWidth = aWidth;
		mHeight = aHeight;
		mData = aPtr;
	}

	STBImageRGBA_::~STBImageRGBA_()
	{
		if( mData )
			stbi_image_free( mData );
	}
}
