/*****************************************************************************
 edgescanner.cpp
 スキャンコンバージョンのエッジスキャナ

 Suika Studio (core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "klib/graphic.h"

KEdgeScanner::KEdgeScanner(int lines_count)
{
	this->lines_count = lines_count;
	lines = new KEdgeLineBuffer[lines_count];
}

KEdgeScanner::~KEdgeScanner()
{
	delete[] lines;
}

void KEdgeScanner::Clear()
{
	for(int i = 0; i < lines_count; i++) {
		lines[i].min = SCBUF_MIN_INVALID;
		lines[i].max = SCBUF_MAX_INVALID;
	}
}

// 点P1と点P2を端点とする線分のエッジをスキャンする。
void
KEdgeScanner::ScanEdge(
	Point	p1,		// P1の座標
	Point	p2)	// P2の座標
{
	if(p2.y < p1.y) {
		Point tmp = p1;
		p1 = p2;
		p2 = tmp;
	}

	int p1y = fp_fix2int(p1.y);
	int	p2y = fp_fix2int(p2.y);

	if(p1y == p2y) {
		if(p1y < 0 || p1y >= lines_count)
			return;

		KEdgeLineBuffer& l = lines[p1y];
		if(p1.x < l.min) {
			l.min = p1.x;
			l.min_tx = p1.tx;
			l.min_ty = p1.ty;
		}
		if(p1.x > l.max) {
			l.max = p1.x;
			l.max_tx = p1.tx;
			l.max_ty = p1.ty;
		}
		if(p2.x < l.min) {
			l.min = p2.x;
			l.min_tx = p2.tx;
			l.min_ty = p2.ty;
		}
		if(p2.x > l.max) {
			l.max = p2.x;
			l.max_tx = p2.tx;
			l.max_ty = p2.ty;
		}
		return;
	}

	fixedpoint px = p1.x, tx = p1.tx, ty = p1.ty;
	fixedpoint d_py = p2.y - p1.y;
	fixedpoint d_px = fp_div( (p2.x - px) , d_py );
	fixedpoint d_tx = fp_div( (p2.tx - tx) , d_py );
	fixedpoint d_ty = fp_div( (p2.ty - ty) , d_py );

	if(p1.y < 0) {
		px += fp_mul( d_px , -p1.y );
		tx += fp_mul( d_tx , -p1.y );
		ty += fp_mul( d_ty , -p1.y );
		p1.y = fp_int2fix(0);
	}
	if(p2y >= lines_count)
		p2y = lines_count - 1;

	for(int i = p1y; i <= p2y; i++, px += d_px, tx += d_tx, ty += d_ty) {
		KEdgeLineBuffer& l = lines[i];
		if(px < l.min) {
			l.min = px;
			l.min_tx = tx;
			l.min_ty = ty;
		}
		if(px > l.max) {
			l.max = px;
			l.max_tx = tx;
			l.max_ty = ty;
		}
	}
}
