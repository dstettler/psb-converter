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

	struct CTable
	{
		// String = r + g + b, int = frequency
		std::map<std::string, int> table;
		std::map<std::string, int> shorthands;
		
		// Returns a vector with the contents of table sorted in order by frequency (0 will be highest freq)
		// string = key, int = shorthand
		std::vector<std::pair<std::string, int>> sort();
		
		// Adds pixel value to this table
		void addToTable(Pixel* pix);
	};

	enum class TableMode
	{
		AllShorthand,
		Max2Bytes,
		GreaterThanHalf
	};

	int width;
	int height;
	char shorthandBytes;
	TableMode tableMode;

	CTable imageCTable;

	std::vector<Pixel> pixels;

	int checkForColorInTable(Pixel pix, CTable* table);
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

