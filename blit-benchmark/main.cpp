#include <benchmark/benchmark.h>

#include <algorithm>

#include <cassert>

#include "../draw2d/image.hpp"
#include "../draw2d/draw-ex.hpp"
#include "../draw2d/surface-ex.hpp"

namespace
{
	void blit_masked_benchmark_( benchmark::State& aState )
	{
		auto const width = std::uint32_t(aState.range(0));
		auto const height = std::uint32_t(aState.range(1));

		SurfaceEx surface( width, height );
		surface.clear();

		auto source = load_image( "assets/earth.png" );
		assert( source );

		// Position image at center to maximize visible area
		float posX = width * 0.5f;
		float posY = height * 0.5f;

		for( auto _ : aState )
		{
			blit_masked( surface, *source, {posX, posY} );
			benchmark::ClobberMemory(); 
		}

		auto const maxBlitX = std::min( width, source->get_width() );
		auto const maxBlitY = std::min( height, source->get_height() );
		aState.SetBytesProcessed( 2*maxBlitX*maxBlitY*4 * aState.iterations() );
	}

	void blit_ex_solid_benchmark_( benchmark::State& aState )
	{
		auto const width = std::uint32_t(aState.range(0));
		auto const height = std::uint32_t(aState.range(1));

		SurfaceEx surface( width, height );
		surface.clear();

		auto source = load_image( "assets/earth.png" );
		assert( source );

		float posX = width * 0.5f;
		float posY = height * 0.5f;

		for( auto _ : aState )
		{
			blit_ex_solid( surface, *source, {posX, posY} );
			benchmark::ClobberMemory(); 
		}

		auto const maxBlitX = std::min( width, source->get_width() );
		auto const maxBlitY = std::min( height, source->get_height() );
		aState.SetBytesProcessed( 2*maxBlitX*maxBlitY*4 * aState.iterations() );
	}

	void blit_ex_memcpy_benchmark_( benchmark::State& aState )
	{
		auto const width = std::uint32_t(aState.range(0));
		auto const height = std::uint32_t(aState.range(1));

		SurfaceEx surface( width, height );
		surface.clear();

		auto source = load_image( "assets/earth.png" );
		assert( source );

		float posX = width * 0.5f;
		float posY = height * 0.5f;

		for( auto _ : aState )
		{
			blit_ex_memcpy( surface, *source, {posX, posY} );
			benchmark::ClobberMemory(); 
		}

		auto const maxBlitX = std::min( width, source->get_width() );
		auto const maxBlitY = std::min( height, source->get_height() );
		aState.SetBytesProcessed( 2*maxBlitX*maxBlitY*4 * aState.iterations() );
	}
}

// Benchmark all three variants with different surface sizes
BENCHMARK( blit_masked_benchmark_ )
	->Args( { 640, 480 } )
	->Args( { 1920, 1080 } )
	->Args( { 3840, 2160 } )
	->Args( { 7680, 4320 } )
	->Unit( benchmark::kMicrosecond );

BENCHMARK( blit_ex_solid_benchmark_ )
	->Args( { 640, 480 } )
	->Args( { 1920, 1080 } )
	->Args( { 3840, 2160 } )
	->Args( { 7680, 4320 } )
	->Unit( benchmark::kMicrosecond );

BENCHMARK( blit_ex_memcpy_benchmark_ )
	->Args( { 640, 480 } )
	->Args( { 1920, 1080 } )
	->Args( { 3840, 2160 } )
	->Args( { 7680, 4320 } )
	->Unit( benchmark::kMicrosecond );

BENCHMARK_MAIN();
