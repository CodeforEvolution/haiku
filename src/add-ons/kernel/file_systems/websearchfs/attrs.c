/*
 * Copyright 2004-2008, François Revol, <revol@free.fr>.
 * Distributed under the terms of the MIT License.
 */

#include <SupportDefs.h>
#include <TypeConstants.h>
#include "websearchfs.h"

#define SZSTR(s) sizeof(s), s
#define SZTAB(s) sizeof(s), s

// #pragma mark - root folder icons


// icons by ahwayakchih

const char websearch_icon_HVIF[] = {
	0x6e, 0x63, 0x69, 0x66, 0x09, 0x05, 0x00, 0x02, 0x01, 0x02, 0x02, 0x3b,
	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3b, 0x40, 0x00, 0x48,
	0x60, 0x00, 0x4a, 0xe0, 0x00, 0x00, 0xdc, 0xf3, 0xff, 0x5e, 0xff, 0x5f,
	0x94, 0xaa, 0x78, 0x02, 0x00, 0x16, 0x02, 0x34, 0x4f, 0xd5, 0x34, 0x79,
	0x20, 0xba, 0x5f, 0xcc, 0x3a, 0x38, 0x28, 0x47, 0xb3, 0xd1, 0x4b, 0x15,
	0x01, 0x00, 0xd4, 0xff, 0x47, 0x05, 0xff, 0x05, 0x00, 0x04, 0x00, 0x57,
	0x02, 0x01, 0x06, 0x03, 0x3d, 0x8a, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xbd, 0x8a, 0xfe, 0x48, 0xf1, 0x07, 0x48, 0xa0, 0x78, 0x31, 0x32,
	0xe4, 0xff, 0xc8, 0x00, 0x66, 0xff, 0xff, 0x00, 0x34, 0xcc, 0x02, 0x01,
	0x06, 0x03, 0x3d, 0x0a, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbd,
	0x0a, 0xfe, 0x48, 0xf1, 0x07, 0x48, 0xa0, 0x78, 0x54, 0x09, 0xb4, 0xb6,
	0xd8, 0x03, 0x37, 0x84, 0xff, 0x05, 0x1d, 0x61, 0x02, 0x01, 0x06, 0x04,
	0x3d, 0x95, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3d, 0x95, 0x0b,
	0x49, 0x4f, 0xbc, 0x47, 0x9b, 0xec, 0x00, 0xd5, 0xff, 0xd5, 0x4c, 0x6e,
	0xff, 0x6e, 0x90, 0x1e, 0xbc, 0x1e, 0xff, 0x04, 0xb2, 0x04, 0x10, 0x0a,
	0x0a, 0x23, 0x2f, 0x2d, 0x49, 0x4a, 0x5b, 0x5c, 0x2c, 0x50, 0x2a, 0x4c,
	0x2c, 0x3a, 0x28, 0x38, 0x2d, 0x30, 0x2b, 0xb9, 0x57, 0xba, 0xa7, 0x02,
	0x04, 0x32, 0x46, 0xbb, 0xee, 0x46, 0xb8, 0x69, 0x46, 0x2a, 0x4e, 0x2a,
	0xc3, 0x91, 0x2a, 0xc7, 0x16, 0x32, 0x56, 0xb8, 0x69, 0x56, 0xbb, 0xee,
	0x56, 0x3a, 0x4e, 0x3a, 0xc7, 0x16, 0x3a, 0xc3, 0x91, 0x06, 0x06, 0xba,
	0x06, 0x2b, 0x54, 0x24, 0x5a, 0xb4, 0xbe, 0xca, 0xc1, 0xb4, 0x8b, 0xca,
	0x8e, 0xb4, 0xf1, 0xca, 0xf4, 0x26, 0x5c, 0x2c, 0x55, 0x54, 0x02, 0x04,
	0x3c, 0x26, 0xc2, 0xff, 0x26, 0xb9, 0x50, 0x26, 0x26, 0x3c, 0x26, 0xb9,
	0x50, 0x26, 0xc2, 0xff, 0x3c, 0x52, 0xb9, 0x50, 0x52, 0xc2, 0xff, 0x52,
	0x52, 0x3c, 0x52, 0xc2, 0xff, 0x52, 0xb9, 0x50, 0x02, 0x6e, 0xc5, 0xdc,
	0xb8, 0xb6, 0xc7, 0x13, 0xba, 0xa0, 0xc4, 0xca, 0xb7, 0x06, 0xc1, 0x10,
	0xb5, 0x79, 0xc3, 0x07, 0xb5, 0xd5, 0xc0, 0xa2, 0xb5, 0x65, 0xbf, 0xc2,
	0xb5, 0x5b, 0xc0, 0x32, 0xb5, 0x5b, 0xbd, 0xbc, 0xb5, 0x5b, 0xba, 0x69,
	0xb7, 0xba, 0xbb, 0xc5, 0xb6, 0x3a, 0xb8, 0xf9, 0xb9, 0x4f, 0xb8, 0x9d,
	0xbd, 0x9c, 0xb8, 0x4f, 0xbb, 0x7e, 0xb8, 0xc7, 0xbe, 0x9a, 0xb9, 0x24,
	0xbf, 0x77, 0xb8, 0xe0, 0xbe, 0xe7, 0xb9, 0xde, 0xc1, 0x1f, 0xbc, 0xe0,
	0xc3, 0x32, 0xbb, 0x38, 0xc2, 0x79, 0xbe, 0xc5, 0xc4, 0x05, 0xc2, 0xd8,
	0xc3, 0x1a, 0xc0, 0xfb, 0xc3, 0xfc, 0xc2, 0xc6, 0xc2, 0xc1, 0xc2, 0x2f,
	0xc2, 0xb7, 0xc2, 0x7a, 0xc2, 0xbe, 0xc2, 0x2f, 0xc2, 0xb7, 0xc2, 0x27,
	0xc2, 0xb7, 0xc2, 0x27, 0xc2, 0xb7, 0xc2, 0x27, 0xc2, 0xb7, 0xc2, 0x41,
	0xc2, 0xbd, 0xc2, 0x41, 0xc2, 0xbd, 0xc2, 0x06, 0xc2, 0xb0, 0xc1, 0x99,
	0xc2, 0xdb, 0xc1, 0xd0, 0xc2, 0xcb, 0xc1, 0x59, 0xc2, 0xee, 0xc0, 0xd4,
	0xc2, 0xdd, 0xc1, 0x15, 0xc2, 0xe5, 0xc0, 0x51, 0xc2, 0xce, 0xbf, 0x43,
	0xc3, 0x13, 0xbf, 0xb0, 0xc2, 0xb4, 0xbf, 0x3d, 0xc3, 0x13, 0xbf, 0x36,
	0xc3, 0x13, 0xbf, 0x36, 0xc3, 0x13, 0xbf, 0x36, 0xc3, 0x13, 0xbf, 0x20,
	0xc3, 0x13, 0xbf, 0x20, 0xc3, 0x13, 0xbf, 0x20, 0xc3, 0x13, 0xbd, 0xed,
	0xc3, 0x13, 0xbe, 0x23, 0xc3, 0x13, 0xbd, 0xb9, 0xc2, 0xed, 0xbd, 0x57,
	0xc2, 0xdf, 0xbd, 0x87, 0xc2, 0xe3, 0xbd, 0x54, 0xc2, 0xdc, 0xbd, 0x3e,
	0xc2, 0xc3, 0xbd, 0x3e, 0xc2, 0xc3, 0xbd, 0x31, 0xc2, 0xb4, 0xbd, 0x13,
	0xc2, 0xa5, 0xbd, 0x26, 0xc2, 0xab, 0xbd, 0x13, 0xc2, 0xa5, 0xbc, 0xa4,
	0xc2, 0x7e, 0xbc, 0xa4, 0xc2, 0x7e, 0xbc, 0xa4, 0xc2, 0x7e, 0xbc, 0x31,
	0xc2, 0x54, 0xbc, 0x31, 0xc2, 0x54, 0xbc, 0x27, 0xc2, 0x51, 0xbc, 0x12,
	0xc2, 0x4f, 0xbc, 0x1d, 0xc2, 0x50, 0xbc, 0x10, 0xc2, 0x4b, 0xbc, 0x0f,
	0xc2, 0x42, 0xbc, 0x11, 0xc2, 0x46, 0xbc, 0x0f, 0xc2, 0x42, 0xbb, 0xf5,
	0xc2, 0x19, 0xbb, 0xf5, 0xc2, 0x19, 0xbb, 0xd3, 0xc1, 0xde, 0xbb, 0x68,
	0xc1, 0x95, 0xbb, 0xae, 0xc1, 0xa4, 0xbb, 0x71, 0xc1, 0x6e, 0xbb, 0x58,
	0xc1, 0x29, 0xbb, 0x63, 0xc1, 0x4e, 0xbb, 0x4b, 0xc1, 0x01, 0xbb, 0x4b,
	0xc0, 0xad, 0xbb, 0x48, 0xc0, 0xd7, 0xbb, 0x4b, 0xc0, 0xad, 0xbb, 0x4e,
	0xc0, 0x87, 0xbb, 0x4e, 0xc0, 0x87, 0xbb, 0x52, 0xc0, 0x54, 0xbb, 0x4d,
	0xbf, 0xee, 0xbb, 0x55, 0xc0, 0x21, 0xbb, 0x9e, 0xbf, 0x93, 0xbb, 0xa9,
	0xbe, 0xbf, 0xbb, 0xb0, 0xbf, 0x36, 0xbb, 0xa9, 0xbe, 0xbf, 0xbb, 0xa7,
	0xbe, 0x93, 0xbb, 0xa7, 0xbe, 0x93, 0xbb, 0xa7, 0xbe, 0x53, 0xbb, 0xbc,
	0xbe, 0x2f, 0xbb, 0xa8, 0xbe, 0x4b, 0xbb, 0xbc, 0xbe, 0x2f, 0xbb, 0xc7,
	0xbe, 0x21, 0xbb, 0xc7, 0xbe, 0x21, 0xbc, 0x00, 0xbd, 0xd5, 0xbb, 0xe3,
	0xbc, 0xfc, 0xbc, 0x5b, 0xbd, 0x3d, 0xbb, 0xe3, 0xbc, 0xfc, 0xba, 0xb7,
	0xbc, 0x58, 0xba, 0xb7, 0xbc, 0x58, 0xba, 0xb7, 0xbc, 0x58, 0xba, 0x8b,
	0xbc, 0x44, 0xba, 0x90, 0xbc, 0x46, 0xba, 0x8b, 0xbc, 0x44, 0xba, 0x8b,
	0xbc, 0x42, 0xba, 0x8b, 0xbc, 0x42, 0xba, 0x89, 0xbb, 0xea, 0xba, 0x23,
	0xbb, 0x79, 0xba, 0x6a, 0xbb, 0xad, 0xba, 0x23, 0xbb, 0x79, 0xba, 0x12,
	0xbb, 0x6b, 0xba, 0x12, 0xbb, 0x6b, 0xba, 0x12, 0xbb, 0x6b, 0xb9, 0xf8,
	0xbb, 0x49, 0xb9, 0xf8, 0xbb, 0x49, 0xb9, 0xf8, 0xbb, 0x49, 0xb9, 0xae,
	0xba, 0xf0, 0xb9, 0xae, 0xba, 0xf0, 0xb9, 0x7e, 0xba, 0xc3, 0xb9, 0x28,
	0xba, 0xb1, 0xb9, 0x50, 0xba, 0xb5, 0xb9, 0x24, 0xba, 0xa9, 0xb9, 0x1b,
	0xba, 0x9a, 0xb9, 0x20, 0xba, 0xa1, 0xb9, 0x84, 0xba, 0x6e, 0xb9, 0xde,
	0xb9, 0x97, 0xb9, 0xb5, 0xb9, 0xf9, 0xb9, 0xf6, 0xb9, 0x5d, 0xba, 0x36,
	0xb8, 0xf0, 0xba, 0x0b, 0xb9, 0x1f, 0xba, 0x5e, 0xb8, 0xc4, 0xba, 0x78,
	0xb8, 0x5e, 0xba, 0x83, 0xb8, 0x9e, 0xba, 0xcc, 0xb8, 0x3d, 0xbb, 0x61,
	0xb7, 0xe7, 0xbb, 0x18, 0xb8, 0x13, 0xbb, 0x61, 0xb7, 0xe7, 0xbb, 0x6b,
	0xb7, 0xe1, 0xbb, 0x6b, 0xb7, 0xe1, 0xbb, 0x6b, 0xb7, 0xe1, 0xbb, 0xfe,
	0xb7, 0x87, 0xbb, 0xfe, 0xb7, 0x87, 0xbc, 0x65, 0xb7, 0x4a, 0xbc, 0xb6,
	0xb6, 0x84, 0xbc, 0x94, 0xb6, 0xf6, 0xbc, 0xf5, 0xb6, 0x8a, 0xbd, 0x58,
	0xb6, 0x65, 0xbd, 0x2d, 0xb6, 0x7b, 0xbd, 0xa8, 0xb6, 0x95, 0xbe, 0x6d,
	0xb6, 0x51, 0xbe, 0x29, 0xb6, 0x97, 0xbe, 0x6f, 0xb6, 0x50, 0xbe, 0x88,
	0xb6, 0x4f, 0xbe, 0x7c, 0xb6, 0x4f, 0xbe, 0x62, 0xb6, 0xa0, 0xbe, 0xe1,
	0xb7, 0x2f, 0xbe, 0x88, 0xb7, 0x0e, 0xbe, 0xe4, 0xb7, 0x33, 0xbe, 0xe9,
	0xb7, 0x3e, 0xbe, 0xe5, 0xb7, 0x3a, 0xbe, 0xd8, 0xb7, 0x44, 0xbe, 0xc6,
	0xb7, 0x4a, 0xbe, 0xc6, 0xb7, 0x4a, 0xbe, 0xc6, 0xb7, 0x4a, 0xbe, 0x75,
	0xb7, 0xa5, 0xbe, 0x75, 0xb7, 0xa5, 0xbe, 0x63, 0xb7, 0xdd, 0xbe, 0x90,
	0xb8, 0x49, 0xbe, 0x74, 0xb8, 0x17, 0xbe, 0x91, 0xb8, 0x4c, 0xbe, 0x92,
	0xb8, 0x52, 0xbe, 0x92, 0xb8, 0x52, 0xbe, 0x94, 0xb8, 0x5c, 0xbe, 0x9d,
	0xb8, 0x7a, 0xbe, 0x97, 0xb8, 0x6b, 0xbe, 0x94, 0xb8, 0x84, 0xbe, 0x8c,
	0xb8, 0x8d, 0xbe, 0x8c, 0xb8, 0x8d, 0xbe, 0x3a, 0xb8, 0xf0, 0xbd, 0x92,
	0xb9, 0x8f, 0xbd, 0xb7, 0xb9, 0x06, 0xbd, 0x92, 0xb9, 0x8f, 0xbd, 0x8d,
	0xb9, 0xa1, 0xbd, 0x8d, 0xb9, 0xa1, 0xbd, 0x64, 0xb9, 0xd8, 0xbd, 0x7e,
	0xba, 0x55, 0xbd, 0x59, 0xba, 0x19, 0xbd, 0x7b, 0xba, 0x5c, 0xbd, 0x77,
	0xba, 0x63, 0xbd, 0x77, 0xba, 0x63, 0xbd, 0x56, 0xba, 0xa0, 0xbd, 0x4d,
	0xbb, 0x2c, 0xbd, 0x39, 0xba, 0xe6, 0xbd, 0x60, 0xbb, 0x70, 0xbd, 0xcc,
	0xbb, 0xce, 0xbd, 0x98, 0xbb, 0xa2, 0xbd, 0xff, 0xbb, 0xfa, 0xbe, 0x73,
	0xbc, 0x3f, 0xbe, 0x33, 0xbc, 0x27, 0xbe, 0xbe, 0xbc, 0x5c, 0xbf, 0x5e,
	0xbc, 0x42, 0xbf, 0x12, 0xbc, 0x56, 0xbf, 0x5e, 0xbc, 0x42, 0xbf, 0xac,
	0x37, 0xbf, 0xac, 0x37, 0xbf, 0xd0, 0xbc, 0x1e, 0xc0, 0x1c, 0xbc, 0x0f,
	0xbf, 0xf5, 0xbc, 0x0b, 0xc0, 0x46, 0xbc, 0x14, 0xc0, 0x83, 0xbc, 0x4e,
	0xc0, 0x66, 0xbc, 0x35, 0xc0, 0x4d, 0xbc, 0xd9, 0xc0, 0xf4, 0xbd, 0xda,
	0xc0, 0xb6, 0xbd, 0x63, 0xc0, 0xf4, 0xbd, 0xda, 0xc1, 0x07, 0xbd, 0xf9,
	0xc1, 0x02, 0xbd, 0xf2, 0xc0, 0xba, 0xbe, 0x84, 0xc1, 0x27, 0xbf, 0x91,
	0xc0, 0xb2, 0xbf, 0x1d, 0xc1, 0x27, 0xbf, 0x91, 0xc1, 0x33, 0xbf, 0x9d,
	0xc1, 0x33, 0xbf, 0x9d, 0xc1, 0x31, 0xc0, 0x0f, 0xc1, 0xc5, 0xc0, 0xf9,
	0xc1, 0x35, 0xc0, 0xd6, 0xc2, 0x59, 0xc1, 0x1c, 0xc3, 0x48, 0xc0, 0x59,
	0xc2, 0xee, 0xc0, 0xc9, 0xc3, 0x48, 0xc0, 0x59, 0xc3, 0x6b, 0xc0, 0x2f,
	0xc3, 0x6b, 0xc0, 0x2f, 0xc3, 0x6b, 0xc0, 0x2f, 0xc3, 0x88, 0xc0, 0x19,
	0xc3, 0x88, 0xc0, 0x19, 0xc3, 0xa8, 0xc0, 0x03, 0xc3, 0xdb, 0xbf, 0xc5,
	0xc3, 0xc9, 0xbf, 0xe8, 0xc3, 0xee, 0xbf, 0xa0, 0xc4, 0x09, 0xbf, 0x52,
	0xc3, 0xf0, 0xbf, 0x73, 0xc4, 0x09, 0xbf, 0x52, 0xc4, 0x09, 0xbf, 0x53,
	0xc4, 0x09, 0xbf, 0x53, 0xc4, 0x0d, 0xbf, 0x9a, 0xc4, 0x95, 0xbf, 0xc1,
	0xc4, 0x50, 0xbf, 0xce, 0xc4, 0xeb, 0xbf, 0xb3, 0xc5, 0x42, 0xbf, 0x10,
	0xc5, 0x1f, 0xbf, 0x56, 0xc5, 0x42, 0xbf, 0x10, 0xc5, 0x51, 0xbe, 0xf2,
	0xc5, 0x51, 0xbe, 0xf2, 0xc5, 0x51, 0xbe, 0xf2, 0xc5, 0x55, 0xbe, 0xeb,
	0xc5, 0x55, 0xbe, 0xeb, 0xc5, 0x7f, 0xbe, 0xa3, 0xc5, 0x68, 0xbd, 0xfc,
	0xc5, 0x9e, 0xbe, 0x47, 0xc5, 0x3a, 0xbd, 0xbb, 0xc4, 0xa3, 0xbd, 0xdf,
	0xc4, 0xe1, 0xbd, 0xaa, 0xc4, 0xa3, 0xbd, 0xca, 0xc4, 0xa3, 0xbd, 0xb5,
	0xc4, 0xa3, 0xbd, 0xb5, 0xc4, 0xa3, 0xbd, 0xb5, 0xc4, 0xa5, 0xbd, 0x31,
	0xc4, 0xa5, 0xbd, 0x31, 0xc4, 0xa8, 0xbd, 0x1e, 0xc4, 0xe8, 0xbc, 0xdf,
	0xc4, 0xd2, 0xbc, 0xf5, 0xc4, 0xe8, 0xbc, 0xdf, 0xc4, 0xfe, 0xbc, 0xc9,
	0xc4, 0xfe, 0xbc, 0xc9, 0xc4, 0xfe, 0xbc, 0xc9, 0xc5, 0x16, 0xbc, 0xb3,
	0xc5, 0x16, 0xbc, 0xb3, 0xc5, 0x45, 0xbc, 0x8b, 0xc5, 0x82, 0xbc, 0x20,
	0xc5, 0x71, 0xbc, 0x5e, 0xc5, 0x8c, 0xbc, 0x05, 0xc5, 0x88, 0xbb, 0xbc,
	0xc5, 0x86, 0xbb, 0xd9, 0xc5, 0x88, 0xbb, 0xbc, 0xc5, 0x87, 0xbb, 0xa8,
	0xc5, 0x87, 0xbb, 0xa8, 0xc5, 0x87, 0xbb, 0xa8, 0xc5, 0x88, 0xbb, 0x9d,
	0xc5, 0x88, 0xbb, 0x9d, 0xc5, 0x8d, 0xbb, 0x61, 0x4e, 0xba, 0xfa, 0xc5,
	0x84, 0xbb, 0x24, 0xc5, 0x8a, 0xba, 0xda, 0xc5, 0xc2, 0xba, 0x5d, 0xc5,
	0xb0, 0xba, 0xa2, 0xc5, 0xc4, 0xba, 0x62, 0xc5, 0xcb, 0xba, 0x70, 0xc5,
	0xcb, 0xba, 0x70, 0xc5, 0xd9, 0xba, 0x8e, 0xc6, 0x0f, 0xba, 0xd4, 0xc5,
	0xed, 0xba, 0xb3, 0xc6, 0x0a, 0xbb, 0x1f, 0xc6, 0x48, 0xbb, 0xaa, 0xc6,
	0x27, 0xbb, 0x67, 0xc6, 0x69, 0xbb, 0xeb, 0xc6, 0x6f, 0xbc, 0x7b, 0xc6,
	0x70, 0xbc, 0x33, 0xc6, 0x6f, 0xbc, 0x7b, 0xc6, 0x72, 0xbc, 0xc4, 0xc6,
	0x72, 0xbc, 0xc4, 0xc6, 0x74, 0xbc, 0xea, 0xc6, 0x6e, 0xbd, 0x37, 0xc6,
	0x78, 0xbd, 0x11, 0xc6, 0x64, 0xbd, 0x5a, 0xc6, 0x49, 0xbd, 0x9c, 0xc6,
	0x52, 0xbd, 0x79, 0xc6, 0x33, 0xbd, 0xec, 0xc6, 0x2c, 0xbe, 0x92, 0xc6,
	0x2d, 0xbe, 0x3f, 0xc6, 0x21, 0xbe, 0xd5, 0xc6, 0x7f, 0xbf, 0x2c, 0xc6,
	0x48, 0xbf, 0x16, 0xc7, 0x50, 0xbd, 0x0f, 0x00, 0x06, 0xbc, 0x12, 0xb6,
	0xe4, 0xbc, 0x12, 0xb6, 0xe4, 0xbc, 0x32, 0xb6, 0xb0, 0xbc, 0x55, 0xb6,
	0x38, 0xbc, 0x47, 0xb6, 0x74, 0xbb, 0x81, 0xb6, 0xab, 0xba, 0x2b, 0xb8,
	0x02, 0xba, 0xc4, 0xb7, 0x47, 0xba, 0x88, 0xb7, 0xe3, 0xbb, 0x30, 0xb7,
	0x7e, 0xba, 0xdd, 0xb7, 0xb0, 0xbb, 0x59, 0xb7, 0x65, 0xbb, 0xaa, 0xb7,
	0x33, 0xbb, 0x82, 0xb7, 0x4d, 0xbb, 0xcb, 0xb7, 0x1d, 0xbc, 0x12, 0xb6,
	0xe4, 0xbb, 0xfc, 0xb7, 0x07, 0xbc, 0x12, 0xb6, 0xe4, 0x00, 0x1d, 0xba,
	0xcd, 0xbf, 0xd1, 0xba, 0xcd, 0xbf, 0xd1, 0xba, 0xeb, 0xbf, 0xaa, 0xbb,
	0x29, 0xbf, 0x5c, 0xbb, 0x13, 0xbf, 0x89, 0xbb, 0x43, 0xbf, 0x24, 0xbb,
	0x34, 0xbe, 0x93, 0xbb, 0x34, 0xbe, 0xd0, 0xbb, 0x34, 0xbe, 0x4d, 0xbb,
	0x60, 0xbd, 0xea, 0xbb, 0x34, 0xbe, 0x25, 0xbb, 0x80, 0xbd, 0xbe, 0xbb,
	0xab, 0xbd, 0x60, 0xbb, 0xa1, 0xbd, 0x98, 0xbb, 0x48, 0xbd, 0x2a, 0xba,
	0x81, 0xbc, 0xbe, 0xba, 0xe5, 0xbc, 0xf3, 0xba, 0x67, 0xbc, 0xb1, 0xba,
	0x20, 0xbc, 0x83, 0xba, 0x2d, 0xbc, 0xa0, 0xba, 0x0f, 0xbc, 0x5b, 0xba,
	0x0d, 0xbc, 0x02, 0xba, 0x1f, 0xbc, 0x28, 0xb9, 0xfe, 0xbb, 0xe5, 0xb9,
	0xba, 0xbb, 0xb6, 0xb9, 0xd0, 0xbb, 0xd0, 0xb9, 0x9b, 0xbb, 0x91, 0xb9,
	0x5f, 0xbb, 0x44, 0xb9, 0x83, 0xbb, 0x66, 0xb9, 0x3d, 0xbb, 0x24, 0xb8,
	0xf0, 0xbb, 0x2d, 0xb9, 0x1c, 0xbb, 0x16, 0xb8, 0xc0, 0xbb, 0x48, 0xb8,
	0xbe, 0xba, 0xdf, 0xb8, 0xc3, 0xba, 0xff, 0xb8, 0xb1, 0xbb, 0x14, 0xb8,
	0x9f, 0xbb, 0x7e, 0xb8, 0xa7, 0xbb, 0x49, 0xb8, 0xc9, 0xbb, 0x7e, 0xb8,
	0xc5, 0xbb, 0xb2, 0xb8, 0xc3, 0xbb, 0x8a, 0xb8, 0xc6, 0xbb, 0xdb, 0xb8,
	0xc9, 0xbc, 0x2e, 0xb8, 0xc7, 0xbc, 0x05, 0xb8, 0xd0, 0xbc, 0x40, 0xb8,
	0xba, 0xbc, 0x49, 0xb8, 0xcb, 0xbc, 0x49, 0xb8, 0xb4, 0xbc, 0x4b, 0xb8,
	0xaa, 0xbc, 0x4e, 0xb8, 0xaf, 0xbc, 0x4d, 0xb8, 0xa0, 0xbc, 0x57, 0xb8,
	0x93, 0xbc, 0x4b, 0xb8, 0x98, 0xbc, 0x56, 0xb8, 0x82, 0xbc, 0x4a, 0xb8,
	0x8f, 0xbd, 0x10, 0xb8, 0x8e, 0xbd, 0x00, 0xb8, 0x93, 0xbd, 0x49, 0xb8,
	0xe1, 0xbd, 0xe7, 0xb8, 0x8d, 0xbe, 0x03, 0xb9, 0x0d, 0xbe, 0x04, 0xb9,
	0x23, 0xbe, 0x75, 0xb9, 0x06, 0xbe, 0x4b, 0xb9, 0x46, 0xbe, 0xa8, 0xb9,
	0x85, 0xbf, 0x00, 0xb9, 0x73, 0xbe, 0xc1, 0xb9, 0x97, 0xbf, 0x3e, 0xb9,
	0xa3, 0xbf, 0xc1, 0xb9, 0x98, 0xbf, 0x81, 0xb9, 0xb0, 0xc0, 0x11, 0xba,
	0x17, 0xc0, 0x5f, 0xb9, 0xdb, 0xc0, 0x2e, 0xba, 0x25, 0xc0, 0xb3, 0xba,
	0x75, 0xc1, 0x27, 0xba, 0x3d, 0xc0, 0xe7, 0xba, 0x8f, 0xc1, 0x44, 0xba,
	0xc0, 0xc1, 0x7f, 0xba, 0xa9, 0xc1, 0x60, 0xba, 0xd8, 0xc1, 0x9f, 0xba,
	0xfc, 0xc1, 0x7c, 0xba, 0xe4, 0xc1, 0x9d, 0xba, 0xdd, 0xc1, 0x3a, 0xba,
	0xd7, 0xc0, 0xa6, 0xba, 0xd3, 0xc0, 0xef, 0xba, 0xdb, 0xc0, 0x61, 0xba,
	0xcd, 0xbf, 0xd1, 0xba, 0xea, 0xc0, 0x13, 0xba, 0xcd, 0xbf, 0xd1, 0x00,
	0x09, 0xbd, 0x5e, 0xb5, 0xd7, 0xbd, 0x5e, 0xb5, 0xd7, 0xbd, 0x81, 0xb5,
	0xee, 0xbd, 0xb1, 0xb6, 0x0e, 0xbd, 0x84, 0xb6, 0x05, 0xbd, 0xd0, 0xb6,
	0x13, 0xbe, 0x19, 0xb6, 0x01, 0xbe, 0x01, 0xb6, 0x1a, 0xbe, 0x55, 0xb5,
	0xc3, 0xbe, 0xf6, 0xb5, 0xc4, 0xbe, 0xb0, 0xb5, 0xef, 0xbf, 0x2d, 0xb5,
	0xa2, 0xbf, 0xa3, 0xb5, 0x5e, 0xbf, 0x5b, 0xb5, 0x5e, 0xbf, 0xce, 0xb5,
	0x5d, 0xc0, 0x0c, 0xb5, 0x8c, 0xbf, 0xe9, 0xb5, 0x78, 0xc0, 0x3c, 0xb5,
	0xa7, 0xc0, 0x90, 0xb5, 0x66, 0xc0, 0x64, 0xb5, 0x75, 0xbf, 0x3a, 0xb5,
	0x41, 0xbc, 0xa7, 0xb6, 0x0e, 0xbd, 0xdd, 0xb5, 0x7a, 0xbc, 0xec, 0xb6,
	0x23, 0xbd, 0x5e, 0xb5, 0xd7, 0xbd, 0x29, 0xb6, 0x01, 0xbd, 0x5e, 0xb5,
	0xd7, 0x00, 0x05, 0xbf, 0x35, 0xb6, 0xea, 0xbf, 0x35, 0xb6, 0xea, 0xbf,
	0x55, 0xb6, 0xec, 0xbf, 0x9a, 0xb6, 0xf5, 0xbf, 0x7b, 0xb7, 0x02, 0xbf,
	0xce, 0xb6, 0xe0, 0xbf, 0x87, 0xb6, 0x7d, 0xbf, 0x9c, 0xb6, 0x93, 0xbf,
	0x78, 0xb6, 0x6d, 0xbf, 0x3c, 0xb6, 0x64, 0xbf, 0x2f, 0xb6, 0x23, 0xbf,
	0x45, 0xb6, 0x90, 0xbf, 0x35, 0xb6, 0xea, 0xbf, 0x6b, 0xb6, 0xcc, 0xbf,
	0x35, 0xb6, 0xea, 0x00, 0x04, 0xba, 0x07, 0xb8, 0x7f, 0xba, 0x07, 0xb8,
	0x7f, 0xba, 0x0c, 0xb8, 0x68, 0xb9, 0xef, 0xb8, 0x50, 0xb9, 0xfe, 0xb8,
	0x5f, 0xb9, 0x82, 0xb8, 0xe4, 0xb8, 0xf0, 0xba, 0x35, 0xb9, 0x2c, 0xb9,
	0x88, 0xb9, 0x7e, 0x31, 0xba, 0x07, 0xb8, 0x7f, 0xb9, 0x74, 0xb8, 0xf0,
	0xba, 0x07, 0xb8, 0x7f, 0x00, 0x66, 0xc6, 0xf7, 0xbc, 0x24, 0xc6, 0xf7,
	0xbc, 0x24, 0xc6, 0xdc, 0xbb, 0x0a, 0xc6, 0x00, 0x2f, 0xc6, 0x8b, 0xb9,
	0xf2, 0xc5, 0xb9, 0xb8, 0x7c, 0xc4, 0xff, 0xb7, 0x9c, 0xc5, 0x63, 0xb8,
	0x05, 0xc4, 0xce, 0xb7, 0x68, 0xc4, 0x63, 0xb7, 0x09, 0xc4, 0x9a, 0xb7,
	0x37, 0xc4, 0x52, 0xb6, 0xfb, 0xc4, 0x28, 0xb6, 0xef, 0xc4, 0x40, 0xb6,
	0xe2, 0xc4, 0x12, 0xb6, 0xfa, 0xc4, 0x07, 0xb6, 0xc1, 0xc4, 0x01, 0xb6,
	0xd1, 0xc3, 0x16, 0xb6, 0x0f, 0xc0, 0xd0, 0xb5, 0x6f, 0xc1, 0xf9, 0xb5,
	0x9a, 0xc0, 0xfb, 0xb5, 0x8b, 0xc1, 0x5f, 0xb5, 0x9c, 0xc1, 0x31, 0xb5,
	0x89, 0xc1, 0x70, 0xb5, 0xa2, 0xc1, 0x79, 0xb5, 0xc9, 0xc1, 0x9f, 0xb5,
	0xbd, 0xc1, 0x2d, 0xb5, 0xe2, 0xc0, 0x8a, 0xb5, 0xc9, 0xc0, 0xd7, 0xb5,
	0xc4, 0xc0, 0x5a, 0xb5, 0xcd, 0xc0, 0x3f, 0xb6, 0x0e, 0xc0, 0x43, 0xb5,
	0xdd, 0xc0, 0x3d, 0xb6, 0x29, 0xc0, 0x40, 0xb6, 0x76, 0xc0, 0x36, 0xb6,
	0x5d, 0xc0, 0x4b, 0xb6, 0x94, 0xc0, 0xb6, 0xb6, 0x5f, 0xc0, 0xa8, 0xb6,
	0x6f, 0xc0, 0xcd, 0xb6, 0x43, 0xc0, 0xf1, 0xb6, 0x1a, 0xc0, 0xb1, 0xb6,
	0x08, 0xc1, 0x08, 0xb6, 0x4b, 0xc0, 0xd0, 0xb6, 0xb1, 0xc1, 0x3a, 0xb6,
	0xd6, 0xc0, 0xb5, 0xb6, 0xa7, 0xc0, 0x66, 0xb6, 0x89, 0xc0, 0x83, 0xb6,
	0x7c, 0xc0, 0x64, 0xb6, 0x8a, 0xc0, 0x5d, 0xb6, 0xaf, 0xc0, 0x5f, 0xb6,
	0xab, 0xc0, 0x53, 0xb6, 0xc0, 0xc0, 0x31, 0xb6, 0xd0, 0xc0, 0x44, 0xb6,
	0xcb, 0xbf, 0xe6, 0xb6, 0xe5, 0xbf, 0x94, 0xb7, 0x41, 0xbf, 0xb0, 0xb6,
	0xef, 0xbf, 0x87, 0xb7, 0x69, 0xbf, 0x5d, 0xb7, 0x91, 0xbf, 0x88, 0xb7,
	0x81, 0xbf, 0x3b, 0xb7, 0x9d, 0xbe, 0xf7, 0xb7, 0xb2, 0xbf, 0x17, 0xb7,
	0xa4, 0xbe, 0xcc, 0xb7, 0xc7, 0xbe, 0xfa, 0xb8, 0x1c, 0xbe, 0xea, 0xb7,
	0xff, 0xbf, 0x06, 0xb8, 0x30, 0xbf, 0x19, 0xb8, 0x6c, 0xbf, 0x03, 0xb8,
	0x60, 0xbf, 0x59, 0xb8, 0x91, 0xbf, 0xbe, 0xb7, 0xe0, 0xbf, 0xa3, 0xb8,
	0x02, 0xbf, 0xe1, 0xb7, 0xb3, 0xc0, 0x38, 0xb7, 0xad, 0xc0, 0x08, 0xb7,
	0xbd, 0xc0, 0x4f, 0xb7, 0xa6, 0xc0, 0x84, 0xb7, 0x77, 0xc0, 0x6c, 0xb7,
	0x6e, 0xc0, 0xd3, 0xb7, 0x95, 0xc0, 0xf8, 0xb8, 0x3e, 0xc0, 0xeb, 0xb7,
	0xf4, 0xc1, 0x10, 0xb8, 0x3f, 0xc1, 0x35, 0xb8, 0x10, 0xc1, 0x00, 0xb8,
	0x02, 0xc1, 0x0d, 0xb7, 0xe5, 0xc0, 0xec, 0xb7, 0x70, 0x43, 0xb7, 0xab,
	0xc1, 0x25, 0xb7, 0x95, 0xc1, 0x5f, 0xb7, 0xfd, 0xc1, 0x4d, 0xb7, 0xbd,
	0xc1, 0x70, 0xb8, 0x3d, 0xc1, 0xc2, 0xb8, 0x20, 0xc1, 0x8e, 0xb8, 0x60,
	0xc1, 0xcf, 0xb8, 0x13, 0xc1, 0xfd, 0xb7, 0xd5, 0xc1, 0xe2, 0xb7, 0xbd,
	0xc2, 0x14, 0xb7, 0xea, 0xc1, 0xf1, 0xb8, 0x38, 0xc2, 0x15, 0xb8, 0x2f,
	0xc2, 0x06, 0xb8, 0x4b, 0xc2, 0x3a, 0xb8, 0x3d, 0xc2, 0x22, 0xb8, 0x43,
	0xc2, 0x5a, 0xb8, 0x34, 0xc2, 0x8b, 0xb8, 0x42, 0xc2, 0x6c, 0xb8, 0x36,
	0xc2, 0xaf, 0xb8, 0x50, 0xc2, 0xfa, 0xb8, 0x57, 0xc2, 0xd5, 0xb8, 0x54,
	0xc3, 0x20, 0xb8, 0x59, 0xc3, 0x47, 0xb8, 0x98, 0xc3, 0x35, 0xb8, 0x7b,
	0xc3, 0x77, 0xb8, 0xe7, 0xc2, 0x4a, 0xb8, 0xc5, 0xc2, 0x59, 0xb8, 0xc6,
	0xc2, 0x28, 0xb8, 0xc3, 0xc1, 0xe2, 0xb8, 0xd0, 0xc2, 0x02, 0xb8, 0xc2,
	0xc1, 0xc9, 0xb8, 0xda, 0xc1, 0x99, 0xb8, 0xf0, 0xc1, 0xb6, 0x2f, 0xc1,
	0x79, 0xb8, 0xe5, 0xc1, 0x3f, 0xb8, 0xb6, 0xc1, 0x5b, 0xb8, 0xc8, 0xc1,
	0x18, 0xb8, 0x9c, 0xc0, 0xc5, 0xb8, 0x72, 0xc0, 0xef, 0xb8, 0x85, 0xc0,
	0x7f, 0xb8, 0x53, 0xbf, 0xe3, 0xb8, 0x4f, 0xc0, 0x2f, 0xb8, 0x3f, 0xbf,
	0xbf, 0xb8, 0x56, 0xbf, 0x7f, 0xb8, 0x7c, 0xbf, 0x9c, 0xb8, 0x66, 0xbf,
	0x5c, 0xb8, 0x97, 0xbf, 0x11, 0xb8, 0xa4, 0xbf, 0x35, 0xb8, 0x7f, 0xbe,
	0xdc, 0xb8, 0xdc, 0xbe, 0x6e, 0xb9, 0x42, 0xbe, 0xb6, 0xb9, 0x1f, 0xbe,
	0x51, 0xb9, 0x50, 0xbe, 0x19, 0xb9, 0x76, 0xbe, 0x2e, 0xb9, 0x5c, 0xbe,
	0x04, 0xb9, 0x90, 0xbd, 0xf6, 0xb9, 0xd1, 0xbe, 0x04, 0xb9, 0xb4, 0xbd,
	0xe9, 0xb9, 0xee, 0xbd, 0xe2, 0xba, 0x1d, 0xbd, 0xca, 0xb9, 0xfc, 0xbd,
	0xf8, 0xba, 0x3c, 0xbd, 0xef, 0xba, 0x76, 0xbd, 0xfe, 0xba, 0x53, 0xbd,
	0xd9, 0xba, 0xa9, 0xbd, 0xc9, 0xbb, 0x27, 0xbd, 0x9f, 0xba, 0xef, 0xbd,
	0xf2, 0x35, 0xbe, 0x71, 0xbb, 0xbc, 0xbe, 0x3a, 0xbb, 0x94, 0xbe, 0xaf,
	0xbb, 0xe9, 0xbf, 0x41, 0xbb, 0xd2, 0xbe, 0xfb, 0xbb, 0xe5, 0xbf, 0x83,
	0xbb, 0xc1, 0xc0, 0x09, 0xbb, 0x9c, 0xbf, 0xc5, 0xbb, 0xa1, 0xc0, 0x5f,
	0xbb, 0x96, 0xc0, 0xdd, 0xbc, 0x03, 0xc0, 0xa1, 0xbb, 0xcd, 0xc1, 0x04,
	0xbc, 0x26, 0xc0, 0xf5, 0xbc, 0x67, 0xc1, 0x14, 0xbc, 0x34, 0xc0, 0xd3,
	0xbc, 0x9f, 0xc1, 0x03, 0xbd, 0x00, 0xc0, 0xe8, 0xbc, 0xc8, 0xc1, 0x1f,
	0xbd, 0x37, 0xc1, 0x59, 0xbd, 0xa3, 0xc1, 0x3c, 0xbd, 0x6d, 0xc1, 0x66,
	0xbd, 0xba, 0xc1, 0x82, 0xbd, 0xe8, 0xc1, 0x77, 0xbd, 0xd0, 0xc1, 0x8e,
	0xbe, 0x04, 0xc1, 0x6d, 0xbe, 0x2d, 0xc1, 0x7b, 0xbe, 0x15, 0xc1, 0x52,
	0xbe, 0x5e, 0xc1, 0x3f, 0xbe, 0xcf, 0xc1, 0x3f, 0xbe, 0x97, 0xc1, 0x3f,
	0xbf, 0x0e, 0xc1, 0x85, 0xbf, 0x4c, 0xc1, 0x5d, 0xbf, 0x24, 0xc1, 0xaa,
	0xbf, 0x71, 0xc1, 0xa4, 0xbf, 0xe3, 0xc1, 0xa8, 0xbf, 0xb3, 0xc1, 0xa0,
	0xc0, 0x25, 0xc1, 0xdb, 0xc0, 0x87, 0xc1, 0xb3, 0xc0, 0x53, 0xc2, 0x16,
	0xc0, 0x8a, 0xc2, 0x89, 0xc0, 0x6c, 0xc2, 0x53, 0xc0, 0x86, 0xc2, 0xcb,
	0xc0, 0x4d, 0xc3, 0x19, 0xbf, 0xde, 0xc2, 0xe8, 0xc0, 0x0f, 0xc3, 0x47,
	0xbf, 0xb0, 0xc3, 0x86, 0xbf, 0x66, 0xc3, 0x78, 0xbf, 0xab, 0xc3, 0x96,
	0xbf, 0x13, 0xc3, 0xfe, 0xbe, 0xbf, 0xc3, 0xc2, 0xbe, 0xf2, 0xc4, 0x35,
	0xbe, 0x90, 0xc4, 0x30, 0xbe, 0x0c, 0xc4, 0x2e, 0xbe, 0x4d, 0xc4, 0x32,
	0xbd, 0xc0, 0xc4, 0x33, 0xbd, 0x22, 0xc4, 0x29, 0xbd, 0x6d, 0xc4, 0x3c,
	0xbc, 0xde, 0xc4, 0xab, 0xbc, 0x78, 0xc4, 0x7e, 0xbc, 0xa7, 0xc4, 0xcd,
	0xbc, 0x56, 0xc5, 0x12, 0xbc, 0x03, 0xc5, 0x05, 0xbc, 0x34, 0xc5, 0x19,
	0xbb, 0xe7, 0xc5, 0x13, 0xbb, 0xa8, 0xc5, 0x13, 0x36, 0xc5, 0x13, 0xbb,
	0x93, 0xc5, 0x11, 0x35, 0xc5, 0x1a, 0xbb, 0x71, 0xc4, 0xf8, 0xbb, 0x29,
	0xc4, 0x7e, 0xbb, 0x2c, 0xc4, 0x8a, 0xbb, 0x5d, 0xc4, 0x76, 0xbb, 0x0e,
	0xc4, 0x8b, 0xbb, 0x01, 0xc4, 0x77, 0xbb, 0x14, 0xc4, 0xa1, 0xba, 0xed,
	0xc4, 0x78, 0xba, 0xcd, 0xc4, 0x8b, 0xba, 0xd8, 0xc4, 0x42, 0xba, 0xad,
	0xc3, 0xf1, 0xba, 0x66, 0xc4, 0x18, 0xba, 0x9a, 0xc3, 0xcc, 0xba, 0x36,
	0xc3, 0x91, 0xb9, 0xcb, 0xc3, 0xae, 0xba, 0x00, 0xc3, 0x89, 0xb9, 0xbe,
	0xc3, 0x6c, 0xb9, 0x3b, 0xc3, 0x42, 0xb9, 0x2e, 0xc3, 0x9a, 0xb9, 0x4a,
	0xc3, 0xe2, 0xb9, 0xa3, 0xc3, 0xc4, 0xb9, 0x81, 0xc4, 0x19, 0xb9, 0xdf,
	0xc4, 0x6d, 0xba, 0x6a, 0xc4, 0x4a, 0xba, 0x20, 0xc4, 0x6e, 0xba, 0x73,
	0xc4, 0x96, 0xba, 0x8a, 0xc4, 0x8f, 0xba, 0x85, 0xc4, 0xaf, 0xba, 0x9c,
	0xc4, 0xea, 0xba, 0xa8, 0xc4, 0xca, 0xba, 0xab, 0xc5, 0x33, 0xba, 0xa2,
	0xc5, 0x5a, 0xba, 0x20, 0xc5, 0x55, 0xba, 0x61, 0xc5, 0x62, 0xb9, 0xc7,
	0xc4, 0xe8, 0xb9, 0xaf, 0xc5, 0x1d, 0xb9, 0xda, 0xc4, 0xca, 0xb9, 0x97,
	0xc4, 0x9e, 0xb9, 0x58, 0xc4, 0xb6, 0xb9, 0x75, 0xc4, 0x8f, 0xb9, 0x46,
	0xc4, 0x74, 0xb9, 0x0e, 0xc4, 0x66, 0xb9, 0x2a, 0xc4, 0x93, 0xb8, 0xf9,
	0xc4, 0xbd, 0xb9, 0x1d, 0xc4, 0x9f, 0xb9, 0x0b, 0xc4, 0xe0, 0xb9, 0x32,
	0xc5, 0x2c, 0xb9, 0x51, 0xc5, 0x05, 0xb9, 0x44, 0xc5, 0x55, 0xb9, 0x5e,
	0xc5, 0xb4, 0xb9, 0x6c, 0xc5, 0x92, 0xb9, 0x52, 0xc5, 0xc9, 0xb9, 0x7c,
	0xc5, 0xe7, 0xb9, 0xb8, 0xc5, 0xd9, 0xb9, 0xa2, 0xc5, 0xfe, 0xb9, 0xdc,
	0xc6, 0x27, 0xba, 0x26, 0xc6, 0x14, 0xba, 0x00, 0xc6, 0x45, 0xba, 0x61,
	0xc6, 0x94, 0xba, 0x9f, 0xc6, 0x4d, 0xba, 0x89, 0xc6, 0x67, 0xba, 0xe8,
	0xc6, 0xb6, 0xbb, 0x83, 0xc6, 0x93, 0xbb, 0x3f, 0xc6, 0xdf, 0xbb, 0xd4,
	0xc6, 0xe3, 0xbc, 0x7c, 0xc6, 0xe4, 0xbc, 0x23, 0xc6, 0xe2, 0xbc, 0xce,
	0xc6, 0xd0, 0xbd, 0x79, 0xc6, 0xf5, 0xbd, 0x2c, 0xc6, 0xa7, 0xbd, 0xcc,
	0xc6, 0x9f, 0xbe, 0x9a, 0xc6, 0x9f, 0xbe, 0x3e, 0xc6, 0xa1, 0xbe, 0x95,
	0xc6, 0xa5, 0xbe, 0x88, 0xc6, 0xa3, 0xbe, 0x8d, 0xc6, 0xa2, 0xbe, 0x8f,
	0xc6, 0xa1, 0xbe, 0xc0, 0xc6, 0x96, 0xbe, 0xbe, 0xc6, 0xb2, 0xbe, 0xc4,
	0xc6, 0xd6, 0xbe, 0x02, 0xc6, 0xd2, 0xbe, 0x15, 0xc6, 0xf6, 0xbd, 0x65,
	0xc6, 0xf7, 0xbc, 0x24, 0xc7, 0x00, 0xbc, 0xc4, 0xc6, 0xf7, 0xbc, 0x24,
	0x00, 0x08, 0xc3, 0x01, 0xb7, 0xd8, 0xc3, 0x01, 0xb7, 0xd8, 0xc2, 0xe6,
	0xb7, 0xdc, 0xc2, 0xd4, 0xb7, 0xa4, 0xc2, 0xe8, 0xb7, 0xae, 0xc2, 0xb1,
	0xb7, 0x92, 0xc2, 0x69, 0xb7, 0xb8, 0xc2, 0x88, 0xb7, 0xa7, 0xc2, 0x04,
	0xb7, 0xef, 0xc2, 0x36, 0xb7, 0x43, 0xc2, 0x21, 0xb7, 0x7b, 0xc2, 0x39,
	0xb7, 0x3a, 0xc2, 0x85, 0xb7, 0x44, 0xc2, 0x7a, 0xb7, 0x46, 0xc2, 0xa7,
	0xb7, 0x40, 0xc2, 0xf3, 0xb7, 0x4f, 0xc2, 0xd8, 0xb7, 0x31, 0xc3, 0x0c,
	0xb7, 0x6b, 0xc3, 0x3c, 0xb7, 0xa1, 0xc3, 0x1f, 0xb7, 0x87, 0xc3, 0x60,
	0xb7, 0xc2, 0xc3, 0x01, 0xb7, 0xd8, 0xc3, 0x1c, 0xb7, 0xd4, 0xc3, 0x01,
	0xb7, 0xd8, 0x00, 0x04, 0xbb, 0x53, 0xc2, 0x03, 0xbb, 0x53, 0xc2, 0x03,
	0xbb, 0x45, 0xc2, 0x0d, 0xbb, 0x26, 0xc2, 0x21, 0xbb, 0x38, 0xc2, 0x1d,
	0xbb, 0x55, 0xc2, 0x49, 0xbb, 0xba, 0xc2, 0x91, 0xbb, 0x87, 0xc2, 0x6e,
	0xbb, 0x92, 0xc2, 0x65, 0xbb, 0x53, 0xc2, 0x03, 0xbb, 0x84, 0xc2, 0x27,
	0xbb, 0x53, 0xc2, 0x03, 0x00, 0x05, 0xc4, 0xf4, 0xbe, 0x2e, 0xc4, 0xf4,
	0xbe, 0x2e, 0xc4, 0xd3, 0xbe, 0x57, 0xc4, 0x8c, 0xbe, 0xa4, 0xc4, 0xa6,
	0xbe, 0x76, 0xc4, 0x6e, 0xbe, 0xda, 0xc4, 0x7d, 0xbf, 0x53, 0xc4, 0x7d,
	0xbf, 0x18, 0xc4, 0xb6, 0xbf, 0x38, 0xc4, 0xed, 0xbe, 0xb9, 0xc4, 0xd0,
	0xbe, 0xec, 0xc5, 0x02, 0xbe, 0x93, 0xc4, 0xf4, 0xbe, 0x2e, 0xc5, 0x31,
	0xbe, 0x48, 0xc4, 0xf4, 0xbe, 0x2e, 0x00, 0x09, 0xc2, 0x13, 0xc3, 0x27,
	0xc2, 0x13, 0xc3, 0x27, 0xc1, 0x7f, 0xc3, 0x90, 0xbf, 0xf7, 0xc3, 0x45,
	0xc0, 0x9d, 0xc3, 0x32, 0xbf, 0xd0, 0xc3, 0x49, 0xbf, 0x8e, 0xc3, 0x6a,
	0xbf, 0xac, 0xc3, 0x50, 0xbf, 0x67, 0xc3, 0x8c, 0xbf, 0x20, 0xc3, 0x86,
	0xbf, 0x56, 0xc3, 0x86, 0xbe, 0xaf, 0xc3, 0x86, 0xbd, 0xcd, 0xc3, 0x86,
	0xbe, 0x3e, 0xc3, 0x86, 0xbe, 0xc1, 0xc3, 0xcb, 0xc0, 0xc0, 0xc3, 0xba,
	0xbf, 0xc4, 0xc3, 0xdd, 0xc1, 0x37, 0xc3, 0xaa, 0xc2, 0x21, 0xc3, 0x67,
	0xc1, 0xaf, 0xc3, 0x8e, 0xc2, 0x2a, 0xc3, 0x64, 0xc2, 0x62, 0xc3, 0x2b,
	0xc2, 0x80, 0xc3, 0x37, 0xc2, 0x49, 0xc3, 0x22, 0xc2, 0x13, 0xc3, 0x27,
	0xc2, 0x2a, 0xc3, 0x31, 0xc2, 0x13, 0xc3, 0x27, 0x02, 0x04, 0x48, 0x44,
	0xc8, 0x37, 0x44, 0xbd, 0xa8, 0x44, 0x30, 0x4f, 0x30, 0xc3, 0x4e, 0x30,
	0xc8, 0x25, 0x48, 0x5a, 0xbd, 0xa8, 0x5a, 0xc8, 0x37, 0x5a, 0x60, 0x4f,
	0x60, 0xc8, 0x25, 0x60, 0xc3, 0x4e, 0x0a, 0x0a, 0x05, 0x01, 0x0f, 0x00,
	0x0a, 0x04, 0x01, 0x03, 0x10, 0x01, 0x17, 0x84, 0x00, 0x04, 0x0a, 0x06,
	0x01, 0x03, 0x00, 0x0a, 0x07, 0x01, 0x04, 0x02, 0x40, 0x65, 0xc8, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x65, 0xc8, 0xc6, 0x97, 0x79, 0xbf,
	0x1c, 0xdb, 0x0a, 0x08, 0x0a, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0d,
	0x0e, 0x05, 0x0c, 0x02, 0x40, 0x65, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x40, 0x65, 0xc8, 0xc6, 0x97, 0x79, 0xbf, 0x1c, 0xdb, 0x0a, 0x03,
	0x01, 0x01, 0x02, 0x3d, 0xff, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x3d, 0xff, 0x88, 0x48, 0xa0, 0x71, 0x40, 0x02, 0xb1, 0x0a, 0x00, 0x01,
	0x02, 0x12, 0x41, 0xff, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41,
	0xff, 0x88, 0xc1, 0xf3, 0x87, 0xcb, 0xdf, 0x74, 0x01, 0x17, 0x82, 0x00,
	0x04, 0x0a, 0x02, 0x01, 0x02, 0x02, 0x41, 0xff, 0x88, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x41, 0xff, 0x88, 0xc1, 0xf3, 0x87, 0xcb, 0xdf, 0x74,
	0x0a, 0x00, 0x01, 0x01, 0x12, 0x41, 0xff, 0x88, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x41, 0xff, 0x88, 0xc1, 0xf3, 0x87, 0xcb, 0xdf, 0x74, 0x01,
	0x17, 0x82, 0x00, 0x04, 0x0a, 0x01, 0x01, 0x01, 0x02, 0x41, 0xff, 0x88,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xff, 0x88, 0xc1, 0xf3, 0x87,
	0xcb, 0xdf, 0x74
};

