/*
 * Copyright 1999-2000 Y.Takagi
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef _PACK_BITS_H
#define _PACK_BITS_H


int	pack_bits_size(const unsigned char* source, int size);
int	pack_bits(unsigned char* destination, const unsigned char* source,
		int size);


#endif	/* _PACK_BITS_H */
