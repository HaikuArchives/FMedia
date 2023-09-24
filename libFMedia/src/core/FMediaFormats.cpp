
#include "FMediaFormats.h"
#include <stdio.h>
#include <memory.h>

#define FORMAT //printf
#define WARNING printf
#define ERROR printf
#define METADATA printf

media_raw_video_format FWildcardVideoFormat = { 0, 0, 0, 0, 0, 0, 0, { B_NO_COLOR_SPACE, 0, 0, 0, 0, 0 } };

media_raw_video_format FRGB32VideoFormat = { 0, 1, 0, 0, B_VIDEO_TOP_LEFT_RIGHT, 1, 1, { B_RGB32, 0, 0, 0, 0, 0 } };
media_raw_video_format F320x240x25RGB32VideoFormat = { 25, 1, 0, 239, B_VIDEO_TOP_LEFT_RIGHT, 1, 1, { B_RGB32, 320, 240, 320*4, 0, 0 } };
media_raw_video_format FLoFiRGB32VideoFormat = { 25, 1, 0, 79, B_VIDEO_TOP_LEFT_RIGHT, 1, 1, { B_RGB32, 80, 60, 80*4, 0, 0 } };

media_raw_video_format FGRAY8VideoFormat = { 0, 0, 0, 0, B_VIDEO_TOP_LEFT_RIGHT, 1, 1, { B_GRAY8, 0, 0, 0, 0, 0 } };
media_raw_video_format F320x240x25GRAY8VideoFormat = { 25, 1, 0, 239, B_VIDEO_TOP_LEFT_RIGHT, 1, 1, { B_GRAY8, 320, 240, 320, 0, 0 } };

media_raw_video_format F640x480VideoFormat = { 25, 1, 0, 479, B_VIDEO_TOP_LEFT_RIGHT, 1, 1, { B_NO_COLOR_SPACE, 640, 480, 640, 0, 0 } };

media_raw_audio_format FWildcardAudioFormat = { 0, 0, 0, 0, 0 };
media_raw_audio_format FStandardAudioFormat = { 44100, 2, media_raw_audio_format::B_AUDIO_SHORT, B_MEDIA_LITTLE_ENDIAN, 1024 };
media_raw_audio_format FFloatAudioFormat = { 44100, 2, media_raw_audio_format::B_AUDIO_FLOAT, B_MEDIA_LITTLE_ENDIAN, 1024 };
media_raw_audio_format F4416monoAudioFormat = { 44100, 1, media_raw_audio_format::B_AUDIO_SHORT, B_MEDIA_LITTLE_ENDIAN, 1024 };


media_format FGetWildcardFormat() {
	media_format ret;
	ret.type = B_MEDIA_ANY_TYPE;
	return ret;
}

media_format FGetVideoFormat( media_raw_video_format inFormat ) {
	media_format format;
	format.type = B_MEDIA_RAW_VIDEO;
	format.u.raw_video = inFormat;
	return( format );
}

media_format FGetWildcardVideoFormat() {
	return FGetVideoFormat( FWildcardVideoFormat );
}

media_format FGetRGB32VideoFormat() {
	return FGetVideoFormat( FRGB32VideoFormat );
}

media_format FGetStandardVideoFormat() {
	return FGetVideoFormat( F320x240x25RGB32VideoFormat );
}

media_format FGetGRAY8VideoFormat() {
	return FGetVideoFormat( FGRAY8VideoFormat );
}

media_format FGetStandardGRAY8VideoFormat() {
	return FGetVideoFormat( F320x240x25GRAY8VideoFormat );
}

media_format FGet640x480VideoFormat() {
	return FGetVideoFormat( F640x480VideoFormat );
}

media_format FGetLoFiVideoFormat() {
	return FGetVideoFormat( FLoFiRGB32VideoFormat );
}

media_format FGetAudioFormat( media_raw_audio_format inFormat ) {
	media_format format;
	format.type = B_MEDIA_RAW_AUDIO;
	MultiizeAudioFormat( &format.u.raw_audio, inFormat );
	return( format );
}

media_format FGetWildcardAudioFormat() {
	return FGetAudioFormat( FWildcardAudioFormat );
}

