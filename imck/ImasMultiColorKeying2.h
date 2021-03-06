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

#pragma once

#include <avisynth.h>
#include "Thread.h"
                                        // fmt    RGB-ch  A-ch
#define IMCK2_OUTTYPE_RGB32_RGBA      0 // RGB32   RGB    Alpha
#define IMCK2_OUTTYPE_RGB32_ALPHAONLY 1 // RGB32    白    Alpha
#define IMCK2_OUTTYPE_RGB24_RGBONLY   2 // RGB24   RGB      -
#define IMCK2_OUTTYPE_RGB24_ALPHAONLY 3 // RGB24  Alpha     -

class ImasMultiColorKeying2 :
	public GenericVideoFilter
{
protected:
	const PClip clip1;
	const PClip clip2;
	const int outtype;

	double base1r;
	double base1g;
	double base1b;
	double base2r;
	double base2g;
	double base2b;

	CThreadManager *m_ptm;

	int m_bypp;
	int m_nDivideCount;

	BYTE *m_pDstBegin;
	BYTE *m_pDstEnd;
	const BYTE *m_pSrc1Begin;
	const BYTE *m_pSrc2Begin;

public:
	static AVSValue __cdecl GetObject(AVSValue args, void *user_data, IScriptEnvironment *env);
	ImasMultiColorKeying2(int _framepos, PClip _clip1, PClip _clip2, int _outtype, IScriptEnvironment *env);
	virtual ~ImasMultiColorKeying2();

public:
	PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment *env);

private:
	void FilterProc(DWORD nIndex);
	class CFilterJob : public CThreadJob
	{
	private:
		const DWORD m_nIndex;
		ImasMultiColorKeying2 *const m_pFilter;
	public:
		CFilterJob(ImasMultiColorKeying2 *pFilter, DWORD nIndex) : m_pFilter(pFilter), m_nIndex(nIndex)
		{
		}
		void JobProc(CThreadManager *)
		{
			m_pFilter->FilterProc(m_nIndex);
		}
	};
};
