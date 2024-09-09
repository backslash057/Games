#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "SDL_gfxPrimitives.h"


#define DEFAULT_ALPHA_PIXEL_ROUTINE
#undef EXPERIMENTAL_ALPHA_PIXEL_ROUTINE


/* ----- Defines for pixel clipping tests */

#define clip_xmin(surface) surface->clip_rect.x
#define clip_xmax(surface) surface->clip_rect.x+surface->clip_rect.w-1
#define clip_ymin(surface) surface->clip_rect.y
#define clip_ymax(surface) surface->clip_rect.y+surface->clip_rect.h-1

/*!
\brief Internal pixel drawing - fast, no blending, no locking, clipping.

\param dst The surface to draw on.
\param x The horizontal coordinate of the pixel.
\param y The vertical position of the pixel.
\param color The color value of the pixel to draw. 

\returns Returns 0 on success, -1 on failure.
*/
int fastPixelColorNolock(SDL_Surface * dst, Sint16 x, Sint16 y, Uint32 color)
{
	int bpp;
	Uint8 *p;

	/*
	* Honor clipping setup at pixel level 
	*/
	if ((x >= clip_xmin(dst)) && (x <= clip_xmax(dst)) && (y >= clip_ymin(dst)) && (y <= clip_ymax(dst))) {

		/*
		* Get destination format 
		*/
		bpp = dst->format->BytesPerPixel;
		p = (Uint8 *) dst->pixels + y * dst->pitch + x * bpp;
		switch (bpp) {
	case 1:
		*p = color;
		break;
	case 2:
		*(Uint16 *) p = color;
		break;
	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (color >> 16) & 0xff;
			p[1] = (color >> 8) & 0xff;
			p[2] = color & 0xff;
		} else {
			p[0] = color & 0xff;
			p[1] = (color >> 8) & 0xff;
			p[2] = (color >> 16) & 0xff;
		}
		break;
	case 4:
		*(Uint32 *) p = color;
		break;
		}			/* switch */


	}

	return (0);
}


/*!
\brief Internal pixel drawing - fast, no blending, locking, clipping.

\param dst The surface to draw on.
\param x The horizontal coordinate of the pixel.
\param y The vertical position of the pixel.
\param color The color value of the pixel to draw. 

\returns Returns 0 on success, -1 on failure.
*/
int fastPixelColor(SDL_Surface * dst, Sint16 x, Sint16 y, Uint32 color)
{
	int result;

	/*
	* Lock the surface 
	*/
	if (SDL_MUSTLOCK(dst)) {
		if (SDL_LockSurface(dst) < 0) {
			return (-1);
		}
	}

	result = fastPixelColorNolock(dst, x, y, color);

	/*
	* Unlock surface 
	*/
	if (SDL_MUSTLOCK(dst)) {
		SDL_UnlockSurface(dst);
	}

	return (result);
}


/*!
\brief Internal pixel drawing function with alpha blending where input color in in destination format.

Contains two alternative 32 bit alpha blending routines which can be enabled at the source
level with the defines DEFAULT_ALPHA_PIXEL_ROUTINE or EXPERIMENTAL_ALPHA_PIXEL_ROUTINE.
Only the bits up to the surface depth are significant in the color value.

\param dst The surface to draw on.
\param x The horizontal coordinate of the pixel.
\param y The vertical position of the pixel.
\param color The color value of the pixel to draw. 
\param alpha The blend factor to apply while drawing.

\returns Returns 0 on success, -1 on failure.
*/
int _putPixelAlpha(SDL_Surface *dst, Sint16 x, Sint16 y, Uint32 color, Uint8 alpha)
{
	SDL_PixelFormat *format;
	Uint32 Rmask, Gmask, Bmask, Amask;
	Uint32 Rshift, Gshift, Bshift, Ashift;
	Uint32 R, G, B, A;

	if (dst == NULL)
	{
		return (-1);
	}

	if (x >= clip_xmin(dst) && x <= clip_xmax(dst) && 
		y >= clip_ymin(dst) && y <= clip_ymax(dst)) 
	{

		format = dst->format;

		switch (format->BytesPerPixel) {
	case 1:
		{		/* Assuming 8-bpp */
			if (alpha == 255) {
				*((Uint8 *) dst->pixels + y * dst->pitch + x) = color;
			} else {
				Uint8 *pixel = (Uint8 *) dst->pixels + y * dst->pitch + x;
				SDL_Palette *palette = format->palette;
				SDL_Color *colors = palette->colors;
				SDL_Color dColor = colors[*pixel];
				SDL_Color sColor = colors[color];
				Uint8 dR = dColor.r;
				Uint8 dG = dColor.g;
				Uint8 dB = dColor.b;
				Uint8 sR = sColor.r;
				Uint8 sG = sColor.g;
				Uint8 sB = sColor.b;

				dR = dR + ((sR - dR) * alpha >> 8);
				dG = dG + ((sG - dG) * alpha >> 8);
				dB = dB + ((sB - dB) * alpha >> 8);

				*pixel = SDL_MapRGB(format, dR, dG, dB);
			}
		}
		break;

	case 2:
		{		/* Probably 15-bpp or 16-bpp */
			if (alpha == 255) {
				*((Uint16 *) dst->pixels + y * dst->pitch / 2 + x) = color;
			} else {
				Uint16 *pixel = (Uint16 *) dst->pixels + y * dst->pitch / 2 + x;
				Uint32 dc = *pixel;

				Rmask = format->Rmask;
				Gmask = format->Gmask;
				Bmask = format->Bmask;
				Amask = format->Amask;
				R = ((dc & Rmask) + (((color & Rmask) - (dc & Rmask)) * alpha >> 8)) & Rmask;
				G = ((dc & Gmask) + (((color & Gmask) - (dc & Gmask)) * alpha >> 8)) & Gmask;
				B = ((dc & Bmask) + (((color & Bmask) - (dc & Bmask)) * alpha >> 8)) & Bmask;
				*pixel = R | G | B;
				if (Amask!=0) {
					A = ((dc & Amask) + (((color & Amask) - (dc & Amask)) * alpha >> 8)) & Amask;
					*pixel |= A;
				}
			}
		}
		break;

	case 3: 
		{		
			/* Slow 24-bpp mode. This is patched on the pygame end because the
			 * original code failed on big endian */
			Uint8 *pixel = (Uint8 *) dst->pixels + y * dst->pitch + x * 3;
			Uint8 *dR, *dG, *dB;
			Uint8 sR, sG, sB;

			SDL_GetRGB(color, format, &sR, &sG, &sB);

#if (SDL_BYTEORDER == SDL_LIL_ENDIAN)
            dR = pixel + (format->Rshift >> 3);
            dG = pixel + (format->Gshift >> 3);
            dB = pixel + (format->Bshift >> 3);
#else
            dR = pixel + 2 - (format->Rshift >> 3);
            dG = pixel + 2 - (format->Gshift >> 3);
            dB = pixel + 2 - (format->Bshift >> 3);
#endif

			if (alpha == 255) {
				*dR = sR;
				*dG = sG;
				*dB = sB;
			} else {
				*dR = *dR + ((sR - *dR) * alpha >> 8);
				*dG = *dG + ((sG - *dG) * alpha >> 8);
				*dB = *dB + ((sB - *dB) * alpha >> 8);
			}
		}
		break;

#ifdef DEFAULT_ALPHA_PIXEL_ROUTINE

	case 4:
		{		/* Probably :-) 32-bpp */
			if (alpha == 255) {
				*((Uint32 *) dst->pixels + y * dst->pitch / 4 + x) = color;
			} else {
				Uint32 *pixel = (Uint32 *) dst->pixels + y * dst->pitch / 4 + x;
				Uint32 dc = *pixel;

				Rmask = format->Rmask;
				Gmask = format->Gmask;
				Bmask = format->Bmask;
				Amask = format->Amask;

				Rshift = format->Rshift;
				Gshift = format->Gshift;
				Bshift = format->Bshift;
				Ashift = format->Ashift;

				R = ((dc & Rmask) + (((((color & Rmask) - (dc & Rmask)) >> Rshift) * alpha >> 8) << Rshift)) & Rmask;
				G = ((dc & Gmask) + (((((color & Gmask) - (dc & Gmask)) >> Gshift) * alpha >> 8) << Gshift)) & Gmask;
				B = ((dc & Bmask) + (((((color & Bmask) - (dc & Bmask)) >> Bshift) * alpha >> 8) << Bshift)) & Bmask;
				*pixel = R | G | B;
				if (Amask!=0) {
					A = ((dc & Amask) + (((((color & Amask) - (dc & Amask)) >> Ashift) * alpha >> 8) << Ashift)) & Amask;
					*pixel |= A;
				}
			}
		}
		break;
#endif

#ifdef EXPERIMENTAL_ALPHA_PIXEL_ROUTINE

	case 4:{		/* Probably :-) 32-bpp */
		if (alpha == 255) {
			*((Uint32 *) dst->pixels + y * dst->pitch / 4 + x) = color;
		} else {
			Uint32 *pixel = (Uint32 *) dst->pixels + y * dst->pitch / 4 + x;
			Uint32 dR, dG, dB, dA;
			Uint32 dc = *pixel;

			Uint32 surfaceAlpha, preMultR, preMultG, preMultB;
			Uint32 aTmp;

			Rmask = format->Rmask;
			Gmask = format->Gmask;
			Bmask = format->Bmask;
			Amask = format->Amask;

			dR = (color & Rmask);
			dG = (color & Gmask);
			dB = (color & Bmask);
			dA = (color & Amask);

			Rshift = format->Rshift;
			Gshift = format->Gshift;
			Bshift = format->Bshift;
			Ashift = format->Ashift;

			preMultR = (alpha * (dR >> Rshift));
			preMultG = (alpha * (dG >> Gshift));
			preMultB = (alpha * (dB >> Bshift));

			surfaceAlpha = ((dc & Amask) >> Ashift);
			aTmp = (255 - alpha);
			if (A = 255 - ((aTmp * (255 - surfaceAlpha)) >> 8 )) {
				aTmp *= surfaceAlpha;
				R = (preMultR + ((aTmp * ((dc & Rmask) >> Rshift)) >> 8)) / A << Rshift & Rmask;
				G = (preMultG + ((aTmp * ((dc & Gmask) >> Gshift)) >> 8)) / A << Gshift & Gmask;
				B = (preMultB + ((aTmp * ((dc & Bmask) >> Bshift)) >> 8)) / A << Bshift & Bmask;
			}
			*pixel = R | G | B | (A << Ashift & Amask);

		}
		   }
		   break;
#endif
		}
	}

	return (0);
}

/*!
\brief Pixel draw with blending enabled if a<255.

\param dst The surface to draw on.
\param x X (horizontal) coordinate of the pixel.
\param y Y (vertical) coordinate of the pixel.
\param color The color value of the pixel to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
int pixelColor(SDL_Surface * dst, Sint16 x, Sint16 y, Uint32 color)
{
	Uint8 alpha;
	Uint32 mcolor;
	int result = 0;

	/*
	* Lock the surface 
	*/
	if (SDL_MUSTLOCK(dst)) {
		if (SDL_LockSurface(dst) < 0) {
			return (-1);
		}
	}

	/*
	* Setup color 
	*/
	alpha = color & 0x000000ff;
	mcolor =
		SDL_MapRGBA(dst->format, (color & 0xff000000) >> 24,
		(color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, alpha);

	/*
	* Draw 
	*/
	result = _putPixelAlpha(dst, x, y, mcolor, alpha);

	/*
	* Unlock the surface 
	*/
	if (SDL_MUSTLOCK(dst)) {
		SDL_UnlockSurface(dst);
	}

	return (result);
}

/*!
\brief Pixel draw with blending enabled if a<255 - no surface locking.

\param dst The surface to draw on.
\param x X (horizontal) coordinate of the pixel.
\param y Y (vertical) coordinate of the pixel.
\param color The color value of the pixel to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
int pixelColorNolock(SDL_Surface * dst, Sint16 x, Sint16 y, Uint32 color)
{
	Uint8 alpha;
	Uint32 mcolor;
	int result = 0;

	/*
	* Setup color 
	*/
	alpha = color & 0x000000ff;
	mcolor =
		SDL_MapRGBA(dst->format, (color & 0xff000000) >> 24,
		(color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, alpha);

	/*
	* Draw 
	*/
	result = _putPixelAlpha(dst, x, y, mcolor, alpha);

	return (result);
}


