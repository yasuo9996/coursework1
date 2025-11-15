#include "draw.hpp"
#include "surface.hpp"
#include "color.hpp"
#include <algorithm>
#include <cmath>

bool clip_line( Rect2F const& aTargetArea, Vec2f& aBegin, Vec2f& aEnd )
{
	float xmin = aTargetArea.xmin;
	float xmax = aTargetArea.xmin + aTargetArea.width;
	float ymin = aTargetArea.ymin;
	float ymax = aTargetArea.ymin + aTargetArea.height;
	
	// Check if line segment endpoints are within the clipping region
// Note: Use strict less-than comparison because pixel indices range from 0 to width-1/height-1
	bool begin_inside = (aBegin.x >= xmin && aBegin.x < xmax && aBegin.y >= ymin && aBegin.y < ymax);
	bool end_inside = (aEnd.x >= xmin && aEnd.x < xmax && aEnd.y >= ymin && aEnd.y < ymax);
	
	// Case 1: Both endpoints are within the clipping region
	if (begin_inside && end_inside) {
		return true;
	}
	
	// Handle vertical lines (same x-coordinate)
	if (aBegin.x == aEnd.x) {
		// If the vertical line is outside the clipping region, return false
		if (aBegin.x < xmin || aBegin.x >= xmax) return false;
		
		// Calculate the visible portion of the line segment on the y-axis
		float y_start = std::max(ymin, std::min(aBegin.y, aEnd.y));
		float y_end = std::min(ymax, std::max(aBegin.y, aEnd.y));
		
		// If the line segment is completely outside the clipping region, return false
		if (y_start >= y_end) return false;
		
		// Update the y-coordinates of the line segment to the visible portion
		aBegin.y = y_start;
		aEnd.y = y_end;
		return true;
	}
	
	// Handle horizontal lines (same y-coordinate)
	if (aBegin.y == aEnd.y) {
		// If the horizontal line is outside the clipping region, return false
		if (aBegin.y < ymin || aBegin.y >= ymax) return false;
		
		// Calculate the visible portion of the line segment on the x-axis
		float x_start = std::max(xmin, std::min(aBegin.x, aEnd.x));
		float x_end = std::min(xmax, std::max(aBegin.x, aEnd.x));
		
		// If the line segment is completely outside the clipping region, return false
		if (x_start >= x_end) return false;
		
		// Update the x-coordinates of the line segment to the visible portion
		aBegin.x = x_start;
		aEnd.x = x_end;
		return true;
	}
	
	// General line segment clipping - using Cohen-Sutherland algorithm
	float dx = aEnd.x - aBegin.x;
	float dy = aEnd.y - aBegin.y;
	
	// If the line segment is a point, return false directly
	if (dx == 0 && dy == 0) {
		return false;
	}
	
	// Calculate intersection parameters with the four boundaries
	float t_enter = 0.0f;  // Parameter for entering the clipping region
	float t_exit = 1.0f;   // Parameter for exiting the clipping region
	
	// Intersections with left and right boundaries
	if (dx != 0) {
		float t_left = (xmin - aBegin.x) / dx;
		float t_right = (xmax - aBegin.x) / dx;
		
		if (dx > 0) {
			t_enter = std::max(t_enter, t_left);
			t_exit = std::min(t_exit, t_right);
		} else {
			t_enter = std::max(t_enter, t_right);
			t_exit = std::min(t_exit, t_left);
		}
	}
	
	// Intersections with top and bottom boundaries
	if (dy != 0) {
		float t_bottom = (ymin - aBegin.y) / dy;
		float t_top = (ymax - aBegin.y) / dy;
		
		if (dy > 0) {
			t_enter = std::max(t_enter, t_bottom);
			t_exit = std::min(t_exit, t_top);
		} else {
			t_enter = std::max(t_enter, t_top);
			t_exit = std::min(t_exit, t_bottom);
		}
	}
	
	// Check if there are valid intersections
	if (t_enter <= t_exit && t_enter >= 0 && t_exit <= 1) {
		// Calculate the clipped line segment endpoints
		float new_begin_x = aBegin.x + t_enter * dx;
		float new_begin_y = aBegin.y + t_enter * dy;
		float new_end_x = aBegin.x + t_exit * dx;
		float new_end_y = aBegin.y + t_exit * dy;
		
		// Update the line segment endpoints
		aBegin.x = new_begin_x;
		aBegin.y = new_begin_y;
		aEnd.x = new_end_x;
		aEnd.y = new_end_y;
		
		return true;
	}
	
	// No valid intersections, line segment is completely outside the clipping region
	return false;
}

