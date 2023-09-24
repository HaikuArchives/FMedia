#ifndef F_FAverager_H
#define F_FAverager_H

template <class T>
class FDamp {
	public:
		FDamp( float velocity=.5, float brake = .5, T init = 0 ) {
			cValue = init;
			Set( velocity, brake );
		}
		
		~FDamp() {
		}
		
		void Set( float velocity=.5, float brake = .5 ) {
			mVelocity = velocity;
			mBrake = brake;
		}
		 
		T Add( T value ) {
			T diff = value;
			diff -= cValue;
			cValue += diff*mVelocity;
			return( cValue );
		}
		
		T Value() {
			return( cValue );
		}
	
	protected:
		T cValue;
		float mVelocity;
		float mBrake;
		T mSpeed;
};

template <class T>
class FUpDownChaser {
	public:
		FUpDownChaser( float upvelocity=.5, float downvelocity=.5, float brake = .5, T init = 0 ) {
			cValue = 0;
			mUpVelocity = upvelocity;
			mDownVelocity = downvelocity;
			mBrake = brake;
			mSpeed = 0;
		}
		
		~FUpDownChaser() {
		}
		
		void Add( T value ) {
			mSpeed *= mBrake;
			T acc = (value-cValue);
			if( acc > 0 ) acc*=mUpVelocity;
			else acc *= mDownVelocity;
			mSpeed += acc;
			cValue += mSpeed;
		}
		
		T Value() {
			return( cValue );
		}
	
	protected:
		T cValue;
		float mUpVelocity, mDownVelocity;
		float mBrake;
		float mSpeed;
};


#endif