/*!
\brief Internal function to draw filled rectangle with alpha blending.

Assumes color is in destination format.

\param dst The surface to draw on.
\param x1 X coordinate of the first corner (upper left) of the rectangle.
\param y1 Y coordinate of the first corner (upper left) of the rectangle.
\param x2 X coordinate of the second corner (lower right) of the rectangle.
\param y2 Y coordinate of the second corner (lower right) of the rectangle.
\param color The color value of the rectangle to draw (0xRRGGBBAA). 
\param alpha Alpha blending amount for pixels.

\returns Returns 0 on success, -1 on failure.
*/
int _filledRectAlpha(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color, Uint8 alpha)
{
	SDL_PixelFormat *format;
	Uint32 Rmask, Bmask, Gmask, Amask;
	Uint32 Rshift, Bshift, Gshift, Ashift;
	Uint8 sR, sG, sB, sA;
	Uint32 R, G, B, A;
	Sint16 x, y;

	format = dst->format;
	switch (format->BytesPerPixel) {
	case 1:
		{			/* Assuming 8-bpp */
			Uint8 *row, *pixel;
			Uint8 dR, dG, dB;
			SDL_Palette *palette = format->palette;
			SDL_Color *colors = palette->colors;
			sR = colors[color].r;
			sG = colors[color].g;
			sB = colors[color].b;

			for (y = y1; y <= y2; y++) {
				row = (Uint8 *) dst->pixels + y * dst->pitch;
				for (x = x1; x <= x2; x++) {
					pixel = row + x;

					dR = colors[*pixel].r;
					dG = colors[*pixel].g;
					dB = colors[*pixel].b;

					dR = dR + ((sR - dR) * alpha >> 8);
					dG = dG + ((sG - dG) * alpha >> 8);
					dB = dB + ((sB - dB) * alpha >> 8);

					*pixel = SDL_MapRGB(format, dR, dG, dB);
				}
			}
		}
		break;

	case 2:
		{			/* Probably 15-bpp or 16-bpp */
			Uint16 *row, *pixel;
			Uint32 dR, dG, dB, dA;
			Rmask = format->Rmask;
			Gmask = format->Gmask;
			Bmask = format->Bmask;
			Amask = format->Amask;

			dR = (color & Rmask); 
			dG = (color & Gmask);
			dB = (color & Bmask);
			dA = (color & Amask);

			for (y = y1; y <= y2; y++) {
				row = (Uint16 *) dst->pixels + y * dst->pitch / 2;
				for (x = x1; x <= x2; x++) {
					pixel = row + x;

					R = ((*pixel & Rmask) + ((dR - (*pixel & Rmask)) * alpha >> 8)) & Rmask;
					G = ((*pixel & Gmask) + ((dG - (*pixel & Gmask)) * alpha >> 8)) & Gmask;
					B = ((*pixel & Bmask) + ((dB - (*pixel & Bmask)) * alpha >> 8)) & Bmask;
					*pixel = R | G | B;
					if (Amask!=0)
					{
						A = ((*pixel & Amask) + ((dA - (*pixel & Amask)) * alpha >> 8)) & Amask;
						*pixel |= A;
					} 
				}
			}
		}
		break;

	case 3:
		{	
			/* Slow 24-bpp mode. This is patched on the pygame end because the
			 * original code failed on big endian */
			Uint8 *row, *pix;
			Uint8 *dR, *dG, *dB;
			Uint8 sR, sG, sB;

			SDL_GetRGB(color, format, &sR, &sG, &sB);
			for (y = y1; y <= y2; y++) {
				row = (Uint8 *) dst->pixels + y * dst->pitch;
				for (x = x1; x <= x2; x++) {
					pix = row + x * 3;

#if (SDL_BYTEORDER == SDL_LIL_ENDIAN)
            		dR = pix + (format->Rshift >> 3);
            		dG = pix + (format->Gshift >> 3);
            		dB = pix + (format->Bshift >> 3);
#else
            		dR = pix + 2 - (format->Rshift >> 3);
            		dG = pix + 2 - (format->Gshift >> 3);
            		dB = pix + 2 - (format->Bshift >> 3);
#endif

					*dR = *dR + ((sR - *dR) * alpha >> 8);
					*dG = *dG + ((sG - *dG) * alpha >> 8);
					*dB = *dB + ((sB - *dB) * alpha >> 8);
				}
			}
		}
		break;

#ifdef DEFAULT_ALPHA_PIXEL_ROUTINE
	case 4:
		{			/* Probably :-) 32-bpp */
			Uint32 *row, *pixel;
			Uint32 dR, dG, dB, dA;

			Rmask = format->Rmask;
			Gmask = format->Gmask;
			Bmask = format->Bmask;
			Amask = format->Amask;

			Rshift = format->Rshift;
			Gshift = format->Gshift;
			Bshift = format->Bshift;
			Ashift = format->Ashift;

			dR = (color & Rmask);
			dG = (color & Gmask);
			dB = (color & Bmask);
			dA = (color & Amask);

			for (y = y1; y <= y2; y++) {
				row = (Uint32 *) dst->pixels + y * dst->pitch / 4;
				for (x = x1; x <= x2; x++) {
					pixel = row + x;

					R = ((*pixel & Rmask) + ((((dR - (*pixel & Rmask)) >> Rshift) * alpha >> 8) << Rshift)) & Rmask;
					G = ((*pixel & Gmask) + ((((dG - (*pixel & Gmask)) >> Gshift) * alpha >> 8) << Gshift)) & Gmask;
					B = ((*pixel & Bmask) + ((((dB - (*pixel & Bmask)) >> Bshift) * alpha >> 8) << Bshift)) & Bmask;
					*pixel = R | G | B;
					if (Amask!=0)
					{
						A = ((*pixel & Amask) + ((((dA - (*pixel & Amask)) >> Ashift) * alpha >> 8) << Ashift)) & Amask;
						*pixel |= A;
					}					
				}
			}
		}
		break;
#endif

#ifdef EXPERIMENTAL_ALPHA_PIXEL_ROUTINE
	case 4:{			/* Probably :-) 32-bpp */
		Uint32 *row, *pixel;
		Uint32 dR, dG, dB, dA;
		Uint32 dc;
		Uint32 surfaceAlpha, preMultR, preMultG, preMultB;
		Uint32 aTmp;

		Rmask = format->Rmask;
		Gmask = format->Gmask;
		Bmask = format->Bmask;
		Amask = format->Amask;

		dR = (color & Rmask);
		dG = (color & Gmask);
		dB = (color & Bmask);
		dA = (color & Amask);

		Rshift = format->Rshift;
		Gshift = format->Gshift;
		Bshift = format->Bshift;
		Ashift = format->Ashift;

		preMultR = (alpha * (dR >> Rshift));
		preMultG = (alpha * (dG >> Gshift));
		preMultB = (alpha * (dB >> Bshift));

		for (y = y1; y <= y2; y++) {
			row = (Uint32 *) dst->pixels + y * dst->pitch / 4;
			for (x = x1; x <= x2; x++) {
				pixel = row + x;
				dc = *pixel;

				surfaceAlpha = ((dc & Amask) >> Ashift);
				aTmp = (255 - alpha);
				if (A = 255 - ((aTmp * (255 - surfaceAlpha)) >> 8 )) {
					aTmp *= surfaceAlpha;
					R = (preMultR + ((aTmp * ((dc & Rmask) >> Rshift)) >> 8)) / A << Rshift & Rmask;
					G = (preMultG + ((aTmp * ((dc & Gmask) >> Gshift)) >> 8)) / A << Gshift & Gmask;
					B = (preMultB + ((aTmp * ((dc & Bmask) >> Bshift)) >> 8)) / A << Bshift & Bmask;
				}
				*pixel = R | G | B | (A << Ashift & Amask);

			}
		}
		   }
		   break;
#endif

	}

	return (0);
}

/*!
\brief Draw filled rectangle of RGBA color with alpha blending.

\param dst The surface to draw on.
\param x1 X coordinate of the first corner (upper left) of the rectangle.
\param y1 Y coordinate of the first corner (upper left) of the rectangle.
\param x2 X coordinate of the second corner (lower right) of the rectangle.
\param y2 Y coordinate of the second corner (lower right) of the rectangle.
\param color The color value of the rectangle to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
int filledRectAlpha(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color)
{
	Uint8 alpha;
	Uint32 mcolor;
	int result = 0;

	/*
	* Lock the surface 
	*/
	if (SDL_MUSTLOCK(dst)) {
		if (SDL_LockSurface(dst) < 0) {
			return (-1);
		}
	}

	/*
	* Setup color 
	*/
	alpha = color & 0x000000ff;
	mcolor =
		SDL_MapRGBA(dst->format, (color & 0xff000000) >> 24,
		(color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, alpha);

	/*
	* Draw 
	*/
	result = _filledRectAlpha(dst, x1, y1, x2, y2, mcolor, alpha);

	/*
	* Unlock the surface 
	*/
	if (SDL_MUSTLOCK(dst)) {
		SDL_UnlockSurface(dst);
	}

	return (result);
}

/*!
\brief Internal function to draw horizontal line of RGBA color with alpha blending.

\param dst The surface to draw on.
\param x1 X coordinate of the first point (i.e. left) of the line.
\param x2 X coordinate of the second point (i.e. right) of the line.
\param y Y coordinate of the points of the line.
\param color The color value of the line to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
int _HLineAlpha(SDL_Surface * dst, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color)
{
	return (filledRectAlpha(dst, x1, y, x2, y, color));
}

/*!
\brief Internal function to draw vertical line of RGBA color with alpha blending.

\param dst The surface to draw on.
\param x X coordinate of the points of the line.
\param y1 Y coordinate of the first point (top) of the line.
\param y2 Y coordinate of the second point (bottom) of the line.
\param color The color value of the line to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
int _VLineAlpha(SDL_Surface * dst, Sint16 x, Sint16 y1, Sint16 y2, Uint32 color)
{
	return (filledRectAlpha(dst, x, y1, x, y2, color));
}

/*!
\brief Pixel draw with blending enabled and using alpha weight on color.

\param dst The surface to draw on.
\param x The horizontal coordinate of the pixel.
\param y The vertical position of the pixel.
\param color The color value of the pixel to draw (0xRRGGBBAA). 
\param weight The weight multiplied into the alpha value of the pixel.

\returns Returns 0 on success, -1 on failure.
*/
int pixelColorWeight(SDL_Surface * dst, Sint16 x, Sint16 y, Uint32 color, Uint32 weight)
{
	Uint32 a;

	/*
	* Get alpha 
	*/
	a = (color & (Uint32) 0x000000ff);

	/*
	* Modify Alpha by weight 
	*/
	a = ((a * weight) >> 8);

	return (pixelColor(dst, x, y, (color & (Uint32) 0xffffff00) | (Uint32) a));
}

/*!
\brief Pixel draw with blending enabled and using alpha weight on color - no locking.

\param dst The surface to draw on.
\param x The horizontal coordinate of the pixel.
\param y The vertical position of the pixel.
\param color The color value of the pixel to draw (0xRRGGBBAA). 
\param weight The weight multiplied into the alpha value of the pixel.

\returns Returns 0 on success, -1 on failure.
*/
int pixelColorWeightNolock(SDL_Surface * dst, Sint16 x, Sint16 y, Uint32 color, Uint32 weight)
{
	Uint32 a;

	/*
	* Get alpha 
	*/
	a = (color & (Uint32) 0x000000ff);

	/*
	* Modify Alpha by weight 
	*/
	a = ((a * weight) >> 8);

	return (pixelColorNolock(dst, x, y, (color & (Uint32) 0xffffff00) | (Uint32) a));
}