// #pragma mark - root folder attribute list
//0x6949434F           7  "BEOS:D:STD_ICON"                   49 44 58 5A 05 53 00
uint8 root_folder_attrs_1[] = { 0x49, 0x44, 0x58, 0x5A, 0x05, 0x53, 0x00 }; // needed for SVG icons in Zeta...
uint64 root_folder_attrs_2 = 0xdeadbeef50385038LL; // volume id, just in case

struct attr_entry root_folder_attrs[] = {
{ "BEOS:TYPE", /*B_MIME_STRING_TYPE*/'MIMS', SZSTR("application/x-vnd.Be-directory") },
{ "BEOS:ICON", 'VICN', sizeof(websearch_icon_HVIF), (char *)websearch_icon_HVIF },
//{ "BEOS:EMBLEMS", 'CSTR', SZSTR("palm") },
{ "be:volume_id", B_UINT64_TYPE, sizeof(root_folder_attrs_2), &root_folder_attrs_2 },
{ NULL, 0, 0, NULL } /* end of list */
};

static uint8 folders_attrs_1[] = {
 0x52, 0x56, 0xf2, 0x4f, 0x15, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x54, 0x69, 0x74, 0x6c,
 0x65, 0x00, 0x00, 0x00, 0x20, 0x42, 0x00, 0x00, 0x02, 0x43, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00,
 0x00, 0x00, 0x4d, 0x45, 0x54, 0x41, 0x3a, 0x74, 0x69, 0x74, 0x6c, 0x65, 0x00, 0x52, 0x7d, 0xfb,
 0x77, 0x52, 0x54, 0x53, 0x43, 0x00, 0x01, 0x52, 0x56, 0xf2, 0x4f, 0x15, 0x00, 0x00, 0x00, 0x03,
 0x00, 0x00, 0x00, 0x55, 0x52, 0x4c, 0x00, 0x00, 0x00, 0x39, 0x43, 0x00, 0x00, 0x2a, 0x43, 0x00,
 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x4d, 0x45, 0x54, 0x41, 0x3a, 0x75, 0x72, 0x6c, 0x00,
 0x52, 0x54, 0x5b, 0xe3, 0x52, 0x54, 0x53, 0x43, 0x00, 0x01, 0x52, 0x56, 0xf2, 0x4f, 0x15, 0x00,
 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x4b, 0x65, 0x79, 0x77, 0x6f, 0x72, 0x64, 0x73, 0x00, 0x00,
 0x00, 0xb9, 0x43, 0x00, 0x00, 0x02, 0x43, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x4d,
 0x45, 0x54, 0x41, 0x3a, 0x6b, 0x65, 0x79, 0x77, 0x00, 0x52, 0xdc, 0xf3, 0xdb, 0x52, 0x54, 0x53,
 0x43, 0x00, 0x01, 0x52, 0x56, 0xf2, 0x4f, 0x15, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x4d,
 0x6f, 0x64, 0x69, 0x66, 0x69, 0x65, 0x64, 0x00, 0x00, 0xc0, 0x00, 0x44, 0x00, 0x00, 0x16, 0x43,
 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x5f, 0x73, 0x74, 0x61, 0x74, 0x2f, 0x6d, 0x6f,
 0x64, 0x69, 0x66, 0x69, 0x65, 0x64, 0x00, 0x45, 0x6d, 0x4b, 0x5d, 0x45, 0x4d, 0x49, 0x54, 0x01,
 0x00, 0x52, 0x56, 0xf2, 0x4f, 0x15, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x47, 0x6f, 0x6f,
 0x67, 0x6c, 0x65, 0x20, 0x6f, 0x72, 0x64, 0x65, 0x72, 0x00, 0x00, 0x00, 0x2a, 0x44, 0x00, 0x00,
 0xa0, 0x41, 0x01, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x47, 0x4f, 0x4f, 0x47, 0x4c, 0x45,
 0x3a, 0x6f, 0x72, 0x64, 0x65, 0x72, 0x00, 0x47, 0xde, 0xef, 0xfc, 0x47, 0x4e, 0x4f, 0x4c, 0x00,
 0x01 };
