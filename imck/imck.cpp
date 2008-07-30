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

#include "stdafx.h"
#include "ImasMultiColorKeying.h"
#include "ImasMultiColorKeying2.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

#define SET_GLOBAL_DEFINE(x) SetGlobalVar(#x, AVSValue(x))

extern "C" __declspec(dllexport) const char* __stdcall AvisynthPluginInit2(IScriptEnvironment* env)
{
	env->AddFunction("ImasMultiColorKeying", "[red1]c[red2]c[red3]c[green1]c[green2]c[green3]c", ImasMultiColorKeying::GetObject, 0);
	env->AddFunction("ImasMultiColorKeying2", "i[ckip1]c[clip2]c[outtype]i", ImasMultiColorKeying2::GetObject, 0);

	env->SET_GLOBAL_DEFINE(IMCK2_OUTTYPE_RGB32_RGBA);
	env->SET_GLOBAL_DEFINE(IMCK2_OUTTYPE_RGB32_ALPHAONLY);
	env->SET_GLOBAL_DEFINE(IMCK2_OUTTYPE_RGB24_RGBONLY);
	env->SET_GLOBAL_DEFINE(IMCK2_OUTTYPE_RGB24_ALPHAONLY);

	return "iM@S MultiColor Keying for AviSynth";
}