/*!
\brief Pixel draw with blending enabled if a<255.

\param dst The surface to draw on.
\param x X (horizontal) coordinate of the pixel.
\param y Y (vertical) coordinate of the pixel.
\param r The red color value of the pixel to draw. 
\param g The green color value of the pixel to draw.
\param b The blue color value of the pixel to draw.
\param a The alpha value of the pixel to draw.

\returns Returns 0 on success, -1 on failure.
*/
int pixelRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	Uint32 color;

	/*
	* Check Alpha 
	*/
	if (a == 255) {
		/*
		* No alpha blending required 
		*/
		/*
		* Setup color 
		*/
		color = SDL_MapRGBA(dst->format, r, g, b, a);
		/*
		* Draw 
		*/
		return (fastPixelColor(dst, x, y, color));
	} else {
		/*
		* Alpha blending required 
		*/
		/*
		* Draw 
		*/
		return (pixelColor(dst, x, y, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
	}
}


/*!
\brief Draw horizontal line without blending;

Just stores the color value (including the alpha component) without blending.
Only the same number of bits of the destination surface are transferred
from the input color value.

\param dst The surface to draw on.
\param x1 X coordinate of the first point (i.e. left) of the line.
\param x2 X coordinate of the second point (i.e. right) of the line.
\param y Y coordinate of the points of the line.
\param color The color value of the line to draw. 

\returns Returns 0 on success, -1 on failure.
*/
int hlineColorStore(SDL_Surface * dst, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color)
{
	Sint16 left, right, top, bottom;
	Uint8 *pixel, *pixellast;
	int dx;
	int pixx, pixy;
	Sint16 w;
	Sint16 xtmp;
	int result = -1;

	/*
	* Check visibility of clipping rectangle
	*/
	if ((dst->clip_rect.w==0) || (dst->clip_rect.h==0)) {
		return(0);
	}

	/*
	* Swap x1, x2 if required to ensure x1<=x2
	*/
	if (x1 > x2) {
		xtmp = x1;
		x1 = x2;
		x2 = xtmp;
	}

	/*
	* Get clipping boundary and
	* check visibility of hline 
	*/
	left = dst->clip_rect.x;
	if (x2<left) {
		return(0);
	}
	right = dst->clip_rect.x + dst->clip_rect.w - 1;
	if (x1>right) {
		return(0);
	}
	top = dst->clip_rect.y;
	bottom = dst->clip_rect.y + dst->clip_rect.h - 1;
	if ((y<top) || (y>bottom)) {
		return (0);
	}

	/*
	* Clip x 
	*/
	if (x1 < left) {
		x1 = left;
	}
	if (x2 > right) {
		x2 = right;
	}

	/*
	* Calculate width 
	*/
	w = x2 - x1;

	/*
	* Lock the surface 
	*/
	if (SDL_MUSTLOCK(dst)) {
		if (SDL_LockSurface(dst) < 0) {
			return (-1);
		}
	}

	/*
	* More variable setup 
	*/
	dx = w;
	pixx = dst->format->BytesPerPixel;
	pixy = dst->pitch;
	pixel = ((Uint8 *) dst->pixels) + pixx * (int) x1 + pixy * (int) y;

	/*
	* Draw 
	*/
	switch (dst->format->BytesPerPixel) {
	case 1:
		memset(pixel, color, dx+1);
		break;
	case 2:
		pixellast = pixel + dx + dx;
		for (; pixel <= pixellast; pixel += pixx) {
			*(Uint16 *) pixel = color;
		}
		break;
	case 3:
		pixellast = pixel + dx + dx + dx;
		for (; pixel <= pixellast; pixel += pixx) {
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				pixel[0] = (color >> 16) & 0xff;
				pixel[1] = (color >> 8) & 0xff;
				pixel[2] = color & 0xff;
			} else {
				pixel[0] = color & 0xff;
				pixel[1] = (color >> 8) & 0xff;
				pixel[2] = (color >> 16) & 0xff;
			}
		}
		break;
	default:		/* case 4 */
		dx = dx + dx;
		pixellast = pixel + dx + dx;
		for (; pixel <= pixellast; pixel += pixx) {
			*(Uint32 *) pixel = color;
		}
		break;
	}

	/* 
	* Unlock surface 
	*/
	if (SDL_MUSTLOCK(dst)) {
		SDL_UnlockSurface(dst);
	}

	/*
	* Set result code 
	*/
	result = 0;

	return (result);
}

/*!
\brief Draw horizontal line without blending

Just stores the color value (including the alpha component) without blending.
Function should only be used for 32 bit target surfaces.

\param dst The surface to draw on.
\param x1 X coordinate of the first point (i.e. left) of the line.
\param x2 X coordinate of the second point (i.e. right) of the line.
\param y Y coordinate of the points of the line.
\param r The red value of the line to draw. 
\param g The green value of the line to draw. 
\param b The blue value of the line to draw. 
\param a The alpha value of the line to draw. 

\returns Returns 0 on success, -1 on failure.
*/
int hlineRGBAStore(SDL_Surface * dst, Sint16 x1, Sint16 x2, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	/*
	* Draw 
	*/
	return (hlineColorStore(dst, x1, x2, y, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}

/*!
\brief Draw horizontal line with blending.

\param dst The surface to draw on.
\param x1 X coordinate of the first point (i.e. left) of the line.
\param x2 X coordinate of the second point (i.e. right) of the line.
\param y Y coordinate of the points of the line.
\param color The color value of the line to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
int hlineColor(SDL_Surface * dst, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color)
{
	Sint16 left, right, top, bottom;
	Uint8 *pixel, *pixellast;
	int dx;
	int pixx, pixy;
	Sint16 xtmp;
	int result = -1;
	Uint8 *colorptr;
	Uint8 color3[3];

	/*
	* Check visibility of clipping rectangle
	*/
	if ((dst->clip_rect.w==0) || (dst->clip_rect.h==0)) {
		return(0);
	}

	/*
	* Swap x1, x2 if required to ensure x1<=x2
	*/
	if (x1 > x2) {
		xtmp = x1;
		x1 = x2;
		x2 = xtmp;
	}

	/*
	* Get clipping boundary and
	* check visibility of hline 
	*/
	left = dst->clip_rect.x;
	if (x2<left) {
		return(0);
	}
	right = dst->clip_rect.x + dst->clip_rect.w - 1;
	if (x1>right) {
		return(0);
	}
	top = dst->clip_rect.y;
	bottom = dst->clip_rect.y + dst->clip_rect.h - 1;
	if ((y<top) || (y>bottom)) {
		return (0);
	}

	/*
	* Clip x 
	*/
	if (x1 < left) {
		x1 = left;
	}
	if (x2 > right) {
		x2 = right;
	}

	/*
	* Calculate width difference
	*/
	dx = x2 - x1;

	/*
	* Alpha check 
	*/
	if ((color & 255) == 255) {

		/*
		* No alpha-blending required 
		*/

		/*
		* Setup color 
		*/
		colorptr = (Uint8 *) & color;
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
		} else {
			color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
		}

		/*
		* Lock the surface 
		*/
		if (SDL_MUSTLOCK(dst)) {
			if (SDL_LockSurface(dst) < 0) {
				return (-1);
			}
		}

		/*
		* More variable setup 
		*/
		pixx = dst->format->BytesPerPixel;
		pixy = dst->pitch;
		pixel = ((Uint8 *) dst->pixels) + pixx * (int) x1 + pixy * (int) y;

		/*
		* Draw 
		*/
		switch (dst->format->BytesPerPixel) {
	case 1:
		memset(pixel, color, dx + 1);
		break;
	case 2:
		pixellast = pixel + dx + dx;
		for (; pixel <= pixellast; pixel += pixx) {
			*(Uint16 *) pixel = color;
		}
		break;
	case 3:
		pixellast = pixel + dx + dx + dx;
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			color3[0] = (color >> 16) & 0xff;
			color3[1] = (color >> 8) & 0xff;
			color3[2] = color & 0xff;
		} else {
			color3[0] = color & 0xff;
			color3[1] = (color >> 8) & 0xff;
			color3[2] = (color >> 16) & 0xff;
		}
		for (; pixel <= pixellast; pixel += pixx) {
			memcpy(pixel, color3, 3);
		}
		break;
	default:		/* case 4 */
		dx = dx + dx;
		pixellast = pixel + dx + dx;
		for (; pixel <= pixellast; pixel += pixx) {
			*(Uint32 *) pixel = color;
		}
		break;
		}

		/* 
		* Unlock surface 
		*/
		if (SDL_MUSTLOCK(dst)) {
			SDL_UnlockSurface(dst);
		}

		/*
		* Set result code 
		*/
		result = 0;

	} else {

		/*
		* Alpha blending blit 
		*/
		result = _HLineAlpha(dst, x1, x1 + dx, y, color);
	}

	return (result);
}

/*!
\brief Draw horizontal line with blending.

\param dst The surface to draw on.
\param x1 X coordinate of the first point (i.e. left) of the line.
\param x2 X coordinate of the second point (i.e. right) of the line.
\param y Y coordinate of the points of the line.
\param r The red value of the line to draw. 
\param g The green value of the line to draw. 
\param b The blue value of the line to draw. 
\param a The alpha value of the line to draw. 

\returns Returns 0 on success, -1 on failure.
*/
int hlineRGBA(SDL_Surface * dst, Sint16 x1, Sint16 x2, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	/*
	* Draw 
	*/
	return (hlineColor(dst, x1, x2, y, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}

/*!
\brief Draw vertical line with blending.

\param dst The surface to draw on.
\param x X coordinate of the points of the line.
\param y1 Y coordinate of the first point (i.e. top) of the line.
\param y2 Y coordinate of the second point (i.e. bottom) of the line.
\param color The color value of the line to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
int vlineColor(SDL_Surface * dst, Sint16 x, Sint16 y1, Sint16 y2, Uint32 color)
{
	Sint16 left, right, top, bottom;
	Uint8 *pixel, *pixellast;
	int dy;
	int pixx, pixy;
	Sint16 h;
	Sint16 ytmp;
	int result = -1;
	Uint8 *colorptr;

	/*
	* Check visibility of clipping rectangle
	*/
	if ((dst->clip_rect.w==0) || (dst->clip_rect.h==0)) {
		return(0);
	}

	/*
	* Swap y1, y2 if required to ensure y1<=y2
	*/
	if (y1 > y2) {
		ytmp = y1;
		y1 = y2;
		y2 = ytmp;
	}

	/*
	* Get clipping boundary and
	* check visibility of vline 
	*/
	left = dst->clip_rect.x;
	right = dst->clip_rect.x + dst->clip_rect.w - 1;
	if ((x<left) || (x>right)) {
		return (0);
	}    
	top = dst->clip_rect.y;
	if (y2<top) {
		return(0);
	}
	bottom = dst->clip_rect.y + dst->clip_rect.h - 1;
	if (y1>bottom) {
		return(0);
	}

	/*
	* Clip x 
	*/
	if (y1 < top) {
		y1 = top;
	}
	if (y2 > bottom) {
		y2 = bottom;
	}

	/*
	* Calculate height
	*/
	h = y2 - y1;

	/*
	* Alpha check 
	*/
	if ((color & 255) == 255) {

		/*
		* No alpha-blending required 
		*/

		/*
		* Setup color 
		*/
		colorptr = (Uint8 *) & color;
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
		} else {
			color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
		}

		/*
		* Lock the surface 
		*/
		if (SDL_MUSTLOCK(dst)) {
			if (SDL_LockSurface(dst) < 0) {
				return (-1);
			}
		}

		/*
		* More variable setup 
		*/
		dy = h;
		pixx = dst->format->BytesPerPixel;
		pixy = dst->pitch;
		pixel = ((Uint8 *) dst->pixels) + pixx * (int) x + pixy * (int) y1;
		pixellast = pixel + pixy * dy;

		/*
		* Draw 
		*/
		switch (dst->format->BytesPerPixel) {
	case 1:
		for (; pixel <= pixellast; pixel += pixy) {
			*(Uint8 *) pixel = color;
		}
		break;
	case 2:
		for (; pixel <= pixellast; pixel += pixy) {
			*(Uint16 *) pixel = color;
		}
		break;
	case 3:
		for (; pixel <= pixellast; pixel += pixy) {
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				pixel[0] = (color >> 16) & 0xff;
				pixel[1] = (color >> 8) & 0xff;
				pixel[2] = color & 0xff;
			} else {
				pixel[0] = color & 0xff;
				pixel[1] = (color >> 8) & 0xff;
				pixel[2] = (color >> 16) & 0xff;
			}
		}
		break;
	default:		/* case 4 */
		for (; pixel <= pixellast; pixel += pixy) {
			*(Uint32 *) pixel = color;
		}
		break;
		}

		/* Unlock surface */
		if (SDL_MUSTLOCK(dst)) {
			SDL_UnlockSurface(dst);
		}

		/*
		* Set result code 
		*/
		result = 0;

	} else {

		/*
		* Alpha blending blit 
		*/

		result = _VLineAlpha(dst, x, y1, y1 + h, color);

	}

	return (result);
}

