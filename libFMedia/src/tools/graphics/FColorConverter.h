#ifndef F_FColorConverter_H
#define F_FColorConverter_H

/*
	010726 dan		created
					rgb(a)32->rgb32
					rgb(a)32->rgb16
					rgb(a)32->cmap8 (uses BScreen's colormap)
					cmap8->cmap8
*/

#include <InterfaceDefs.h>
#include <Screen.h>
#include <string.h>

class FColorConverterBase {
	public:
		virtual ~FColorConverterBase() { ; }

		virtual void Convert( void *, void * ) = 0;
		static FColorConverterBase *GetConverter( color_space to, color_space from );
};

template <class T, class F>
class FColorConverter : public virtual FColorConverterBase {
	public:
		virtual ~FColorConverter() { ; }

		virtual inline void Convert( T *to, F *from ) {
			// dumb(!) default copy.
			memcpy( to, from, MIN(sizeof(T),sizeof(F)) );
		}
		
/*		static FColorConverter<T,F> *GetConverter( color_space to, color_space from ) {
			FColorConverterBase *base = FColorConverterBase::GetConverter( from, to );
			FColorConverter<T,F> *specific = dynamic_cast<FColorConverter<T,F>* >( base );

			if( specific ) return( specific );
			if( base ) delete base;
			return( NULL );
		}
*/
};


class FRGBA32toRGBA32Converter : public virtual FColorConverter<rgb_color,rgb_color> {
	public:
		virtual ~FRGBA32toRGBA32Converter() { ; }

		virtual inline void Convert( void *to, void *from ) {
			// i think these casts are save...
			Convert( (rgb_color*)to, (rgb_color*)from );
		}

		inline void Convert( rgb_color *t, rgb_color *f ) {
			*t = *f;
		}
};

class FRGBA32toRGB16Converter : public virtual FColorConverter<uint16,rgb_color> {
	public:
		virtual ~FRGBA32toRGB16Converter() { ; }

		virtual inline void Convert( void *to, void *from ) {
			// i think these casts are save...
			Convert( (uint16*)to, (rgb_color*)from );
		}

		inline void Convert( uint16 *rgb16, rgb_color *f ) {
			// B_RGB16 has 5 bits for red and blue, but 6 for green
			*rgb16 = f->red >> 3;
			*rgb16 <<= 6;
		
			*rgb16 |= f->green >> 2;
			*rgb16 <<= 5;
		
			*rgb16 |= f->blue >> 3;
		}
};

class FRGBA32toCMAP8Converter : public virtual FColorConverter<uint8,rgb_color> {
	public:
		virtual ~FRGBA32toCMAP8Converter() { ; }

		virtual inline void Convert( void *to, void *from ) {
			// i think these casts are save...
			Convert( (uint8*)to, (rgb_color*)from );
		}

		inline void Convert( uint8 *cmap8, rgb_color *f ) {
			*cmap8 = mScreen.IndexForColor( *f );
		}
		
	protected:
		BScreen mScreen;
};


class FCMAP8toCMAP8Converter : public virtual FColorConverter<uint8,uint8> {
	public:
		virtual ~FCMAP8toCMAP8Converter() { ; }
		
		virtual inline void Convert( void *to, void *from ) {
			// i think these casts are save...
			Convert( (uint8*)to, (uint8*)from );
		}

		inline void Convert( uint8 *t, uint8 *f ) {
			*t = *f;
		}
};

#endif
