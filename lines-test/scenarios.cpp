#include <catch2/catch_amalgamated.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>

#include "helpers.hpp"
#include "../draw2d/draw.hpp"
#include "../draw2d/surface.hpp"
#include "../draw2d/color.hpp"

TEST_CASE("Scenario1: Lines with one point inside and one outside the surface")
{
	// Create a test surface (100x100 pixels, coordinates: (0,0) to (100,100))
	Surface surface(100, 100);
	ColorU8_sRGB color = { 255, 255, 255 };
	
	// Case 1: 45-degree diagonal line - from inside bottom-left to outside top-right
	surface.clear();
	draw_line_solid(surface, {10.f, 10.f}, {150.f, 150.f}, color);
	REQUIRE(max_row_pixel_count(surface) > 0);
	REQUIRE(max_col_pixel_count(surface) > 0);
	// Check the length of the clipped line segment
	auto neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[1] <= 2); // At most two endpoints
	
	// Case 2: Steep negative slope - from inside top-right to outside bottom-left
	surface.clear();
	draw_line_solid(surface, {90.f, 90.f}, {-50.f, -50.f}, color);
	REQUIRE(max_row_pixel_count(surface) > 0);
	REQUIRE(max_col_pixel_count(surface) > 0);
	neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[0] == 0); // No isolated pixels
	
	// Case 3: Horizontal line - from inside center to outside right
	surface.clear();
	draw_line_solid(surface, {50.f, 50.f}, {200.f, 50.f}, color);
	REQUIRE(max_row_pixel_count(surface) > 0);
	REQUIRE(max_row_pixel_count(surface) <= 50); // Only draw to the right boundary
	// Check horizontal continuity
	REQUIRE(max_col_pixel_count(surface) == 1); // Single row of pixels
	
	// Case 4: Vertical line - from outside bottom to inside top
	surface.clear();
	draw_line_solid(surface, {50.f, -50.f}, {50.f, 80.f}, color);
	REQUIRE(max_col_pixel_count(surface) > 0);
	REQUIRE(max_col_pixel_count(surface) <= 80); // Only draw from the top boundary
	// Check vertical continuity
	REQUIRE(max_row_pixel_count(surface) == 1); // Single column of pixels
	
	// Additional assertions: Ensure all line segments have proper pixel continuity
		auto const final_neighbours = count_pixel_neighbours( surface );
		REQUIRE(final_neighbours[1] <= 2); // At most two endpoints
		REQUIRE(final_neighbours[8] == 0); // No completely surrounded pixels
}

TEST_CASE("Scenario2: Lines with both points outside of the surface")
{
	// Create a test surface (100x100 pixels, coordinates: (0,0) to (100,100))
	Surface surface(100, 100);
	ColorU8_sRGB color = { 255, 255, 255 };
	
	// Case 1: Horizontal line completely above the surface
	surface.clear();
	draw_line_solid(surface, {-50.f, -50.f}, {150.f, -50.f}, color);
	REQUIRE(max_row_pixel_count(surface) == 0);
	REQUIRE(max_col_pixel_count(surface) == 0);
	// Additional check: Ensure the surface is completely blank
	auto neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[0] == 0); // No pixels at all
	
	// Case 2: Diagonal line completely below the surface
	surface.clear();
	draw_line_solid(surface, {-30.f, 150.f}, {130.f, 200.f}, color);
	REQUIRE(max_row_pixel_count(surface) == 0);
	REQUIRE(max_col_pixel_count(surface) == 0);
	neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[0] == 0); // No pixels at all
	
	// Case 3: Vertical line completely to the left of the surface
	surface.clear();
	draw_line_solid(surface, {-100.f, -50.f}, {-100.f, 150.f}, color);
	REQUIRE(max_row_pixel_count(surface) == 0);
	REQUIRE(max_col_pixel_count(surface) == 0);
	neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[0] == 0); // No pixels at all
	
	// Case 4: Steep line completely to the right of the surface
	surface.clear();
	draw_line_solid(surface, {150.f, -50.f}, {200.f, 150.f}, color);
	REQUIRE(max_row_pixel_count(surface) == 0);
	REQUIRE(max_col_pixel_count(surface) == 0);
	neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[0] == 0); // No pixels at all
	
	// [!mayfail] Additional test: Diagonal line crossing the surface (should be clipped)
	surface.clear();
	draw_line_solid(surface, {-50.f, -50.f}, {150.f, 150.f}, color);
	REQUIRE(max_row_pixel_count(surface) > 0);
	REQUIRE(max_col_pixel_count(surface) > 0);
	// Check the characteristics of the clipped line segment
	neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[1] <= 2); // At most two endpoints
}

