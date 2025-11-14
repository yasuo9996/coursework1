#include <catch2/catch_amalgamated.hpp>

#include "helpers.hpp"

#include "../draw2d/surface.hpp"
#include "../draw2d/draw.hpp"
#include "../draw2d/color.hpp"


TEST_CASE( "Scenario1: Triangle Clipping", "[clipping][triangle]" )
{
	Surface surface( 320, 240 );
	surface.clear();

	SECTION( "Triangle partially inside - left edge" )
	{
		// Triangle with one vertex outside left edge, two vertices inside
		draw_triangle_interp( surface,
			{ -50.f, 120.f }, { 100.f, 50.f }, { 100.f, 190.f },
			{ 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f }
		);

		// Should have some pixels drawn (clipped portion)
		auto const col = find_most_red_pixel( surface );
		REQUIRE( (col.r > 0 || col.g > 0 || col.b > 0) );
	}

	SECTION( "Triangle partially inside - right edge" )
	{
		surface.clear();
		
		// Triangle with one vertex outside right edge, two vertices inside
		draw_triangle_interp( surface,
			{ 370.f, 120.f }, { 220.f, 50.f }, { 220.f, 190.f },
			{ 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f }
		);

		// Should have some pixels drawn (clipped portion)
		auto const col = find_most_red_pixel( surface );
		REQUIRE( (col.r > 0 || col.g > 0 || col.b > 0) );
	}

	SECTION( "Triangle partially inside - top edge" )
	{
		surface.clear();
		
		// Triangle with one vertex outside top edge, two vertices inside
		draw_triangle_interp( surface,
			{ 160.f, -50.f }, { 50.f, 100.f }, { 270.f, 100.f },
			{ 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f }
		);

		// Should have some pixels drawn (clipped portion)
		auto const col = find_most_red_pixel( surface );
		REQUIRE( (col.r > 0 || col.g > 0 || col.b > 0) );
	}

	SECTION( "Triangle partially inside - bottom edge" )
	{
		surface.clear();
		
		// Triangle with one vertex outside bottom edge, two vertices inside
		draw_triangle_interp( surface,
			{ 160.f, 290.f }, { 50.f, 140.f }, { 270.f, 140.f },
			{ 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f }
		);

		// Should have some pixels drawn (clipped portion)
		auto const col = find_most_red_pixel( surface );
		REQUIRE( (col.r > 0 || col.g > 0 || col.b > 0) );
	}
}

TEST_CASE( "Scenario2: Color Interpolation Accuracy", "[interpolation][color]" )
{
	Surface surface( 320, 240 );
	surface.clear();

	SECTION( "Linear gradient - horizontal" )
	{
		// Triangle with horizontal color gradient
		draw_triangle_interp( surface,
			{ 10.f, 230.f }, { 10.f, 10.f }, { 310.f, 10.f },
			{ 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f }
		);

		// Check that we have a range of colors (not just one color)
		auto const max_col = find_most_red_pixel( surface );
		auto const min_col = find_least_red_nonzero_pixel( surface );
		
		// Should have different colors indicating interpolation worked
		REQUIRE( (max_col.r != min_col.r || max_col.g != min_col.g || max_col.b != min_col.b) );
	}

	SECTION( "Linear gradient - vertical" )
	{
		surface.clear();
		
		// Triangle with vertical color gradient
		draw_triangle_interp( surface,
			{ 10.f, 230.f }, { 310.f, 230.f }, { 310.f, 10.f },
			{ 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f }
		);

		auto const max_col = find_most_red_pixel( surface );
		auto const min_col = find_least_red_nonzero_pixel( surface );
		
		// Should have different colors indicating interpolation worked
		REQUIRE( (max_col.r != min_col.r || max_col.g != min_col.g || max_col.b != min_col.b) );
	}

	SECTION( "Uniform color triangle" )
	{
		surface.clear();
		
		// Triangle with uniform color (all vertices same)
		draw_triangle_interp( surface,
			{ 50.f, 200.f }, { 150.f, 50.f }, { 250.f, 200.f },
			{ 0.5f, 0.3f, 0.7f }, { 0.5f, 0.3f, 0.7f }, { 0.5f, 0.3f, 0.7f }
		);

		auto const max_col = find_most_red_pixel( surface );
		auto const min_col = find_least_red_nonzero_pixel( surface );
		
		// For uniform color triangle, max and min should be very similar
		REQUIRE( std::abs(int(max_col.r) - int(min_col.r)) <= 1 );
		REQUIRE( std::abs(int(max_col.g) - int(min_col.g)) <= 1 );
		REQUIRE( std::abs(int(max_col.b) - int(min_col.b)) <= 1 );
	}

	SECTION( "Extreme color values" )
	{
		surface.clear();
		
		// Triangle with extreme color values (0 and 1)
		draw_triangle_interp( surface,
			{ 10.f, 230.f }, { 10.f, 10.f }, { 310.f, 10.f },
			{ 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f }, { 0.5f, 0.5f, 0.5f }
		);

		auto const max_col = find_most_red_pixel( surface );
		auto const min_col = find_least_red_nonzero_pixel( surface );
		
		// Should have both very dark and very light pixels
		REQUIRE( max_col.r >= 250 ); // Nearly white
		REQUIRE( min_col.r <= 5 );   // Nearly black
	}
}