media_format FGetStandardAudioFormat() {
	return FGetAudioFormat( FStandardAudioFormat );
}

media_format FGetControlDataFormat( const char *name, float from, float to ) {
	FControlDataFormat cdFormat( name, from, to );

	media_format format;
	memset( &format, 0, sizeof( format ) );
	format.type = (media_type)F_MEDIA_CONTROL_DATA;
	format.SetMetaData( &cdFormat, sizeof( FControlDataFormat ) );
	return( format );
}

media_format FGetWildcardFFTFormat() {
	FFFTFormat fftformat;
	memset( &fftformat, 0, sizeof( FFFTFormat ) );
	
	return( FGetFFTFormat( &fftformat ) );
}

media_format FGetUnifiedFFTFormat() {
	FFFTFormat fftformat;
	memset( &fftformat, 0, sizeof( FFFTFormat ) );
	fftformat.Format = FFFTFormat::unified;
	
	return( FGetFFTFormat( &fftformat ) );
}

media_format FGetFFTFormat( FFFTFormat *fftformat ) {
	media_format format;
	memset( &format, 0, sizeof( format ) );
	format.type = (media_type)F_MEDIA_FFT_DATA;

	status_t err = format.SetMetaData( (void*)fftformat, sizeof( FFFTFormat ) );	
	if( err != B_OK ) {
		WARNING("Couldn't set FFT Format MetaData. (%s)\n", strerror( err ));
	} else {
		METADATA("Set FFT Format MetaData, Size %i, %p/%p\n", format.MetaDataSize(), format.MetaData(), fftformat );
	}
	
	return( format );
}



//media_multi_audio_format FStandardMultiAudioFormat;
//{ 44100, 2, media_raw_audio_format::B_AUDIO_SHORT, B_MEDIA_LITTLE_ENDIAN, 1024, 0, 0, 0 };

