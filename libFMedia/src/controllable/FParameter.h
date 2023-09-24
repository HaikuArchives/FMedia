#ifndef F_FParameter_H
#define F_FParameter_H

#include <ParameterWeb.h>
#include <string.h>

class FParameter {
	public:
		FParameter( media_type mtype, const char *name, const char *kind, type_code type ) {
			mID = -1;
			mType = type;
			mMediaType = mtype;
			mName = strdup( name );
			mKind = kind;
			mParameter = NULL;
		}
		
		~FParameter() {
			if( mName ) delete mName;
		}
		
		const char *Name() { return mName; }
		void SetName( const char *name ) { if( mName ) delete mName; mName = strdup( name ); }

		virtual status_t Set( const void *value, size_t ioSize ) = 0;
		virtual status_t Get( void *value, size_t *ioSize ) = 0;

		void SetID( int32 id ) { mID = id; }
		int32 GetID() { return( mID ); }
		
		virtual BParameterGroup * CreateInGroup( BParameterGroup* group, int32 id ) = 0;
		
	protected:
		type_code mType;
		int32 mID;

		char *mName;
		media_type mMediaType;
		const char *mKind;
		BParameter *mParameter;	
};

template <class T>
class FTypedParameter : public FParameter {
	public:
		FTypedParameter( media_type mtype, const char *name, const char *kind, T value, type_code type )
			: FParameter( mtype, name, kind, type ) {
			mValue = value;
			mMediaType = mtype;
			mName = strdup( name );
			mKind = kind;
			mParameter = NULL;
		}
		
		~FTypedParameter() {
			if( mName ) delete mName;
		}
		
		const char *Name() { return mName; }
		
		inline T Get() { return mValue; }
		inline void Set( T value ) { mValue=value; }
		
		virtual status_t Set( const void *value, size_t ioSize ) {
			if( ioSize == sizeof(T) ) {
				mValue = *((T*)value);
				return B_OK;
			}
			return B_ERROR;
		}
		virtual status_t Get( void *value, size_t *ioSize ) {
				if( *ioSize >= sizeof(T) ) {
					*((T*)value) = mValue;
					*ioSize = sizeof(T);
					return B_OK;
				}
				return B_ERROR;
			}

	protected:
		T mValue;
};

class FNullParameter : public FTypedParameter<int32> {
	public:
		FNullParameter( media_type type, const char *name, const char *kind ) 
				: FTypedParameter<int32>( type, name, kind, 0, B_INT32_TYPE ) {
			}

		BParameterGroup *CreateInGroup( BParameterGroup* group, int32 id ) {
			mParameter = group->MakeNullParameter( id, mMediaType, mName, mKind );
			mID = id;
			return( group );
		}
};

class FParameterGroup : public FNullParameter {
	public:
		FParameterGroup( const char *name ) 
				: FNullParameter( B_MEDIA_NO_TYPE, name, B_GENERIC ) {
			}

		BParameterGroup *CreateInGroup( BParameterGroup* group, int32 id ) {
			mID = id;
			return( group->MakeGroup( mName ) );
		}
};

class FDiscreteParameter : public FTypedParameter<int32> {
	public:
		FDiscreteParameter( media_type type, const char *name, const char *kind, int32 value ) 
				: FTypedParameter<int32>( type, name, kind, value, B_INT32_TYPE ) {
			}
				
		BParameterGroup *CreateInGroup( BParameterGroup* group, int32 id ) {
			mParameter = group->MakeDiscreteParameter( id, mMediaType, mName, mKind );
			mID = id;
			return( group );
		}

	protected:
};

class FChoiceParameter : public FDiscreteParameter {
	public:
		FChoiceParameter( media_type type, const char *name, int32 value = 0, int32 allocChoices = 10 ) 
				: FDiscreteParameter( type, name, B_INPUT_MUX, value ) {
				aChoices = allocChoices;
				nChoices=0;
				mChoices = new const char *[ aChoices ];
				mIDs = new int32[ aChoices ];
			}

		virtual ~FChoiceParameter() {
				for( int i=0; i<nChoices; i++ ) {
					if( mChoices[i] ) delete mChoices[i];
				}
			}
				
		BParameterGroup *CreateInGroup( BParameterGroup* group, int32 id ) {
			BDiscreteParameter *parameter = group->MakeDiscreteParameter( id, mMediaType, mName, mKind );
			
			for( int i=0; i<nChoices; i++ ) {
				parameter->AddItem( mIDs[i], mChoices[i] );
			}
			
			mParameter = parameter;
			mID = id;
			
			return( group );
		}

		int32 AddChoice( const char *name, int32 id=-1 ) {
			if( nChoices >= aChoices ) return( -1 );
			if( id == -1 ) id = nChoices;
			mIDs[ nChoices ] = id;
			mChoices[ nChoices ] = strdup(name);
			nChoices++;
			return id;
		}

	protected:
		const char **mChoices;
		int32 *mIDs;
		int32 nChoices, aChoices;
};

class FContinuousParameter : public FTypedParameter<float> {
	public:
		FContinuousParameter( media_type type, const char *name, const char *kind, const char *unit, float min, float max, float step, float value )
			: FTypedParameter<float>( type, name, kind, value, B_FLOAT_TYPE ) {
			mUnit = strdup( unit );
			mMin = min;
			mMax = max;
			mStep = step;
		}
				
		BParameterGroup *CreateInGroup( BParameterGroup* group, int32 id ) {
			mParameter = group->MakeContinuousParameter( id, mMediaType, mName, mKind, mUnit, mMin, mMax, mStep );
			mID = id;
			return( group );
		}

	protected:
		char *mUnit;
		float mMin, mMax, mStep;
};


#endif
