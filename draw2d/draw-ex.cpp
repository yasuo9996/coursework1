#include "draw-ex.hpp"

#include <algorithm>

#include <cstring> // for std::memcpy()

#include "draw.hpp"
#include "image.hpp"
#include "surface-ex.hpp"

void draw_ex_line_solid( SurfaceEx& aSurface, Vec2f aBegin, Vec2f aEnd, ColorU8_sRGB aColor )
{
	// Implementation using Digital Differential Analyzer (DDA) algorithm
	// Reference: Computer Graphics: Principles and Practice (Foley et al., 1996)
	// DDA uses floating-point arithmetic for simplicity and accuracy
	
	// Get surface dimensions for bounds checking
	SurfaceEx::Index surfaceWidth = aSurface.get_width();
	SurfaceEx::Index surfaceHeight = aSurface.get_height();
	
	// Get surface pointer for direct pixel access
	std::uint8_t* surfacePtr = aSurface.get_surface_ptr();
	std::size_t surfaceStride = 4 * surfaceWidth;
	
	// Calculate differences
	float dx = aEnd.x - aBegin.x;
	float dy = aEnd.y - aBegin.y;
	
	// Calculate number of steps (use the larger of dx or dy)
	float steps = std::max(std::abs(dx), std::abs(dy));
	
	// Avoid division by zero
	if (steps < 0.5f) {
		// Single pixel case
		int x = static_cast<int>(std::round(aBegin.x));
		int y = static_cast<int>(std::round(aBegin.y));
		if (x >= 0 && x < static_cast<int>(surfaceWidth) && 
		    y >= 0 && y < static_cast<int>(surfaceHeight)) {
			std::size_t pixelIndex = static_cast<std::size_t>(y) * surfaceStride + 4 * static_cast<std::size_t>(x);
			surfacePtr[pixelIndex + 0] = aColor.r;
			surfacePtr[pixelIndex + 1] = aColor.g;
			surfacePtr[pixelIndex + 2] = aColor.b;
			surfacePtr[pixelIndex + 3] = 0;
		}
		return;
	}
	
	// Calculate increments per step
	float xIncrement = dx / steps;
	float yIncrement = dy / steps;
	
	// Start from beginning point
	float x = aBegin.x;
	float y = aBegin.y;
	
	// Draw pixels along the line
	for (float i = 0; i <= steps; i += 1.0f) {
		int px = static_cast<int>(std::round(x));
		int py = static_cast<int>(std::round(y));
		
		// Check bounds before drawing
		if (px >= 0 && px < static_cast<int>(surfaceWidth) && 
		    py >= 0 && py < static_cast<int>(surfaceHeight)) {
			
			std::size_t pixelIndex = static_cast<std::size_t>(py) * surfaceStride + 4 * static_cast<std::size_t>(px);
			surfacePtr[pixelIndex + 0] = aColor.r;
			surfacePtr[pixelIndex + 1] = aColor.g;
			surfacePtr[pixelIndex + 2] = aColor.b;
			surfacePtr[pixelIndex + 3] = 0;
		}
		
		// Move to next point
		x += xIncrement;
		y += yIncrement;
	}
}

void draw_ex_line_bresenham( SurfaceEx& aSurface, Vec2f aBegin, Vec2f aEnd, ColorU8_sRGB aColor )
{
	// Implementation based on Bresenham's line algorithm
	// Reference: Bresenham, J. E. (1965). "Algorithm for computer control of a digital plotter"
	// This algorithm uses only integer arithmetic for efficiency
	
	// Convert floating-point coordinates to integers
	int x0 = static_cast<int>(std::round(aBegin.x));
	int y0 = static_cast<int>(std::round(aBegin.y));
	int x1 = static_cast<int>(std::round(aEnd.x));
	int y1 = static_cast<int>(std::round(aEnd.y));
	
	// Get surface dimensions for bounds checking
	SurfaceEx::Index surfaceWidth = aSurface.get_width();
	SurfaceEx::Index surfaceHeight = aSurface.get_height();
	
	// Get surface pointer for direct pixel access
	std::uint8_t* surfacePtr = aSurface.get_surface_ptr();
	std::size_t surfaceStride = 4 * surfaceWidth;
	
	// Calculate differences
	int dx = std::abs(x1 - x0);
	int dy = std::abs(y1 - y0);
	
	// Determine step directions
	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;
	
	// Initialize decision parameter
	int err = dx - dy;
	
	// Main drawing loop
	while (true) {
		// Check if current pixel is within surface bounds
		if (x0 >= 0 && x0 < static_cast<int>(surfaceWidth) && 
		    y0 >= 0 && y0 < static_cast<int>(surfaceHeight)) {
			
			// Calculate pixel position in surface buffer
			std::size_t pixelIndex = static_cast<std::size_t>(y0) * surfaceStride + 4 * static_cast<std::size_t>(x0);
			
			// Set pixel color (RGB channels)
			surfacePtr[pixelIndex + 0] = aColor.r;
			surfacePtr[pixelIndex + 1] = aColor.g;
			surfacePtr[pixelIndex + 2] = aColor.b;
			surfacePtr[pixelIndex + 3] = 0; // Alpha channel (not used)
		}
		
		// Check if we've reached the end point
		if (x0 == x1 && y0 == y1) break;
		
		// Calculate next pixel position using Bresenham's algorithm
		int e2 = 2 * err;
		
		if (e2 > -dy) {
			err -= dy;
			x0 += sx;
		}
		
		if (e2 < dx) {
			err += dx;
			y0 += sy;
		}
	}
}