/*!
\brief Draw vertical line with blending.

\param dst The surface to draw on.
\param x X coordinate of the points of the line.
\param y1 Y coordinate of the first point (i.e. top) of the line.
\param y2 Y coordinate of the second point (i.e. bottom) of the line.
\param r The red value of the line to draw. 
\param g The green value of the line to draw. 
\param b The blue value of the line to draw. 
\param a The alpha value of the line to draw. 

\returns Returns 0 on success, -1 on failure.
*/
int vlineRGBA(SDL_Surface * dst, Sint16 x, Sint16 y1, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	/*
	* Draw 
	*/
	return (vlineColor(dst, x, y1, y2, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}

/*!
\brief Draw rectangle with blending.

\param dst The surface to draw on.
\param x1 X coordinate of the first point (i.e. top right) of the rectangle.
\param y1 Y coordinate of the first point (i.e. top right) of the rectangle.
\param x2 X coordinate of the second point (i.e. bottom left) of the rectangle.
\param y2 Y coordinate of the second point (i.e. bottom left) of the rectangle.
\param color The color value of the rectangle to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
int rectangleColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color)
{
	int result;
	Sint16 tmp;

	/* Check destination surface */
	if (dst == NULL)
	{
		return -1;
	}

	/*
	* Check visibility of clipping rectangle
	*/
	if ((dst->clip_rect.w==0) || (dst->clip_rect.h==0)) {
		return 0;
	}

	/*
	* Test for special cases of straight lines or single point 
	*/
	if (x1 == x2) {
		if (y1 == y2) {
			return (pixelColor(dst, x1, y1, color));
		} else {
			return (vlineColor(dst, x1, y1, y2, color));
		}
	} else {
		if (y1 == y2) {
			return (hlineColor(dst, x1, x2, y1, color));
		}
	}

	/*
	* Swap x1, x2 if required 
	*/
	if (x1 > x2) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}

	/*
	* Swap y1, y2 if required 
	*/
	if (y1 > y2) {
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	/*
	* Draw rectangle 
	*/
	result = 0;
	result |= hlineColor(dst, x1, x2, y1, color);
	result |= hlineColor(dst, x1, x2, y2, color);
	y1 += 1;
	y2 -= 1;
	if (y1 <= y2) {
		result |= vlineColor(dst, x1, y1, y2, color);
		result |= vlineColor(dst, x2, y1, y2, color);
	}

	return (result);

}

/*!
\brief Draw rectangle with blending.

\param dst The surface to draw on.
\param x1 X coordinate of the first point (i.e. top right) of the rectangle.
\param y1 Y coordinate of the first point (i.e. top right) of the rectangle.
\param x2 X coordinate of the second point (i.e. bottom left) of the rectangle.
\param y2 Y coordinate of the second point (i.e. bottom left) of the rectangle.
\param r The red value of the rectangle to draw. 
\param g The green value of the rectangle to draw. 
\param b The blue value of the rectangle to draw. 
\param a The alpha value of the rectangle to draw. 

\returns Returns 0 on success, -1 on failure.
*/
int rectangleRGBA(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	/*
	* Draw 
	*/
	return (rectangleColor
		(dst, x1, y1, x2, y2, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}

/*
\brief Draw rounded-corner rectangle with blending.

\param dst The surface to draw on.
\param x1 X coordinate of the first point (i.e. top right) of the rectangle.
\param y1 Y coordinate of the first point (i.e. top right) of the rectangle.
\param x2 X coordinate of the second point (i.e. bottom left) of the rectangle.
\param y2 Y coordinate of the second point (i.e. bottom left) of the rectangle.
\param rad The radius of the corner arc.
\param color The color value of the rectangle to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
int roundedRectangleColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint32 color)
{
	int result;
	Sint16 w, h, tmp;
	Sint16 xx1, xx2, yy1, yy2;

	/* 
	* Check destination surface 
	*/
	if (dst == NULL)
	{
		return -1;
	}

	/*
	* Check radius vor valid range
	*/
	if (rad < 0) {
		return -1;
	}

	/*
	* Special case - no rounding
	*/
	if (rad == 0) {
		return rectangleColor(dst, x1, y1, x2, y2, color);
	}

	/*
	* Check visibility of clipping rectangle
	*/
	if ((dst->clip_rect.w==0) || (dst->clip_rect.h==0)) {
		return 0;
	}

	/*
	* Test for special cases of straight lines or single point 
	*/
	if (x1 == x2) {
		if (y1 == y2) {
			return (pixelColor(dst, x1, y1, color));
		} else {
			return (vlineColor(dst, x1, y1, y2, color));
		}
	} else {
		if (y1 == y2) {
			return (hlineColor(dst, x1, x2, y1, color));
		}
	}

	/*
	* Swap x1, x2 if required 
	*/
	if (x1 > x2) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}

	/*
	* Swap y1, y2 if required 
	*/
	if (y1 > y2) {
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	/*
	* Calculate width&height 
	*/
	w = x2 - x1;
	h = y2 - y1;

	/*
	* Maybe adjust radius
	*/
	if ((rad * 2) > w)  
	{
		rad = w / 2;
	}
	if ((rad * 2) > h)
	{
		rad = h / 2;
	}

	/*
	* Draw corners
	*/
	result = 0;
	xx1 = x1 + rad;
	xx2 = x2 - rad;
	yy1 = y1 + rad;
	yy2 = y2 - rad;
	result |= arcColor(dst, xx1, yy1, rad, 180, 270, color);
	result |= arcColor(dst, xx2, yy1, rad, 270, 360, color);
	result |= arcColor(dst, xx1, yy2, rad,  90, 180, color);
	result |= arcColor(dst, xx2, yy2, rad,   0,  90, color);

	/*
	* Draw lines
	*/
	if (xx1 <= xx2) {
		result |= hlineColor(dst, xx1, xx2, y1, color);
		result |= hlineColor(dst, xx1, xx2, y2, color);
	}
	if (yy1 <= yy2) {
		result |= vlineColor(dst, x1, yy1, yy2, color);
		result |= vlineColor(dst, x2, yy1, yy2, color);
	}

	return result;
}

/*!
\brief Draw rounded-corner rectangle with blending.

\param dst The surface to draw on.
\param x1 X coordinate of the first point (i.e. top right) of the rectangle.
\param y1 Y coordinate of the first point (i.e. top right) of the rectangle.
\param x2 X coordinate of the second point (i.e. bottom left) of the rectangle.
\param y2 Y coordinate of the second point (i.e. bottom left) of the rectangle.
\param rad The radius of the corner arc.
\param r The red value of the rectangle to draw. 
\param g The green value of the rectangle to draw. 
\param b The blue value of the rectangle to draw. 
\param a The alpha value of the rectangle to draw. 

\returns Returns 0 on success, -1 on failure.
*/
int roundedRectangleRGBA(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	/*
	* Draw 
	*/
	return (roundedRectangleColor
		(dst, x1, y1, x2, y2, rad, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}

/*!
\brief Draw rounded-corner box (filled rectangle) with blending.

\param dst The surface to draw on.
\param x1 X coordinate of the first point (i.e. top right) of the box.
\param y1 Y coordinate of the first point (i.e. top right) of the box.
\param x2 X coordinate of the second point (i.e. bottom left) of the box.
\param y2 Y coordinate of the second point (i.e. bottom left) of the box.
\param rad The radius of the corner arcs of the box.
\param color The color value of the box to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
int roundedBoxColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint32 color)
{
	int result;
	Sint16 w, h, tmp;
	Sint16 xx1, xx2, yy1, yy2;

	/* 
	* Check destination surface 
	*/
	if (dst == NULL)
	{
		return -1;
	}

	/*
	* Check radius vor valid range
	*/
	if (rad < 0) {
		return -1;
	}

	/*
	* Special case - no rounding
	*/
	if (rad == 0) {
		return rectangleColor(dst, x1, y1, x2, y2, color);
	}

	/*
	* Check visibility of clipping rectangle
	*/
	if ((dst->clip_rect.w==0) || (dst->clip_rect.h==0)) {
		return 0;
	}

	/*
	* Test for special cases of straight lines or single point 
	*/
	if (x1 == x2) {
		if (y1 == y2) {
			return (pixelColor(dst, x1, y1, color));
		} else {
			return (vlineColor(dst, x1, y1, y2, color));
		}
	} else {
		if (y1 == y2) {
			return (hlineColor(dst, x1, x2, y1, color));
		}
	}

	/*
	* Swap x1, x2 if required 
	*/
	if (x1 > x2) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}

	/*
	* Swap y1, y2 if required 
	*/
	if (y1 > y2) {
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	/*
	* Calculate width&height 
	*/
	w = x2 - x1;
	h = y2 - y1;

	/*
	* Maybe adjust radius
	*/
	if ((rad * 2) > w)  
	{
		rad = w / 2;
	}
	if ((rad * 2) > h)
	{
		rad = h / 2;
	}

	/*
	* Draw corners
	*/
	result = 0;
	xx1 = x1 + rad;
	xx2 = x2 - rad;
	yy1 = y1 + rad;
	yy2 = y2 - rad;
	result |= filledPieColor(dst, xx1, yy1, rad, 180, 270, color);
	result |= filledPieColor(dst, xx2, yy1, rad, 270, 360, color);
	result |= filledPieColor(dst, xx1, yy2, rad,  90, 180, color);
	result |= filledPieColor(dst, xx2, yy2, rad,   0,  90, color);

	/*
	* Draw body
	*/
	xx1++;
	xx2--;
	yy1++;
	yy2--;
	if (xx1 <= xx2) {
		result |= boxColor(dst, xx1, y1, xx2, y2, color);
	}
	if (yy1 <= yy2) {
		result |= boxColor(dst, x1, yy1, xx1-1, yy2, color);
		result |= boxColor(dst, xx2+1, yy1, x2, yy2, color);
	}

	return result;
}

/*!
\brief Draw rounded-corner box (filled rectangle) with blending.

\param dst The surface to draw on.
\param x1 X coordinate of the first point (i.e. top right) of the box.
\param y1 Y coordinate of the first point (i.e. top right) of the box.
\param x2 X coordinate of the second point (i.e. bottom left) of the box.
\param y2 Y coordinate of the second point (i.e. bottom left) of the box.
\param rad The radius of the corner arcs of the box.
\param r The red value of the box to draw. 
\param g The green value of the box to draw. 
\param b The blue value of the box to draw. 
\param a The alpha value of the box to draw. 

\returns Returns 0 on success, -1 on failure.
*/
int roundedBoxRGBA(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2,
				   Sint16 y2, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	/*
	* Draw 
	*/
	return (roundedBoxColor
		(dst, x1, y1, x2, y2, rad, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}

/* --------- Clipping routines for line */

/* Clipping based heavily on code from                       */
/* http://www.ncsa.uiuc.edu/Vis/Graphics/src/clipCohSuth.c   */

#define CLIP_LEFT_EDGE   0x1
#define CLIP_RIGHT_EDGE  0x2
#define CLIP_BOTTOM_EDGE 0x4
#define CLIP_TOP_EDGE    0x8
#define CLIP_INSIDE(a)   (!a)
#define CLIP_REJECT(a,b) (a&b)
#define CLIP_ACCEPT(a,b) (!(a|b))

/*!
\brief Internal clip-encoding routine.

Calculates a segement-based clipping encoding for a point against a rectangle.

\param x X coordinate of point.
\param y Y coordinate of point.
\param left X coordinate of left edge of the rectangle.
\param top Y coordinate of top edge of the rectangle.
\param right X coordinate of right edge of the rectangle.
\param bottom Y coordinate of bottom edge of the rectangle.
*/
static int _clipEncode(Sint16 x, Sint16 y, Sint16 left, Sint16 top, Sint16 right, Sint16 bottom)
{
	int code = 0;

	if (x < left) {
		code |= CLIP_LEFT_EDGE;
	} else if (x > right) {
		code |= CLIP_RIGHT_EDGE;
	}
	if (y < top) {
		code |= CLIP_TOP_EDGE;
	} else if (y > bottom) {
		code |= CLIP_BOTTOM_EDGE;
	}
	return code;
}

/*!
\brief Clip line to a the clipping rectangle of a surface.

\param dst Target surface to draw on.
\param x1 Pointer to X coordinate of first point of line.
\param y1 Pointer to Y coordinate of first point of line.
\param x2 Pointer to X coordinate of second point of line.
\param y2 Pointer to Y coordinate of second point of line.
*/
static int _clipLine(SDL_Surface * dst, Sint16 * x1, Sint16 * y1, Sint16 * x2, Sint16 * y2)
{
	Sint16 left, right, top, bottom;
	int code1, code2;
	int draw = 0;
	Sint16 swaptmp;
	float m;

	/*
	* Get clipping boundary 
	*/
	left = dst->clip_rect.x;
	right = dst->clip_rect.x + dst->clip_rect.w - 1;
	top = dst->clip_rect.y;
	bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

	while (1) {
		code1 = _clipEncode(*x1, *y1, left, top, right, bottom);
		code2 = _clipEncode(*x2, *y2, left, top, right, bottom);
		if (CLIP_ACCEPT(code1, code2)) {
			draw = 1;
			break;
		} else if (CLIP_REJECT(code1, code2))
			break;
		else {
			if (CLIP_INSIDE(code1)) {
				swaptmp = *x2;
				*x2 = *x1;
				*x1 = swaptmp;
				swaptmp = *y2;
				*y2 = *y1;
				*y1 = swaptmp;
				swaptmp = code2;
				code2 = code1;
				code1 = swaptmp;
			}
			if (*x2 != *x1) {
				m = (float)(*y2 - *y1) / (float)(*x2 - *x1);
			} else {
				m = 1.0f;
			}
			if (code1 & CLIP_LEFT_EDGE) {
				*y1 += (Sint16) ((left - *x1) * m);
				*x1 = left;
			} else if (code1 & CLIP_RIGHT_EDGE) {
				*y1 += (Sint16) ((right - *x1) * m);
				*x1 = right;
			} else if (code1 & CLIP_BOTTOM_EDGE) {
				if (*x2 != *x1) {
					*x1 += (Sint16) ((bottom - *y1) / m);
				}
				*y1 = bottom;
			} else if (code1 & CLIP_TOP_EDGE) {
				if (*x2 != *x1) {
					*x1 += (Sint16) ((top - *y1) / m);
				}
				*y1 = top;
			}
		}
	}

	return draw;
}

/*!
\brief Draw box (filled rectangle) with blending.

\param dst The surface to draw on.
\param x1 X coordinate of the first point (i.e. top right) of the box.
\param y1 Y coordinate of the first point (i.e. top right) of the box.
\param x2 X coordinate of the second point (i.e. bottom left) of the box.
\param y2 Y coordinate of the second point (i.e. bottom left) of the box.
\param color The color value of the box to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
int boxColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color)
{
	Sint16 left, right, top, bottom;
	Uint8 *pixel, *pixellast;
	int x, dx;
	int dy;
	int pixx, pixy;
	Sint16 w, h, tmp;
	int result;
	Uint8 *colorptr;

	/*
	* Check visibility of clipping rectangle
	*/
	if ((dst->clip_rect.w==0) || (dst->clip_rect.h==0)) {
		return(0);
	}

	/*
	* Order coordinates to ensure that
	* x1<=x2 and y1<=y2 
	*/
	if (x1 > x2) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if (y1 > y2) {
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	/* 
	* Get clipping boundary and 
	* check visibility 
	*/
	left = dst->clip_rect.x;
	if (x2<left) {
		return(0);
	}
	right = dst->clip_rect.x + dst->clip_rect.w - 1;
	if (x1>right) {
		return(0);
	}
	top = dst->clip_rect.y;
	if (y2<top) {
		return(0);
	}
	bottom = dst->clip_rect.y + dst->clip_rect.h - 1;
	if (y1>bottom) {
		return(0);
	}

	/* Clip all points */
	if (x1<left) { 
		x1=left; 
	} else if (x1>right) {
		x1=right;
	}
	if (x2<left) { 
		x2=left; 
	} else if (x2>right) {
		x2=right;
	}
	if (y1<top) { 
		y1=top; 
	} else if (y1>bottom) {
		y1=bottom;
	}
	if (y2<top) { 
		y2=top; 
	} else if (y2>bottom) {
		y2=bottom;
	}

	/*
	* Test for special cases of straight line or single point 
	*/
	if (x1 == x2) {
		if (y1 == y2) {
			return (pixelColor(dst, x1, y1, color));
		} else { 
			return (vlineColor(dst, x1, y1, y2, color));
		}
	}
	if (y1 == y2) {
		return (hlineColor(dst, x1, x2, y1, color));
	}

	/*
	* Calculate width&height 
	*/
	w = x2 - x1;
	h = y2 - y1;

	/*
	* Alpha check 
	*/
	if ((color & 255) == 255) {

		/*
		* No alpha-blending required 
		*/

		/*
		* Setup color 
		*/
		colorptr = (Uint8 *) & color;
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
		} else {
			color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
		}

		/*
		* Lock the surface 
		*/
		if (SDL_MUSTLOCK(dst)) {
			if (SDL_LockSurface(dst) < 0) {
				return (-1);
			}
		}

		/*
		* More variable setup 
		*/
		dx = w;
		dy = h;
		pixx = dst->format->BytesPerPixel;
		pixy = dst->pitch;
		pixel = ((Uint8 *) dst->pixels) + pixx * (int) x1 + pixy * (int) y1;
		pixellast = pixel + pixx * dx + pixy * dy;
		dx++;

		/*
		* Draw 
		*/
		switch (dst->format->BytesPerPixel) {
	case 1:
		for (; pixel <= pixellast; pixel += pixy) {
			memset(pixel, (Uint8) color, dx);
		}
		break;
	case 2:
		pixy -= (pixx * dx);
		for (; pixel <= pixellast; pixel += pixy) {
			for (x = 0; x < dx; x++) {
				*(Uint16*) pixel = color;
				pixel += pixx;
			}
		}
		break;
	case 3:
		pixy -= (pixx * dx);
		for (; pixel <= pixellast; pixel += pixy) {
			for (x = 0; x < dx; x++) {
				if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
					pixel[0] = (color >> 16) & 0xff;
					pixel[1] = (color >> 8) & 0xff;
					pixel[2] = color & 0xff;
				} else {
					pixel[0] = color & 0xff;
					pixel[1] = (color >> 8) & 0xff;
					pixel[2] = (color >> 16) & 0xff;
				}
				pixel += pixx;
			}
		}
		break;
	default:		/* case 4 */
		pixy -= (pixx * dx);
		for (; pixel <= pixellast; pixel += pixy) {
			for (x = 0; x < dx; x++) {
				*(Uint32 *) pixel = color;
				pixel += pixx;
			}
		}
		break;
		}

		/* Unlock surface */
		if (SDL_MUSTLOCK(dst)) {
			SDL_UnlockSurface(dst);
		}

		result = 0;

	} else {

		result = filledRectAlpha(dst, x1, y1, x1 + w, y1 + h, color);

	}

	return (result);
}

/*!
\brief Draw box (filled rectangle) with blending.

\param dst The surface to draw on.
\param x1 X coordinate of the first point (i.e. top right) of the box.
\param y1 Y coordinate of the first point (i.e. top right) of the box.
\param x2 X coordinate of the second point (i.e. bottom left) of the box.
\param y2 Y coordinate of the second point (i.e. bottom left) of the box.
\param r The red value of the box to draw. 
\param g The green value of the box to draw. 
\param b The blue value of the box to draw. 
\param a The alpha value of the box to draw.

\returns Returns 0 on success, -1 on failure.
*/
int boxRGBA(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	/*
	* Draw 
	*/
	return (boxColor(dst, x1, y1, x2, y2, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}

/* ----- Line */

/* Non-alpha line drawing code adapted from routine          */
/* by Pete Shinners, pete@shinners.org                       */
/* Originally from pygame, http://pygame.seul.org            */

#define ABS(a) (((a)<0) ? -(a) : (a))

/*!
\brief Draw line with alpha blending.

\param dst The surface to draw on.
\param x1 X coordinate of the first point of the line.
\param y1 Y coordinate of the first point of the line.
\param x2 X coordinate of the second point of the line.
\param y2 Y coordinate of the second point of the line.
\param color The color value of the line to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
int lineColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color)
{
	int pixx, pixy;
	int x, y;
	int dx, dy;
	int ax, ay;
	int sx, sy;
	int swaptmp;
	Uint8 *pixel;
	Uint8 *colorptr;

	/*
	* Clip line and test if we have to draw 
	*/
	if (!(_clipLine(dst, &x1, &y1, &x2, &y2))) {
		return (0);
	}

	/*
	* Test for special cases of straight lines or single point 
	*/
	if (x1 == x2) {
		if (y1 < y2) {
			return (vlineColor(dst, x1, y1, y2, color));
		} else if (y1 > y2) {
			return (vlineColor(dst, x1, y2, y1, color));
		} else {
			return (pixelColor(dst, x1, y1, color));
		}
	}
	if (y1 == y2) {
		if (x1 < x2) {
			return (hlineColor(dst, x1, x2, y1, color));
		} else if (x1 > x2) {
			return (hlineColor(dst, x2, x1, y1, color));
		}
	}

	/*
	* Variable setup 
	*/
	dx = x2 - x1;
	dy = y2 - y1;
	sx = (dx >= 0) ? 1 : -1;
	sy = (dy >= 0) ? 1 : -1;

	/* Lock surface */
	if (SDL_MUSTLOCK(dst)) {
		if (SDL_LockSurface(dst) < 0) {
			return (-1);
		}
	}

	/*
	* Check for alpha blending 
	*/
	if ((color & 255) == 255) {

		/*
		* No alpha blending - use fast pixel routines 
		*/

		/*
		* Setup color 
		*/
		colorptr = (Uint8 *) & color;
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
		} else {
			color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
		}

		/*
		* More variable setup 
		*/
		dx = sx * dx + 1;
		dy = sy * dy + 1;
		pixx = dst->format->BytesPerPixel;
		pixy = dst->pitch;
		pixel = ((Uint8 *) dst->pixels) + pixx * (int) x1 + pixy * (int) y1;
		pixx *= sx;
		pixy *= sy;
		if (dx < dy) {
			swaptmp = dx;
			dx = dy;
			dy = swaptmp;
			swaptmp = pixx;
			pixx = pixy;
			pixy = swaptmp;
		}

		/*
		* Draw 
		*/
		x = 0;
		y = 0;
		switch (dst->format->BytesPerPixel) {
	case 1:
		for (; x < dx; x++, pixel += pixx) {
			*pixel = color;
			y += dy;
			if (y >= dx) {
				y -= dx;
				pixel += pixy;
			}
		}
		break;
	case 2:
		for (; x < dx; x++, pixel += pixx) {
			*(Uint16 *) pixel = color;
			y += dy;
			if (y >= dx) {
				y -= dx;
				pixel += pixy;
			}
		}
		break;
	case 3:
		for (; x < dx; x++, pixel += pixx) {
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				pixel[0] = (color >> 16) & 0xff;
				pixel[1] = (color >> 8) & 0xff;
				pixel[2] = color & 0xff;
			} else {
				pixel[0] = color & 0xff;
				pixel[1] = (color >> 8) & 0xff;
				pixel[2] = (color >> 16) & 0xff;
			}
			y += dy;
			if (y >= dx) {
				y -= dx;
				pixel += pixy;
			}
		}
		break;
	default:		/* case 4 */
		for (; x < dx; x++, pixel += pixx) {
			*(Uint32 *) pixel = color;
			y += dy;
			if (y >= dx) {
				y -= dx;
				pixel += pixy;
			}
		}
		break;
		}

	} else {

		/*
		* Alpha blending required - use single-pixel blits 
		*/

		ax = ABS(dx) << 1;
		ay = ABS(dy) << 1;
		x = x1;
		y = y1;
		if (ax > ay) {
			int d = ay - (ax >> 1);

			while (x != x2) {
				pixelColorNolock (dst, x, y, color);
				if (d > 0 || (d == 0 && sx == 1)) {
					y += sy;
					d -= ax;
				}
				x += sx;
				d += ay;
			}
		} else {
			int d = ax - (ay >> 1);

			while (y != y2) {
				pixelColorNolock (dst, x, y, color);
				if (d > 0 || ((d == 0) && (sy == 1))) {
					x += sx;
					d -= ay;
				}
				y += sy;
				d += ax;
			}
		}
		pixelColorNolock (dst, x, y, color);

	}

	/* Unlock surface */
	if (SDL_MUSTLOCK(dst)) {
		SDL_UnlockSurface(dst);
	}

	return (0);
}

/*!
\brief Draw line with alpha blending.

\param dst The surface to draw on.
\param x1 X coordinate of the first point of the line.
\param y1 Y coordinate of the first point of the line.
\param x2 X coordinate of the second point of the line.
\param y2 Y coordinate of the second point of the line.
\param r The red value of the line to draw. 
\param g The green value of the line to draw. 
\param b The blue value of the line to draw. 
\param a The alpha value of the line to draw.

\returns Returns 0 on success, -1 on failure.
*/
int lineRGBA(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	/*
	* Draw 
	*/
	return (lineColor(dst, x1, y1, x2, y2, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}


/* ----- Circle */

/*!
\brief Draw circle with blending.

Note: Circle drawing routine is based on an algorithms from the sge library, 
but modified by A. Schiffler for multiple pixel-draw removal and other 
minor speedup changes.

\param dst The surface to draw on.
\param x X coordinate of the center of the circle.
\param y Y coordinate of the center of the circle.
\param rad Radius in pixels of the circle.
\param color The color value of the circle to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
int circleColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rad, Uint32 color)
{
	Sint16 left, right, top, bottom;
	int result;
	Sint16 x1, y1, x2, y2;
	Sint16 cx = 0;
	Sint16 cy = rad;
	Sint16 df = 1 - rad;
	Sint16 d_e = 3;
	Sint16 d_se = -2 * rad + 5;
	Sint16 xpcx, xmcx, xpcy, xmcy;
	Sint16 ypcy, ymcy, ypcx, ymcx;
	Uint8 *colorptr;

	/*
	* Check visibility of clipping rectangle
	*/
	if ((dst->clip_rect.w==0) || (dst->clip_rect.h==0)) {
		return(0);
	}

	/*
	* Sanity check radius 
	*/
	if (rad < 0) {
		return (-1);
	}

	/*
	* Special case for rad=0 - draw a point 
	*/
	if (rad == 0) {
		return (pixelColor(dst, x, y, color));
	}

	/*
	* Get circle and clipping boundary and 
	* test if bounding box of circle is visible 
	*/
	x2 = x + rad;
	left = dst->clip_rect.x;
	if (x2<left) {
		return(0);
	} 
	x1 = x - rad;
	right = dst->clip_rect.x + dst->clip_rect.w - 1;
	if (x1>right) {
		return(0);
	} 
	y2 = y + rad;
	top = dst->clip_rect.y;
	if (y2<top) {
		return(0);
	} 
	y1 = y - rad;
	bottom = dst->clip_rect.y + dst->clip_rect.h - 1;
	if (y1>bottom) {
		return(0);
	} 

	/*
	* Draw circle 
	*/
	result = 0;

	/* Lock surface */
	if (SDL_MUSTLOCK(dst)) {
		if (SDL_LockSurface(dst) < 0) {
			return (-1);
		}
	}

	/*
	* Alpha Check 
	*/
	if ((color & 255) == 255) {

		/*
		* No Alpha - direct memory writes 
		*/

		/*
		* Setup color 
		*/
		colorptr = (Uint8 *) & color;
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
		} else {
			color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
		}

		/*
		* Draw 
		*/
		do {
			ypcy = y + cy;
			ymcy = y - cy;
			if (cx > 0) {
				xpcx = x + cx;
				xmcx = x - cx;
				result |= fastPixelColorNolock(dst, xmcx, ypcy, color);
				result |= fastPixelColorNolock(dst, xpcx, ypcy, color);
				result |= fastPixelColorNolock(dst, xmcx, ymcy, color);
				result |= fastPixelColorNolock(dst, xpcx, ymcy, color);
			} else {
				result |= fastPixelColorNolock(dst, x, ymcy, color);
				result |= fastPixelColorNolock(dst, x, ypcy, color);
			}
			xpcy = x + cy;
			xmcy = x - cy;
			if ((cx > 0) && (cx != cy)) {
				ypcx = y + cx;
				ymcx = y - cx;
				result |= fastPixelColorNolock(dst, xmcy, ypcx, color);
				result |= fastPixelColorNolock(dst, xpcy, ypcx, color);
				result |= fastPixelColorNolock(dst, xmcy, ymcx, color);
				result |= fastPixelColorNolock(dst, xpcy, ymcx, color);
			} else if (cx == 0) {
				result |= fastPixelColorNolock(dst, xmcy, y, color);
				result |= fastPixelColorNolock(dst, xpcy, y, color);
			}
			/*
			* Update 
			*/
			if (df < 0) {
				df += d_e;
				d_e += 2;
				d_se += 2;
			} else {
				df += d_se;
				d_e += 2;
				d_se += 4;
				cy--;
			}
			cx++;
		} while (cx <= cy);

		/*
		* Unlock surface 
		*/
		SDL_UnlockSurface(dst);

	} else {

		/*
		* Using Alpha - blended pixel blits 
		*/

		do {
			/*
			* Draw 
			*/
			ypcy = y + cy;
			ymcy = y - cy;
			if (cx > 0) {
				xpcx = x + cx;
				xmcx = x - cx;
				result |= pixelColorNolock (dst, xmcx, ypcy, color);
				result |= pixelColorNolock (dst, xpcx, ypcy, color);
				result |= pixelColorNolock (dst, xmcx, ymcy, color);
				result |= pixelColorNolock (dst, xpcx, ymcy, color);
			} else {
				result |= pixelColorNolock (dst, x, ymcy, color);
				result |= pixelColorNolock (dst, x, ypcy, color);
			}
			xpcy = x + cy;
			xmcy = x - cy;
			if ((cx > 0) && (cx != cy)) {
				ypcx = y + cx;
				ymcx = y - cx;
				result |= pixelColorNolock (dst, xmcy, ypcx, color);
				result |= pixelColorNolock (dst, xpcy, ypcx, color);
				result |= pixelColorNolock (dst, xmcy, ymcx, color);
				result |= pixelColorNolock (dst, xpcy, ymcx, color);
			} else if (cx == 0) {
				result |= pixelColorNolock (dst, xmcy, y, color);
				result |= pixelColorNolock (dst, xpcy, y, color);
			}
			/*
			* Update 
			*/
			if (df < 0) {
				df += d_e;
				d_e += 2;
				d_se += 2;
			} else {
				df += d_se;
				d_e += 2;
				d_se += 4;
				cy--;
			}
			cx++;
		} while (cx <= cy);

	}				/* Alpha check */

	/* Unlock surface */
	if (SDL_MUSTLOCK(dst)) {
		SDL_UnlockSurface(dst);
	}

	return (result);
}

/*!
\brief Draw circle with blending.

\param dst The surface to draw on.
\param x X coordinate of the center of the circle.
\param y Y coordinate of the center of the circle.
\param rad Radius in pixels of the circle.
\param r The red value of the circle to draw. 
\param g The green value of the circle to draw. 
\param b The blue value of the circle to draw. 
\param a The alpha value of the circle to draw.

\returns Returns 0 on success, -1 on failure.
*/
int circleRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	/*
	* Draw 
	*/
	return (circleColor(dst, x, y, rad, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}

/* ----- Arc */

/*!
\brief Arc with blending.

Note Arc drawing is based on circle algorithm by A. Schiffler and 
written by D. Raber. Calculates which octants arc goes through and 
renders pixels accordingly.

\param dst The surface to draw on.
\param x X coordinate of the center of the arc.
\param y Y coordinate of the center of the arc.
\param rad Radius in pixels of the arc.
\param start Starting radius in degrees of the arc. 0 degrees is down, increasing counterclockwise.
\param end Ending radius in degrees of the arc. 0 degrees is down, increasing counterclockwise.
\param color The color value of the arc to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
int arcColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rad, Sint16 start, Sint16 end, Uint32 color)
{
	Sint16 left, right, top, bottom;
	int result;
	Sint16 x1, y1, x2, y2;
	Sint16 cx = 0;
	Sint16 cy = rad;
	Sint16 df = 1 - rad;
	Sint16 d_e = 3;
	Sint16 d_se = -2 * rad + 5;
	Sint16 xpcx, xmcx, xpcy, xmcy;
	Sint16 ypcy, ymcy, ypcx, ymcx;
	Uint8 *colorptr;
	Uint8 drawoct;
	int startoct, endoct, oct, stopval_start = 0, stopval_end = 0;
	double dstart, dend, temp = 0.;

	/*
	* Check visibility of clipping rectangle
	*/
	if ((dst->clip_rect.w==0) || (dst->clip_rect.h==0)) {
		return(0);
	}

	/*
	* Sanity check radius 
	*/
	if (rad < 0) {
		return (-1);
	}

	/*
	* Special case for rad=0 - draw a point 
	*/
	if (rad == 0) {
		return (pixelColor(dst, x, y, color));
	}

	/*
	* Get arc's circle and clipping boundary and 
	* test if bounding box of circle is visible 
	*/
	x2 = x + rad;
	left = dst->clip_rect.x;
	if (x2<left) {
		return(0);
	} 
	x1 = x - rad;
	right = dst->clip_rect.x + dst->clip_rect.w - 1;
	if (x1>right) {
		return(0);
	} 
	y2 = y + rad;
	top = dst->clip_rect.y;
	if (y2<top) {
		return(0);
	} 
	y1 = y - rad;
	bottom = dst->clip_rect.y + dst->clip_rect.h - 1;
	if (y1>bottom) {
		return(0);
	}  

	// Octant labelling
	//      
	//  \ 5 | 6 /
	//   \  |  /
	//  4 \ | / 7
	//     \|/
	//------+------ +x
	//     /|\
	//  3 / | \ 0
	//   /  |  \
	//  / 2 | 1 \
	//      +y

	// Initially reset bitmask to 0x00000000
	// the set whether or not to keep drawing a given octant.
	// For example: 0x00111100 means we're drawing in octants 2-5
	drawoct = 0; 

	/*
	* Fixup angles
	*/
	start %= 360;
	end %= 360;
	// 0 <= start & end < 360; note that sometimes start > end - if so, arc goes back through 0.
	while (start < 0) start += 360;
	while (end < 0) end += 360;
	start %= 360;
	end %= 360;

	// now, we find which octants we're drawing in.
	startoct = start / 45;
	endoct = end / 45;
	oct = startoct - 1; // we increment as first step in loop

	// stopval_start, stopval_end; 
	// what values of cx to stop at.
	do {
		oct = (oct + 1) % 8;

		if (oct == startoct) {
			// need to compute stopval_start for this octant.  Look at picture above if this is unclear
			dstart = (double)start;
			switch (oct) 
			{
			case 0:
			case 3:
				temp = sin(dstart * M_PI / 180.);
				break;
			case 1:
			case 6:
				temp = cos(dstart * M_PI / 180.);
				break;
			case 2:
			case 5:
				temp = -cos(dstart * M_PI / 180.);
				break;
			case 4:
			case 7:
				temp = -sin(dstart * M_PI / 180.);
				break;
			}
			temp *= rad;
			stopval_start = (int)temp; // always round down.
			// This isn't arbitrary, but requires graph paper to explain well.
			// The basic idea is that we're always changing drawoct after we draw, so we
			// stop immediately after we render the last sensible pixel at x = ((int)temp).

			// and whether to draw in this octant initially
			if (oct % 2) drawoct |= (1 << oct); // this is basically like saying drawoct[oct] = true, if drawoct were a bool array
			else		 drawoct &= 255 - (1 << oct); // this is basically like saying drawoct[oct] = false
		}
		if (oct == endoct) {
			// need to compute stopval_end for this octant
			dend = (double)end;
			switch (oct)
			{
			case 0:
			case 3:
				temp = sin(dend * M_PI / 180);
				break;
			case 1:
			case 6:
				temp = cos(dend * M_PI / 180);
				break;
			case 2:
			case 5:
				temp = -cos(dend * M_PI / 180);
				break;
			case 4:
			case 7:
				temp = -sin(dend * M_PI / 180);
				break;
			}
			temp *= rad;
			stopval_end = (int)temp;

			// and whether to draw in this octant initially
			if (startoct == endoct)	{
				// note:      we start drawing, stop, then start again in this case
				// otherwise: we only draw in this octant, so initialize it to false, it will get set back to true
				if (start > end) {
					// unfortunately, if we're in the same octant and need to draw over the whole circle, 
					// we need to set the rest to true, because the while loop will end at the bottom.
					drawoct = 255;
				} else {
					drawoct &= 255 - (1 << oct);
				}
			} 
			else if (oct % 2) drawoct &= 255 - (1 << oct);
			else			  drawoct |= (1 << oct);
		} else if (oct != startoct) { // already verified that it's != endoct
			drawoct |= (1 << oct); // draw this entire segment
		}
	} while (oct != endoct);

	// so now we have what octants to draw and when to draw them.  all that's left is the actual raster code.

	/* Lock surface */
	if (SDL_MUSTLOCK(dst)) {
		if (SDL_LockSurface(dst) < 0) {
			return (-1);
		}
	}

	/*
	* Draw arc 
	*/
	result = 0;

	/*
	* Alpha Check 
	*/
	if ((color & 255) == 255) {

		/*
		* No Alpha - direct memory writes 
		*/

		/*
		* Setup color 
		*/
		colorptr = (Uint8 *) & color;
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
		} else {
			color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
		}

		/*
		* Draw 
		*/
		do {
			ypcy = y + cy;
			ymcy = y - cy;
			if (cx > 0) {
				xpcx = x + cx;
				xmcx = x - cx;
				// always check if we're drawing a certain octant before adding a pixel to that octant.
				if (drawoct & 4)  result |= fastPixelColorNolock(dst, xmcx, ypcy, color); // drawoct & 4 = 22; drawoct[2]
				if (drawoct & 2)  result |= fastPixelColorNolock(dst, xpcx, ypcy, color);
				if (drawoct & 32) result |= fastPixelColorNolock(dst, xmcx, ymcy, color);
				if (drawoct & 64) result |= fastPixelColorNolock(dst, xpcx, ymcy, color);
			} else {
				if (drawoct & 6)  result |= fastPixelColorNolock(dst, x, ypcy, color); // 4 + 2; drawoct[2] || drawoct[1]
				if (drawoct & 96) result |= fastPixelColorNolock(dst, x, ymcy, color); // 32 + 64
			}

			xpcy = x + cy;
			xmcy = x - cy;
			if (cx > 0 && cx != cy) {
				ypcx = y + cx;
				ymcx = y - cx;
				if (drawoct & 8)   result |= fastPixelColorNolock(dst, xmcy, ypcx, color);
				if (drawoct & 1)   result |= fastPixelColorNolock(dst, xpcy, ypcx, color);
				if (drawoct & 16)  result |= fastPixelColorNolock(dst, xmcy, ymcx, color);
				if (drawoct & 128) result |= fastPixelColorNolock(dst, xpcy, ymcx, color);
			} else if (cx == 0) {
				if (drawoct & 24)  result |= fastPixelColorNolock(dst, xmcy, y, color); // 8 + 16
				if (drawoct & 129) result |= fastPixelColorNolock(dst, xpcy, y, color); // 1 + 128
			}

			/*
			* Update whether we're drawing an octant
			*/
			if (stopval_start == cx) {
				// works like an on-off switch because start & end may be in the same octant.
				if (drawoct & (1 << startoct)) drawoct &= 255 - (1 << startoct);		
				else drawoct |= (1 << startoct);
			}
			if (stopval_end == cx) {
				if (drawoct & (1 << endoct)) drawoct &= 255 - (1 << endoct);
				else drawoct |= (1 << endoct);
			}

			/*
			* Update pixels
			*/
			if (df < 0) {
				df += d_e;
				d_e += 2;
				d_se += 2;
			} else {
				df += d_se;
				d_e += 2;
				d_se += 4;
				cy--;
			}
			cx++;
		} while (cx <= cy);

		/*
		* Unlock surface 
		*/
		SDL_UnlockSurface(dst);

	} else {

		/*
		* Using Alpha - blended pixel blits 
		*/

		do {
			ypcy = y + cy;
			ymcy = y - cy;
			if (cx > 0) {
				xpcx = x + cx;
				xmcx = x - cx;

				// always check if we're drawing a certain octant before adding a pixel to that octant.
				if (drawoct & 4)  result |= pixelColorNolock(dst, xmcx, ypcy, color);
				if (drawoct & 2)  result |= pixelColorNolock(dst, xpcx, ypcy, color);
				if (drawoct & 32) result |= pixelColorNolock(dst, xmcx, ymcy, color);
				if (drawoct & 64) result |= pixelColorNolock(dst, xpcx, ymcy, color);
			} else {
				if (drawoct & 96) result |= pixelColorNolock(dst, x, ymcy, color);
				if (drawoct & 6)  result |= pixelColorNolock(dst, x, ypcy, color);
			}

			xpcy = x + cy;
			xmcy = x - cy;
			if (cx > 0 && cx != cy) {
				ypcx = y + cx;
				ymcx = y - cx;
				if (drawoct & 8)   result |= pixelColorNolock(dst, xmcy, ypcx, color);
				if (drawoct & 1)   result |= pixelColorNolock(dst, xpcy, ypcx, color);
				if (drawoct & 16)  result |= pixelColorNolock(dst, xmcy, ymcx, color);
				if (drawoct & 128) result |= pixelColorNolock(dst, xpcy, ymcx, color);
			} else if (cx == 0) {
				if (drawoct & 24)  result |= pixelColorNolock(dst, xmcy, y, color);
				if (drawoct & 129) result |= pixelColorNolock(dst, xpcy, y, color);
			}

			/*
			* Update whether we're drawing an octant
			*/
			if (stopval_start == cx) {
				// works like an on-off switch.  
				// This is just in case start & end are in the same octant.
				if (drawoct & (1 << startoct)) drawoct &= 255 - (1 << startoct);		
				else						   drawoct |= (1 << startoct);
			}
			if (stopval_end == cx) {
				if (drawoct & (1 << endoct)) drawoct &= 255 - (1 << endoct);
				else						 drawoct |= (1 << endoct);
			}

			/*
			* Update pixels
			*/
			if (df < 0) {
				df += d_e;
				d_e += 2;
				d_se += 2;
			} else {
				df += d_se;
				d_e += 2;
				d_se += 4;
				cy--;
			}
			cx++;
		} while (cx <= cy);

	}				/* Alpha check */

	/* Unlock surface */
	if (SDL_MUSTLOCK(dst)) {
		SDL_UnlockSurface(dst);
	}

	return (result);
}

/*!
\brief Arc with blending.

\param dst The surface to draw on.
\param x X coordinate of the center of the arc.
\param y Y coordinate of the center of the arc.
\param rad Radius in pixels of the arc.
\param start Starting radius in degrees of the arc. 0 degrees is down, increasing counterclockwise.
\param end Ending radius in degrees of the arc. 0 degrees is down, increasing counterclockwise.
\param r The red value of the arc to draw. 
\param g The green value of the arc to draw. 
\param b The blue value of the arc to draw. 
\param a The alpha value of the arc to draw.

\returns Returns 0 on success, -1 on failure.
*/
int arcRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rad, Sint16 start, Sint16 end, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	/*
	* Draw 
	*/
	return (arcColor(dst, x, y, rad, start, end, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}

/* ----- Filled Circle */

/*!
\brief Draw filled circle with blending.

Note: Based on algorithms from sge library with modifications by A. Schiffler for
multiple-hline draw removal and other minor speedup changes.

\param dst The surface to draw on.
\param x X coordinate of the center of the filled circle.
\param y Y coordinate of the center of the filled circle.
\param rad Radius in pixels of the filled circle.
\param color The color value of the filled circle to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
int filledCircleColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rad, Uint32 color)
{
	Sint16 left, right, top, bottom;
	int result;
	Sint16 x1, y1, x2, y2;
	Sint16 cx = 0;
	Sint16 cy = rad;
	Sint16 ocx = (Sint16) 0xffff;
	Sint16 ocy = (Sint16) 0xffff;
	Sint16 df = 1 - rad;
	Sint16 d_e = 3;
	Sint16 d_se = -2 * rad + 5;
	Sint16 xpcx, xmcx, xpcy, xmcy;
	Sint16 ypcy, ymcy, ypcx, ymcx;

	/*
	* Check visibility of clipping rectangle
	*/
	if ((dst->clip_rect.w==0) || (dst->clip_rect.h==0)) {
		return(0);
	}

	/*
	* Sanity check radius 
	*/
	if (rad < 0) {
		return (-1);
	}

	/*
	* Special case for rad=0 - draw a point 
	*/
	if (rad == 0) {
		return (pixelColor(dst, x, y, color));
	}

	/*
	* Get circle and clipping boundary and 
	* test if bounding box of circle is visible 
	*/
	x2 = x + rad;
	left = dst->clip_rect.x;
	if (x2<left) {
		return(0);
	} 
	x1 = x - rad;
	right = dst->clip_rect.x + dst->clip_rect.w - 1;
	if (x1>right) {
		return(0);
	} 
	y2 = y + rad;
	top = dst->clip_rect.y;
	if (y2<top) {
		return(0);
	} 
	y1 = y - rad;
	bottom = dst->clip_rect.y + dst->clip_rect.h - 1;
	if (y1>bottom) {
		return(0);
	} 

	/*
	* Draw 
	*/
	result = 0;
	do {
		xpcx = x + cx;
		xmcx = x - cx;
		xpcy = x + cy;
		xmcy = x - cy;
		if (ocy != cy) {
			if (cy > 0) {
				ypcy = y + cy;
				ymcy = y - cy;
				result |= hlineColor(dst, xmcx, xpcx, ypcy, color);
				result |= hlineColor(dst, xmcx, xpcx, ymcy, color);
			} else {
				result |= hlineColor(dst, xmcx, xpcx, y, color);
			}
			ocy = cy;
		}
		if (ocx != cx) {
			if (cx != cy) {
				if (cx > 0) {
					ypcx = y + cx;
					ymcx = y - cx;
					result |= hlineColor(dst, xmcy, xpcy, ymcx, color);
					result |= hlineColor(dst, xmcy, xpcy, ypcx, color);
				} else {
					result |= hlineColor(dst, xmcy, xpcy, y, color);
				}
			}
			ocx = cx;
		}
		/*
		* Update 
		*/
		if (df < 0) {
			df += d_e;
			d_e += 2;
			d_se += 2;
		} else {
			df += d_se;
			d_e += 2;
			d_se += 4;
			cy--;
		}
		cx++;
	} while (cx <= cy);

	return (result);
}

/*!
\brief Draw filled circle with blending.

\param dst The surface to draw on.
\param x X coordinate of the center of the filled circle.
\param y Y coordinate of the center of the filled circle.
\param rad Radius in pixels of the filled circle.
\param r The red value of the filled circle to draw. 
\param g The green value of the filled circle to draw. 
\param b The blue value of the filled circle to draw. 
\param a The alpha value of the filled circle to draw.

\returns Returns 0 on success, -1 on failure.
*/
int filledCircleRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	/*
	* Draw 
	*/
	return (filledCircleColor
		(dst, x, y, rad, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}

/* ----- Ellipse */

/*!
\brief Draw ellipse with blending.

Note: Based on algorithms from sge library with modifications by A. Schiffler for
multiple-pixel draw removal and other minor speedup changes.

\param dst The surface to draw on.
\param x X coordinate of the center of the ellipse.
\param y Y coordinate of the center of the ellipse.
\param rx Horizontal radius in pixels of the ellipse.
\param ry Vertical radius in pixels of the ellipse.
\param color The color value of the ellipse to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
int ellipseColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color)
{
	Sint16 left, right, top, bottom;
	int result;
	Sint16 x1, y1, x2, y2;
	int ix, iy;
	int h, i, j, k;
	int oh, oi, oj, ok;
	int xmh, xph, ypk, ymk;
	int xmi, xpi, ymj, ypj;
	int xmj, xpj, ymi, ypi;
	int xmk, xpk, ymh, yph;
	Uint8 *colorptr;

	/*
	* Check visibility of clipping rectangle
	*/
	if ((dst->clip_rect.w==0) || (dst->clip_rect.h==0)) {
		return(0);
	}

	/*
	* Sanity check radii 
	*/
	if ((rx < 0) || (ry < 0)) {
		return (-1);
	}

	/*
	* Special case for rx=0 - draw a vline 
	*/
	if (rx == 0) {
		return (vlineColor(dst, x, y - ry, y + ry, color));
	}
	/*
	* Special case for ry=0 - draw a hline 
	*/
	if (ry == 0) {
		return (hlineColor(dst, x - rx, x + rx, y, color));
	}

	/*
	* Get circle and clipping boundary and 
	* test if bounding box of circle is visible 
	*/
	x2 = x + rx;
	left = dst->clip_rect.x;
	if (x2<left) {
		return(0);
	} 
	x1 = x - rx;
	right = dst->clip_rect.x + dst->clip_rect.w - 1;
	if (x1>right) {
		return(0);
	} 
	y2 = y + ry;
	top = dst->clip_rect.y;
	if (y2<top) {
		return(0);
	} 
	y1 = y - ry;
	bottom = dst->clip_rect.y + dst->clip_rect.h - 1;
	if (y1>bottom) {
		return(0);
	} 

	/*
	* Init vars 
	*/
	oh = oi = oj = ok = 0xFFFF;

	/*
	* Draw 
	*/
	result = 0;

	/* Lock surface */
	if (SDL_MUSTLOCK(dst)) {
		if (SDL_LockSurface(dst) < 0) {
			return (-1);
		}
	}

	/*
	* Check alpha 
	*/
	if ((color & 255) == 255) {

		/*
		* No Alpha - direct memory writes 
		*/

		/*
		* Setup color 
		*/
		colorptr = (Uint8 *) & color;
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
		} else {
			color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
		}


		if (rx > ry) {
			ix = 0;
			iy = rx * 64;

			do {
				h = (ix + 32) >> 6;
				i = (iy + 32) >> 6;
				j = (h * ry) / rx;
				k = (i * ry) / rx;

				if (((ok != k) && (oj != k)) || ((oj != j) && (ok != j)) || (k != j)) {
					xph = x + h;
					xmh = x - h;
					if (k > 0) {
						ypk = y + k;
						ymk = y - k;
						result |= fastPixelColorNolock(dst, xmh, ypk, color);
						result |= fastPixelColorNolock(dst, xph, ypk, color);
						result |= fastPixelColorNolock(dst, xmh, ymk, color);
						result |= fastPixelColorNolock(dst, xph, ymk, color);
					} else {
						result |= fastPixelColorNolock(dst, xmh, y, color);
						result |= fastPixelColorNolock(dst, xph, y, color);
					}
					ok = k;
					xpi = x + i;
					xmi = x - i;
					if (j > 0) {
						ypj = y + j;
						ymj = y - j;
						result |= fastPixelColorNolock(dst, xmi, ypj, color);
						result |= fastPixelColorNolock(dst, xpi, ypj, color);
						result |= fastPixelColorNolock(dst, xmi, ymj, color);
						result |= fastPixelColorNolock(dst, xpi, ymj, color);
					} else {
						result |= fastPixelColorNolock(dst, xmi, y, color);
						result |= fastPixelColorNolock(dst, xpi, y, color);
					}
					oj = j;
				}

				ix = ix + iy / rx;
				iy = iy - ix / rx;

			} while (i > h);
		} else {
			ix = 0;
			iy = ry * 64;

			do {
				h = (ix + 32) >> 6;
				i = (iy + 32) >> 6;
				j = (h * rx) / ry;
				k = (i * rx) / ry;

				if (((oi != i) && (oh != i)) || ((oh != h) && (oi != h) && (i != h))) {
					xmj = x - j;
					xpj = x + j;
					if (i > 0) {
						ypi = y + i;
						ymi = y - i;
						result |= fastPixelColorNolock(dst, xmj, ypi, color);
						result |= fastPixelColorNolock(dst, xpj, ypi, color);
						result |= fastPixelColorNolock(dst, xmj, ymi, color);
						result |= fastPixelColorNolock(dst, xpj, ymi, color);
					} else {
						result |= fastPixelColorNolock(dst, xmj, y, color);
						result |= fastPixelColorNolock(dst, xpj, y, color);
					}
					oi = i;
					xmk = x - k;
					xpk = x + k;
					if (h > 0) {
						yph = y + h;
						ymh = y - h;
						result |= fastPixelColorNolock(dst, xmk, yph, color);
						result |= fastPixelColorNolock(dst, xpk, yph, color);
						result |= fastPixelColorNolock(dst, xmk, ymh, color);
						result |= fastPixelColorNolock(dst, xpk, ymh, color);
					} else {
						result |= fastPixelColorNolock(dst, xmk, y, color);
						result |= fastPixelColorNolock(dst, xpk, y, color);
					}
					oh = h;
				}

				ix = ix + iy / ry;
				iy = iy - ix / ry;

			} while (i > h);
		}

	} else {

		if (rx > ry) {
			ix = 0;
			iy = rx * 64;

			do {
				h = (ix + 32) >> 6;
				i = (iy + 32) >> 6;
				j = (h * ry) / rx;
				k = (i * ry) / rx;

				if (((ok != k) && (oj != k)) || ((oj != j) && (ok != j)) || (k != j)) {
					xph = x + h;
					xmh = x - h;
					if (k > 0) {
						ypk = y + k;
						ymk = y - k;
						result |= pixelColorNolock (dst, xmh, ypk, color);
						result |= pixelColorNolock (dst, xph, ypk, color);
						result |= pixelColorNolock (dst, xmh, ymk, color);
						result |= pixelColorNolock (dst, xph, ymk, color);
					} else {
						result |= pixelColorNolock (dst, xmh, y, color);
						result |= pixelColorNolock (dst, xph, y, color);
					}
					ok = k;
					xpi = x + i;
					xmi = x - i;
					if (j > 0) {
						ypj = y + j;
						ymj = y - j;
						result |= pixelColorNolock (dst, xmi, ypj, color);
						result |= pixelColorNolock (dst, xpi, ypj, color);
						result |= pixelColorNolock (dst, xmi, ymj, color);
						result |= pixelColor(dst, xpi, ymj, color);
					} else {
						result |= pixelColorNolock (dst, xmi, y, color);
						result |= pixelColorNolock (dst, xpi, y, color);
					}
					oj = j;
				}

				ix = ix + iy / rx;
				iy = iy - ix / rx;

			} while (i > h);
		} else {
			ix = 0;
			iy = ry * 64;

			do {
				h = (ix + 32) >> 6;
				i = (iy + 32) >> 6;
				j = (h * rx) / ry;
				k = (i * rx) / ry;

				if (((oi != i) && (oh != i)) || ((oh != h) && (oi != h) && (i != h))) {
					xmj = x - j;
					xpj = x + j;
					if (i > 0) {
						ypi = y + i;
						ymi = y - i;
						result |= pixelColorNolock (dst, xmj, ypi, color);
						result |= pixelColorNolock (dst, xpj, ypi, color);
						result |= pixelColorNolock (dst, xmj, ymi, color);
						result |= pixelColorNolock (dst, xpj, ymi, color);
					} else {
						result |= pixelColorNolock (dst, xmj, y, color);
						result |= pixelColorNolock (dst, xpj, y, color);
					}
					oi = i;
					xmk = x - k;
					xpk = x + k;
					if (h > 0) {
						yph = y + h;
						ymh = y - h;
						result |= pixelColorNolock (dst, xmk, yph, color);
						result |= pixelColorNolock (dst, xpk, yph, color);
						result |= pixelColorNolock (dst, xmk, ymh, color);
						result |= pixelColorNolock (dst, xpk, ymh, color);
					} else {
						result |= pixelColorNolock (dst, xmk, y, color);
						result |= pixelColorNolock (dst, xpk, y, color);
					}
					oh = h;
				}

				ix = ix + iy / ry;
				iy = iy - ix / ry;

			} while (i > h);
		}

	}				/* Alpha check */

	/* Unlock surface */
	if (SDL_MUSTLOCK(dst)) {
		SDL_UnlockSurface(dst);
	}

	return (result);
}