void draw_clip_line_solid( Surface& aSurface, Vec2f aBegin, Vec2f aEnd, ColorU8_sRGB aColor )
{
	// Use Bresenham algorithm to draw single-pixel width continuous lines
	auto const surfaceWidth  = static_cast<int>( aSurface.get_width() );
	auto const surfaceHeight = static_cast<int>( aSurface.get_height() );

	auto clamp_to_surface_x = [surfaceWidth]( float value ) -> int
	{
		int v = static_cast<int>( std::lround( value ) );
		if( v < 0 )
			return 0;
		if( v >= surfaceWidth )
			return surfaceWidth - 1;
		return v;
	};

	auto clamp_to_surface_y = [surfaceHeight]( float value ) -> int
	{
		int v = static_cast<int>( std::lround( value ) );
		if( v < 0 )
			return 0;
		if( v >= surfaceHeight )
			return surfaceHeight - 1;
		return v;
	};

	int x0 = clamp_to_surface_x( aBegin.x );
	int y0 = clamp_to_surface_y( aBegin.y );
	int x1 = clamp_to_surface_x( aEnd.x );
	int y1 = clamp_to_surface_y( aEnd.y );
	
	// Calculate coordinate differences
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	bool steep = false;
	
	if( dy > dx ){
		std::swap( x0, y0 );
		std::swap( x1, y1 );
		std::swap( dx, dy );
		steep = true;
	}

	if( x0 > x1 ){
		std::swap( x0, x1 );
		std::swap( y0, y1 );
	}
	
	// 
	int p = 2 * dy - dx;
	const int stepY = (y0 < y1) ? 1 : -1;

	
	//
	while (!steep && x0 <= x1) {
		// draw pixels
		aSurface.set_pixel_srgb(x0, y0, aColor);
		if(p >= 0){
			y0 += stepY;
			p -= 2 * dx;
		}
		x0 += 1;
		p += 2 * dy;
	}
	while(steep && x0 <= x1){
		aSurface.set_pixel_srgb(y0, x0, aColor);
		if(p >= 0){
			y0 += stepY;
			p -= 2 * dx;
		}
		x0 += 1;
		p += 2 * dy;
	}
}


void draw_line_solid( Surface& aSurface, Vec2f aBegin, Vec2f aEnd, ColorU8_sRGB aColor )
{
	if( clip_line( aSurface.clip_area(), aBegin, aEnd ) )
		draw_clip_line_solid( aSurface, aBegin, aEnd, aColor );
}
void draw_line_solid( Surface& aSurface, Rect2F const& aClipArea, Vec2f aBegin, Vec2f aEnd, ColorU8_sRGB aColor )
{
	if( clip_line( aClipArea, aBegin, aEnd ) )
		draw_clip_line_solid( aSurface, aBegin, aEnd, aColor );
}


