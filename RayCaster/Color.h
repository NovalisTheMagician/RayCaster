#ifndef _COLOR_H
#define _COLOR_H

#include <stdint.h>

#define RGB_RED 	0xFF0000
#define RGB_GREEN	0x00FF00
#define RGB_BLUE	0x0000FF
#define RGB_WHITE	0xFFFFFF
#define RGB_BLACK	0x000000
#define RGB_GRAY	0x888888
#define RGB_CYAN	0x00FFFF
#define RGB_YELLOW	0xFFFF00
#define RGB_MAGENTA	0xFF00FF

#pragma pack(1)
struct RGBTriplet
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

struct BMPTriplet
{
	uint8_t blue;
	uint8_t green;
	uint8_t red;
};
#pragma pack(4)

struct color_t
{
	uint8_t a, r, g, b;

	color_t() : a(255), r(0), g(0), b(0) {}
	color_t(uint8_t r, uint8_t g, uint8_t b) : a(255), r(r), g(g), b(b) {}
	color_t(uint8_t a, uint8_t r, uint8_t g, uint8_t b) : a(a), r(r), g(g), b(b) {}
	color_t(int other)
	{
		a = (other >> 24) & 0xFF;
		r = (other >> 16) & 0xFF;
		g = (other >>  8) & 0xFF;
		b = (other >>  0) & 0xFF;
	}
	color_t(const color_t& other)
	{
		a = other.a;
		r = other.r;
		g = other.g;
		b = other.b;
	}

	color_t& operator=(const color_t& rhs)
	{
		if(this == &rhs)
			return *this;
		a = rhs.a;
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		return *this;
	}
	color_t& operator=(int rhs)
	{
		a = (rhs >> 24) & 0xFF;
		r = (rhs >> 16) & 0xFF;
		g = (rhs >>  8) & 0xFF;
		b = (rhs >>  0) & 0xFF;
		return *this;
	}

	operator int() const
	{
		return int((a << 24) | (r << 16) | (g << 8) | b);
	}

	operator RGBTriplet() const
	{
		RGBTriplet rgb = {r, g, b};
		return rgb;
	}

	color_t& operator+=(const color_t& rhs)
	{
		a += rhs.a;
		r += rhs.r;
		g += rhs.g;
		b += rhs.b;
		return *this;
	}

	color_t& operator-=(const color_t& rhs)
	{
		a -= rhs.a;
		r -= rhs.r;
		g -= rhs.g;
		b -= rhs.b;
		return *this;
	}

	color_t& operator*=(float rhs)
	{
		a = int(a * rhs);
		r = int(r * rhs);
		g = int(g * rhs);
		b = int(b * rhs);
		return *this;
	}

	color_t& operator/=(float rhs)
	{
		a = int(a / rhs);
		r = int(r / rhs);
		g = int(g / rhs);
		b = int(b / rhs);
		return *this;
	}
};

color_t operator+(const color_t& rhs, const color_t& lhs)
{
	color_t res;
	res.a = rhs.a + lhs.a;
	res.r = rhs.r + lhs.r;
	res.g = rhs.g + lhs.g;
	res.b = rhs.b + lhs.b;
	return res;
}

color_t operator-(const color_t& rhs, const color_t& lhs)
{
	color_t res;
	res.a = rhs.a - lhs.a;
	res.r = rhs.r - lhs.r;
	res.g = rhs.g - lhs.g;
	res.b = rhs.b - lhs.b;
	return res;
}

color_t operator*(const color_t& rhs, float lhs)
{
	color_t res;
	res.a = int(rhs.a * lhs);
	res.r = int(rhs.r * lhs);
	res.g = int(rhs.g * lhs);
	res.b = int(rhs.b * lhs);
	return res;
}

color_t operator/(const color_t& rhs, float lhs)
{
	color_t res;
	res.a = int(rhs.a / lhs);
	res.r = int(rhs.r / lhs);
	res.g = int(rhs.g / lhs);
	res.b = int(rhs.b / lhs);
	return res;
}