void blit_ex_solid( SurfaceEx& aSurface, ImageRGBA const& aImage, Vec2f aPosition )
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
	SurfaceEx::Index surfaceWidth = aSurface.get_width();
	SurfaceEx::Index surfaceHeight = aSurface.get_height();
	
	// Get surface pointer for direct access
	std::uint8_t* surfacePtr = aSurface.get_surface_ptr();
	std::size_t surfaceStride = 4 * surfaceWidth;
	
	// Iterate through each pixel in the source image
	for (ImageRGBA::Index y = 0; y < imageHeight; ++y)
	{
		int surfaceY = intStartY + static_cast<int>(y);
		
		// Skip if this row is outside the surface bounds
		if (surfaceY < 0 || surfaceY >= static_cast<int>(surfaceHeight))
			continue;
		
		for (ImageRGBA::Index x = 0; x < imageWidth; ++x)
		{
			int surfaceX = intStartX + static_cast<int>(x);
			
			// Skip if this column is outside the surface bounds
			if (surfaceX < 0 || surfaceX >= static_cast<int>(surfaceWidth))
				continue;
			
			// Get the source pixel color
			ColorU8_sRGB_Alpha sourcePixel = aImage.get_pixel(x, y);
			
			// Calculate destination pixel position in surface
			std::size_t destIndex = static_cast<std::size_t>(surfaceY) * surfaceStride + 4 * static_cast<std::size_t>(surfaceX);
			
			// Copy RGB channels (ignore alpha - solid blit)
			surfacePtr[destIndex + 0] = sourcePixel.r;
			surfacePtr[destIndex + 1] = sourcePixel.g;
			surfacePtr[destIndex + 2] = sourcePixel.b;
			surfacePtr[destIndex + 3] = 0; // Alpha channel (not used)
		}
	}
}

void blit_ex_memcpy( SurfaceEx& aSurface, ImageRGBA const& aImage, Vec2f aPosition )
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
	SurfaceEx::Index surfaceWidth = aSurface.get_width();
	SurfaceEx::Index surfaceHeight = aSurface.get_height();
	
	// Get surface pointer for direct access
	std::uint8_t* surfacePtr = aSurface.get_surface_ptr();
	std::size_t surfaceStride = 4 * surfaceWidth;
	
	// Iterate through each row in the source image
	for (ImageRGBA::Index y = 0; y < imageHeight; ++y)
	{
		int surfaceY = intStartY + static_cast<int>(y);
		
		// Skip if this row is outside the surface bounds
		if (surfaceY < 0 || surfaceY >= static_cast<int>(surfaceHeight))
			continue;
		
		// Calculate the number of pixels that can be copied in this row
		int startCopyX = std::max(0, intStartX);
		int endCopyX = std::min(static_cast<int>(surfaceWidth), intStartX + static_cast<int>(imageWidth));
		
		// Skip if no pixels in this row are within surface bounds
		if (startCopyX >= endCopyX)
			continue;
		
		// Calculate source and destination offsets
		int sourceStartX = startCopyX - intStartX;
		std::size_t copyWidth = static_cast<std::size_t>(endCopyX - startCopyX);
		
		// Calculate source and destination pointers for this row
		std::uint8_t* destPtr = surfacePtr + static_cast<std::size_t>(surfaceY) * surfaceStride + 4 * static_cast<std::size_t>(startCopyX);
		
		// For each pixel in the row, copy RGB channels (ignore alpha)
		for (std::size_t x = 0; x < copyWidth; ++x)
		{
			// Get source pixel
			ColorU8_sRGB_Alpha sourcePixel = aImage.get_pixel(static_cast<ImageRGBA::Index>(sourceStartX + x), y);
			
			// Copy RGB channels using memcpy for the 3 bytes
			std::memcpy(destPtr + 4*x, &sourcePixel, 3);
			destPtr[4*x + 3] = 0; // Alpha channel (not used)
		}
	}
}

void draw_ex_diagonal( SurfaceEx& aSurface, Vec2f aBegin, float aSteps, ColorU8_sRGB aColor )
{
	std::size_t const steps = std::size_t(aSteps);
	std::size_t const stride = 4*aSurface.get_width();

	std::uint8_t* sptr = aSurface.get_surface_ptr();
	sptr += std::size_t(aBegin.y) * stride + 4*std::size_t(aBegin.x);

	for( std::size_t i = 0; i < steps; ++i )
	{
		sptr[0] = aColor.r;
		sptr[1] = aColor.g;
		sptr[2] = aColor.b;
		sptr[3] = 0;
		sptr += stride + 4;
	}
}
