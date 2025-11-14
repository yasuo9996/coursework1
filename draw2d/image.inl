inline
ColorU8_sRGB_Alpha ImageRGBA::get_pixel( Index aX, Index aY ) const
{
	assert( aX < mWidth && aY < mHeight ); // Leave this at the top of the function.

	Index const linear_index = get_linear_index(aX, aY);
	std::uint8_t const* const pixel_ptr = mData + linear_index * 4;
	
	// Use reinterpret_cast for better performance (assuming little-endian architecture)
	// This avoids individual byte accesses and uses a single 32-bit read
	std::uint32_t const pixel_data = *reinterpret_cast<std::uint32_t const*>(pixel_ptr);
	
	return {
		static_cast<std::uint8_t>((pixel_data >> 0) & 0xFF),  // R
		static_cast<std::uint8_t>((pixel_data >> 8) & 0xFF),  // G
		static_cast<std::uint8_t>((pixel_data >> 16) & 0xFF), // B
		static_cast<std::uint8_t>((pixel_data >> 24) & 0xFF)  // A
	};
}

inline
auto ImageRGBA::get_width() const noexcept -> Index
{
	return mWidth;
}
inline
auto ImageRGBA::get_height() const noexcept -> Index
{
	return mHeight;
}

inline
std::uint8_t* ImageRGBA::get_image_ptr() noexcept
{
	return mData;
}
inline
std::uint8_t const* ImageRGBA::get_image_ptr() const noexcept
{
	return mData;
}

inline
ImageRGBA::Index ImageRGBA::get_linear_index( Index aX, Index aY ) const noexcept
{
	return aY * mWidth + aX;
}