static uint8 folders_attrs_2[] = {
 0x52, 0xf5, 0x5e, 0x6f, 0x0a, 0x00, 0x00, 0x00, 0x74, 0x73, 0x6c, 0x54, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x47, 0xde, 0xef, 0xfc, 0x47, 0x4e, 0x4f, 0x4c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x01 };
struct attr_entry folders_attrs[] = {
{ "BEOS:TYPE", /*B_MIME_STRING_TYPE*/'MIMS', SZSTR("application/x-vnd.Be-directory") },
{ "_trk/columns_le", 'RAWT', SZTAB(folders_attrs_1) },
{ "_trk/viewstate_le", 'RAWT', SZTAB(folders_attrs_2) },
{ "", 'RAWT', SZTAB(folders_attrs_2) },
{ NULL, 0, 0, NULL } /* end of list */
};

struct attr_entry bookmark_attrs[] = {
{ "BEOS:TYPE", /*B_MIME_STRING_TYPE*/'MIMS', SZSTR("application/x-vnd.Be-bookmark") },
{ NULL, 0, 0, NULL } /* end of list */
};

/* for debugging */
//static int32 fake_bm_attr_1 = 1;
struct attr_entry fake_bookmark_attrs[] = {
{ "BEOS:TYPE", /*B_MIME_STRING_TYPE*/'MIMS', SZSTR("application/x-vnd.Be-bookmark") },
{ "META:title", 'CSTR', SZSTR("Plop!") },
{ "META:url", 'CSTR', SZSTR("http://127.0.0.1/") },
{ "META:keyw", 'CSTR', SZSTR("plop") },
{ NULL, 0, 0, NULL } /* end of list */
};

