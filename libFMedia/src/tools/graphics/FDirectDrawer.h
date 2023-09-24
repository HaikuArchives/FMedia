#ifndef F_FDirectDrawer_H
#define F_FDirectDrawer_H


struct FDirectContext {
	clipping_rect mBounds;

	clipping_rect *mClipRects;
	uint32 nClipRects;
	
	color_space mCSpace;
	uint32 mBytesPerRow;
	void *mBits;
	
	FDirectContext &operator =( FDirectContext& c );
	FDirectContext &operator =( direct_buffer_info* c );
};

class FDirectDrawer {
	public:
		FDirectDrawer();
		~FDirectDrawer();
	
		virtual void AllocateBuffer() = 0;
		virtual void Draw() = 0;
		
	protected:
		FDirectContext *mContext;

		sem_id mDrawLock;
};

template <class P>
class FTypedDirectDrawer : public FDirectDrawer {
	public:

	protected:
		P *bCurrent, *bLast;		// bCurrent is being filled and drawed, bLast is for clearing
		uint32 nCurrent;			// last valid _byte_ in bCurrent
};

#endif