status_t FAcceptAndSpecializeMediaFormat( media_format* format, const media_format* preferred, const media_format* required ) {
	char format_string[256];
/*	string_for_format(*format, format_string, 256);
	FORMAT("FAcceptAndSpecializeMediaFormat: Format    %s\n", format_string);
	string_for_format(*required, format_string, 256);
	FORMAT("FAcceptAndSpecializeMediaFormat: Required  %s\n", format_string);
	string_for_format(*preferred, format_string, 256);
	FORMAT("FAcceptAndSpecializeMediaFormat: Preferred %s\n", format_string);
*/
	if( required->type == B_MEDIA_NO_TYPE ) return( B_OK );
	
	if( format->type == B_MEDIA_UNKNOWN_TYPE ) format->type = preferred->type;
	if( format->type != required->type ) {
		if( required->type != B_MEDIA_UNKNOWN_TYPE && format->type != B_MEDIA_UNKNOWN_TYPE && 
			required->type != B_MEDIA_NO_TYPE && format->type != B_MEDIA_NO_TYPE ) {
			FORMAT("FAcceptAndSpecializeMediaFormat: Format has type %i, but %i is required (use %i or %i)\n", format->type, required->type, B_MEDIA_NO_TYPE, B_MEDIA_UNKNOWN_TYPE);
			return( B_MEDIA_BAD_FORMAT );
		}
	}
	if( format->type == B_MEDIA_RAW_VIDEO ) {
		if( format->u.raw_video.display.format == media_video_display_info::wildcard.format )
			format->u.raw_video.display.format = preferred->u.raw_video.display.format;
		if( required->u.raw_video.display.format != media_video_display_info::wildcard.format ) {
			if( format->u.raw_video.display.format != required->u.raw_video.display.format ) {
				if( format->u.raw_video.display.format == B_RGB32 && required->u.raw_video.display.format == B_RGBA32 ) {
				} else if( format->u.raw_video.display.format == B_RGBA32 && required->u.raw_video.display.format == B_RGB32 ) {
				} else {
					FORMAT("FAcceptAndSpecializeMediaFormat: Format has format %i, but %i is required\n", format->u.raw_video.display.format, required->u.raw_video.display.format);
					return( B_MEDIA_BAD_FORMAT );
				}
			}
		}

		if( format->u.raw_video.display.line_width == media_video_display_info::wildcard.line_width )
			format->u.raw_video.display.line_width = preferred->u.raw_video.display.line_width;
		if( required->u.raw_video.display.line_width != media_video_display_info::wildcard.line_width ) {
			if( format->u.raw_video.display.line_width != required->u.raw_video.display.line_width ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Format has line_width %i, but %i is required\n", format->u.raw_video.display.line_width, required->u.raw_video.display.line_width);
				return( B_MEDIA_BAD_FORMAT );
			}
		}
		
		if( format->u.raw_video.display.line_count == media_video_display_info::wildcard.line_count )
			format->u.raw_video.display.line_count = preferred->u.raw_video.display.line_count;
		if( required->u.raw_video.display.line_count != media_video_display_info::wildcard.line_count ) {
			if( format->u.raw_video.display.line_count != required->u.raw_video.display.line_count ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Format has line_count %i, but %i is required\n", format->u.raw_video.display.line_count, required->u.raw_video.display.line_count);
				return( B_MEDIA_BAD_FORMAT );
			}
		}

		if( format->u.raw_video.display.bytes_per_row == media_video_display_info::wildcard.bytes_per_row )
			format->u.raw_video.display.bytes_per_row = preferred->u.raw_video.display.bytes_per_row;
		if( required->u.raw_video.display.bytes_per_row != media_video_display_info::wildcard.bytes_per_row ) {
			if( format->u.raw_video.display.bytes_per_row != required->u.raw_video.display.bytes_per_row ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Format has bytes_per_row %i, but %i is required\n", format->u.raw_video.display.bytes_per_row, required->u.raw_video.display.bytes_per_row);
				return( B_MEDIA_BAD_FORMAT );
			}
		}
		if( ( format->u.raw_video.display.format == B_RGB32 || format->u.raw_video.display.format == B_RGBA32 ) 
			&& format->u.raw_video.display.bytes_per_row != format->u.raw_video.display.line_width*4 ) {
				WARNING("FAcceptAndSpecializeMediaFormat: Illegal value %i for bytes_per_row. Needs to be line_width*4 bytes (for RGB(A)), line_width is %i; adjusted.\n", format->u.raw_video.last_active, format->u.raw_video.display.line_width);
				format->u.raw_video.display.bytes_per_row = format->u.raw_video.display.line_width*4;
//				return( B_MEDIA_BAD_FORMAT );
		}
		if( ( format->u.raw_video.display.format == B_GRAY8 || format->u.raw_video.display.format == B_CMAP8 ) 
			&& format->u.raw_video.display.bytes_per_row != format->u.raw_video.display.line_width*4 ) {
				WARNING("FAcceptAndSpecializeMediaFormat: Illegal value %i for bytes_per_row. Needs to be line_width bytes (for CMAP8/GRAY8), line_width is %i; adjusted.\n", format->u.raw_video.last_active, format->u.raw_video.display.line_width);
				format->u.raw_video.display.bytes_per_row = format->u.raw_video.display.line_width*4;
//				return( B_MEDIA_BAD_FORMAT );
		}

		if( format->u.raw_video.display.pixel_offset == media_video_display_info::wildcard.pixel_offset )
			format->u.raw_video.display.pixel_offset = preferred->u.raw_video.display.pixel_offset;
		if( required->u.raw_video.display.pixel_offset != media_video_display_info::wildcard.pixel_offset ) {
			if( format->u.raw_video.display.pixel_offset != required->u.raw_video.display.pixel_offset ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Format has pixel_offset %i, but %i is required\n", format->u.raw_video.display.pixel_offset, required->u.raw_video.display.pixel_offset);
				return( B_MEDIA_BAD_FORMAT );
			}
		}

		if( format->u.raw_video.display.line_offset == media_video_display_info::wildcard.line_offset )
			format->u.raw_video.display.line_offset = preferred->u.raw_video.display.line_offset;
		if( required->u.raw_video.display.line_offset != media_video_display_info::wildcard.line_offset ) {
			if( format->u.raw_video.display.line_offset != required->u.raw_video.display.line_offset ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Format has line_offset %i, but %i is required\n", format->u.raw_video.display.line_offset, required->u.raw_video.display.line_offset);
				return( B_MEDIA_BAD_FORMAT );
			}
		}

		if( format->u.raw_video.field_rate == media_raw_video_format::wildcard.field_rate )
			format->u.raw_video.field_rate = preferred->u.raw_video.field_rate;
		if( required->u.raw_video.field_rate != media_raw_video_format::wildcard.field_rate ) {
			if( format->u.raw_video.field_rate != required->u.raw_video.field_rate ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Format has field_rate %i, but %i is required\n", format->u.raw_video.field_rate, required->u.raw_video.field_rate);
				return( B_MEDIA_BAD_FORMAT );
			}
		}

		if( format->u.raw_video.interlace == media_raw_video_format::wildcard.interlace )
			format->u.raw_video.interlace = preferred->u.raw_video.interlace;
		if( required->u.raw_video.interlace != media_raw_video_format::wildcard.interlace ) {
			if( format->u.raw_video.interlace != required->u.raw_video.interlace ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Format has interlace %i, but %i is required\n", format->u.raw_video.interlace, required->u.raw_video.interlace);
				return( B_MEDIA_BAD_FORMAT );
			}
		}

		if( format->u.raw_video.first_active == media_raw_video_format::wildcard.first_active )
			format->u.raw_video.first_active = preferred->u.raw_video.first_active;
		if( required->u.raw_video.first_active != media_raw_video_format::wildcard.first_active ) {
			if( format->u.raw_video.first_active != required->u.raw_video.first_active ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Format has first_active %i, but %i is required\n", format->u.raw_video.first_active, required->u.raw_video.first_active);
				return( B_MEDIA_BAD_FORMAT );
			}
		}

		if( format->u.raw_video.last_active == media_raw_video_format::wildcard.last_active )
			format->u.raw_video.last_active = preferred->u.raw_video.last_active;
		if( required->u.raw_video.last_active != media_raw_video_format::wildcard.last_active ) {
			if( format->u.raw_video.last_active != required->u.raw_video.last_active ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Format has last_active %i, but %i is required\n", format->u.raw_video.last_active, required->u.raw_video.last_active);
				return( B_MEDIA_BAD_FORMAT );
			}
		}
		if( format->u.raw_video.last_active != format->u.raw_video.display.line_count-1 ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Illegal value %i for last_active. Needs to be line_count-1, line_count is %i. (ignored!)\n", format->u.raw_video.last_active, format->u.raw_video.display.line_count);
				format->u.raw_video.last_active = format->u.raw_video.display.line_count-1;
//				return( B_MEDIA_BAD_FORMAT );
		}

		if( format->u.raw_video.orientation == media_raw_video_format::wildcard.orientation )
			format->u.raw_video.orientation = preferred->u.raw_video.orientation;
		if( required->u.raw_video.orientation != media_raw_video_format::wildcard.orientation ) {
			if( format->u.raw_video.orientation != required->u.raw_video.orientation ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Format has orientation %i, but %i is required\n", format->u.raw_video.orientation, required->u.raw_video.orientation);
				return( B_MEDIA_BAD_FORMAT );
			}
		}

		if( format->u.raw_video.pixel_width_aspect == media_raw_video_format::wildcard.pixel_width_aspect )
			format->u.raw_video.pixel_width_aspect = preferred->u.raw_video.pixel_width_aspect;
		if( required->u.raw_video.pixel_width_aspect != media_raw_video_format::wildcard.pixel_width_aspect ) {
			if( format->u.raw_video.pixel_width_aspect != required->u.raw_video.pixel_width_aspect ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Format has pixel_width_aspect %i, but %i is required\n", format->u.raw_video.pixel_width_aspect, required->u.raw_video.pixel_width_aspect);
				return( B_MEDIA_BAD_FORMAT );
			}
		}

		if( format->u.raw_video.pixel_height_aspect == media_raw_video_format::wildcard.pixel_height_aspect )
			format->u.raw_video.pixel_height_aspect = preferred->u.raw_video.pixel_height_aspect;
		if( required->u.raw_video.pixel_height_aspect != media_raw_video_format::wildcard.pixel_height_aspect ) {
			if( format->u.raw_video.pixel_height_aspect != required->u.raw_video.pixel_height_aspect ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Format has pixel_height_aspect %i, but %i is required\n", format->u.raw_video.pixel_height_aspect, required->u.raw_video.pixel_height_aspect);
				return( B_MEDIA_BAD_FORMAT );
			}
		}
		
		string_for_format(*format, format_string, 256);
		FORMAT("FAcceptAndSpecializeMediaFormat: Format OK: %s\n", format_string );
		return B_OK;
	} else if( format->type == B_MEDIA_RAW_AUDIO ) {
		if( format->u.raw_audio.frame_rate == media_raw_audio_format::wildcard.frame_rate )
			format->u.raw_audio.frame_rate = preferred->u.raw_audio.frame_rate;
		if( required->u.raw_audio.frame_rate != media_raw_audio_format::wildcard.frame_rate ) {
			if( format->u.raw_audio.frame_rate != required->u.raw_audio.frame_rate ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Format has wrong frame_rate\n");
				return( B_MEDIA_BAD_FORMAT );
			}
		}

		if( format->u.raw_audio.channel_count == media_raw_audio_format::wildcard.channel_count )
			format->u.raw_audio.channel_count = preferred->u.raw_audio.channel_count;
		if( required->u.raw_audio.channel_count != media_raw_audio_format::wildcard.channel_count ) {
			if( format->u.raw_audio.channel_count != required->u.raw_audio.channel_count ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Format has wrong frame_rate\n");
				return( B_MEDIA_BAD_FORMAT );
			}
		}

		if( format->u.raw_audio.byte_order == media_raw_audio_format::wildcard.byte_order )
			format->u.raw_audio.byte_order = preferred->u.raw_audio.byte_order;
		if( required->u.raw_audio.byte_order != media_raw_audio_format::wildcard.byte_order ) {
			if( format->u.raw_audio.byte_order != required->u.raw_audio.byte_order ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Format has wrong byte_order\n");
				return( B_MEDIA_BAD_FORMAT );
			}
		}

		if( format->u.raw_audio.format == media_raw_audio_format::wildcard.format )
			format->u.raw_audio.format = preferred->u.raw_audio.format;
		if( required->u.raw_audio.format != media_raw_audio_format::wildcard.format ) {
			if( format->u.raw_audio.format != required->u.raw_audio.format ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Format has wrong format\n");
				return( B_MEDIA_BAD_FORMAT );
			}
		}

		if( format->u.raw_audio.buffer_size == media_raw_audio_format::wildcard.buffer_size )
			format->u.raw_audio.buffer_size = preferred->u.raw_audio.buffer_size;
		if( required->u.raw_audio.buffer_size != media_raw_audio_format::wildcard.buffer_size ) {
			if( format->u.raw_audio.buffer_size != required->u.raw_audio.buffer_size ) {
				FORMAT("FAcceptAndSpecializeMediaFormat: Format has wrong buffer_size\n");
				return( B_MEDIA_BAD_FORMAT );
			}
		}
		
		string_for_format(*format, format_string, 256);
		FORMAT("FAcceptAndSpecializeMediaFormat: Format OK: %s\n", format_string );
		return B_OK;
	} else if( format->type == F_MEDIA_FFT_DATA ) {
		METADATA("FAcceptAndSpecializeMediaFormat: Matching FFT format...\n");
		if( required->MetaDataSize() != sizeof( FFFTFormat ) ) {
			METADATA("\tRequired Format has no MetaData, that means wildcard (OK)...");
			return B_OK;
		}
		if( format->MetaDataSize() != sizeof( FFFTFormat ) ) {
			METADATA("\tSuggested Format has no MetaData, using preferred (sz %i)...\n", preferred->MetaDataSize() );
			if( preferred->MetaDataSize() != sizeof( FFFTFormat ) ) {
				format->SetMetaData( preferred->MetaData(), preferred->MetaDataSize() );
				METADATA("\tPreferred has no MetaData either...\n");
			}
			return B_OK;
		}
		
		METADATA("\tComparing MetaData\n");
		FFFTFormat *fftformat = (FFFTFormat*)format->MetaData();
		FFFTFormat *fftrequired = (FFFTFormat*)required->MetaData();
		FFFTFormat *fftpreferred = (FFFTFormat*)preferred->MetaData();
		
		if( fftformat->BufferSize == 0 )
			fftformat->BufferSize = fftpreferred->BufferSize;
		if( fftrequired->BufferSize != 0 ) {
			if( fftformat->BufferSize != fftrequired->BufferSize ) {
				METADATA("FAcceptAndSpecializeMediaFormat: FFT Format has wrong BufferSize\n");
				return( B_MEDIA_BAD_FORMAT );
			}
		}

		if( fftformat->Format == FFFTFormat::wildcard )
			fftformat->Format = fftpreferred->Format;
		if( fftrequired->Format != FFFTFormat::wildcard ) {
			if( fftformat->Format != fftrequired->Format ) {
				METADATA("FAcceptAndSpecializeMediaFormat: FFT Format has wrong Format\n");
				return( B_MEDIA_BAD_FORMAT );
			}
		}

		if( fftformat->SamplingRate == 0 )
			fftformat->SamplingRate = fftpreferred->SamplingRate;
		if( fftrequired->SamplingRate != 0 ) {
			if( fftformat->SamplingRate != fftrequired->SamplingRate ) {
				METADATA("FAcceptAndSpecializeMediaFormat: FFT Format has wrong SamplingRate\n");
				return( B_MEDIA_BAD_FORMAT );
			}
		}
		
		if( fftformat->AnalysisOverlap == 0 )
			fftformat->AnalysisOverlap = fftpreferred->AnalysisOverlap;
		if( fftrequired->AnalysisOverlap != 0 ) {
			if( fftformat->AnalysisOverlap != fftrequired->AnalysisOverlap ) {
				METADATA("FAcceptAndSpecializeMediaFormat: FFT Format has wrong AnalysisOverlap\n");
				return( B_MEDIA_BAD_FORMAT );
			}
		}
		METADATA("\tMatch\n");
		return B_OK;
	} else {
		bool match = format->Matches( required );
		FORMAT("FAcceptAndSpecializeMediaFormat: Unknown media format, using generic Match function: %s\n", match?"OK":"not OK");
		return( B_OK ); //match?B_OK:B_MEDIA_BAD_FORMAT );
	}
	return B_ERROR;
}

