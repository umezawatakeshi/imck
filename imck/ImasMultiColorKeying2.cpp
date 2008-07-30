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
#include "ImasMultiColorKeying2.h"

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

AVSValue __cdecl ImasMultiColorKeying2::GetObject(AVSValue args, void *user_data, IScriptEnvironment *env)
{
	return new ImasMultiColorKeying2(args[0].AsInt(), args[1].AsClip(), args[2].AsClip(), args[3].AsInt(IMCK2_OUTTYPE_RGB32_RGBA), env);  
}

ImasMultiColorKeying2::ImasMultiColorKeying2(int _framepos, PClip _clip1, PClip _clip2, int _outtype, IScriptEnvironment *env) :
	clip1(_clip1),
	clip2(_clip2),
	outtype(_outtype),
	GenericVideoFilter(_clip1)
{
	PVideoFrame frame1;
	PVideoFrame frame2;

	if (!clip1->GetVideoInfo().IsRGB24() ||
		!clip2->GetVideoInfo().IsRGB24())
	{
		env->ThrowError("ImasMultiColorKeying2: Inputs must be RGB24");
	}

	switch(outtype)
	{
	case IMCK2_OUTTYPE_RGB32_RGBA:
	case IMCK2_OUTTYPE_RGB32_ALPHAONLY:
		vi.pixel_type = VideoInfo::CS_BGR32;
		break;
	case IMCK2_OUTTYPE_RGB24_RGBONLY:
	case IMCK2_OUTTYPE_RGB24_ALPHAONLY:
		vi.pixel_type = VideoInfo::CS_BGR24;
		break;
	default:
		env->ThrowError("ImasMultiColorKeying2: Argument 'outtype' invalid");
	}

	frame1 = clip1->GetFrame(_framepos, env);
	frame2 = clip2->GetFrame(_framepos, env);

	base1r = 0.0;
	base1g = 0.0;
	base1b = 0.0;
	base2r = 0.0;
	base2g = 0.0;
	base2b = 0.0;

	for (int y = 8; y < 16; y++)
	{
		for (int x = 8; x < 16; x++)
		{
			int pos =y * frame1->GetPitch() + x * 3;
			base1b += frame1->GetReadPtr()[pos];
			base1g += frame1->GetReadPtr()[pos+1];
			base1r += frame1->GetReadPtr()[pos+2];
			base2b += frame2->GetReadPtr()[pos];
			base2g += frame2->GetReadPtr()[pos+1];
			base2r += frame2->GetReadPtr()[pos+2];
		}

		for (int x = vi.width - 16; x < vi.width - 8; x++)
		{
			int pos =y * frame1->GetPitch() + x * 3;
			base1b += frame1->GetReadPtr()[pos];
			base1g += frame1->GetReadPtr()[pos+1];
			base1r += frame1->GetReadPtr()[pos+2];
			base2b += frame2->GetReadPtr()[pos];
			base2g += frame2->GetReadPtr()[pos+1];
			base2r += frame2->GetReadPtr()[pos+2];
		}
	}

	for (int y = vi.height - 16; y < vi.height - 8; y++)
	{
		for (int x = 8; x < 16; x++)
		{
			int pos =y * frame1->GetPitch() + x * 3;
			base1b += frame1->GetReadPtr()[pos];
			base1g += frame1->GetReadPtr()[pos+1];
			base1r += frame1->GetReadPtr()[pos+2];
			base2b += frame2->GetReadPtr()[pos];
			base2g += frame2->GetReadPtr()[pos+1];
			base2r += frame2->GetReadPtr()[pos+2];
		}

		for (int x = vi.width - 16; x < vi.width - 8; x++)
		{
			int pos =y * frame1->GetPitch() + x * 3;
			base1b += frame1->GetReadPtr()[pos];
			base1g += frame1->GetReadPtr()[pos+1];
			base1r += frame1->GetReadPtr()[pos+2];
			base2b += frame2->GetReadPtr()[pos];
			base2g += frame2->GetReadPtr()[pos+1];
			base2r += frame2->GetReadPtr()[pos+2];
		}
	}

	base1r /= 256.0;
	base1g /= 256.0;
	base1b /= 256.0;
	base2r /= 256.0;
	base2g /= 256.0;
	base2b /= 256.0;
}

