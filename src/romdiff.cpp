/*
 * This is the list of neccessary changes in the form <offset, value>
 * for patching the original TOS 4.04 for 68040 compatibility.
 *
 * Autogenerated by tools/romdiff.c tool
 */

#include "romdiff.h"

ROMdiff tosdiff[] = {
	{0x5a, 0x4d},
	{0x5b, 0xfa},
	{0x5d, 0x08},
	{0x5e, 0x4e},
	{0x5f, 0xf9},
	{0x60, 0x00},
	{0x61, 0xe7},
	{0x62, 0xb0},
	{0x63, 0x00},
	{0x6a, 0x4e},
	{0x6b, 0x71},
	{0x6c, 0x4e},
	{0x6d, 0x71},
	{0x6e, 0x4e},
	{0x6f, 0x71},
	{0x70, 0x4e},
	{0x71, 0x71},
	{0x72, 0x4e},
	{0x73, 0x71},
	{0x74, 0x4e},
	{0x75, 0x71},
	{0x76, 0x4e},
	{0x77, 0x71},
	{0x78, 0x4e},
	{0x79, 0x71},
	{0x7a, 0x4e},
	{0x7b, 0x71},
	{0x7c, 0x4e},
	{0x7d, 0x71},
	{0x7e, 0x4e},
	{0x7f, 0x71},
	{0x80, 0x4e},
	{0x81, 0x71},
	{0x3c4, 0x20},
	{0x3c5, 0xfc},
	{0x3c6, 0x00},
	{0x3c7, 0x00},
	{0x3c9, 0x28},
	{0x3ca, 0x4e},
	{0x3cb, 0x71},
	{0x3cc, 0xf5},
	{0x3cd, 0x18},
	{0x3ce, 0x4e},
	{0x3cf, 0x71},
	{0x3d0, 0x20},
	{0x3d1, 0xfc},
	{0x3d2, 0x5f},
	{0x3d3, 0x5f},
	{0x3d4, 0x4e},
	{0x3d5, 0x46},
	{0x3d6, 0x20},
	{0x3d7, 0xfc},
	{0x3d8, 0x00},
	{0x3d9, 0xe7},
	{0x3da, 0xb1},
	{0x3db, 0x90},
	{0x3dd, 0x71},
	{0x3de, 0x4e},
	{0x3df, 0x71},
	{0x3e0, 0x4e},
	{0x3e1, 0x71},
	{0x3e2, 0x4e},
	{0x3e3, 0x71},
	{0x3e5, 0x71},
	{0x3e6, 0x4e},
	{0x3e7, 0x71},
	{0x3e9, 0x71},
	{0x3ea, 0x4e},
	{0x3eb, 0x71},
	{0x3ec, 0x4e},
	{0x3ed, 0x71},
	{0x3ee, 0x4e},
	{0x3ef, 0x71},
	{0x3f0, 0x4e},
	{0x3f1, 0x71},
	{0x3f2, 0x4e},
	{0x3f3, 0x71},
	{0x3f5, 0x71},
	{0x3f6, 0x4e},
	{0x3f7, 0x71},
	{0x3f8, 0x4e},
	{0x3f9, 0x71},
	{0x3fa, 0x4e},
	{0x3fb, 0x71},
	{0x3fc, 0x4e},
	{0x3fd, 0x71},
	{0x63f, 0x71},
	{0x640, 0x4e},
	{0x641, 0x71},
	{0x6ac, 0x60},
	{0x756, 0x4e},
	{0x757, 0x71},
	{0x758, 0x4e},
	{0x759, 0x71},
	{0x75a, 0x4e},
	{0x75b, 0x71},
	{0x75c, 0xf4},
	{0x75d, 0xf8},
	{0x75e, 0x4e},
	{0x75f, 0x71},
	{0x861, 0x71},
	{0x862, 0xf4},
	{0x863, 0xf8},
	{0x864, 0x4e},
	{0x865, 0x71},
	{0x866, 0x4e},
	{0x867, 0x71},
	{0x868, 0x4e},
	{0x869, 0x71},
	{0x86b, 0x71},
	{0x86c, 0x4e},
	{0x86d, 0x71},
	{0x14e6, 0x4e},
	{0x14e7, 0x71},
	{0x14e8, 0x4e},
	{0x14e9, 0x71},
	{0x14ea, 0x4e},
	{0x14eb, 0x71},
	{0x14ec, 0x4e},
	{0x14ed, 0x71},
	{0x14ee, 0x4e},
	{0x14ef, 0x71},
	{0x14f0, 0x4e},
	{0x14f1, 0x71},
	{0x14f2, 0x4e},
	{0x14f3, 0x71},
	{0x14f4, 0x4e},
	{0x14f5, 0x71},
	{0x14f6, 0x4e},
	{0x14f7, 0x71},
	{0x14f8, 0x4e},
	{0x14f9, 0x71},
	{0x14fa, 0x4e},
	{0x14fb, 0x71},
	{0x14fc, 0x4e},
	{0x14fd, 0x71},
	{0x14fe, 0x4e},
	{0x14ff, 0x71},
	{0x1500, 0x4e},
	{0x1501, 0x71},
	{0x1502, 0x4e},
	{0x1503, 0x71},
	{0x1504, 0x4e},
	{0x1505, 0x71},
	{0x18d1, 0x71},
	{0x18d2, 0xf4},
	{0x18d3, 0xf8},
	{0x18d4, 0x4e},
	{0x18d5, 0x71},
	{0x18d6, 0x4e},
	{0x18d7, 0x71},
	{0x18d9, 0x71},
	{0x18da, 0x4e},
	{0x18db, 0x71},
	{0x1953, 0x71},
	{0x1954, 0x4e},
	{0x1955, 0x71},
	{0x398c, 0x4e},
	{0x398d, 0xf9},
	{0x398e, 0x00},
	{0x398f, 0xe7},
	{0x3990, 0xb0},
	{0x3991, 0xf4},
	{0x3997, 0x71},
	{0x3998, 0x4e},
	{0x3999, 0x71},
	{0x39a0, 0x4e},
	{0x39a1, 0x71},
	{0x39a2, 0x4e},
	{0x39a3, 0x71},
	{0x39a4, 0x4e},
	{0x39a5, 0x71},
	{0x39a6, 0x4e},
	{0x39a7, 0x71},
	{0x39a8, 0x4e},
	{0x39a9, 0x71},
	{0x39aa, 0x4e},
	{0x39ab, 0x71},
	{0x39ac, 0x4e},
	{0x39ad, 0x71},
	{0x39ae, 0x4e},
	{0x39af, 0x71},
	{0x39b0, 0x4e},
	{0x39b1, 0x71},
	{0x39b2, 0x4e},
	{0x39b3, 0x71},
	{0x39b4, 0x4e},
	{0x39b5, 0x71},
	{0x39b6, 0x4e},
	{0x39b7, 0x71},
	{0x990d, 0x71},
	{0x990e, 0xf4},
	{0x990f, 0x98},
	{0x9910, 0x4e},
	{0x9911, 0x71},
	{0x9912, 0x4e},
	{0x9913, 0x71},
	{0x9915, 0x71},
	{0x9916, 0x4e},
	{0x9917, 0x71},
	{0x11bd7, 0x71},
	{0x11bd8, 0x4e},
	{0x11bd9, 0x71},
	{0x11c5f, 0x71},
	{0x11c60, 0x4e},
	{0x11c61, 0x71},
	{0x11d5b, 0x71},
	{0x11d5c, 0x4e},
	{0x11d5d, 0x71},
	{0x125c1, 0x71},
	{0x125c2, 0x4e},
	{0x125c3, 0x71},
	{0x12937, 0x71},
	{0x12938, 0x4e},
	{0x12939, 0x71},
	{0x12983, 0x71},
	{0x12984, 0x4e},
	{0x12985, 0x71},
	{0x129e7, 0x71},
	{0x129e8, 0x4e},
	{0x129e9, 0x71},
	{0x12bff, 0x71},
	{0x12c00, 0x4e},
	{0x12c01, 0x71},
	{0x12c7f, 0x71},
	{0x12c80, 0x4e},
	{0x12c81, 0x71},
	{0x12cf7, 0x71},
	{0x12cf8, 0x4e},
	{0x12cf9, 0x71},
	{0x12d75, 0x71},
	{0x12d76, 0x4e},
	{0x12d77, 0x71},
	{0x12ddd, 0x71},
	{0x12dde, 0x4e},
	{0x12ddf, 0x71},
	{0x12e9f, 0x71},
	{0x12ea0, 0x4e},
	{0x12ea1, 0x71},
	{0x17b55, 0x71},
	{0x17b56, 0x4e},
	{0x17b57, 0x71},
	{0x17bed, 0x71},
	{0x17bee, 0x4e},
	{0x17bef, 0x71},
	{0x250d9, 0x71},
	{0x250da, 0x4e},
	{0x250db, 0x71},
	{0x44511, 0x71},
	{0x44512, 0x4e},
	{0x44513, 0x71},
	{0x49435, 0x16},
	{0x49438, 0x00},
	{0x49439, 0x00},
	{0x4943a, 0x00},
	{0x49441, 0x7f},
	{0x49442, 0xc0},
	{0x49444, 0x00},
	{0x49445, 0x00},
	{0x49446, 0x00},
	{0x4944d, 0xff},
	{0x4944e, 0xe0},
	{0x49450, 0x00},
	{0x49451, 0x00},
	{0x49452, 0x00},
	{0x49459, 0xfb},
	{0x4945a, 0xf0},
	{0x4945c, 0x00},
	{0x4945d, 0x00},
	{0x4945e, 0x00},
	{0x49464, 0x01},
	{0x49465, 0xbe},
	{0x49466, 0xf0},
	{0x49468, 0x00},
	{0x49469, 0x00},
	{0x4946a, 0x00},
	{0x49470, 0x01},
	{0x49471, 0xff},
	{0x49472, 0xb8},
	{0x49474, 0x00},
	{0x49475, 0x00},
	{0x49476, 0x00},
	{0x4947c, 0x01},
	{0x4947d, 0xef},
	{0x4947e, 0x78},
	{0x49480, 0x00},
	{0x49481, 0x00},
	{0x49482, 0x00},
	{0x49488, 0x01},
	{0x49489, 0x70},
	{0x4948a, 0x38},
	{0x4948c, 0x00},
	{0x4948d, 0x00},
	{0x4948e, 0x00},
	{0x49494, 0x01},
	{0x49495, 0xe0},
	{0x49496, 0x38},
	{0x49498, 0x00},
	{0x49499, 0x00},
	{0x4949a, 0x00},
	{0x494a0, 0x03},
	{0x494a1, 0xe0},
	{0x494a2, 0x18},
	{0x494a4, 0x00},
	{0x494a5, 0x00},
	{0x494a6, 0x00},
	{0x494ac, 0x01},
	{0x494ad, 0xf0},
	{0x494ae, 0x10},
	{0x494b0, 0x00},
	{0x494b1, 0x00},
	{0x494b2, 0x00},
	{0x494b8, 0x03},
	{0x494b9, 0xf0},
	{0x494ba, 0x10},
	{0x494bc, 0x00},
	{0x494bd, 0x00},
	{0x494be, 0x00},
	{0x494c4, 0x01},
	{0x494c5, 0xe0},
	{0x494c6, 0x10},
	{0x494c8, 0x00},
	{0x494c9, 0x00},
	{0x494ca, 0x00},
	{0x494d0, 0x01},
	{0x494d1, 0xf0},
	{0x494d4, 0x00},
	{0x494d5, 0x00},
	{0x494d6, 0x00},
	{0x494dc, 0x01},
	{0x494dd, 0x60},
	{0x494e0, 0x00},
	{0x494e1, 0x00},
	{0x494e2, 0x00},
	{0x494e9, 0xe0},
	{0x494ec, 0x00},
	{0x494ed, 0x00},
	{0x494ee, 0x00},
	{0x494f5, 0xe8},
	{0x494f8, 0x00},
	{0x494f9, 0x00},
	{0x494fa, 0x00},
	{0x49501, 0xe8},
	{0x49504, 0x00},
	{0x49505, 0x00},
	{0x49506, 0x00},
	{0x4950d, 0xe8},
	{0x49510, 0x00},
	{0x49511, 0x00},
	{0x49512, 0x00},
	{0x49519, 0xfc},
	{0x4951c, 0x00},
	{0x4951d, 0x00},
	{0x4951e, 0x00},
	{0x49525, 0x7c},
	{0x49528, 0x00},
	{0x49529, 0x00},
	{0x4952a, 0x00},
	{0x49531, 0x7e},
	{0x49534, 0x00},
	{0x49535, 0x00},
	{0x49536, 0x00},
	{0x4953d, 0x3e},
	{0x4953e, 0x80},
	{0x4953f, 0x00},
	{0x49540, 0x00},
	{0x49541, 0x00},
	{0x49542, 0x00},
	{0x49549, 0x1e},
	{0x4954a, 0x80},
	{0x4954b, 0x00},
	{0x4954c, 0x00},
	{0x4954d, 0x00},
	{0x4954e, 0x00},
	{0x49555, 0x1e},
	{0x49556, 0x80},
	{0x49557, 0x00},
	{0x49558, 0x00},
	{0x49559, 0x0c},
	{0x4955a, 0x00},
	{0x49561, 0x0f},
	{0x49562, 0xf0},
	{0x49563, 0x00},
	{0x49564, 0x00},
	{0x49565, 0x0e},
	{0x49566, 0x00},
	{0x4956d, 0x07},
	{0x4956e, 0xf6},
	{0x4956f, 0x80},
	{0x49570, 0x00},
	{0x49571, 0x1c},
	{0x49572, 0x00},
	{0x49573, 0x08},
	{0x49579, 0x01},
	{0x4957a, 0xff},
	{0x4957b, 0xff},
	{0x4957c, 0xd0},
	{0x4957d, 0x16},
	{0x4957e, 0x03},
	{0x4957f, 0x1c},
	{0x49580, 0x0a},
	{0x49586, 0xff},
	{0x49587, 0xff},
	{0x49588, 0xdf},
	{0x49589, 0x15},
	{0x4958a, 0x03},
	{0x4958b, 0x1e},
	{0x4958c, 0x1f},
	{0x49592, 0xff},
	{0x49593, 0xff},
	{0x49594, 0xfe},
	{0x49596, 0x05},
	{0x49597, 0x36},
	{0x49598, 0x3f},
	{0x49599, 0x80},
	{0x4959e, 0x7f},
	{0x4959f, 0xff},
	{0x495a0, 0x7b},
	{0x495a1, 0xf7},
	{0x495a2, 0xd7},
	{0x495a3, 0x36},
	{0x495a4, 0x33},
	{0x495a5, 0x94},
	{0x495aa, 0x0f},
	{0x495ab, 0xff},
	{0x495ac, 0xfb},
	{0x495ad, 0xee},
	{0x495ae, 0xef},
	{0x495af, 0x53},
	{0x495b0, 0x31},
	{0x495b1, 0x7f},
	{0x495b2, 0xe0},
	{0x495b6, 0x01},
	{0x495b7, 0xff},
	{0x495b8, 0xff},
	{0x495b9, 0xdf},
	{0x495ba, 0xbd},
	{0x495bb, 0xb1},
	{0x495bc, 0x63},
	{0x495bd, 0xff},
	{0x495be, 0xfc},
	{0x495c3, 0x5f},
	{0x495c4, 0xff},
	{0x495c5, 0xfa},
	{0x495c6, 0x75},
	{0x495c7, 0x45},
	{0x495c8, 0xe7},
	{0x495c9, 0x7d},
	{0x495ca, 0xd7},
	{0x495cb, 0xc0},
	{0x495cf, 0x1f},
	{0x495d0, 0xff},
	{0x495d1, 0xbd},
	{0x495d2, 0xe8},
	{0x495d3, 0x29},
	{0x495d4, 0xc6},
	{0x495d5, 0xe3},
	{0x495d6, 0xff},
	{0x495d7, 0xf0},
	{0x495db, 0x01},
	{0x495dc, 0xff},
	{0x495dd, 0xf6},
	{0x495de, 0xe0},
	{0x495df, 0x80},
	{0x495e0, 0x43},
	{0x495e1, 0x60},
	{0x495e2, 0x02},
	{0x495e3, 0xe0},
	{0x495e7, 0x71},
	{0x495e8, 0x7f},
	{0x495ea, 0xc0},
	{0x495eb, 0x02},
	{0x495ec, 0xff},
	{0x495ed, 0xc0},
	{0x495f2, 0x01},
	{0x495f3, 0xfe},
	{0x495f4, 0x2f},
	{0x495f6, 0xfa},
	{0x495f7, 0x54},
	{0x495f8, 0xaf},
	{0x495f9, 0xe0},
	{0x495ff, 0xdf},
	{0x49600, 0xed},
	{0x49601, 0x7f},
	{0x49602, 0xfe},
	{0x49603, 0xed},
	{0x49604, 0xff},
	{0x49605, 0xc0},
	{0x4960b, 0x83},
	{0x4960c, 0xff},
	{0x4960d, 0xef},
	{0x4960e, 0xff},
	{0x49610, 0xff},
	{0x49611, 0x60},
	{0x49617, 0x87},
	{0x49618, 0xff},
	{0x49619, 0xbf},
	{0x4961a, 0xff},
	{0x4961c, 0x7e},
	{0x4961d, 0x20},
	{0x49622, 0x07},
	{0x49623, 0x87},
	{0x49624, 0x1f},
	{0x49625, 0xfc},
	{0x49626, 0xff},
	{0x49627, 0xfe},
	{0x49628, 0xf8},
	{0x49629, 0x18},
	{0x4962e, 0x0e},
	{0x4962f, 0x0f},
	{0x49630, 0x0d},
	{0x49631, 0xeb},
	{0x49632, 0x9b},
	{0x49634, 0x70},
	{0x4963a, 0x0e},
	{0x4963b, 0x0f},
	{0x4963c, 0x03},
	{0x4963e, 0x8e},
	{0x4963f, 0x5f},
	{0x49640, 0xe0},
	{0x49646, 0x18},
	{0x49647, 0x1e},
	{0x49648, 0x0f},
	{0x49649, 0xfe},
	{0x4964a, 0x0a},
	{0x4964b, 0x03},
	{0x4964c, 0xa0},
	{0x49651, 0x00},
	{0x49652, 0x10},
	{0x49653, 0x1c},
	{0x49654, 0x0f},
	{0x49655, 0xbc},
	{0x49656, 0x0c},
	{0x49657, 0x0a},
	{0x49658, 0xe0},
	{0x49659, 0x00},
	{0x4965d, 0x00},
	{0x4965e, 0x30},
	{0x4965f, 0x3c},
	{0x49660, 0x0e},
	{0x49661, 0x40},
	{0x49662, 0x1c},
	{0x49663, 0x57},
	{0x49664, 0xe0},
	{0x49665, 0x00},
	{0x49669, 0x00},
	{0x4966a, 0x30},
	{0x4966b, 0x18},
	{0x4966c, 0x1e},
	{0x4966d, 0x00},
	{0x4966e, 0x1c},
	{0x4966f, 0x7f},
	{0x49670, 0xe0},
	{0x49671, 0x00},
	{0x49675, 0x00},
	{0x49676, 0x60},
	{0x49677, 0x30},
	{0x49678, 0x1c},
	{0x49679, 0x00},
	{0x4967a, 0x39},
	{0x4967b, 0xff},
	{0x4967c, 0xe0},
	{0x4967d, 0x00},
	{0x49681, 0x00},
	{0x49682, 0x20},
	{0x49683, 0x20},
	{0x49684, 0x1c},
	{0x49685, 0x00},
	{0x49686, 0x3b},
	{0x49687, 0x7f},
	{0x49688, 0xc0},
	{0x49689, 0x00},
	{0x4968d, 0x00},
	{0x4968e, 0x60},
	{0x4968f, 0x20},
	{0x49690, 0x18},
	{0x49691, 0x00},
	{0x49692, 0x73},
	{0x49693, 0x7d},
	{0x49694, 0x00},
	{0x49695, 0x00},
	{0x49699, 0x00},
	{0x4969a, 0x40},
	{0x4969b, 0x30},
	{0x4969c, 0x18},
	{0x4969d, 0x00},
	{0x4969e, 0x53},
	{0x4969f, 0x37},
	{0x496a0, 0x00},
	{0x496a1, 0x00},
	{0x496a5, 0x00},
	{0x496a6, 0xc0},
	{0x496a7, 0x30},
	{0x496a8, 0x30},
	{0x496a9, 0x00},
	{0x496aa, 0x53},
	{0x496ab, 0x78},
	{0x496ac, 0x00},
	{0x496ad, 0x00},
	{0x496b1, 0x07},
	{0x496b3, 0x10},
	{0x496b4, 0x10},
	{0x496b5, 0x00},
	{0x496b6, 0x33},
	{0x496b7, 0xa0},
	{0x496b8, 0x00},
	{0x496b9, 0x00},
	{0x496bd, 0x05},
	{0x496bf, 0x30},
	{0x496c0, 0x10},
	{0x496c1, 0x00},
	{0x496c2, 0x73},
	{0x496c3, 0xe0},
	{0x496c5, 0x00},
	{0x496c9, 0x00},
	{0x496cb, 0x18},
	{0x496cc, 0x18},
	{0x496cd, 0x00},
	{0x496ce, 0x71},
	{0x496cf, 0xb0},
	{0x496d1, 0x00},
	{0x496d7, 0x18},
	{0x496d8, 0x30},
	{0x496da, 0x73},
	{0x496db, 0xe1},
	{0x496e3, 0x08},
	{0x496e4, 0x18},
	{0x496e6, 0x71},
	{0x496e7, 0xff},
	{0x496e8, 0x40},
	{0x496ef, 0x08},
	{0x496f0, 0x18},
	{0x496f2, 0x31},
	{0x496f3, 0xff},
	{0x496f4, 0xf0},
	{0x496fb, 0x08},
	{0x496fc, 0x18},
	{0x496fe, 0x78},
	{0x496ff, 0xff},
	{0x49700, 0xff},
	{0x49701, 0x80},
	{0x49707, 0x18},
	{0x49708, 0x08},
	{0x4970a, 0x30},
	{0x4970b, 0xbf},
	{0x4970c, 0xbf},
	{0x4970d, 0x60},
	{0x49712, 0x00},
	{0x49713, 0x70},
	{0x49714, 0x0c},
	{0x49715, 0x00},
	{0x49716, 0x30},
	{0x49717, 0x17},
	{0x49718, 0xeb},
	{0x49719, 0xf8},
	{0x4971a, 0x00},
	{0x4971d, 0x00},
	{0x4971e, 0x00},
	{0x4971f, 0x20},
	{0x49720, 0x08},
	{0x49721, 0x00},
	{0x49722, 0x30},
	{0x49723, 0x0d},
	{0x49725, 0xfe},
	{0x49726, 0x00},
	{0x49727, 0x00},
	{0x49729, 0x00},
	{0x4972a, 0x00},
	{0x4972b, 0x00},
	{0x4972c, 0x0c},
	{0x4972d, 0x00},
	{0x4972e, 0x10},
	{0x4972f, 0x02},
	{0x49730, 0xbf},
	{0x49731, 0x05},
	{0x49732, 0x00},
	{0x49733, 0x00},
	{0x49735, 0x00},
	{0x49736, 0x00},
	{0x49737, 0x00},
	{0x49738, 0x08},
	{0x49739, 0x00},
	{0x4973a, 0x30},
	{0x4973b, 0x00},
	{0x4973c, 0x0f},
	{0x4973d, 0xe0},
	{0x4973e, 0x00},
	{0x4973f, 0x00},
	{0x49741, 0x00},
	{0x49742, 0x00},
	{0x49743, 0x00},
	{0x49744, 0x04},
	{0x49745, 0x00},
	{0x49746, 0x10},
	{0x49747, 0x00},
	{0x49748, 0x03},
	{0x49749, 0xf8},
	{0x4974a, 0x00},
	{0x4974b, 0x00},
	{0x4974d, 0x00},
	{0x4974e, 0x00},
	{0x4974f, 0x00},
	{0x49750, 0x08},
	{0x49751, 0x00},
	{0x49752, 0x30},
	{0x49753, 0x00},
	{0x49754, 0x00},
	{0x49755, 0x7c},
	{0x49756, 0x00},
	{0x49757, 0x00},
	{0x49759, 0x00},
	{0x4975a, 0x00},
	{0x4975b, 0x00},
	{0x4975c, 0x0c},
	{0x4975d, 0x00},
	{0x4975e, 0x18},
	{0x4975f, 0x00},
	{0x49760, 0x00},
	{0x49761, 0x00},
	{0x49762, 0x00},
	{0x49763, 0x00},
	{0x49765, 0x00},
	{0x49766, 0x00},
	{0x49767, 0x00},
	{0x49768, 0x18},
	{0x49769, 0x00},
	{0x4976a, 0x30},
	{0x4976b, 0x00},
	{0x4976c, 0x00},
	{0x4976d, 0x00},
	{0x4976e, 0x00},
	{0x4976f, 0x00},
	{0x49771, 0x00},
	{0x49772, 0x00},
	{0x49773, 0x00},
	{0x49774, 0x10},
	{0x49775, 0x00},
	{0x49776, 0x18},
	{0x49777, 0x00},
	{0x49778, 0x00},
	{0x49779, 0x00},
	{0x4977a, 0x00},
	{0x4977d, 0x00},
	{0x4977e, 0x00},
	{0x4977f, 0x00},
	{0x49780, 0x10},
	{0x49781, 0x00},
	{0x49782, 0x38},
	{0x49783, 0x00},
	{0x49784, 0x00},
	{0x49785, 0x00},
	{0x49789, 0x00},
	{0x4978a, 0x00},
	{0x4978b, 0x00},
	{0x4978c, 0x00},
	{0x4978d, 0x00},
	{0x4978e, 0x18},
	{0x4978f, 0x00},
	{0x49790, 0x00},
	{0x49791, 0x00},
	{0x49795, 0x00},
	{0x49796, 0x00},
	{0x49797, 0x00},
	{0x49798, 0x00},
	{0x49799, 0x00},
	{0x4979a, 0x18},
	{0x4979b, 0x00},
	{0x4979c, 0x00},
	{0x4979d, 0x00},
	{0x497a1, 0x00},
	{0x497a2, 0x00},
	{0x497a3, 0x00},
	{0x497a4, 0x00},
	{0x497a5, 0x00},
	{0x497a6, 0x18},
	{0x497a7, 0x00},
	{0x497a8, 0x00},
	{0x497a9, 0x00},
	{0x497ad, 0x00},
	{0x497ae, 0x00},
	{0x497af, 0x00},
	{0x497b0, 0x00},
	{0x497b1, 0x00},
	{0x497b2, 0x18},
	{0x497b3, 0x00},
	{0x497b4, 0x00},
	{0x497b5, 0x00},
	{0x497b9, 0x00},
	{0x497ba, 0x00},
	{0x497bb, 0x00},
	{0x497bc, 0x00},
	{0x497bd, 0x00},
	{0x497be, 0x18},
	{0x497bf, 0x00},
	{0x497c0, 0x00},
	{0x497c1, 0x00},
	{0x497c5, 0x00},
	{0x497c6, 0x00},
	{0x497c7, 0x00},
	{0x497c8, 0x00},
	{0x497c9, 0x00},
	{0x497ca, 0x18},
	{0x497cb, 0x00},
	{0x497cc, 0x00},
	{0x497cd, 0x00},
	{0x497d1, 0x00},
	{0x497d2, 0x00},
	{0x497d3, 0x00},
	{0x497d4, 0x00},
	{0x497d5, 0x00},
	{0x497d6, 0x08},
	{0x497d7, 0x00},
	{0x497d8, 0x00},
	{0x497d9, 0x00},
	{0x497dd, 0x00},
	{0x497de, 0x00},
	{0x497df, 0x00},
	{0x497e0, 0x00},
	{0x497e1, 0x00},
	{0x497e2, 0x08},
	{0x497e3, 0x00},
	{0x497e4, 0x00},
	{0x497e5, 0x00},
	{0x497e9, 0x00},
	{0x497ea, 0x00},
	{0x497eb, 0x00},
	{0x497ec, 0x00},
	{0x497ed, 0x00},
	{0x497ee, 0x08},
	{0x497ef, 0x00},
	{0x497f0, 0x00},
	{0x497f1, 0x00},
	{0x497f5, 0x00},
	{0x497f6, 0x00},
	{0x497f7, 0x00},
	{0x497f8, 0x00},
	{0x497f9, 0x00},
	{0x497fa, 0x08},
	{0x497fb, 0x00},
	{0x497fc, 0x00},
	{0x497fd, 0x00},
	{0x49801, 0x00},
	{0x49802, 0x00},
	{0x49803, 0x00},
	{0x49804, 0x00},
	{0x49805, 0x00},
	{0x49806, 0x18},
	{0x49807, 0x00},
	{0x49808, 0x00},
	{0x49809, 0x00},
	{0x4980d, 0x00},
	{0x4980e, 0x00},
	{0x4980f, 0x00},
	{0x49810, 0x00},
	{0x49811, 0x00},
	{0x49812, 0x08},
	{0x49813, 0x00},
	{0x49814, 0x00},
	{0x49815, 0x00},
	{0x49819, 0x00},
	{0x4981a, 0x00},
	{0x4981b, 0x00},
	{0x4981c, 0x00},
	{0x4981d, 0x00},
	{0x4981e, 0x18},
	{0x4981f, 0x00},
	{0x49820, 0x00},
	{0x49821, 0x00},
	{0x49824, 0x00},
	{0x49825, 0x00},
	{0x49826, 0x00},
	{0x49827, 0x00},
	{0x49828, 0x00},
	{0x49829, 0x00},
	{0x4982a, 0x08},
	{0x4982b, 0x00},
	{0x4982c, 0x00},
	{0x4982d, 0x00},
	{0x49830, 0x00},
	{0x49831, 0x00},
	{0x49832, 0x00},
	{0x49833, 0x00},
	{0x49834, 0x00},
	{0x49835, 0x00},
	{0x49836, 0x00},
	{0x49837, 0x00},
	{0x49838, 0x00},
	{0x49839, 0x00},
	{0x7b000, 0x40},
	{0x7b001, 0xc7},
	{0x7b002, 0x00},
	{0x7b003, 0x7c},
	{0x7b004, 0x07},
	{0x7b005, 0x00},
	{0x7b006, 0x4e},
	{0x7b007, 0x7a},
	{0x7b008, 0x10},
	{0x7b009, 0x06},
	{0x7b00a, 0x70},
	{0x7b00b, 0x00},
	{0x7b00c, 0x4e},
	{0x7b00d, 0x7b},
	{0x7b00e, 0x00},
	{0x7b00f, 0x02},
	{0x7b010, 0x4e},
	{0x7b011, 0x71},
	{0x7b012, 0xf4},
	{0x7b013, 0xd8},
	{0x7b014, 0x4e},
	{0x7b015, 0x71},
	{0x7b016, 0x20},
	{0x7b017, 0x3c},
	{0x7b018, 0x00},
	{0x7b01a, 0xe0},
	{0x7b01b, 0x40},
	{0x7b01c, 0x4e},
	{0x7b01d, 0x7b},
	{0x7b01e, 0x00},
	{0x7b01f, 0x06},
	{0x7b020, 0x4e},
	{0x7b021, 0x71},
	{0x7b022, 0xf5},
	{0x7b023, 0x18},
	{0x7b024, 0x4e},
	{0x7b025, 0x71},
	{0x7b026, 0x7c},
	{0x7b027, 0x00},
	{0x7b028, 0x4e},
	{0x7b029, 0x7a},
	{0x7b02a, 0x08},
	{0x7b02b, 0x07},
	{0x7b02c, 0x4a},
	{0x7b02d, 0x80},
	{0x7b02e, 0x67},
	{0x7b02f, 0x32},
	{0x7b030, 0xb0},
	{0x7b031, 0xbc},
	{0x7b032, 0x05},
	{0x7b033, 0x00},
	{0x7b034, 0x00},
	{0x7b035, 0x00},
	{0x7b036, 0x64},
	{0x7b037, 0x2a},
	{0x7b038, 0xb0},
	{0x7b039, 0xbc},
	{0x7b03a, 0x00},
	{0x7b03b, 0xe0},
	{0x7b03c, 0x00},
	{0x7b03d, 0x00},
	{0x7b03e, 0x65},
	{0x7b03f, 0x08},
	{0x7b040, 0xb0},
	{0x7b041, 0xbc},
	{0x7b042, 0x01},
	{0x7b043, 0x00},
	{0x7b044, 0x00},
	{0x7b045, 0x00},
	{0x7b046, 0x65},
	{0x7b047, 0x1a},
	{0x7b048, 0x20},
	{0x7b049, 0x40},
	{0x7b04a, 0x0c},
	{0x7b04b, 0xa8},
	{0x7b04c, 0x54},
	{0x7b04d, 0x52},
	{0x7b04e, 0x45},
	{0x7b04f, 0x45},
	{0x7b051, 0xe8},
	{0x7b052, 0x66},
	{0x7b053, 0x00},
	{0x7b054, 0x00},
	{0x7b055, 0x0e},
	{0x7b056, 0x0c},
	{0x7b057, 0xa8},
	{0x7b058, 0x4b},
	{0x7b059, 0x45},
	{0x7b05a, 0x45},
	{0x7b05b, 0x50},
	{0x7b05d, 0xec},
	{0x7b05e, 0x66},
	{0x7b05f, 0x02},
	{0x7b060, 0x7c},
	{0x7b061, 0x01},
	{0x7b062, 0x4e},
	{0x7b063, 0x71},
	{0x7b064, 0x4e},
	{0x7b065, 0x7b},
	{0x7b066, 0x10},
	{0x7b067, 0x06},
	{0x7b068, 0x4e},
	{0x7b069, 0x71},
	{0x7b06a, 0xf5},
	{0x7b06b, 0x18},
	{0x7b06c, 0x4e},
	{0x7b06d, 0x71},
	{0x7b06e, 0x4a},
	{0x7b06f, 0x86},
	{0x7b070, 0x67},
	{0x7b071, 0x2e},
	{0x7b072, 0x20},
	{0x7b073, 0x08},
	{0x7b074, 0x4e},
	{0x7b075, 0x7b},
	{0x7b076, 0x08},
	{0x7b077, 0x07},
	{0x7b078, 0x4e},
	{0x7b079, 0x7b},
	{0x7b07a, 0x08},
	{0x7b07b, 0x06},
	{0x7b07c, 0x20},
	{0x7b07d, 0x3c},
	{0x7b07e, 0x00},
	{0x7b07f, 0x00},
	{0x7b080, 0xc0},
	{0x7b081, 0x00},
	{0x7b082, 0x4e},
	{0x7b083, 0x7b},
	{0x7b084, 0x00},
	{0x7b085, 0x03},
	{0x7b086, 0x70},
	{0x7b087, 0x00},
	{0x7b088, 0x4e},
	{0x7b089, 0x7b},
	{0x7b08a, 0x00},
	{0x7b08b, 0x04},
	{0x7b08c, 0x4e},
	{0x7b08d, 0x7b},
	{0x7b08e, 0x00},
	{0x7b08f, 0x05},
	{0x7b090, 0x4e},
	{0x7b091, 0x7b},
	{0x7b092, 0x00},
	{0x7b093, 0x06},
	{0x7b094, 0x4e},
	{0x7b095, 0x7b},
	{0x7b096, 0x00},
	{0x7b097, 0x07},
	{0x7b098, 0x4e},
	{0x7b099, 0x71},
	{0x7b09a, 0xf5},
	{0x7b09b, 0x18},
	{0x7b09c, 0x4e},
	{0x7b09d, 0x71},
	{0x7b09e, 0x60},
	{0x7b09f, 0x32},
	{0x7b0a0, 0x20},
	{0x7b0a1, 0x3c},
	{0x7b0a2, 0x00},
	{0x7b0a4, 0xe0},
	{0x7b0a5, 0x00},
	{0x7b0a6, 0x4e},
	{0x7b0a7, 0x7b},
	{0x7b0a8, 0x00},
	{0x7b0a9, 0x04},
	{0x7b0aa, 0x20},
	{0x7b0ab, 0x3c},
	{0x7b0ac, 0x00},
	{0x7b0ae, 0xe0},
	{0x7b0af, 0x40},
	{0x7b0b0, 0x4e},
	{0x7b0b1, 0x7b},
	{0x7b0b2, 0x00},
	{0x7b0b3, 0x06},
	{0x7b0b4, 0x70},
	{0x7b0b5, 0x00},
	{0x7b0b6, 0x4e},
	{0x7b0b7, 0x7b},
	{0x7b0b8, 0x00},
	{0x7b0b9, 0x05},
	{0x7b0ba, 0x4e},
	{0x7b0bb, 0x7b},
	{0x7b0bc, 0x00},
	{0x7b0bd, 0x07},
	{0x7b0be, 0x70},
	{0x7b0bf, 0x00},
	{0x7b0c0, 0x4e},
	{0x7b0c1, 0x7b},
	{0x7b0c2, 0x00},
	{0x7b0c3, 0x03},
	{0x7b0c4, 0x4e},
	{0x7b0c5, 0x7b},
	{0x7b0c6, 0x08},
	{0x7b0c7, 0x07},
	{0x7b0c8, 0x4e},
	{0x7b0c9, 0x7b},
	{0x7b0ca, 0x08},
	{0x7b0cb, 0x06},
	{0x7b0cc, 0x4e},
	{0x7b0cd, 0x71},
	{0x7b0ce, 0xf5},
	{0x7b0cf, 0x18},
	{0x7b0d0, 0x4e},
	{0x7b0d1, 0x71},
	{0x7b0d2, 0x20},
	{0x7b0d3, 0x3c},
	{0x7b0d4, 0x00},
	{0x7b0d5, 0x00},
	{0x7b0d6, 0x80},
	{0x7b0d7, 0x00},
	{0x7b0d8, 0x4e},
	{0x7b0d9, 0x7b},
	{0x7b0da, 0x00},
	{0x7b0db, 0x02},
	{0x7b0dc, 0x46},
	{0x7b0dd, 0xc7},
	{0x7b0de, 0x4e},
	{0x7b0df, 0xd6},
	{0x7b0e0, 0x08},
	{0x7b0e1, 0xb8},
	{0x7b0e2, 0x00},
	{0x7b0e3, 0x05},
	{0x7b0e4, 0x82},
	{0x7b0e5, 0x66},
	{0x7b0e6, 0x08},
	{0x7b0e7, 0xb8},
	{0x7b0e8, 0x00},
	{0x7b0e9, 0x06},
	{0x7b0ea, 0x82},
	{0x7b0eb, 0x66},
	{0x7b0ec, 0x08},
	{0x7b0ed, 0xb8},
	{0x7b0ee, 0x00},
	{0x7b0ef, 0x00},
	{0x7b0f0, 0x82},
	{0x7b0f1, 0x0a},
	{0x7b0f2, 0x4e},
	{0x7b0f3, 0xd0},
	{0x7b0f4, 0x00},
	{0x7b0f5, 0x7c},
	{0x7b0f6, 0x07},
	{0x7b0f7, 0x00},
	{0x7b0f8, 0x72},
	{0x7b0f9, 0x00},
	{0x7b0fa, 0x41},
	{0x7b0fb, 0xf8},
	{0x7b0fc, 0x98},
	{0x7b0fd, 0x00},
	{0x7b0fe, 0x30},
	{0x7b0ff, 0x3c},
	{0x7b100, 0x00},
	{0x7b102, 0x20},
	{0x7b103, 0xc1},
	{0x7b104, 0x51},
	{0x7b105, 0xc8},
	{0x7b107, 0xfc},
	{0x7b108, 0x41},
	{0x7b109, 0xf8},
	{0x7b10a, 0x82},
	{0x7b10b, 0x40},
	{0x7b10c, 0x70},
	{0x7b10d, 0x07},
	{0x7b10e, 0x20},
	{0x7b10f, 0xc1},
	{0x7b110, 0x51},
	{0x7b111, 0xc8},
	{0x7b113, 0xfc},
	{0x7b114, 0x70},
	{0x7b115, 0x00},
	{0x7b116, 0x4e},
	{0x7b117, 0x7b},
	{0x7b118, 0x00},
	{0x7b119, 0x02},
	{0x7b11a, 0x4e},
	{0x7b11b, 0x71},
	{0x7b11c, 0x4e},
	{0x7b11d, 0x71},
	{0x7b11e, 0xf4},
	{0x7b11f, 0xd8},
	{0x7b120, 0x4e},
	{0x7b121, 0x71},
	{0x7b122, 0x41},
	{0x7b123, 0xf8},
	{0x7b124, 0x00},
	{0x7b125, 0x08},
	{0x7b126, 0x20},
	{0x7b127, 0x3c},
	{0x7b128, 0x00},
	{0x7b129, 0x00},
	{0x7b12a, 0x06},
	{0x7b12b, 0x00},
	{0x7b12c, 0x90},
	{0x7b12d, 0x88},
	{0x7b12e, 0xe4},
	{0x7b12f, 0x88},
	{0x7b130, 0x42},
	{0x7b131, 0x81},
	{0x7b132, 0x20},
	{0x7b133, 0xc1},
	{0x7b134, 0x53},
	{0x7b135, 0x80},
	{0x7b136, 0x66},
	{0x7b137, 0xfa},
	{0x7b138, 0x4e},
	{0x7b139, 0x71},
	{0x7b13a, 0xf5},
	{0x7b13b, 0x18},
	{0x7b13c, 0x4e},
	{0x7b13d, 0x71},
	{0x7b13e, 0x20},
	{0x7b13f, 0x3c},
	{0x7b140, 0x00},
	{0x7b142, 0xe0},
	{0x7b143, 0x00},
	{0x7b144, 0x4e},
	{0x7b145, 0x7b},
	{0x7b146, 0x00},
	{0x7b147, 0x04},
	{0x7b148, 0x20},
	{0x7b149, 0x3c},
	{0x7b14a, 0x00},
	{0x7b14c, 0xe0},
	{0x7b14d, 0x40},
	{0x7b14e, 0x4e},
	{0x7b14f, 0x7b},
	{0x7b150, 0x00},
	{0x7b151, 0x06},
	{0x7b152, 0x70},
	{0x7b153, 0x00},
	{0x7b154, 0x4e},
	{0x7b155, 0x7b},
	{0x7b156, 0x00},
	{0x7b157, 0x05},
	{0x7b158, 0x4e},
	{0x7b159, 0x7b},
	{0x7b15a, 0x00},
	{0x7b15b, 0x07},
	{0x7b15c, 0x4e},
	{0x7b15d, 0x71},
	{0x7b15e, 0x70},
	{0x7b15f, 0x00},
	{0x7b160, 0x4e},
	{0x7b161, 0x7b},
	{0x7b162, 0x00},
	{0x7b163, 0x03},
	{0x7b164, 0x4e},
	{0x7b165, 0x71},
	{0x7b166, 0x4e},
	{0x7b167, 0x7b},
	{0x7b168, 0x08},
	{0x7b169, 0x07},
	{0x7b16a, 0x4e},
	{0x7b16b, 0x7b},
	{0x7b16c, 0x08},
	{0x7b16d, 0x06},
	{0x7b16e, 0x4e},
	{0x7b16f, 0x71},
	{0x7b170, 0x4e},
	{0x7b171, 0xf9},
	{0x7b172, 0x00},
	{0x7b173, 0xe0},
	{0x7b174, 0x00},
	{0x7b175, 0x30},
	{0x7b176, 0x4e},
	{0x7b177, 0x7a},
	{0x7b178, 0x00},
	{0x7b179, 0x02},
	{0x7b17a, 0x08},
	{0x7b17b, 0x80},
	{0x7b17c, 0x00},
	{0x7b17d, 0x0f},
	{0x7b17e, 0x67},
	{0x7b17f, 0x0a},
	{0x7b180, 0x4e},
	{0x7b181, 0x7b},
	{0x7b182, 0x00},
	{0x7b183, 0x02},
	{0x7b184, 0x4e},
	{0x7b185, 0x71},
	{0x7b186, 0xf4},
	{0x7b187, 0x98},
	{0x7b188, 0x4e},
	{0x7b189, 0x71},
	{0x7b18a, 0x4e},
	{0x7b18b, 0xf9},
	{0x7b18c, 0xde},
	{0x7b18d, 0xad},
	{0x7b18e, 0xfa},
	{0x7b18f, 0xce},
	{0x7b190, 0x20},
	{0x7b191, 0x02},
	{0x7b192, 0x10},
	{0x7b193, 0x21},
	{0x7b194, 0x73},
	{0x7b195, 0x00},
	{0x7b196, 0x4e},
	{0x7b197, 0x75},
	{0x7b198, 0x73},
	{0x7b199, 0x01},
	{0x7b19a, 0x4e},
	{0x7b19b, 0x75},
	{0x7fffe, 0x31},
	{0x7ffff, 0x3d},
	{-1, 0}
};
