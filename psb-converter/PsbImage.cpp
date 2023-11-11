#include "PsbImage.h"

#include <algorithm>
#include <vector>
#include <string>
#include <charconv>
#include <fstream>

PsbImage::Pixel::Pixel(char r, char g, char b, char a = 255) 
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

PsbImage::Pixel::Pixel()
{
	this->r = 0;
	this->g = 0;
	this->b = 0;
	this->a = 255;
}

bool PsbImage::Pixel::equals(Pixel otherPix)
{
	return r == otherPix.r &&
		g == otherPix.g &&
		b == otherPix.b &&
		a == otherPix.a;
}

void PsbImage::CTable::addToTable(Pixel* pix)
{
	std::string key = std::string() + pix->r + pix->g + pix->b;
	if (table.find(key) != table.end())
	{
		table[key] += 1;
	}
	else
	{
		table.emplace(key, 1);
		shorthands.emplace(key, nextShorthand);
		nextShorthand++;
	}
}

bool cTableSortComp(std::pair<std::string, int> &a, std::pair<std::string, int> &b)
{
	return a.second < b.second;
}

std::vector<std::pair<std::string, int>> PsbImage::CTable::sort()
{
	std::vector<std::pair<std::string, int>> _freqPairs;
	for (auto iter = table.begin(); iter != table.end(); iter++)
		_freqPairs.push_back(std::pair<std::string, int>(iter->first, iter->second));

	// Sort frequency pairs by frequency
	std::sort(_freqPairs.begin(), _freqPairs.end(), cTableSortComp);

	// Convert newly sorted pairs to versions with their newly given shorthand
	std::vector<std::pair<std::string, int>> _shorthandedPairs;
	for (std::pair<std::string, int> pair : _freqPairs)
		_shorthandedPairs.push_back(std::pair<std::string, int>(pair.first, shorthands[pair.first]));

	return _shorthandedPairs;
}

int PsbImage::checkForColorInTable(Pixel pix, CTable* table)
{
	
	std::string key = std::string() + pix.r + pix.g + pix.b;
	if (table->shorthands.find(key) != table->shorthands.end())
	{
		return table->shorthands.find(key)->second;
	}

	return -1;
}

std::list<char> PsbImage::byteListFromInt(int i)
{
	std::list<char> _bytes;
	
	for (int _i = 0; _i < sizeof(i); _i++)
	{
		if (i == 0)
			break;

		char _byte = i & (255 << (sizeof(i) - 1));
		_bytes.push_back(_byte);
		i <<= 4;
	}

	return _bytes;
}

std::list<char> PsbImage::getShorthandBytes(int i, char shorthandBytesCount)
{
	std::list<char> _id = byteListFromInt(i);
	if (_id.size() < shorthandBytesCount)
	{
		int _j = shorthandBytesCount - static_cast<int>(_id.size());
		while (_j > 0)
		{
			_id.push_front(0);
			_j = shorthandBytesCount - static_cast<int>(_id.size());
		}
	}

	return _id;
}

std::list<char> PsbImage::writeHeader()
{
	std::list<char> _headerChars;

	_headerChars.push_back(shorthandBytes);
	std::map<int, Pixel> _generatedTable = generateCTable();
	for (auto iter = _generatedTable.begin(); iter != _generatedTable.end(); ++iter)
	{
		_headerChars.push_back(0);
		for (int i = 0; i < static_cast<int>(shorthandBytes); i++)
		{
			// Add byte list of header 
			_headerChars.splice(_headerChars.end(), getShorthandBytes(iter->first, shorthandBytes));
		}
		_headerChars.push_back(iter->second.r);
		_headerChars.push_back(iter->second.g);
		_headerChars.push_back(iter->second.b);
	}
	// End CTable
	_headerChars.push_back(5);

	_headerChars.splice(_headerChars.end(), byteListFromInt(width));
	_headerChars.push_back(0);
	_headerChars.splice(_headerChars.end(), byteListFromInt(height));
	_headerChars.push_back(5);
	
	return _headerChars;
}

std::list<char> PsbImage::getPixelBytes(Pixel pix, std::map<int, PsbImage::Pixel> table)
{
	std::list<char> _pixelsList;

	int _shorthand = checkForColorInTable(pix, &imageCTable);
	
	if (pix.a != 255)
	{
		// Add alpha bytes
		_pixelsList.push_back(1);
		_pixelsList.push_back(pix.a);
	}
	
	if (_shorthand != -1)
	{
		// Add shorthand bytes
		_pixelsList.push_back(4);
		_pixelsList.splice(_pixelsList.end(), (getShorthandBytes(_shorthand, shorthandBytes)));
	}
	else
	{
		// Add direct rgb bytes
		_pixelsList.push_back(3);
		_pixelsList.push_back(pix.r);
		_pixelsList.push_back(pix.g);
		_pixelsList.push_back(pix.b);
	}

	return _pixelsList;
}

std::map<int, PsbImage::Pixel> PsbImage::generateCTable()
{
	std::vector<std::pair<std::string, int>> sortedTable = imageCTable.sort();
	int _maxIndex = sortedTable.size();
	int _theoreticalMax = std::pow(2, (shorthandBytes * 4));

	if (_maxIndex <= sortedTable.size())
		_maxIndex = _theoreticalMax - 1;

	std::map<int, PsbImage::Pixel> _finalTable;
	for (int i = 0; i < _maxIndex; i++)
	{
		std::string key = sortedTable.at(i).first;
		PsbImage::Pixel pix(key.at(0), key.at(1), key.at(2));
		_finalTable.emplace(sortedTable.at(i).second);
	}

	return _finalTable;
}

void PsbImage::setShorthandBytesCount()
{
	shorthandBytes = 1;
}

std::list<char> PsbImage::writePixels()
{
	// This is the dumb/brute-force 1 byte at a time solution
	int _charLength = DEFAULT_CHAR_LENGTH;
	std::list<char> _pixelsList;

	for (int i = 0; i < pixels.size(); i++)
	{
		char _repetitions = 0;
		for (int j = 0; j < _charLength; j++)
		{
			if (i + j < pixels.size() && pixels.at(i + j).equals(pixels.at(i)))
				_repetitions++;
			else
				break;
		}

		if (_repetitions > 0)
		{
			_pixelsList.push_back(2);
			_pixelsList.push_back(_repetitions);
		}

		_pixelsList.splice(_pixelsList.end(), getPixelBytes(pixels.at(i), generateCTable()));
	}

	return _pixelsList;
}

void PsbImage::addPixel(PsbImage::Pixel pix)
{
	pixels.push_back(pix);
	imageCTable.addToTable(&pix);
}

void PsbImage::writeImageToFile(std::string filename)
{
	std::list<char> _fileBytes;

	setShorthandBytesCount();

	_fileBytes.push_back(69);
	_fileBytes.splice(_fileBytes.end(), writeHeader());
	_fileBytes.splice(_fileBytes.end(), writePixels());

	std::ofstream _file(filename);

	for (char byte : _fileBytes)
	{
		_file.write(&byte, sizeof(byte));
	}

	_file.close();
}

PsbImage::PsbImage(int width, int height, TableMode tableMode)
{
	imageCTable.nextShorthand = 6;
	this->width = width;
	this->height = height;
	this->tableMode = tableMode;
}