static uint8 template_1_attrs_1 = 1;
static int32 template_1_attrs_2 = 1;
static uint8 template_1_attrs_3[] = {
 0x31, 0x42, 0x4F, 0x46, 0x01, 0x00, 0x00, 0x98, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x01, 0x0F, 0x47, 0x4E, 0x4F, 0x4C, 0x04, 0x0C, 0x63, 0x72, 0x65, 0x61, 0x74, 0x69, 0x6F, 0x6E,
 0x44, 0x61, 0x74, 0x65, 0x51, 0xEA, 0xC7, 0x41, 0x0F, 0x47, 0x4E, 0x4C, 0x4C, 0x08, 0x08, 0x63,
 0x61, 0x70, 0x61, 0x63, 0x69, 0x74, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B,
 0x52, 0x54, 0x53, 0x43, 0x08, 0x0A, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x4E, 0x61, 0x6D, 0x65,
 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x41, 0x0B, 0x52, 0x54, 0x53, 0x43, 0x10, 0x0A, 0x76,
 0x6F, 0x6C, 0x75, 0x6D, 0x65, 0x4E, 0x61, 0x6D, 0x65, 0x07, 0x00, 0x00, 0x00, 0x47, 0x6F, 0x6F,
 0x67, 0x6C, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x52, 0x54, 0x53, 0x43, 0x10, 0x07,
 0x66, 0x73, 0x68, 0x4E, 0x61, 0x6D, 0x65, 0x09, 0x00, 0x00, 0x00, 0x67, 0x6F, 0x6F, 0x67, 0x6C,
 0x65, 0x66, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00 };
