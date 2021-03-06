/*
 * \brief   Basic user interface elements
 * \date    2005-10-24
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2005-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _WIDGETS_H_
#define _WIDGETS_H_

#include "elements.h"


class Texture : public Element { };


class Docview : public Parent_element
{
	private:

		Texture *_bg;
		Element *_cont;
		int      _voffset;
		int      _right_pad;
		int      _padx;

	public:

		/**
		 * Constructor
		 */
		explicit Docview(int padx = 7):
			_bg(0), _cont(0), _voffset(0), _right_pad(0), _padx(padx) { }

		/**
		 * Accessor functions
		 */
		Element *content() { return _cont; }

		/**
		 * Define content to be presented in the Docview
		 */
		inline void content(Element *cont)
		{
			_cont = cont;
			_last = _first = 0;
			append(cont);
		}

		inline void voffset(int voffset) { _voffset = voffset; }

		/**
		 * Define background texture
		 */
		inline void texture(Texture  *bg) { _bg = bg; }

		/**
		 * Define right padding
		 */
		inline void right_pad(int pad) { _right_pad = pad; }

		/**
		 * Element interface
		 */
		void     format_fixed_width(int w);
		void     draw(Canvas *c, int x, int y);
		Element *find(int x, int y);
		void     geometry(int x, int y, int w, int h);
};


template <typename PT, int INTENSITY>
class Horizontal_shadow : public Element
{
	public:

		explicit Horizontal_shadow(int height = 8) { _min_h = height; }

		/**
		 * Element interface
		 */
		void draw(Canvas *c, int x, int y);
		Element *find(int x, int y) { return 0; }
		void format_fixed_width(int w) { _min_w = w; }
};


class Generic_icon : public Element
{
	public:

		/**
		 * Request current alpha value
		 */
		virtual int alpha() = 0;

		/**
		 * Define alpha value of the icon
		 */
		virtual void alpha(int alpha) = 0;
};


template <typename PT, int W, int H>
class Icon : public Generic_icon
{
	private:

		PT            _pixel  [H][W];   /* icon pixels in PT pixel format */
		unsigned char _alpha  [H][W];   /* alpha channel of icon pixels   */
		unsigned char _shadow [H][W];   /* shadow calculation buffer      */
		int           _icon_alpha;      /* alpha value of whole icon      */

	public:

		/**
		 * Constructor
		 */
		Icon();

		/**
		 * Define new icon pixels from rgba buffer
		 *
		 * \param vshift  vertical shift of pixels
		 * \param shadow  shadow divisor, low value -> dark shadow
		 *                special case zero -> no shadow
		 *
		 * The buffer must contains W*H pixels. Each pixels consists
		 * of four bytes, red, green, blue, and alpha.
		 */
		void rgba(unsigned char *src, int vshift = 0, int shadow = 4);

		/**
		 * Define icon to be a glow of an rgba image
		 *
		 * \param src  source rgba image to extract the glow's shape from
		 * \param c    glow color
		 */
		void glow(unsigned char *src, Color c);

		/**
		 * Generic_icon interface
		 */
		int alpha() { return _icon_alpha; }
		virtual void alpha(int alpha)
		{
			_icon_alpha = alpha;
			refresh();
		}

		/**
		 * Element interface
		 */
		void draw(Canvas *c, int x, int y);
		Element *find(int x, int y);
};


#endif /* _WIDGETS_H_ */
