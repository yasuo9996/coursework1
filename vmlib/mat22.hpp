#ifndef MAT22_HPP_1F974C02_D0D1_4FBD_B5EE_A69C88112088
#define MAT22_HPP_1F974C02_D0D1_4FBD_B5EE_A69C88112088

#include <cmath>

#include "vec2.hpp"

/** Mat22f : 2x2 matrix with floats
 *
 * See comments for Vec2f for some discussion.
 *
 * The matrix is stored in row-major order.
 *
 * Example:
 *   Mat22f identity{ 
 *     1.f, 0.f,
 *     0.f, 1.f
 *   };
 */
struct Mat22f
{
	float _00, _01;
	float _10, _11;
};

// Common operators for Mat22f.
// Note that you will need to implement these yourself.

constexpr
Mat22f operator*( Mat22f const& aLeft, Mat22f const& aRight ) noexcept
{
	// Matrix-matrix multiplication for 2x2 matrices
	// Result = Left * Right
	return Mat22f{
		// First row of result
		aLeft._00 * aRight._00 + aLeft._01 * aRight._10,  // (0,0)
		aLeft._00 * aRight._01 + aLeft._01 * aRight._11,  // (0,1)
		
		// Second row of result
		aLeft._10 * aRight._00 + aLeft._11 * aRight._10,  // (1,0)
		aLeft._10 * aRight._01 + aLeft._11 * aRight._11   // (1,1)
	};
}

constexpr
Vec2f operator*( Mat22f const& aLeft, Vec2f const& aRight ) noexcept
{
	// Matrix-vector multiplication for 2x2 matrix and 2D vector
	// Result = Matrix * Vector
	return Vec2f{
		aLeft._00 * aRight.x + aLeft._01 * aRight.y,  // x component
		aLeft._10 * aRight.x + aLeft._11 * aRight.y   // y component
	};
}

// Functions:

inline
Mat22f make_rotation_2d( float aAngle ) noexcept
{
	// Create a 2D rotation matrix for the given angle (in radians)
	// Rotation matrix: [cosθ  -sinθ]
	//                 [sinθ   cosθ]
	float cos_angle = std::cos(aAngle);
	float sin_angle = std::sin(aAngle);
	
	return Mat22f{
		cos_angle, -sin_angle,  // First row
		sin_angle,  cos_angle    // Second row
	};
}

#endif // MAT22_HPP_1F974C02_D0D1_4FBD_B5EE_A69C88112088