static int32 template_1_attrs_4 = 0x4662796E;
static uint8 template_1_attrs_5[] = {
 0x00, 0x00, 0x06, 0xEC, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00 };
static int32 template_1_attrs_6 = 0x00000027;
static int32 template_1_attrs_7 = 0x00000000;
static uint8 template_1_attrs_8[] = {
 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x82, 0x43, 0x00, 0x00, 0xA0, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0xE0, 0x9F, 0x44, 0x00, 0xC0, 0x7F, 0x44, 0xFF, 0xFF, 0xFF, 0xFF };
static uint8 template_1_attrs_9[] = {
 0x00, 0x00, 0x00, 0x00, 0x58, 0x10, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x44,
 0x00, 0x00, 0x0c, 0x44 };
static uint8 template_1_attrs_11[] = {
 0x52, 0xf5, 0x5e, 0x6f, 0x0a, 0x00, 0x00, 0x00, 0x74, 0x73, 0x6c, 0x54, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x47, 0xde, 0xef, 0xfc, 0x47, 0x4e, 0x4f, 0x4c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x01 };
struct attr_entry template_1_attrs[] = {
{ "BEOS:TYPE", /*B_MIME_STRING_TYPE*/'MIMS', SZSTR("application/x-vnd.Be-queryTemplate") },
{ "_trk/qrystr", 'CSTR', SZSTR("((name==\"*[aA][nN][yY][tT][hH][iI][nN][gG] [yY][oO][uU]\\'[dD] [lL][iI][kK][eE] [tT][oO] [sS][eE][aA][rR][cC][hH] ?*\")&&(BEOS:TYPE==\"application/x-vnd.Be-bookmark\"))") },
{ "_trk/queryDynamicDate", 'BOOL', 1, &template_1_attrs_1 },
{ "_trk/recentQuery", 'LONG', sizeof(int32), &template_1_attrs_2 },
{ "_trk/qryvol1", 'MSGG', SZTAB(template_1_attrs_3) },
{ "_trk/qryinitmime", 'CSTR', SZSTR("Bookmark") },
{ "_trk/qryinitmode", 'LONG', sizeof(int32), &template_1_attrs_4 },
{ "_trk/qrymoreoptions_le", 'RAWT', SZTAB(template_1_attrs_5) },
{ "_trk/qryinitstr", 'CSTR', SZSTR("Anything you'd like to search ?") },
{ "_trk/focusedView", 'CSTR', SZSTR("TextControl") },
{ "_trk/focusedSelEnd", 'LONG', sizeof(int32), &template_1_attrs_6 },
{ "_trk/focusedSelStart", 'LONG', sizeof(int32), &template_1_attrs_7 },
{ "_trk/xtpinfo_le", 'RAWT', SZTAB(template_1_attrs_8) },
{ "_trk/pinfo_le", 'RAWT', SZTAB(template_1_attrs_9) },
{ "_trk/columns_le", 'RAWT', SZTAB(folders_attrs_1)/*SZTAB(template_1_attrs_10)*/ },
{ "_trk/viewstate_le", 'RAWT', SZTAB(template_1_attrs_11) },
{ NULL, 0, 0, NULL } /* end of list */
};

