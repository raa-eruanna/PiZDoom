#include "gl_pch.h"
#include <il/il.h>
/*
** gfxfuncs.cpp
** True color graphics manipulation
**
**---------------------------------------------------------------------------
** Copyright 2003-2005 Christoph Oelckers
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
** 4. When not used as part of GZDoom or a GZDoom derivative, this code will be
**    covered by the terms of the GNU Lesser General Public License as published
**    by the Free Software Foundation; either version 2.1 of the License, or (at
**    your option) any later version.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
*/
#include "r_main.h"

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

//===========================================================================
// 
//	Takes a screenshot
//
//===========================================================================
void gl_ScreenShot (const char* fname)
{
	unsigned int imgID;

	byte * scr = (byte *)M_Malloc(SCREENWIDTH * SCREENHEIGHT * 4);
	gl.ReadPixels(0,0,SCREENWIDTH,SCREENHEIGHT,GL_RGBA,GL_UNSIGNED_BYTE,scr);
	
	ilGenImages(1, &imgID);
	ilBindImage(imgID);
	ilTexImage(SCREENWIDTH,SCREENHEIGHT, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, scr);
	ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	ilHint(IL_COMPRESSION_HINT, IL_USE_COMPRESSION);
	ilSave(IL_PNG, (char *)fname);
	ilDeleteImages(1, &imgID);
	free(scr);
}

//===========================================================================
//
// averageColor
//  input is RGBA8 pixel format.
//	The resulting RGB color can be scaled uniformly so that the highest 
//	component becomes one.
//
//===========================================================================
PalEntry averageColor(const unsigned long *data, int size, bool maxout)
{
	int				i;
	unsigned int	r, g, b;



	// First clear them.
	r = g = b = 0;
	if (size==0) 
	{
		return PalEntry(255,255,255);
	}
	for(i = 0; i < size; i++)
	{
		r += BPART(data[i]);
		g += GPART(data[i]);
		b += RPART(data[i]);
	}

	r = r/size;
	g = g/size;
	b = b/size;

	int maxv=max(max(r,g),b);

	if(maxv && maxout)
	{
		r *= 255.0f / maxv;
		g *= 255.0f / maxv;
		b *= 255.0f / maxv;
	}
	return PalEntry(r,g,b);
}


