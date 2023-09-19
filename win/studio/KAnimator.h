#ifndef _KANIMATOR_H_
#define _KANIMATOR_H_

#include "klib/graphic.h"
#include "klib/array.h"

class KAnimeLayer {

};

class KAnimeSequence {
	friend class KAnimator;
public:
	// image setting
	KAnimeSequence(int image_num);
	// normal
	KAnimeSequence(bool ofs,int alpha, RGBA32 color, KPOINT orig,
					int x_scale, int y_scale, int rot);
	// free-transform
	KAnimeSequence(bool ofs, int alpha, RGBA32 color,
					KPOINT topleft, KPOINT topright,
					KPOINT bottomright, KPOINT bottomleft);
protected:
	int		type;
	int		image_num;
	bool	is_offset;
	int		alpha;
	RGBA32	color;
	KPOINT	orig;
	int		x_scale;
	int		y_scale;
	int		rot;
	KPOINT	rect[4];
};

class KAnimator {
public:
	KAnimator(int layers_count);
	~KAnimator();


	void	StartSequenceDefinition();
	void	AddImage(KImage *image, bool need_delete);
	void	AddSequence(int layer, int time, const KAnimeSequence& seq);
	void	EndSequenceDefinition();
	void	DrawFrame(KImage *image, int time);

private:
	KArray<KImage *>	images;
	KArray<bool>		need_delete;
	KArray< KArray<KAnimeSequence> >	layer;
};

#endif
