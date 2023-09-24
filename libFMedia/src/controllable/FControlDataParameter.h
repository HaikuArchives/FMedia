#ifndef F_FInputParameter_H
#define F_FInputParameter_H

#include <FParameter.h>
#include <FMediaInput.h>
#include <FMediaOutput.h>

class FInputParameter : public FContinuousParameter {
	public:
		FInputParameter( const char *name, const char *kind, const char *unit, float min, float max, float step, float value )
		
	protected:
		FMediaInput *mInput;
};

#endif