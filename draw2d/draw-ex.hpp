#ifndef DRAW_EX_HPP_ADF54F6D_BF79_48F4_BB63_DACB47B0B960
#define DRAW_EX_HPP_ADF54F6D_BF79_48F4_BB63_DACB47B0B960

// IMPORTANT: DO NOT CHANGE THIS FILE WHEN YOU ARE SOLVING COURSEWORK 1!
//
// This file is only used in the two benchmarking tasks. Other tasks should not
// attempt to utilize it or its contents.

#include "forward.hpp"

#include "../vmlib/vec2.hpp"

void draw_ex_line_solid( 
	SurfaceEx&,
	Vec2f aBegin, Vec2f aEnd,
	ColorU8_sRGB
);

// Bresenham's line algorithm implementation for comparison
void draw_ex_line_bresenham( 
	SurfaceEx&,
	Vec2f aBegin, Vec2f aEnd,
	ColorU8_sRGB
);

void blit_ex_solid( SurfaceEx&, ImageRGBA const&, Vec2f aPosition );
void blit_ex_memcpy( SurfaceEx&, ImageRGBA const&, Vec2f aPosition );

// Warning: only valid for diagonal lines fully contained by SurfaceEx
// (only intended to be used as a baseline for benchmarking)
void draw_ex_diagonal( 
	SurfaceEx&, 
	Vec2f aBegin, float aNumPixels,
	ColorU8_sRGB
);

#endif // DRAW_EX_HPP_ADF54F6D_BF79_48F4_BB63_DACB47B0B960