struct ColorHSL
{
	uint8_t h, s, l;

	ColorHSL(uint8_t h, uint8_t s, uint8_t l) : h(h), s(s), l(l) {}
};

color_t HSLtoRGB(const ColorHSL& colorHSL)
{
	float r, g, b, h, s, l; //this function works with floats between 0 and 1
	float temp1, temp2, tempr, tempg, tempb;
	h = colorHSL.h / 255.0f;
	s = colorHSL.s / 255.0f;
	l = colorHSL.l / 255.0f;

	//If saturation is 0, the color is a shade of grey
	if(s == 0) r = g = b = l;
	//If saturation > 0, more complex calculations are needed
	else
	{
		//set the temporary values
		if(l < 0.5f) temp2 = l * (1 + s);
		else temp2 = (l + s) - (l * s);
		temp1 = 2 * l - temp2;
		tempr=h + 1.0f / 3.0f;
		if(tempr > 1.0f) tempr--;
		tempg=h;
		tempb=h-1.0f / 3.0f;
		if(tempb < 0.0f) tempb++;

		//red
		if(tempr < 1.0f / 6.0f) r = temp1 + (temp2 - temp1) * 6.0f * tempr;
		else if(tempr < 0.5f) r = temp2;
		else if(tempr < 2.0f / 3.0f) r = temp1 + (temp2 - temp1) * ((2.0f / 3.0f) - tempr) * 6.0f;
		else r = temp1;

		//green
		if(tempg < 1.0f / 6.0f) g = temp1 + (temp2 - temp1) * 6.0f * tempg;
		else if(tempg < 0.5f) g=temp2;
		else if(tempg < 2.0f / 3.0f) g = temp1 + (temp2 - temp1) * ((2.0f / 3.0f) - tempg) * 6.0f;
		else g = temp1;

		//blue
		if(tempb < 1.0f / 6.0f) b = temp1 + (temp2 - temp1) * 6.0f * tempb;
		else if(tempb < 0.5f) b = temp2;
		else if(tempb < 2.0f / 3.0f) b = temp1 + (temp2 - temp1) * ((2.0f / 3.0f) - tempb) * 6.0f;
		else b = temp1;
	}

	color_t colorRGB;
	colorRGB.r = int(r * 255.0f);
	colorRGB.g = int(g * 255.0f);
	colorRGB.b = int(b * 255.0f);
	return colorRGB;
}

struct ColorHSV
{
	uint8_t h, s, v;

	ColorHSV(uint8_t h, uint8_t s, uint8_t v) : h(h), s(s), v(v) {}
};

color_t HSVtoRGB(const ColorHSV& colorHSV)
{
	float r, g, b, h, s, v; //this function works with floats between 0 and 1
	h = colorHSV.h / 255.0f;
	s = colorHSV.s / 255.0f;
	v = colorHSV.v / 255.0f;

	//if saturation is 0, the color is a shade of grey
	if(s == 0.0f) r = g = b = v;
	//if saturation > 0, more complex calculations are needed
	else
	{
		float f, p, q, t;
		int i;
		h *= 6.0f; //to bring hue to a number between 0 and 6, better for the calculations
		i = int(floor(h)); //e.g. 2.7 becomes 2 and 3.01 becomes 3 or 4.9999 becomes 4
		f = h - i;//the fractional part of h

		p = v * (1.0f - s);
		q = v * (1.0f - (s * f));
		t = v * (1.0f - (s * (1.0f - f)));

		switch(i)
		{
		case 0: r=v; g=t; b=p; break;
		case 1: r=q; g=v; b=p; break;
		case 2: r=p; g=v; b=t; break;
		case 3: r=p; g=q; b=v; break;
		case 4: r=t; g=p; b=v; break;
		case 5: r=v; g=p; b=q; break;
		default: r = g = b = 0; break;
		}
	}
	color_t colorRGB;
	colorRGB.r = int(r * 255.0f);
	colorRGB.g = int(g * 255.0f);
	colorRGB.b = int(b * 255.0f);
	return colorRGB;
}

#endif
