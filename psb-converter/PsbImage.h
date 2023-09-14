#pragma once

#include <map>
#include <list>
#include <vector>
#include <string>
#include <utility>

constexpr int DEFAULT_CHAR_LENGTH = 255;

class PsbImage
{
	struct Pixel 
	{
		char r;
		char g;
		char b;
		char a;

		Pixel(char r, char g, char b, char a);
		Pixel();
		bool equals(Pixel otherPix);
	};

	struct CTableValue
	{
		int count;
		Pixel pixelValue;

		CTableValue(Pixel pixelValue, int count);
		CTableValue();
	};

	struct ColorTable
	{
		std::map<int, Pixel> colors;			// colorId: color
		std::map<int, int> colorFrequency;		// colorId: frequency
	};

	enum class TableMode
	{
		AllShorthand,
		Max2Bytes,
		GreaterThanHalf
	};

	std::map<int, CTableValue> cTable;  // cTable_id: [count, pixel_value]
	int nextCTableVal;
	int width;
	int height;
	char shorthandBytes;
	TableMode tableMode;

	std::vector<Pixel> pixels;

	int checkForColorInTable(Pixel pix, std::map<int, PsbImage::Pixel>* table);
	void addColorToTable(Pixel pix);

	std::list<char> byteListFromInt(int i);
	std::list<char> getShorthandBytes(int i, char shorthandBytesCount);
	std::list<char> getPixelBytes(Pixel pix, std::map<int, PsbImage::Pixel> table);

	void setShorthandBytesCount();
	std::map<int, Pixel> generateCTable();
	
	std::list<char> writeHeader();
	std::list<char> writePixels();

public:
	void addPixel(Pixel pix);
	void writeImageToFile(std::string filename);
	PsbImage(int width, int height, TableMode tableMode);
};

