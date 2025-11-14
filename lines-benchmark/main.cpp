#include <benchmark/benchmark.h>
#include <cmath>

#include "../draw2d/draw.hpp"
#include "../draw2d/draw-ex.hpp"
#include "../draw2d/surface-ex.hpp"

namespace
{
	// Benchmark DDA algorithm (floating-point)
	void benchmark_dda_( benchmark::State& aState )
	{
		auto const width = std::uint32_t(aState.range(0));
		auto const height = std::uint32_t(aState.range(1));
		auto const lineLength = std::uint32_t(aState.range(2)); // Line length in pixels

		SurfaceEx surface( width, height );
		surface.clear();

		ColorU8_sRGB color{ 255, 255, 255 };

		// Draw diagonal line from top-left to bottom-right
		Vec2f begin{ 10.f, 10.f };
		Vec2f end{ 10.f + lineLength, 10.f + lineLength };

		for( auto _ : aState )
		{
			draw_ex_line_solid( surface, begin, end, color );
			benchmark::ClobberMemory(); 
		}

		// Set bytes processed: approximately lineLength pixels * 4 bytes per pixel
		aState.SetBytesProcessed( lineLength * 4 * aState.iterations() );
	}

	// Benchmark Bresenham algorithm (integer-only)
	void benchmark_bresenham_( benchmark::State& aState )
	{
		auto const width = std::uint32_t(aState.range(0));
		auto const height = std::uint32_t(aState.range(1));
		auto const lineLength = std::uint32_t(aState.range(2));

		SurfaceEx surface( width, height );
		surface.clear();

		ColorU8_sRGB color{ 255, 255, 255 };

		Vec2f begin{ 10.f, 10.f };
		Vec2f end{ 10.f + lineLength, 10.f + lineLength };

		for( auto _ : aState )
		{
			draw_ex_line_bresenham( surface, begin, end, color );
			benchmark::ClobberMemory(); 
		}

		aState.SetBytesProcessed( lineLength * 4 * aState.iterations() );
	}

	// Benchmark diagonal baseline (optimal case)
	void benchmark_diagonal_( benchmark::State& aState )
	{
		auto const width = std::uint32_t(aState.range(0));
		auto const height = std::uint32_t(aState.range(1));
		auto const lineLength = std::uint32_t(aState.range(2));

		SurfaceEx surface( width, height );
		surface.clear();

		ColorU8_sRGB color{ 255, 255, 255 };

		Vec2f begin{ 10.f, 10.f };
		float steps = static_cast<float>(lineLength);

		for( auto _ : aState )
		{
			draw_ex_diagonal( surface, begin, steps, color );
			benchmark::ClobberMemory(); 
		}

		aState.SetBytesProcessed( lineLength * 4 * aState.iterations() );
	}

	// Benchmark horizontal lines
	void benchmark_dda_horizontal_( benchmark::State& aState )
	{
		auto const width = std::uint32_t(aState.range(0));
		auto const height = std::uint32_t(aState.range(1));
		auto const lineLength = std::uint32_t(aState.range(2));

		SurfaceEx surface( width, height );
		surface.clear();

		ColorU8_sRGB color{ 255, 255, 255 };

		Vec2f begin{ 10.f, 100.f };
		Vec2f end{ 10.f + lineLength, 100.f };

		for( auto _ : aState )
		{
			draw_ex_line_solid( surface, begin, end, color );
			benchmark::ClobberMemory(); 
		}

		aState.SetBytesProcessed( lineLength * 4 * aState.iterations() );
	}

	void benchmark_bresenham_horizontal_( benchmark::State& aState )
	{
		auto const width = std::uint32_t(aState.range(0));
		auto const height = std::uint32_t(aState.range(1));
		auto const lineLength = std::uint32_t(aState.range(2));

		SurfaceEx surface( width, height );
		surface.clear();

		ColorU8_sRGB color{ 255, 255, 255 };

		Vec2f begin{ 10.f, 100.f };
		Vec2f end{ 10.f + lineLength, 100.f };

		for( auto _ : aState )
		{
			draw_ex_line_bresenham( surface, begin, end, color );
			benchmark::ClobberMemory(); 
		}

		aState.SetBytesProcessed( lineLength * 4 * aState.iterations() );
	}
}

// Benchmark diagonal lines of varying lengths
BENCHMARK( benchmark_dda_ )
	->ArgsProduct({
		{1920, 1080},      // Surface size
		{100, 500, 1000, 2000, 5000}  // Line lengths
	})
	->Unit( benchmark::kNanosecond );

BENCHMARK( benchmark_bresenham_ )
	->ArgsProduct({
		{1920, 1080},
		{100, 500, 1000, 2000, 5000}
	})
	->Unit( benchmark::kNanosecond );

BENCHMARK( benchmark_diagonal_ )
	->ArgsProduct({
		{1920, 1080},
		{100, 500, 1000, 2000, 5000}
	})
	->Unit( benchmark::kNanosecond );

// Benchmark horizontal lines (special case)
BENCHMARK( benchmark_dda_horizontal_ )
	->ArgsProduct({
		{1920, 1080},
		{100, 500, 1000, 2000, 5000}
	})
	->Unit( benchmark::kNanosecond );

BENCHMARK( benchmark_bresenham_horizontal_ )
	->ArgsProduct({
		{1920, 1080},
		{100, 500, 1000, 2000, 5000}
	})
	->Unit( benchmark::kNanosecond );

BENCHMARK_MAIN();