static int32 text_attrs_1 = 0x0000FFFF;
static int32 text_attrs_2 = 0x00000000;
static uint8 text_attrs_3 = 1;
static uint8 text_attrs_4[] = {
 0x41, 0x6c, 0x69, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00,
 0x53, 0x77, 0x69, 0x73, 0x37, 0x32, 0x31, 0x20, 0x42, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x52, 0x6f, 0x6d, 0x61, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x41, 0x60, 0x00, 0x00, 0x42, 0xb4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x28, 0x53, 0x77, 0x69, 0x73, 0x37, 0x32, 0x31, 0x20, 0x42, 0x54, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x52, 0x6f, 0x6d, 0x61, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x41, 0x60, 0x00, 0x00, 0x42, 0xb4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x29, 0x53, 0x77, 0x69, 0x73, 0x37, 0x32, 0x31, 0x20,
 0x42, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x52, 0x6f, 0x6d, 0x61, 0x6e, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x60, 0x00, 0x00, 0x42, 0xb4, 0x00, 0x00,
 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x53, 0x77, 0x69, 0x73,
 0x37, 0x32, 0x31, 0x20, 0x42, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x52, 0x6f, 0x6d, 0x61,
 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x60, 0x00, 0x00,
 0x42, 0xb4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47,
 0x53, 0x77, 0x69, 0x73, 0x37, 0x32, 0x31, 0x20, 0x42, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x52, 0x6f, 0x6d, 0x61, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x41, 0x20, 0x00, 0x00, 0x42, 0xb4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00 };