size_t FMediaIdealBufferSizeForFormat( const media_format* format ) {
	if( format->type == B_MEDIA_RAW_VIDEO ) {
		return ( format->u.raw_video.display.bytes_per_row * format->u.raw_video.display.line_count ) / format->u.raw_video.interlace;
	} else if( format->type == B_MEDIA_RAW_AUDIO ) {
		if( format->u.raw_audio.buffer_size != media_raw_audio_format::wildcard.buffer_size ) {
			return( format->u.raw_audio.buffer_size );
		} else {
			return( 1024 );
		}
	} else if( format->type == F_MEDIA_CONTROL_DATA ) {
		return( sizeof( float ) );
	} else if( format->type == F_MEDIA_FFT_DATA ) {
	
			if( format->MetaDataSize() == sizeof( FFFTFormat ) ) {
			
				FFFTFormat *fftformat = (FFFTFormat *)format->MetaData();
				int n = fftformat->Format == FFFTFormat::unified ? 1 : 2;
				return( fftformat->BufferSize*n*sizeof(float) );
			} else {
				WARNING("FMediaIdealBufferSizeForFormat: F_MEDIA_FFT_DATA type format but\n  MetaDataSize is %i (FFFTFormat has size %i)\n",
						format->MetaDataSize(), sizeof( FFFTFormat ) );
			}
	} else {
		FORMAT("FAcceptAndSpecializeMediaFormat: Unknown media format, unknown size\n");
	}
	return F_DEFAULT_BUFFER_SIZE;
}