void draw_triangle_interp( Surface& aSurface, Vec2f aP0, Vec2f aP1, Vec2f aP2, ColorF aC0, ColorF aC1, ColorF aC2 )
{
	// Calculate the bounding box of the triangle
	float min_x = std::min({aP0.x, aP1.x, aP2.x});
	float max_x = std::max({aP0.x, aP1.x, aP2.x});
	float min_y = std::min({aP0.y, aP1.y, aP2.y});
	float max_y = std::max({aP0.y, aP1.y, aP2.y});
	
	// Intersect the bounding box with the clipping region
	Rect2F clip_area = aSurface.clip_area();
	float clip_xmax = clip_area.xmin + clip_area.width;
	float clip_ymax = clip_area.ymin + clip_area.height;
	min_x = std::max(min_x, clip_area.xmin);
	max_x = std::min(max_x, clip_xmax);
	min_y = std::max(min_y, clip_area.ymin);
	max_y = std::min(max_y, clip_ymax);
	
	// Convert to integer pixel coordinates
	// Use floor to ensure all potentially covered pixels are included
	int start_x = static_cast<int>(std::floor(min_x));
	int end_x = static_cast<int>(std::floor(max_x));
	int start_y = static_cast<int>(std::floor(min_y));
	int end_y = static_cast<int>(std::floor(max_y));
	
	// Ensure coordinates are within surface bounds (0 <= x < width, 0 <= y < height)
	start_x = std::max(0, start_x);
	end_x = std::min(static_cast<int>(aSurface.get_width() - 1), end_x);
	start_y = std::max(0, start_y);
	end_y = std::min(static_cast<int>(aSurface.get_height() - 1), end_y);
	
	// Calculate triangle area (for barycentric coordinates)
	float area = (aP1.y - aP2.y) * (aP0.x - aP2.x) + (aP2.x - aP1.x) * (aP0.y - aP2.y);
	
	// If area is 0, triangle is degenerate, don't draw
	if (std::abs(area) < 1e-6f) {
		return;
	}
	
	float inv_area = 1.0f / area;
	
	// Iterate through each pixel in the bounding box
	for (int y = start_y; y <= end_y; ++y) {
		for (int x = start_x; x <= end_x; ++x) {
			// Current pixel center point
			Vec2f pixel_center = Vec2f{static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f};
			
			// Calculate barycentric coordinates
			float w0 = ((aP1.y - aP2.y) * (pixel_center.x - aP2.x) + (aP2.x - aP1.x) * (pixel_center.y - aP2.y)) * inv_area;
			float w1 = ((aP2.y - aP0.y) * (pixel_center.x - aP0.x) + (aP0.x - aP2.x) * (pixel_center.y - aP0.y)) * inv_area;
			float w2 = 1.0f - w0 - w1;
			
			// Check if pixel is inside the triangle
			if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) {
				// Inside the triangle, perform color interpolation
				ColorF interpolated_color{
					w0 * aC0.r + w1 * aC1.r + w2 * aC2.r,
					w0 * aC0.g + w1 * aC1.g + w2 * aC2.g,
					w0 * aC0.b + w1 * aC1.b + w2 * aC2.b
				};
				
				// Ensure color values are within valid range
				interpolated_color.r = std::max(0.0f, std::min(1.0f, interpolated_color.r));
				interpolated_color.g = std::max(0.0f, std::min(1.0f, interpolated_color.g));
				interpolated_color.b = std::max(0.0f, std::min(1.0f, interpolated_color.b));
				
				// Convert to sRGB and draw the pixel
				ColorU8_sRGB final_color = linear_to_srgb(interpolated_color);
				aSurface.set_pixel_srgb(x, y, final_color);
			}
		}
	}
}

// You are not required to implement the following, but they can be useful for
// debugging.
void draw_triangle_wireframe( Surface& aSurface, Vec2f aP0, Vec2f aP1, Vec2f aP2, ColorU8_sRGB aColor )
{
	//TODO: your implementation goes here
	//TODO: your implementation goes here
	//TODO: your implementation goes here

	//TODO: remove the following when you start your implementation
	(void)aSurface; // Avoid warnings about unused arguments until the function
	(void)aP0;   // is properly implemented.
	(void)aP1;
	(void)aP2;
	(void)aColor;
}

void draw_triangle_solid( Surface& aSurface, Vec2f aP0, Vec2f aP1, Vec2f aP2, ColorU8_sRGB aColor )
{
	//TODO: your implementation goes here
	//TODO: your implementation goes here
	//TODO: your implementation goes here

	//TODO: remove the following when you start your implementation
	(void)aSurface; // Avoid warnings about unused arguments until the function
	(void)aP0;   // is properly implemented.
	(void)aP1;
	(void)aP2;
	(void)aColor;
}

void draw_rectangle_solid( Surface& aSurface, Vec2f aMinCorner, Vec2f aMaxCorner, ColorU8_sRGB aColor )
{
	//TODO: your implementation goes here
	//TODO: your implementation goes here
	//TODO: your implementation goes here

	//TODO: remove the following when you start your implementation
	(void)aSurface; // Avoid warnings about unused arguments until the function
	(void)aMinCorner;   // is properly implemented.
	(void)aMaxCorner;
	(void)aColor;
}

void draw_rectangle_outline( Surface& aSurface, Vec2f aMinCorner, Vec2f aMaxCorner, ColorU8_sRGB aColor )
{
	//TODO: your implementation goes here
	//TODO: your implementation goes here
	//TODO: your implementation goes here

	//TODO: remove the following when you start your implementation
	(void)aSurface; // Avoid warnings about unused arguments
	(void)aMinCorner;
	(void)aMaxCorner;
	(void)aColor;
}
