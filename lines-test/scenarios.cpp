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
	
	// Case 1: 45度对角线 - 从内部左下角到外部右上角
	surface.clear();
	draw_line_solid(surface, {10.f, 10.f}, {150.f, 150.f}, color);
	REQUIRE(max_row_pixel_count(surface) > 0);
	REQUIRE(max_col_pixel_count(surface) > 0);
	// 检查裁剪后的线段长度
	auto neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[1] <= 2); // 最多两个端点
	
	// Case 2: 陡峭负斜率 - 从内部右上角到外部左下角
	surface.clear();
	draw_line_solid(surface, {90.f, 90.f}, {-50.f, -50.f}, color);
	REQUIRE(max_row_pixel_count(surface) > 0);
	REQUIRE(max_col_pixel_count(surface) > 0);
	neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[0] == 0); // 无孤立像素
	
	// Case 3: 水平线 - 从内部中心到外部右侧
	surface.clear();
	draw_line_solid(surface, {50.f, 50.f}, {200.f, 50.f}, color);
	REQUIRE(max_row_pixel_count(surface) > 0);
	REQUIRE(max_row_pixel_count(surface) <= 50); // 只绘制到右边界
	// 检查水平连续性
	REQUIRE(max_col_pixel_count(surface) == 1); // 单行像素
	
	// Case 4: 垂直线 - 从外部底部到内部顶部
	surface.clear();
	draw_line_solid(surface, {50.f, -50.f}, {50.f, 80.f}, color);
	REQUIRE(max_col_pixel_count(surface) > 0);
	REQUIRE(max_col_pixel_count(surface) <= 80); // 只绘制从顶部边界开始
	// 检查垂直连续性
	REQUIRE(max_row_pixel_count(surface) == 1); // 单列像素
	
	// 额外断言：确保所有线段都有适当的像素连续性
	auto final_neighbours = count_pixel_neighbours(surface);
	REQUIRE(final_neighbours[1] <= 2); // 最多两个端点
	REQUIRE(final_neighbours[8] == 0); // 无完全包围的像素
}

TEST_CASE("Scenario2: Lines with both points outside of the surface")
{
	// Create a test surface (100x100 pixels, coordinates: (0,0) to (100,100))
	Surface surface(100, 100);
	ColorU8_sRGB color = { 255, 255, 255 };
	
	// Case 1: 完全在表面上方的水平线
	surface.clear();
	draw_line_solid(surface, {-50.f, -50.f}, {150.f, -50.f}, color);
	REQUIRE(max_row_pixel_count(surface) == 0);
	REQUIRE(max_col_pixel_count(surface) == 0);
	// 额外检查：确保表面完全空白
	auto neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[0] == 0); // 无任何像素
	
	// Case 2: 完全在表面下方的斜线
	surface.clear();
	draw_line_solid(surface, {-30.f, 150.f}, {130.f, 200.f}, color);
	REQUIRE(max_row_pixel_count(surface) == 0);
	REQUIRE(max_col_pixel_count(surface) == 0);
	neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[0] == 0); // 无任何像素
	
	// Case 3: 完全在表面左侧的垂直线
	surface.clear();
	draw_line_solid(surface, {-100.f, -50.f}, {-100.f, 150.f}, color);
	REQUIRE(max_row_pixel_count(surface) == 0);
	REQUIRE(max_col_pixel_count(surface) == 0);
	neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[0] == 0); // 无任何像素
	
	// Case 4: 完全在表面右侧的陡峭线
	surface.clear();
	draw_line_solid(surface, {150.f, -50.f}, {200.f, 150.f}, color);
	REQUIRE(max_row_pixel_count(surface) == 0);
	REQUIRE(max_col_pixel_count(surface) == 0);
	neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[0] == 0); // 无任何像素
	
	// [!mayfail] 额外测试：穿过表面的对角线（应该被裁剪）
	surface.clear();
	draw_line_solid(surface, {-50.f, -50.f}, {150.f, 150.f}, color);
	REQUIRE(max_row_pixel_count(surface) > 0);
	REQUIRE(max_col_pixel_count(surface) > 0);
	// 检查裁剪后的线段特性
	neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[1] <= 2); // 最多两个端点
}