ImasMultiColorKeying2::~ImasMultiColorKeying2()
{
}

PVideoFrame __stdcall ImasMultiColorKeying2::GetFrame(int n, IScriptEnvironment *env)
{
	PVideoFrame frame1     = clip1->GetFrame(n, env);
	PVideoFrame frame2     = clip2->GetFrame(n, env);
	PVideoFrame frame_result = env->NewVideoFrame(vi);

	const AS_RGB *ptr1 = (const AS_RGB *)frame1->GetReadPtr();
	const AS_RGB *ptr2 = (const AS_RGB *)frame2->GetReadPtr();
	BYTE *ptr_start    = frame_result->GetWritePtr();
	BYTE *ptr_end      = ptr_start + frame_result->GetHeight() * frame_result->GetPitch();
	AS_RGBA rgba;
	int bypp = 1;

	switch(outtype)
	{
	case IMCK2_OUTTYPE_RGB32_RGBA:
	case IMCK2_OUTTYPE_RGB32_ALPHAONLY:
		bypp = 4;
		break;
	case IMCK2_OUTTYPE_RGB24_RGBONLY:
	case IMCK2_OUTTYPE_RGB24_ALPHAONLY:
		bypp = 3;
		break;
	}

	for (BYTE *p = ptr_start; p < ptr_end; p += bypp)
	{
		double a = 0.0;

		if (fabs(base1b - base2b) >= 32.0)
			a = max(a, 1.0 - ((int)ptr1->b - (int)ptr2->b) / (base1b - base2b));

		if (fabs(base1g - base2g) >= 32.0)
			a = max(a, 1.0 - ((int)ptr1->g - (int)ptr2->g) / (base1g - base2g));

		if (fabs(base1r - base2r) >= 32.0)
			a = max(a, 1.0 - ((int)ptr1->r - (int)ptr2->r) / (base1r - base2r));

		a = clipval(a, 0.0, 1.0);

		if (a == 0.0) {
			rgba.b = 0;
			rgba.g = 0;
			rgba.r = 0;
			rgba.a = 0;
		} else {
			rgba.b = (BYTE)clipval((ptr1->b - base1b * (1.0 - a)) / a, 0.0, 255.9);
			rgba.g = (BYTE)clipval((ptr1->g - base1g * (1.0 - a)) / a, 0.0, 255.9);
			rgba.r = (BYTE)clipval((ptr1->r - base1r * (1.0 - a)) / a, 0.0, 255.9);
			rgba.a = (BYTE)(a*255);
		}

		switch(outtype)
		{
		case IMCK2_OUTTYPE_RGB32_RGBA:
			*(AS_RGBA *)p = rgba;
			break;
		case IMCK2_OUTTYPE_RGB32_ALPHAONLY:
			rgba.b = 255;
			rgba.g = 255;
			rgba.r = 255;
			*(AS_RGBA *)p = rgba;
			break;
		case IMCK2_OUTTYPE_RGB24_RGBONLY:
			((AS_RGB *)p)->b = rgba.b;
			((AS_RGB *)p)->g = rgba.g;
			((AS_RGB *)p)->r = rgba.r;
			break;
		case IMCK2_OUTTYPE_RGB24_ALPHAONLY:
			((AS_RGB *)p)->b = rgba.a;
			((AS_RGB *)p)->g = rgba.a;
			((AS_RGB *)p)->r = rgba.a;
			break;
		}

		ptr1++;
		ptr2++;
	}

	return frame_result;
}