//---------------- Format2Message and vice versa
void FMediaFormatToMessage( media_format *format, BMessage *msg ) {
	msg->AddInt32( "type", (int32)(format->type) );
	if( format->type == B_MEDIA_RAW_AUDIO ) {
		msg->AddFloat("frame_rate", format->u.raw_audio.frame_rate);
		msg->AddInt32("channel_count", format->u.raw_audio.channel_count);
		msg->AddInt32("format", format->u.raw_audio.format);
		msg->AddInt32("byte_order", format->u.raw_audio.byte_order);
		msg->AddInt64("buffer_size", (int64)format->u.raw_audio.buffer_size);
	} else if( format->type == B_MEDIA_RAW_VIDEO ) {
		FMediaRawVideoFormatToMessage( &format->u.raw_video, msg );
	} else if( format->type == B_MEDIA_ENCODED_VIDEO ) {
		BMessage RawFormatMsg;
		FMediaRawVideoFormatToMessage( &format->u.encoded_video.output, &RawFormatMsg );
		msg->AddMessage("output", &RawFormatMsg );
		
		msg->AddFloat("avg_bit_rate", format->u.encoded_video.avg_bit_rate );
		msg->AddFloat("max_bit_rate", format->u.encoded_video.max_bit_rate );
		msg->AddInt32("encoding", format->u.encoded_video.encoding );
		msg->AddInt32("frame_size", format->u.encoded_video.frame_size );
		msg->AddInt16("forward_history", format->u.encoded_video.forward_history );
		msg->AddInt16("backward_history", format->u.encoded_video.backward_history );
	}
}