{
	Surface surface( 320, 240 );
	surface.clear();

	SECTION( "Triangle spanning all four boundaries" )
	{
		// Large triangle that crosses top, bottom, left, and right edges
		draw_triangle_interp( surface,
			{ -50.f, 120.f }, { 370.f, 120.f }, { 160.f, -50.f },
			{ 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f }
		);

		// Should have pixels drawn in all regions
		auto const col = find_most_red_pixel( surface );
		bool has_pixels = (col.r > 0) || (col.g > 0) || (col.b > 0);
		REQUIRE( has_pixels );
	}

	SECTION( "Triangle with two vertices outside opposite corners" )
	{
		surface.clear();

		// Triangle with vertices outside top-left and bottom-right corners
		draw_triangle_interp( surface,
			{ -30.f, -30.f }, { 350.f, 270.f }, { 160.f, 120.f },
			{ 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f }
		);

		// Should clip correctly and show interpolated colors
		auto const max_col = find_most_red_pixel( surface );
		auto const min_col = find_least_red_nonzero_pixel( surface );
		bool has_variation = (max_col.r != min_col.r) || (max_col.g != min_col.g) || (max_col.b != min_col.b);
		REQUIRE( has_variation );
	}

	SECTION( "Narrow triangle crossing horizontal boundaries" )
	{
		surface.clear();

		// Very narrow triangle crossing top and bottom
		draw_triangle_interp( surface,
			{ 160.f, -100.f }, { 160.f, 340.f }, { 170.f, 120.f },
			{ 0.5f, 0.5f, 0.5f }, { 0.3f, 0.3f, 0.3f }, { 0.7f, 0.7f, 0.7f }
		);

		// Should have continuous vertical line of pixels
		auto const col = find_most_red_pixel( surface );
		bool has_pixels = (col.r > 0) || (col.g > 0) || (col.b > 0);
		REQUIRE( has_pixels );
	}

	SECTION( "Wide triangle crossing vertical boundaries" )
	{
		surface.clear();

		// Wide triangle crossing left and right edges
		draw_triangle_interp( surface,
			{ -80.f, 120.f }, { 400.f, 120.f }, { 160.f, 60.f },
			{ 0.f, 1.f, 0.f }, { 0.f, 0.5f, 0.f }, { 0.f, 0.8f, 0.f }
		);

		// Should have continuous horizontal coverage
		auto const col = find_most_red_pixel( surface );
		bool has_pixels = (col.r > 0) || (col.g > 0) || (col.b > 0);
		REQUIRE( has_pixels );
	}
}