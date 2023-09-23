#pragma once

enum ERenderFormat
{
	RENDER_FORMAT_NONE = 0,
	RENDER_FORMAT_R32G32B32A32_TYPELESS = 1,
	RENDER_FORMAT_R32G32B32A32_FLOAT = 2,
	RENDER_FORMAT_R32G32B32A32_UINT = 3,
	RENDER_FORMAT_R32G32B32A32_SINT = 4,
	RENDER_FORMAT_R32G32B32_TYPELESS = 5,
	RENDER_FORMAT_R32G32B32_FLOAT = 6,
	RENDER_FORMAT_R32G32B32_UINT = 7,
	RENDER_FORMAT_R32G32B32_SINT = 8,
	RENDER_FORMAT_R16G16B16A16_TYPELESS = 9,
	RENDER_FORMAT_R16G16B16A16_FLOAT = 10,
	RENDER_FORMAT_R16G16B16A16_UNORM = 11,
	RENDER_FORMAT_R16G16B16A16_UINT = 12,
	RENDER_FORMAT_R16G16B16A16_SNORM = 13,
	RENDER_FORMAT_R16G16B16A16_SINT = 14,
	RENDER_FORMAT_R32G32_TYPELESS = 15,
	RENDER_FORMAT_R32G32_FLOAT = 16,
	RENDER_FORMAT_R32G32_UINT = 17,
	RENDER_FORMAT_R32G32_SINT = 18,
	RENDER_FORMAT_R32G8X24_TYPELESS = 19,
	RENDER_FORMAT_D32_FLOAT_S8X24_UINT = 20,
	RENDER_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
	RENDER_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
	RENDER_FORMAT_R10G10B10A2_TYPELESS = 23,
	RENDER_FORMAT_R10G10B10A2_UNORM = 24,
	RENDER_FORMAT_R10G10B10A2_UINT = 25,
	RENDER_FORMAT_R11G11B10_FLOAT = 26,
	RENDER_FORMAT_R8G8B8A8_TYPELESS = 27,
	RENDER_FORMAT_R8G8B8A8_UNORM = 28,
	RENDER_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
	RENDER_FORMAT_R8G8B8A8_UINT = 30,
	RENDER_FORMAT_R8G8B8A8_SNORM = 31,
	RENDER_FORMAT_R8G8B8A8_SINT = 32,
	RENDER_FORMAT_R16G16_TYPELESS = 33,
	RENDER_FORMAT_R16G16_FLOAT = 34,
	RENDER_FORMAT_R16G16_UNORM = 35,
	RENDER_FORMAT_R16G16_UINT = 36,
	RENDER_FORMAT_R16G16_SNORM = 37,
	RENDER_FORMAT_R16G16_SINT = 38,
	RENDER_FORMAT_R32_TYPELESS = 39,
	RENDER_FORMAT_D32_FLOAT = 40,
	RENDER_FORMAT_R32_FLOAT = 41,
	RENDER_FORMAT_R32_UINT = 42,
	RENDER_FORMAT_R32_SINT = 43,
	RENDER_FORMAT_R24G8_TYPELESS = 44,
	RENDER_FORMAT_D24_UNORM_S8_UINT = 45,
	RENDER_FORMAT_R24_UNORM_X8_TYPELESS = 46,
	RENDER_FORMAT_X24_TYPELESS_G8_UINT = 47,
	RENDER_FORMAT_R9G9B9E5_SHAREDEXP = 48,
	RENDER_FORMAT_R8G8_B8G8_UNORM = 49,
	RENDER_FORMAT_G8R8_G8B8_UNORM = 50,
	RENDER_FORMAT_R8G8_TYPELESS = 51,
	RENDER_FORMAT_R8G8_UNORM = 52,
	RENDER_FORMAT_R8G8_UINT = 53,
	RENDER_FORMAT_R8G8_SNORM = 54,
	RENDER_FORMAT_R8G8_SINT = 55,
	RENDER_FORMAT_R16_TYPELESS = 56,
	RENDER_FORMAT_R16_FLOAT = 57,
	RENDER_FORMAT_D16_UNORM = 58,
	RENDER_FORMAT_R16_UNORM = 59,
	RENDER_FORMAT_R16_UINT = 60,
	RENDER_FORMAT_R16_SNORM = 61,
	RENDER_FORMAT_R16_SINT = 62,
	RENDER_FORMAT_B5G6R5_UNORM = 63,
	RENDER_FORMAT_B5G5R5A1_UNORM = 64,
	RENDER_FORMAT_R8_TYPELESS = 65,
	RENDER_FORMAT_R8_UNORM = 66,
	RENDER_FORMAT_R8_UINT = 67,
	RENDER_FORMAT_R8_SNORM = 68,
	RENDER_FORMAT_R8_SINT = 69,
	RENDER_FORMAT_A8_UNORM = 70,
	RENDER_FORMAT_R1_UNORM = 71,
	RENDER_FORMAT_BC1_TYPELESS = 72,
	RENDER_FORMAT_BC1_UNORM = 73,
	RENDER_FORMAT_BC1_UNORM_SRGB = 74,
	RENDER_FORMAT_BC2_TYPELESS = 75,
	RENDER_FORMAT_BC2_UNORM = 76,
	RENDER_FORMAT_BC2_UNORM_SRGB = 77,
	RENDER_FORMAT_BC3_TYPELESS = 78,
	RENDER_FORMAT_BC3_UNORM = 79,
	RENDER_FORMAT_BC3_UNORM_SRGB = 80,
	RENDER_FORMAT_BC4_TYPELESS = 81,
	RENDER_FORMAT_BC4_UNORM = 82,
	RENDER_FORMAT_BC4_SNORM = 83,
	RENDER_FORMAT_BC5_TYPELESS = 84,
	RENDER_FORMAT_BC5_UNORM = 85,
	RENDER_FORMAT_BC5_SNORM = 86,
	RENDER_FORMAT_R16G16B16_FLOAT = 87,
	RENDER_FORMAT_INDEX_32 = 88,
	RENDER_FORMAT_INDEX_16 = 89,
	RENDER_FORMAT_LE_X2R10G10B10_UNORM = 90,
	RENDER_FORMAT_LE_X8R8G8B8_UNORM = 91,
	RENDER_FORMAT_X16Y16Z16_SNORM = 92
};