void FMediaRawVideoFormatToMessage( media_raw_video_format *format, BMessage *msg ) {
		msg->AddFloat("field_rate", format->field_rate);
		msg->AddInt32("interlace", format->interlace);
		msg->AddInt32("first_active", format->first_active);
		msg->AddInt32("last_active", format->last_active);
		msg->AddInt32("orientation", format->orientation);
		msg->AddInt16("pixel_width_aspect", format->pixel_width_aspect);
		msg->AddInt16("pixel_height_aspect", format->pixel_height_aspect);
		
		msg->AddInt64("format", (int64)format->display.format);
		msg->AddInt32("line_width", format->display.line_width);
		msg->AddInt32("line_count", format->display.line_count);
		msg->AddInt32("bytes_per_row", format->display.bytes_per_row);
		msg->AddInt32("pixel_offset", format->display.pixel_offset);
		msg->AddInt32("line_offset", format->display.line_offset);
}

void FMediaMessageToFormat( BMessage *msg, media_format *format ) {
	msg->FindInt32( "type", (int32*)(&format->type) );
	if( format->type == B_MEDIA_RAW_AUDIO ) {
		msg->FindFloat("frame_rate", &format->u.raw_audio.frame_rate);
		msg->FindInt32("channel_count", (int32*)&format->u.raw_audio.channel_count);
		msg->FindInt32("format", (int32*)&format->u.raw_audio.format);
		msg->FindInt32("byte_order", (int32*)&format->u.raw_audio.byte_order);
		msg->FindInt64("buffer_size", (int64*)&format->u.raw_audio.buffer_size);
	} else if( format->type == B_MEDIA_RAW_VIDEO ) {
		FMediaMessageToRawVideoFormat( msg, &format->u.raw_video );
	} else if( format->type == B_MEDIA_ENCODED_VIDEO ) {
		BMessage RawFormatMsg;
		msg->FindMessage("output", &RawFormatMsg );
		FMediaMessageToRawVideoFormat( &RawFormatMsg, &format->u.encoded_video.output );
		
		msg->FindFloat("avg_bit_rate", &format->u.encoded_video.avg_bit_rate );
		msg->FindFloat("max_bit_rate", &format->u.encoded_video.max_bit_rate );
		msg->FindInt32("encoding", (int32*)&format->u.encoded_video.encoding );
		msg->FindInt32("frame_size", (int32*)&format->u.encoded_video.frame_size );
		msg->FindInt16("forward_history", &format->u.encoded_video.forward_history );
		msg->FindInt16("backward_history", &format->u.encoded_video.backward_history );
	}
}