/*!
\brief Draw ellipse with blending.

\param dst The surface to draw on.
\param x X coordinate of the center of the ellipse.
\param y Y coordinate of the center of the ellipse.
\param rx Horizontal radius in pixels of the ellipse.
\param ry Vertical radius in pixels of the ellipse.
\param r The red value of the ellipse to draw. 
\param g The green value of the ellipse to draw. 
\param b The blue value of the ellipse to draw. 
\param a The alpha value of the ellipse to draw.

\returns Returns 0 on success, -1 on failure.
*/
int ellipseRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	/*
	* Draw 
	*/
	return (ellipseColor(dst, x, y, rx, ry, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}

/*!
\brief Draw filled ellipse with blending.

Note: Based on algorithm from sge library with multiple-hline draw removal
and other speedup changes.

\param dst The surface to draw on.
\param x X coordinate of the center of the filled ellipse.
\param y Y coordinate of the center of the filled ellipse.
\param rx Horizontal radius in pixels of the filled ellipse.
\param ry Vertical radius in pixels of the filled ellipse.
\param color The color value of the filled ellipse to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
int filledEllipseColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color)
{
	Sint16 left, right, top, bottom;
	int result;
	Sint16 x1, y1, x2, y2;
	int ix, iy;
	int h, i, j, k;
	int oh, oi, oj, ok;
	int xmh, xph;
	int xmi, xpi;
	int xmj, xpj;
	int xmk, xpk;

	/*
	* Check visibility of clipping rectangle
	*/
	if ((dst->clip_rect.w==0) || (dst->clip_rect.h==0)) {
		return(0);
	}

	/*
	* Sanity check radii 
	*/
	if ((rx < 0) || (ry < 0)) {
		return (-1);
	}

	/*
	* Special case for rx=0 - draw a vline 
	*/
	if (rx == 0) {
		return (vlineColor(dst, x, y - ry, y + ry, color));
	}
	/*
	* Special case for ry=0 - draw a hline 
	*/
	if (ry == 0) {
		return (hlineColor(dst, x - rx, x + rx, y, color));
	}

	/*
	* Get circle and clipping boundary and 
	* test if bounding box of circle is visible 
	*/
	x2 = x + rx;
	left = dst->clip_rect.x;
	if (x2<left) {
		return(0);
	} 
	x1 = x - rx;
	right = dst->clip_rect.x + dst->clip_rect.w - 1;
	if (x1>right) {
		return(0);
	} 
	y2 = y + ry;
	top = dst->clip_rect.y;
	if (y2<top) {
		return(0);
	} 
	y1 = y - ry;
	bottom = dst->clip_rect.y + dst->clip_rect.h - 1;
	if (y1>bottom) {
		return(0);
	} 

	/*
	* Init vars 
	*/
	oh = oi = oj = ok = 0xFFFF;

	/*
	* Draw 
	*/
	result = 0;
	if (rx > ry) {
		ix = 0;
		iy = rx * 64;

		do {
			h = (ix + 32) >> 6;
			i = (iy + 32) >> 6;
			j = (h * ry) / rx;
			k = (i * ry) / rx;

			if ((ok != k) && (oj != k)) {
				xph = x + h;
				xmh = x - h;
				if (k > 0) {
					result |= hlineColor(dst, xmh, xph, y + k, color);
					result |= hlineColor(dst, xmh, xph, y - k, color);
				} else {
					result |= hlineColor(dst, xmh, xph, y, color);
				}
				ok = k;
			}
			if ((oj != j) && (ok != j) && (k != j)) {
				xmi = x - i;
				xpi = x + i;
				if (j > 0) {
					result |= hlineColor(dst, xmi, xpi, y + j, color);
					result |= hlineColor(dst, xmi, xpi, y - j, color);
				} else {
					result |= hlineColor(dst, xmi, xpi, y, color);
				}
				oj = j;
			}

			ix = ix + iy / rx;
			iy = iy - ix / rx;

		} while (i > h);
	} else {
		ix = 0;
		iy = ry * 64;

		do {
			h = (ix + 32) >> 6;
			i = (iy + 32) >> 6;
			j = (h * rx) / ry;
			k = (i * rx) / ry;

			if ((oi != i) && (oh != i)) {
				xmj = x - j;
				xpj = x + j;
				if (i > 0) {
					result |= hlineColor(dst, xmj, xpj, y + i, color);
					result |= hlineColor(dst, xmj, xpj, y - i, color);
				} else {
					result |= hlineColor(dst, xmj, xpj, y, color);
				}
				oi = i;
			}
			if ((oh != h) && (oi != h) && (i != h)) {
				xmk = x - k;
				xpk = x + k;
				if (h > 0) {
					result |= hlineColor(dst, xmk, xpk, y + h, color);
					result |= hlineColor(dst, xmk, xpk, y - h, color);
				} else {
					result |= hlineColor(dst, xmk, xpk, y, color);
				}
				oh = h;
			}

			ix = ix + iy / ry;
			iy = iy - ix / ry;

		} while (i > h);
	}

	return (result);
}

/*!
\brief Draw filled ellipse with blending.

\param dst The surface to draw on.
\param x X coordinate of the center of the filled ellipse.
\param y Y coordinate of the center of the filled ellipse.
\param rx Horizontal radius in pixels of the filled ellipse.
\param ry Vertical radius in pixels of the filled ellipse.
\param r The red value of the filled ellipse to draw. 
\param g The green value of the filled ellipse to draw. 
\param b The blue value of the filled ellipse to draw. 
\param a The alpha value of the filled ellipse to draw.

\returns Returns 0 on success, -1 on failure.
*/
int filledEllipseRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	/*
	* Draw 
	*/
	return (filledEllipseColor
		(dst, x, y, rx, ry, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}