struct attr_entry text_attrs[] = {
{ "BEOS:TYPE", /*B_MIME_STRING_TYPE*/'MIMS', SZSTR("text/plain") },
{ "be:encoding", 'LONG', sizeof(int32), &text_attrs_1 },
{ "alignment", 'LONG', sizeof(int32), &text_attrs_2 },
{ "wrap", 'BOOL', sizeof(uint8), &text_attrs_3 },
{ "styles", 'RAWT', SZTAB(text_attrs_4) },
{ NULL, 0, 0, NULL } /* end of list */
};

char *readmestr = \
"Welcome to the Web Search FileSystem for BeOS™, Zeta™ and Haiku®.\n"
"Copyright© 2004-2008, François Revol.\n"
"DuckDuckGo is a trademark of DuckDuckGo.\n"
"BeOS is a trademark of ACCESS.\n"
"Zeta is a trademark of yellowTAB GmbH.\n"
"Haiku is a trademark of Haiku, Inc.\n"
"\n"
"Use \"Search the Web\" query template in this folder to search anything.\n"
"\n";

struct attr_entry mailto_me_bookmark_attrs[] = {
{ "BEOS:TYPE", /*B_MIME_STRING_TYPE*/'MIMS', SZSTR("application/x-person") },
{ "META:email", 'CSTR', SZSTR("revol@free.fr") },
{ "META:name", 'CSTR', SZSTR("François Revol") },
{ "META:country", 'CSTR', SZSTR("France") },
{ "META:nickname", 'CSTR', SZSTR("mmu_man") },
//{ "META:company", 'CSTR', SZSTR("yellowTAB GmbH") },
{ "META:url", 'CSTR', SZSTR("http://revolf.free.fr/") },
{ "META:group", 'CSTR', SZSTR("") },
{ "IM:connections", 'CSTR', SZSTR("icq:77792625;gtalk:revolf@gmail.com;yahoo:mmu_man;msn:revol@mail.com") },
{ NULL, 0, 0, NULL } /* end of list */
};