void FMediaMessageToRawVideoFormat( BMessage *msg, media_raw_video_format *format ) {
		msg->FindFloat("field_rate", &format->field_rate);
		msg->FindInt32("interlace", (int32*)&format->interlace);
		msg->FindInt32("first_active", (int32*)&format->first_active);
		msg->FindInt32("last_active", (int32*)&format->last_active);
		msg->FindInt32("orientation", (int32*)&format->orientation);
		msg->FindInt16("pixel_width_aspect", (int16*)&format->pixel_width_aspect);
		msg->FindInt16("pixel_height_aspect", (int16*)&format->pixel_height_aspect);
		
		msg->FindInt64("format", (int64*)&format->display.format);
		msg->FindInt32("line_width", (int32*)&format->display.line_width);
		msg->FindInt32("line_count", (int32*)&format->display.line_count);
		msg->FindInt32("bytes_per_row", (int32*)&format->display.bytes_per_row);
		msg->FindInt32("pixel_offset", (int32*)&format->display.pixel_offset);
		msg->FindInt32("line_offset", (int32*)&format->display.line_offset);
}

const char *FStringForFormat( media_format *format ) {
	static char String[ 0xff ];
	
	if( format->type == B_MEDIA_RAW_VIDEO ) {
		const char *CSpace;
		switch( format->u.raw_video.display.format ) {
			case B_RGB32:
				CSpace = "RGB32";
				break;
			case B_RGBA32:
				CSpace = "RGBA32";
				break;
			defaulft:
				CSpace = "[Unknown Colorspace]";
		}
		sprintf( (char*)&String, "Raw %.1f fps %ix%i %s Video",
			format->u.raw_video.field_rate,
			format->u.raw_video.display.line_width,
			format->u.raw_video.display.line_count,
			CSpace );
	} else if( format->type == B_MEDIA_RAW_AUDIO ) {
		const char *Bitrate;
		char *Channels = new char[32];
		switch( format->u.raw_audio.format ) {
			case media_raw_audio_format::B_AUDIO_FLOAT:
				Bitrate = "floating-point";
				break;
			case media_raw_audio_format::B_AUDIO_SHORT:
				Bitrate = "16bit";
				break;
			case media_raw_audio_format::B_AUDIO_UCHAR:
				Bitrate = "8bit";
				break;
			case media_raw_audio_format::B_AUDIO_INT:
				Bitrate = "32bit";
				break;
			default:
				Bitrate = "?bit";
				break;
		}
		switch( format->u.raw_audio.channel_count ) {
			case 1:
				sprintf( Channels, "Mono" );
				break;
			case 2:
				sprintf( Channels, "Stereo" );
				break;
			case 4:
				sprintf( Channels, "Quadrophonic" );
				break;
			case 8:
				sprintf( Channels, "Octophonic" );
				break;
			default:
				sprintf( Channels, "%i Channel", format->u.raw_audio.channel_count );
				break;
		}
		sprintf( (char*)&String, "Raw %.0f KHz %s %s Audio",
			format->u.raw_audio.frame_rate,
			Bitrate, Channels );
			
		delete Channels;
	} else {
		sprintf( (char*)&String, "Unknown Format" );
	}
	
	return( (char*)&String );
}
/*
media_raw_audio_format& operator =( media_raw_audio_format& m, media_raw_audio_format& r ) {
	m.frame_rate = r.frame_rate;
	m.channel_count = r.channel_count;
	m.format = r.format;
	m.byte_order = r.byte_order;
	m.buffer_size = r.buffer_size;
}
*/

bool MultiizeAudioFormat( media_multi_audio_format * a, const media_raw_audio_format & b) {
	// media_raw_audio_format
	a->frame_rate = b.frame_rate;
	a->channel_count = b.channel_count;
	a->format = b.format;
	a->byte_order = b.byte_order;
	a->buffer_size = b.buffer_size;

	// media_multi_audio_info
	a->channel_mask = 0;
	a->valid_bits = 0;
	a->matrix_mask = 0;
}

int32 FBytesPerPixel( const color_space& s ) {
	switch( s ) {
		case B_RGB32:
		case B_RGBA32:
		case B_RGB32_BIG:
		case B_RGBA32_BIG:
			return 4;

		case B_RGB24:
		case B_RGB24_BIG:
			return 3;

		case B_RGB16:
		case B_RGB15:
		case B_RGBA15:
		case B_RGB16_BIG:
		case B_RGB15_BIG:
		case B_RGBA15_BIG:
			return 2;

		case B_GRAY8:
		case B_CMAP8:
			return 1;
			
		// YCbCr etc...
	}
	WARNING("Unknown Colorspace in FBytesPerPixel: %i\n", s );
	return -1;
}

