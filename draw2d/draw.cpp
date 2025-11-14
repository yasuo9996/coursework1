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
	
	// 检查线段端点是否在裁剪区域内
	// 注意：使用严格小于比较，因为像素索引从0到width-1/height-1
	bool begin_inside = (aBegin.x >= xmin && aBegin.x < xmax && aBegin.y >= ymin && aBegin.y < ymax);
	bool end_inside = (aEnd.x >= xmin && aEnd.x < xmax && aEnd.y >= ymin && aEnd.y < ymax);
	
	// 情况1: 两个端点都在裁剪区域内
	if (begin_inside && end_inside) {
		return true;
	}
	
	// 处理垂直线（x坐标相同）
	if (aBegin.x == aEnd.x) {
		// 如果垂直线在裁剪区域外，返回false
		if (aBegin.x < xmin || aBegin.x >= xmax) return false;
		
		// 计算线段在y轴上的可见部分
		float y_start = std::max(ymin, std::min(aBegin.y, aEnd.y));
		float y_end = std::min(ymax, std::max(aBegin.y, aEnd.y));
		
		// 如果线段完全在裁剪区域外，返回false
		if (y_start >= y_end) return false;
		
		// 更新线段的y坐标到可见部分
		aBegin.y = y_start;
		aEnd.y = y_end;
		return true;
	}
	
	// 处理水平线（y坐标相同）
	if (aBegin.y == aEnd.y) {
		// 如果水平线在裁剪区域外，返回false
		if (aBegin.y < ymin || aBegin.y >= ymax) return false;
		
		// 计算线段在x轴上的可见部分
		float x_start = std::max(xmin, std::min(aBegin.x, aEnd.x));
		float x_end = std::min(xmax, std::max(aBegin.x, aEnd.x));
		
		// 如果线段完全在裁剪区域外，返回false
		if (x_start >= x_end) return false;
		
		// 更新线段的x坐标到可见部分
		aBegin.x = x_start;
		aEnd.x = x_end;
		return true;
	}
	
	// 一般线段裁剪 - 使用Cohen-Sutherland算法
	float dx = aEnd.x - aBegin.x;
	float dy = aEnd.y - aBegin.y;
	
	// 如果线段是点，直接返回false
	if (dx == 0 && dy == 0) {
		return false;
	}
	
	// 计算与四条边的交点参数
	float t_enter = 0.0f;  // 进入裁剪区域的参数
	float t_exit = 1.0f;   // 离开裁剪区域的参数
	
	// 与左右边界的交点
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
	
	// 与上下边界的交点
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
	
	// 检查是否有有效交点
	if (t_enter <= t_exit && t_enter >= 0 && t_exit <= 1) {
		// 计算裁剪后的线段端点
		float new_begin_x = aBegin.x + t_enter * dx;
		float new_begin_y = aBegin.y + t_enter * dy;
		float new_end_x = aBegin.x + t_exit * dx;
		float new_end_y = aBegin.y + t_exit * dy;
		
		// 更新线段端点
		aBegin.x = new_begin_x;
		aBegin.y = new_begin_y;
		aEnd.x = new_end_x;
		aEnd.y = new_end_y;
		
		return true;
	}
	
	// 没有有效交点，线段完全在裁剪区域外
	return false;
}

void draw_clip_line_solid( Surface& aSurface, Vec2f aBegin, Vec2f aEnd, ColorU8_sRGB aColor )
{
	// 使用Bresenham算法绘制单像素宽度连续线条
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
	
	// 计算坐标差
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
	// 计算三角形的边界框
	float min_x = std::min({aP0.x, aP1.x, aP2.x});
	float max_x = std::max({aP0.x, aP1.x, aP2.x});
	float min_y = std::min({aP0.y, aP1.y, aP2.y});
	float max_y = std::max({aP0.y, aP1.y, aP2.y});
	
	// 将边界框与裁剪区域相交
	Rect2F clip_area = aSurface.clip_area();
	float clip_xmax = clip_area.xmin + clip_area.width;
	float clip_ymax = clip_area.ymin + clip_area.height;
	min_x = std::max(min_x, clip_area.xmin);
	max_x = std::min(max_x, clip_xmax);
	min_y = std::max(min_y, clip_area.ymin);
	max_y = std::min(max_y, clip_ymax);
	
	// 转换为整数像素坐标
	// 使用 floor 确保包含所有可能被覆盖的像素
	int start_x = static_cast<int>(std::floor(min_x));
	int end_x = static_cast<int>(std::floor(max_x));
	int start_y = static_cast<int>(std::floor(min_y));
	int end_y = static_cast<int>(std::floor(max_y));
	
	// 确保坐标在表面范围内（0 <= x < width, 0 <= y < height）
	start_x = std::max(0, start_x);
	end_x = std::min(static_cast<int>(aSurface.get_width() - 1), end_x);
	start_y = std::max(0, start_y);
	end_y = std::min(static_cast<int>(aSurface.get_height() - 1), end_y);
	
	// 计算三角形的面积（用于重心坐标计算）
	float area = (aP1.y - aP2.y) * (aP0.x - aP2.x) + (aP2.x - aP1.x) * (aP0.y - aP2.y);
	
	// 如果面积为0，三角形退化，不绘制
	if (std::abs(area) < 1e-6f) {
		return;
	}
	
	float inv_area = 1.0f / area;
	
	// 遍历边界框内的每个像素
	for (int y = start_y; y <= end_y; ++y) {
		for (int x = start_x; x <= end_x; ++x) {
			// 当前像素的中心点
			Vec2f pixel_center = Vec2f{static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f};
			
			// 计算重心坐标
			float w0 = ((aP1.y - aP2.y) * (pixel_center.x - aP2.x) + (aP2.x - aP1.x) * (pixel_center.y - aP2.y)) * inv_area;
			float w1 = ((aP2.y - aP0.y) * (pixel_center.x - aP0.x) + (aP0.x - aP2.x) * (pixel_center.y - aP0.y)) * inv_area;
			float w2 = 1.0f - w0 - w1;
			
			// 检查像素是否在三角形内部
			if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) {
				// 在三角形内部，进行颜色插值
				ColorF interpolated_color{
					w0 * aC0.r + w1 * aC1.r + w2 * aC2.r,
					w0 * aC0.g + w1 * aC1.g + w2 * aC2.g,
					w0 * aC0.b + w1 * aC1.b + w2 * aC2.b
				};
				
				// 确保颜色值在有效范围内
				interpolated_color.r = std::max(0.0f, std::min(1.0f, interpolated_color.r));
				interpolated_color.g = std::max(0.0f, std::min(1.0f, interpolated_color.g));
				interpolated_color.b = std::max(0.0f, std::min(1.0f, interpolated_color.b));
				
				// 转换为sRGB并绘制像素
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
