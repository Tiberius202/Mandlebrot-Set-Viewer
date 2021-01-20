#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <cmath>
#include <thread>

class OLCPanZoom : public olc::PixelGameEngine
{
private:

	olc::vd2d offset = { -2.2, -9.0 / 8.0 };
	olc::vi2d panStart;
	olc::vi2d panNow;
	double magni = 1280.0 / 4.0;
	int8_t smoothI = 0;
	uint16_t iterat = 32;

public:
	OLCPanZoom()
	{
		// Name of the application
		sAppName = "Pan and Zoom";
	}

	olc::vi2d ToScreen(const olc::vd2d vect) {
		return { (int)(magni * (vect.x - offset.x)) , (int)(magni * (vect.y - offset.y)) };
	}
	olc::vd2d ToWorld(const olc::vi2d vect) {
		return { vect.x / magni + offset.x, vect.y / magni + offset.y };
	}
	void DrawMand() {
		uint16_t i;
		olc::vd2d z;
		olc::vd2d c;
		for (int r = 0; r < 1280 * 720; r++) {
			i = 0;
			z = { 0.0, 0.0 };
			c = ToWorld({ r % 1280, r / 1280});
			while (i < iterat && z.mag2() < 4) {
				z = { z.x * z.x - z.y * z.y + c.x,  2 * z.x * z.y + c.y };
				i++;
			}
			Draw({ r % 1280, r / 1280 }, RainbowColor(i * 32));
		}
	}
	static olc::Pixel RainbowColor(const uint16_t ite) {
		int i = ite % (256 * 6);
		//Red up Green
		if (i < 256) {
			return olc::Pixel(255, i % 256, 0);
		}
		//Green down Red
		else if (i < 256 * 2) {
			return olc::Pixel(255 - i % 256, 255, 0);
		}
		//Green up Blue
		else if (i < 256 * 3) {
			return olc::Pixel(0, 255, i % 256);
		}
		//Blue down Green
		else if (i < 256 * 4) {
			return olc::Pixel(0, 255 - i % 256, 255);
		}
		//Blue up Red
		else if (i < 256 * 5) {
			return olc::Pixel(i % 256, 0, 255);
		}
		//Red down Blue
		else {
			return olc::Pixel(255, 0, 255 - i % 256);
		}
	}
protected:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		const int32_t wheel = GetMouseWheel();
		//keyboard controls
		if (GetKey(olc::Key::R).bPressed) {
			magni = 1280.0 / 4.0;
			offset = { -2.2, -9.0 / 8.0 };
			iterat = 32;
			smoothI = 0;
		}
		else if (GetKey(olc::Key::LEFT).bHeld) {
			smoothI = -8;
		}
		else if (GetKey(olc::Key::RIGHT).bHeld) {
			smoothI = 8;
		}
		else if (GetKey(olc::Key::DOWN).bHeld) {
			olc::vd2d tempLoc = ToWorld({ GetMouseX(), GetMouseY() });
			magni /= 1.1;
			offset -= ToWorld({ GetMouseX(),GetMouseY() }) - tempLoc;
		}
		else if (GetKey(olc::Key::UP).bHeld) {
			olc::vd2d tempLoc = ToWorld({ GetMouseX(), GetMouseY() });
			magni *= 1.1;
			offset -= ToWorld({ GetMouseX(),GetMouseY() }) - tempLoc;
		}
		//mouse
		if (wheel) {
			olc::vd2d tempLoc = ToWorld({ GetMouseX(), GetMouseY() });
			if (GetKey(olc::Key::SHIFT).bHeld) {
				magni *= std::pow(1.001, wheel);
			}
			else if (GetKey(olc::Key::CTRL).bHeld) {
				if (wheel > 0) {
					smoothI++;
				}
				else {
					smoothI--;
				}
			}
			else {
				magni *= std::pow(1.0001, wheel);
			}
			offset -= ToWorld({ GetMouseX(),GetMouseY() }) - tempLoc;
		}
		if (GetMouse(0).bPressed) {
			panStart = { GetMouseX(), GetMouseY() };
		}
		else if (GetMouse(0).bHeld) {
			panNow = { GetMouseX(), GetMouseY() };
			offset -= ToWorld(panNow) - ToWorld(panStart);
			panStart = panNow;
		}
		//variables
		if (smoothI > 0) {
			iterat = std::ceil(iterat * (65.0 / 64.0));
			smoothI--;
		}
		else if (smoothI < 0 && iterat > 2) {
			iterat = std::floor(iterat * (64.0 / 65.0));
			smoothI++;
		}
		DrawMand();
		DrawStringDecal({ 50, 50 }, std::to_string(fElapsedTime));
		DrawStringDecal({ 50, 70 }, std::to_string(offset.x) + ", " + std::to_string(offset.y));
		DrawStringDecal({ 50, 90 }, std::to_string(magni));
		DrawStringDecal({ 50, 110 }, std::to_string(iterat));
		return true;
	}
};

int main()
{
	OLCPanZoom mand;
	if (mand.Construct(1280, 720, 1, 1))
		mand.Start();
	return 0;
}