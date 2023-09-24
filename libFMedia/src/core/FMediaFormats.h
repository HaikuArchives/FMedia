#ifndef F_MEDIA_FORMATS_H
#define F_MEDIA_FORMATS_H

#include "FMediaDefs.h"
#include <string.h>

// ------------------------------
// new

struct FFFTFormat {
	int32 BufferSize;
	int32 Start;
	
	enum fft_format {
		wildcard = 0,
		complex = 1,
		unified = 2
	} Format;
	float SamplingRate;
	float AnalysisOverlap;
	
	// --- fucntions 
	double Frequency( const float Index ) const {
	    if ( Index >= BufferSize )
	        return 0.0;
	    else if ( Index <= BufferSize/2 )
	        return ((double)Index / (double)BufferSize)*SamplingRate;
	
	    return (-(double)(BufferSize-Index) / (double)BufferSize)*SamplingRate;
	}
/*	
	void Print() const {
		printf("FFT: size %i, %s format, %.1f - %.1f Hz, %i%% Overlap\n",
			BufferSize, Format==unified?"unified":"complex",
			(float)Frequency(1), (float)Frequency((BufferSize/2)-1), AnalysisOverlap*100 );
	}
*/
};

#define F_PARAMETER_NAME_LENGTH	0xff
struct FControlDataFormat {
	FControlDataFormat( const char *name, float from=0, float to=1 ) {
			strncpy( Name, name, F_PARAMETER_NAME_LENGTH );
			From = from;
			To = to;
		}
	char Name[ F_PARAMETER_NAME_LENGTH ];
	float From, To;
};


// f_media_format utilizes the MetaData mechanism of media_format
// to allow for Text and FFT data.
struct f_media_format : public media_format {	/* no more than 192 bytes */

	public:
		bool IsText() const {
				return type == B_MEDIA_TEXT;
			}
		
		bool IsFFT() const {
				return type == F_MEDIA_FFT_DATA;
			}

		bool IsControlData() const {
				return type == F_MEDIA_CONTROL_DATA;
			}
		
	/*
	bool			Matches(const media_format *otherFormat) const;
	void			SpecializeTo(const media_format *otherFormat);

	status_t		SetMetaData(const void *data, size_t size);
	const void *	MetaData() const;
	int32			MetaDataSize() const;

					media_format();
					media_format(const media_format &other);
					~media_format();
	media_format &	operator=(const media_format & clone);
	*/
};


// ------------------------------------------------------
// functions

media_format FGetWildcardFormat();

media_format FGetStandardVideoFormat();
media_format FGetRGB32VideoFormat();
media_format FGet640x480VideoFormat();
media_format FGetGRAY8VideoFormat();
media_format FGetStandardGRAY8VideoFormat();
media_format FGetLoFiVideoFormat();
media_format FGetStandardVideoFormat();
media_format FGetWildcardVideoFormat();
media_format FGetVideoFormat( media_raw_video_format format );

media_format FGetStandardAudioFormat();
media_format FGetWildcardAudioFormat();
media_format FGetAudioFormat( media_raw_audio_format format );

media_format FGetControlDataFormat( const char *name = "Value", float from=0, float to=1 );

media_format FGetWildcardFFTFormat();
media_format FGetUnifiedFFTFormat();
media_format FGetFFTFormat( FFFTFormat *fftformat );


extern media_format FWildcardFormat;

extern media_raw_video_format FRGB32VideoFormat;
extern media_raw_video_format F320x240x25RGB32VideoFormat;
extern media_raw_video_format FLoFiRGB32VideoFormat;

extern media_raw_video_format FStandardGRAY8VideoFormat;
extern media_raw_video_format F320x240x25GRAY8VideoFormat;

extern media_raw_video_format F640x480VideoFormat;

extern media_raw_video_format FWildcardVideoFormat;

extern media_raw_audio_format FWildcardAudioFormat;
extern media_raw_audio_format FStandardAudioFormat;
extern media_raw_audio_format FFloatAudioFormat;
extern media_raw_audio_format F4416monoAudioFormat;

//extern media_multi_audio_format FStandardMultiAudioFormat;

status_t FAcceptAndSpecializeMediaFormat( media_format* format, const media_format* preferred, const media_format* required );
size_t FMediaIdealBufferSizeForFormat( const media_format* format );
int32 FBytesPerPixel( const color_space& s );

#include <Message.h>

void FMediaFormatToMessage( media_format *format, BMessage *msg );
void FMediaMessageToFormat( BMessage *msg, media_format *format );
void FMediaRawVideoFormatToMessage( media_raw_video_format *format, BMessage *msg );
void FMediaMessageToRawVideoFormat( BMessage *msg, media_raw_video_format *format );

const char *FStringForFormat( media_format *format );

//media_raw_audio_format& media_raw_audio_format::operator =( media_raw_audio_format& raw );
bool MultiizeAudioFormat( media_multi_audio_format *a, const media_raw_audio_format & b);

#define F_ANALYSIS_PRIORITY 50




#endif