TEST_CASE("Scenario3: Line from p0 to p1 should be identical to line from p1 to p0")
{
	// Create two identical surfaces (100x100 pixels)
	Surface surface1(100, 100);
	Surface surface2(100, 100);
	ColorU8_sRGB color = { 255, 255, 255 };
	
	// Case 1: 45度对角线 - 从左上到右下 vs 从右下到左上
	surface1.clear();
	surface2.clear();
	draw_line_solid(surface1, {10.f, 10.f}, {90.f, 90.f}, color);
	draw_line_solid(surface2, {90.f, 90.f}, {10.f, 10.f}, color);
	
	REQUIRE(max_row_pixel_count(surface1) == max_row_pixel_count(surface2));
	REQUIRE(max_col_pixel_count(surface1) == max_col_pixel_count(surface2));
	// 检查像素邻居模式是否相同
	auto neighbours1 = count_pixel_neighbours(surface1);
	auto neighbours2 = count_pixel_neighbours(surface2);
	for (size_t i = 0; i < 9; ++i) {
		REQUIRE(neighbours1[i] == neighbours2[i]);
	}
	
	// Case 2: 陡峭负斜率 - 从右上到左下 vs 从左下到右上
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
	
	// Case 3: 浅斜率水平线 - 从左到右 vs 从右到左
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
	
	// Case 4: 浅斜率垂直线 - 从上到下 vs 从下到上
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
	
	// [!mayfail] 额外测试：完全相同的像素级比较
	surface1.clear();
	surface2.clear();
	draw_line_solid(surface1, {25.f, 25.f}, {75.f, 75.f}, color);
	draw_line_solid(surface2, {75.f, 75.f}, {25.f, 25.f}, color);

	// 像素级比较：检查每个像素是否相同
	auto const stride = surface1.get_width() << 2;
	for (std::uint32_t y = 0; y < surface1.get_height(); ++y) {
		for (std::uint32_t x = 0; x < surface1.get_width(); ++x) {
			auto const idx = y*stride + (x<<2);
			auto const ptr1 = surface1.get_surface_ptr() + idx;
			auto const ptr2 = surface2.get_surface_ptr() + idx;
			
			REQUIRE(ptr1[0] == ptr2[0]); // 红色通道
			REQUIRE(ptr1[1] == ptr2[1]); // 绿色通道
			REQUIRE(ptr1[2] == ptr2[2]); // 蓝色通道
		}
	}
}

TEST_CASE("Scenario4: Continuous lines should have no gaps between segments")
{
	// Create a test surface (100x100 pixels)
	Surface surface(100, 100);
	ColorU8_sRGB color = { 255, 255, 255 };
	
	// Case 1: 折线连接 - 从左上到中心再到右上
	surface.clear();
	draw_line_solid(surface, {10.f, 10.f}, {50.f, 50.f}, color);
	draw_line_solid(surface, {50.f, 50.f}, {90.f, 10.f}, color);
	
	// 检查连接点是否有适当的邻居
	auto neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[1] == 0); // 无孤立像素
	REQUIRE(neighbours[2] >= 2); // 连接点至少有2个邻居
	// 检查整体连续性
	REQUIRE(max_row_pixel_count(surface) > 0);
	REQUIRE(max_col_pixel_count(surface) > 0);
	
	// Case 2: 锯齿形折线 - 四个点形成锯齿形
	surface.clear();
	Vec2f zigzagPoints[] = {{10.f, 90.f}, {30.f, 10.f}, {70.f, 90.f}, {90.f, 10.f}};
	
	for (size_t i = 0; i < 3; ++i) {
		draw_line_solid(surface, zigzagPoints[i], zigzagPoints[i+1], color);
	}
	
	neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[1] == 0); // 无孤立像素
	REQUIRE(neighbours[2] >= 3); // 至少3个连接点
	
	// Case 3: 极短线段连接 - 测试像素级连续性
	surface.clear();
	Vec2f microPoints[] = {{50.f, 50.f}, {51.f, 51.f}, {52.f, 50.f}, {53.f, 51.f}};
	
	for (size_t i = 0; i < 3; ++i) {
		draw_line_solid(surface, microPoints[i], microPoints[i+1], color);
	}
	
	neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[1] == 0); // 即使短线段也应该连接
	REQUIRE(neighbours[0] == 0); // 无孤立像素
	
	// Case 4: 水平线段链 - 测试水平方向的连续性
	surface.clear();
	Vec2f horizontalChain[] = {{10.f, 50.f}, {30.f, 50.f}, {50.f, 50.f}, {70.f, 50.f}, {90.f, 50.f}};
	
	for (size_t i = 0; i < 4; ++i) {
		draw_line_solid(surface, horizontalChain[i], horizontalChain[i+1], color);
	}
	
	// 检查水平连续性
	REQUIRE(max_row_pixel_count(surface) >= 80); // 应覆盖大部分宽度
	REQUIRE(max_col_pixel_count(surface) == 1); // 单行像素
	
	// [!mayfail] 额外测试：端点邻居计数检查
	neighbours = count_pixel_neighbours(surface);
	REQUIRE(neighbours[1] == 2); // 只有两个端点应该有1个邻居
	
	// 额外测试：垂直线段链
	surface.clear();
	Vec2f verticalChain[] = {{50.f, 10.f}, {50.f, 30.f}, {50.f, 50.f}, {50.f, 70.f}, {50.f, 90.f}};
	
	for (size_t i = 0; i < 4; ++i) {
		draw_line_solid(surface, verticalChain[i], verticalChain[i+1], color);
	}
	
	// 检查垂直连续性
	REQUIRE(max_col_pixel_count(surface) >= 80); // 应覆盖大部分高度
	REQUIRE(max_row_pixel_count(surface) == 1); // 单列像素
}