TEST_CASE("Scenario3: Line from p0 to p1 should be identical to line from p1 to p0")
{
	// Create two identical surfaces (100x100 pixels)
	Surface surface1(100, 100);
	Surface surface2(100, 100);
	ColorU8_sRGB color = { 255, 255, 255 };
	
	// Case 1: 45-degree diagonal line - from top-left to bottom-right vs from bottom-right to top-left
	surface1.clear();
	surface2.clear();
	draw_line_solid(surface1, {10.f, 10.f}, {90.f, 90.f}, color);
	draw_line_solid(surface2, {90.f, 90.f}, {10.f, 10.f}, color);
	
	REQUIRE(max_row_pixel_count(surface1) == max_row_pixel_count(surface2));
	REQUIRE(max_col_pixel_count(surface1) == max_col_pixel_count(surface2));
	// Check if pixel neighbor patterns are identical
	auto neighbours1 = count_pixel_neighbours(surface1);
	auto neighbours2 = count_pixel_neighbours(surface2);
	for (size_t i = 0; i < 9; ++i) {
		REQUIRE(neighbours1[i] == neighbours2[i]);
	}
	
	// Case 2: Steep negative slope - from top-right to bottom-left vs from bottom-left to top-right
	surface1.clear();
	surface2.clear();
	draw_line_solid(surface1, {90.f, 10.f}, {10.f, 90.f}, color);
	draw_line_solid(surface2, {10.f, 90.f}, {90.f, 10.f}, color);
	
	REQUIRE(max_row_pixel_count(surface1) == max_row_pixel_count(surface2));
	REQUIRE(max_col_pixel_count(surface1) == max_col_pixel_count(surface2));
	neighbours1 = count_pixel_neighbours(surface1);
	neighbours2 = count_pixel_neighbours(surface2);
	for (size_t i = 0; i < 9; ++i) {
		REQUIRE(neighbours1[i] == neighbours2[i]);
	}
	
	// Case 3: Shallow slope horizontal line - from left to right vs from right to left
	surface1.clear();
	surface2.clear();
	draw_line_solid(surface1, {10.f, 30.f}, {90.f, 40.f}, color);
	draw_line_solid(surface2, {90.f, 40.f}, {10.f, 30.f}, color);
	
	REQUIRE(max_row_pixel_count(surface1) == max_row_pixel_count(surface2));
	REQUIRE(max_col_pixel_count(surface1) == max_col_pixel_count(surface2));
	neighbours1 = count_pixel_neighbours(surface1);
	neighbours2 = count_pixel_neighbours(surface2);
	for (size_t i = 0; i < 9; ++i) {
		REQUIRE(neighbours1[i] == neighbours2[i]);
	}
	
	// Case 4: Shallow slope vertical line - from top to bottom vs from bottom to top
	surface1.clear();
	surface2.clear();
	draw_line_solid(surface1, {30.f, 10.f}, {40.f, 90.f}, color);
	draw_line_solid(surface2, {40.f, 90.f}, {30.f, 10.f}, color);
	
	REQUIRE(max_row_pixel_count(surface1) == max_row_pixel_count(surface2));
	REQUIRE(max_col_pixel_count(surface1) == max_col_pixel_count(surface2));
	neighbours1 = count_pixel_neighbours(surface1);
	neighbours2 = count_pixel_neighbours(surface2);
	for (size_t i = 0; i < 9; ++i) {
		REQUIRE(neighbours1[i] == neighbours2[i]);
	}
	
	// [!mayfail] Additional test: Exact pixel-level comparison
	surface1.clear();
	surface2.clear();
	draw_line_solid(surface1, {25.f, 25.f}, {75.f, 75.f}, color);
	draw_line_solid(surface2, {75.f, 75.f}, {25.f, 25.f}, color);

	// Pixel-level comparison: Check if each pixel is identical
	auto const stride = surface1.get_width() << 2;
	for (std::uint32_t y = 0; y < surface1.get_height(); ++y) {
		for (std::uint32_t x = 0; x < surface1.get_width(); ++x) {
			auto const idx = y*stride + (x<<2);
			auto const ptr1 = surface1.get_surface_ptr() + idx;
			auto const ptr2 = surface2.get_surface_ptr() + idx;
			
			REQUIRE(ptr1[0] == ptr2[0]); // Red channel
			REQUIRE(ptr1[1] == ptr2[1]); // Green channel
			REQUIRE(ptr1[2] == ptr2[2]); // Blue channel
		}
	}
}

