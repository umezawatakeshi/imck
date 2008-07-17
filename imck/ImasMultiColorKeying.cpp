/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */
/*
 * iM@S MultiColor Keying
 * Copyright (C) 2008  UMEZAWA Takeshi
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * 
 * 
 * iM@S MultiColor Keying
 * Copyright (C) 2008  梅澤 威志
 * 
 * このプログラムはフリーソフトウェアです。あなたはこれを、フリーソフ
 * トウェア財団によって発行された GNU 一般公衆利用許諾契約書(バージョ
 * ン2か、希望によってはそれ以降のバージョンのうちどれか)の定める条件
 * の下で再頒布または改変することができます。
 * 
 * このプログラムは有用であることを願って頒布されますが、*全くの無保
 * 証* です。商業可能性の保証や特定の目的への適合性は、言外に示された
 * ものも含め全く存在しません。詳しくはGNU 一般公衆利用許諾契約書をご
 * 覧ください。
 * 
 * あなたはこのプログラムと共に、GNU 一般公衆利用許諾契約書の複製物を
 * 一部受け取ったはずです。もし受け取っていなければ、フリーソフトウェ
 * ア財団まで請求してください(宛先は the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA)。
 */

#include "StdAfx.h"
#include "ImasMultiColorKeying.h"

struct AS_RGB
{
	BYTE b;
	BYTE g;
	BYTE r;
};

struct AS_RGBA
{
	BYTE b;
	BYTE g;
	BYTE r;
	BYTE a;
};

template<typename T>
inline T clipval(T &a, T &_min, T &_max)
{
	return min(max(a, _min), _max);
}

template<typename T>
inline T clipval(T a, T _min, T _max)
{
	return min(max(a, _min), _max);
}

AVSValue __cdecl ImasMultiColorKeying::GetObject(AVSValue args, void *user_data, IScriptEnvironment *env)
{
	return new ImasMultiColorKeying(args[0].AsClip(), args[1].AsClip(), args[2].AsClip(), args[3].AsClip(), args[4].AsClip(), args[5].AsClip(), env);  
}

ImasMultiColorKeying::ImasMultiColorKeying(PClip _clip_r1, PClip _clip_r2, PClip _clip_r3, PClip _clip_g1, PClip _clip_g2, PClip _clip_g3, IScriptEnvironment *env) :
	clip_r1(_clip_r1),
	clip_r2(_clip_r2),
	clip_r3(_clip_r3),
	clip_g1(_clip_g1),
	clip_g2(_clip_g2),
	clip_g3(_clip_g3),
	GenericVideoFilter(_clip_r1)
{
	if (!clip_r1->GetVideoInfo().IsRGB32() ||
		!clip_r2->GetVideoInfo().IsRGB32() ||
		!clip_r3->GetVideoInfo().IsRGB32() ||
		!clip_g1->GetVideoInfo().IsRGB32() ||
		!clip_g2->GetVideoInfo().IsRGB32() ||
		!clip_g3->GetVideoInfo().IsRGB32())
	{
		env->ThrowError("ImasMultiColorKeying: Inputs must be RGB32");
	}
}

ImasMultiColorKeying::~ImasMultiColorKeying()
{
}

PVideoFrame __stdcall ImasMultiColorKeying::GetFrame(int n, IScriptEnvironment *env)
{
	PVideoFrame frame_r1     = clip_r1->GetFrame(n, env);
	PVideoFrame frame_r2     = clip_r2->GetFrame(n, env);
	PVideoFrame frame_r3     = clip_r3->GetFrame(n, env);
	PVideoFrame frame_g1     = clip_g1->GetFrame(n, env);
	PVideoFrame frame_g2     = clip_g2->GetFrame(n, env);
	PVideoFrame frame_g3     = clip_g3->GetFrame(n, env);
	PVideoFrame frame_result = env->NewVideoFrame(vi);

	const AS_RGBA *ptr_r1   = (const AS_RGBA *)frame_r1->GetReadPtr();
	const AS_RGBA *ptr_r2   = (const AS_RGBA *)frame_r2->GetReadPtr();
	const AS_RGBA *ptr_r3   = (const AS_RGBA *)frame_r3->GetReadPtr();
	const AS_RGBA *ptr_g1   = (const AS_RGBA *)frame_g1->GetReadPtr();
	const AS_RGBA *ptr_g2   = (const AS_RGBA *)frame_g2->GetReadPtr();
	const AS_RGBA *ptr_g3   = (const AS_RGBA *)frame_g3->GetReadPtr();
	AS_RGBA *ptr_result     = (AS_RGBA *)frame_result->GetWritePtr();
	AS_RGBA *ptr_result_end = (AS_RGBA *)((BYTE *)ptr_result + frame_result->GetHeight() * frame_result->GetPitch());
	AS_RGBA rgba;

	for (AS_RGBA *p = ptr_result; p < ptr_result_end; p++)
	{
		double ar1g1 = 1.0 - ((int)ptr_g1->g - (int)ptr_r1->g) / 255.0;
		double ar2g1 = 1.0 - ((int)ptr_g1->g - (int)ptr_r2->g) / 255.0;
		double ar3g1 = 1.0 - ((int)ptr_g1->g - (int)ptr_r3->g) / 255.0;
		double ar1g2 = 1.0 - ((int)ptr_g2->g - (int)ptr_r1->g) / 255.0;
		double ar2g2 = 1.0 - ((int)ptr_g2->g - (int)ptr_r2->g) / 255.0;
		double ar3g2 = 1.0 - ((int)ptr_g2->g - (int)ptr_r3->g) / 255.0;
		double ar1g3 = 1.0 - ((int)ptr_g3->g - (int)ptr_r1->g) / 255.0;
		double ar2g3 = 1.0 - ((int)ptr_g3->g - (int)ptr_r2->g) / 255.0;
		double ar3g3 = 1.0 - ((int)ptr_g3->g - (int)ptr_r3->g) / 255.0;
		double a = max(max(max(max(max(max(max(max(ar1g1, ar1g2), ar1g3), ar2g1), ar2g2), ar2g3), ar3g1), ar3g2), ar3g3);	// 馬鹿
		a = clipval(a, 0.0, 1.0);

		if (a == 0.0) {
			rgba.b = 0;
			rgba.g = 0;
			rgba.r = 0;
			rgba.a = 0;
		} else {
			rgba.b = (BYTE)clipval((ptr_g1->b -   8 * (1.0 - a)) / a, 0.0, 255.9);
			rgba.g = (BYTE)clipval((ptr_g1->g - 255 * (1.0 - a)) / a, 0.0, 255.9);
			rgba.r = (BYTE)clipval((ptr_g1->r -  19 * (1.0 - a)) / a, 0.0, 255.9);
			rgba.a = (BYTE)(a*255);
		}
		*p = rgba;

		ptr_r1++;
		ptr_r2++;
		ptr_r3++;
		ptr_g1++;
		ptr_g2++;
		ptr_g3++;
	}

	return frame_result;
}
