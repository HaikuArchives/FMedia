#include "FColorConverter.h"

#include <stdio.h>

#define ERROR printf

FColorConverterBase *FColorConverterBase::GetConverter( color_space to, color_space from ) {
	switch( from ) {
		case B_RGBA32:
		case B_RGB32: {
				switch( to ) {
					case B_RGBA32:
						return( new FRGBA32toRGBA32Converter );
					case B_RGB32:
						return( new FRGBA32toRGBA32Converter );
					case B_RGB16:
						return( new FRGBA32toRGB16Converter );
					case B_CMAP8:
						return( new FRGBA32toCMAP8Converter );
				}
			} break;
		case B_CMAP8: {
				switch( to ) {
					case B_CMAP8:
						return( new FCMAP8toCMAP8Converter );
				}
			} break;
	}

	ERROR("Could not convert color spaces [%i->%i]\n", from, to );
	return( NULL );
}