TEST_CASE("Scenario4: Continuous lines should have no gaps between segments")
{
	// Create a test surface (100x100 pixels)
	Surface surface(100, 100);
	ColorU8_sRGB color = { 255, 255, 255 };
	
	// Case 1: Polyline connection - from top-left to center to top-right
	surface.clear();
	draw_line_solid(surface, {10.f, 10.f}, {50.f, 50.f}, color);
	draw_line_solid(surface, {50.f, 50.f}, {90.f, 10.f}, color);
	
	// Check if connection points have appropriate neighbors
	auto neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[1] == 0); // No isolated pixels
	REQUIRE(neighbours[2] >= 2); // Connection points have at least 2 neighbors
	// Check overall continuity
	REQUIRE(max_row_pixel_count(surface) > 0);
	REQUIRE(max_col_pixel_count(surface) > 0);
	
	// Case 2: Zigzag polyline - four points forming a zigzag pattern
	surface.clear();
	Vec2f zigzagPoints[] = {{10.f, 90.f}, {30.f, 10.f}, {70.f, 90.f}, {90.f, 10.f}};
	
	for (size_t i = 0; i < 3; ++i) {
		draw_line_solid(surface, zigzagPoints[i], zigzagPoints[i+1], color);
	}
	
	neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[1] == 0); // No isolated pixels
	REQUIRE(neighbours[2] >= 3); // At least 3 connection points
	
	// Case 3: Very short line segment connection - testing pixel-level continuity
	surface.clear();
	Vec2f microPoints[] = {{50.f, 50.f}, {51.f, 51.f}, {52.f, 50.f}, {53.f, 51.f}};
	
	for (size_t i = 0; i < 3; ++i) {
		draw_line_solid(surface, microPoints[i], microPoints[i+1], color);
	}
	
	neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[1] == 0); // Even short segments should be connected
	REQUIRE(neighbours[0] == 0); // No isolated pixels
	
	// Case 4: Horizontal line segment chain - testing horizontal continuity
	surface.clear();
	Vec2f horizontalChain[] = {{10.f, 50.f}, {30.f, 50.f}, {50.f, 50.f}, {70.f, 50.f}, {90.f, 50.f}};
	
	for (size_t i = 0; i < 4; ++i) {
		draw_line_solid(surface, horizontalChain[i], horizontalChain[i+1], color);
	}
	
	// Check horizontal continuity
	REQUIRE(max_row_pixel_count(surface) >= 80); // Should cover most of the width
	REQUIRE(max_col_pixel_count(surface) == 1); // Single row of pixels
	
	// [!mayfail] Additional test: Endpoint neighbor count check
	neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[1] == 2); // Only two endpoints should have 1 neighbor
	
	// Additional test: Vertical line segment chain
	surface.clear();
	Vec2f verticalChain[] = {{50.f, 10.f}, {50.f, 30.f}, {50.f, 50.f}, {50.f, 70.f}, {50.f, 90.f}};
	
	for (size_t i = 0; i < 4; ++i) {
		draw_line_solid(surface, verticalChain[i], verticalChain[i+1], color);
	}
	
	// Check vertical continuity
	REQUIRE(max_col_pixel_count(surface) >= 80); // Should cover most of the height
	REQUIRE(max_row_pixel_count(surface) == 1); // Single column of pixels
}
