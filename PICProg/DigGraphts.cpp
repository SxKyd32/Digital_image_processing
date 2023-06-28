// DigGraphts.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <cstdlib>
#include <math.h>
#include <queue>

#include "GraphMsg.h"
#include "Menu.h"

double PI = 3.1415926;
int record = 0;

using namespace std;

//载入图片
void loadBmpImageInfo(const char* fileName)
{
	//读取bmp文件
	FILE* fileInput;
	fileInput = fopen(fileName, "rb");
	if (fileInput != NULL)
	{
		WORD bfType;
		fread(&bfType, 1, sizeof(WORD), fileInput);

		//读取bmp文件的文件头和信息头
		fread(&bmpFileHeader, 1, sizeof(tagBITMAPFILEHEADER), fileInput);
		//showBmpHead(bmpFileHeader);
		fread(&bmpInfoHeader, 1, sizeof(tagBITMAPINFOHEADER), fileInput);
		//showBmpInforHead(bmpInfoHeader);

		if (bmpInfoHeader.biClrUsed > 0)
			RGBPalette = new RGBQUAD[bmpInfoHeader.biClrUsed];

		// 读取调色板
		for (int i = 0; i < bmpInfoHeader.biClrUsed; i++)
		{
			fread((char*)&(RGBPalette[i].rgbBlue), 1, sizeof(BYTE), fileInput);
			fread((char*)&(RGBPalette[i].rgbGreen), 1, sizeof(BYTE), fileInput);
			fread((char*)&(RGBPalette[i].rgbRed), 1, sizeof(BYTE), fileInput);
			fread((char*)&(RGBPalette[i].rgbReserved), 1, sizeof(BYTE), fileInput);
		}
		width = bmpInfoHeader.biWidth;
		height = bmpInfoHeader.biHeight;

		//图像每一行的字节数必须是4的整数倍
		width = (width * bmpInfoHeader.biBitCount / 8 + 3) / 4 * 4;

		//初始化图像数据
		imageData = new IMAGEDATA * [height];
		for (int i = 0; i < height; i++)
		{
			imageData[i] = new IMAGEDATA[bmpInfoHeader.biWidth];
		}
		//空的字节
		emptyBytes = new BYTE * [height];
		for (int i = 0; i < height; i++)
		{
			emptyBytes[i] = new BYTE[width - bmpInfoHeader.biWidth * bmpInfoHeader.biBitCount / 8];
		}

		//初始化原始图片的像素数组为读出图片的像素数据
		if (24 == bmpInfoHeader.biBitCount)
		{
			for (int i = 0; i < height; ++i)
			{
				for (int j = 0; j < bmpInfoHeader.biWidth; ++j)//前bmpInfoHeader.biWidth * 3个字节存放RGB颜色值，后面几个（小于4个）单独的字节空出来
				{
					imageData[i][j].blue = 0;
					imageData[i][j].green = 0;
					imageData[i][j].red = 0;
				}
				for (int j = bmpInfoHeader.biWidth * 3; j < width; ++j)
				{
					emptyBytes[i][j - bmpInfoHeader.biWidth * 3] = 0;
				}
			}
			for (int i = 0; i < height; ++i)
			{
				for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
				{
					fread((char*)&imageData[i][j].blue, sizeof(BYTE), 1, fileInput);
					fread((char*)&imageData[i][j].green, sizeof(BYTE), 1, fileInput);
					fread((char*)&imageData[i][j].red, sizeof(BYTE), 1, fileInput);
				}
				for (int j = bmpInfoHeader.biWidth * 3; j < width; ++j)
				{
					fread((char*)&emptyBytes[i][j - bmpInfoHeader.biWidth * 3], sizeof(BYTE), 1, fileInput);
				}
			}
		}
		else if (8 == bmpInfoHeader.biBitCount)
		{
			for (int i = 0; i < height; ++i)
			{
				for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
				{
					imageData[i][j].blue = 0;
				}
				for (int j = bmpInfoHeader.biWidth; j < width; ++j)
				{
					emptyBytes[i][j - bmpInfoHeader.biWidth] = 0;
				}
			}
			for (int i = 0; i < height; ++i)
			{
				for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
				{
					fread((char*)&imageData[i][j].blue, sizeof(BYTE), 1, fileInput);
					//fread((char *)&imageData[i][j].green, sizeof(BYTE), 1, fpi);
					//fread((char *)&imageData[i][j].red, sizeof(BYTE), 1, fpi);
				}
				for (int j = bmpInfoHeader.biWidth; j < width; ++j)
				{
					fread((char*)&emptyBytes[i][j - bmpInfoHeader.biWidth], sizeof(BYTE), 1, fileInput);
				}
			}
		}
		cout << "图片读取成功" << endl;

		fclose(fileInput);
	}
	else
	{
		cout << "file open error!" << endl;
		return;
	}
}

//保存24位图
void saveBmpImage(const char* fileName)
{
	//保存图片信息写入bmp文件
	FILE* fpw;
	if ((fpw = fopen(fileName, "wb")) == NULL)
	{
		cout << "create the bmp file error!" << endl;
		return;
	}
	WORD bfType_w = 0x4d42;
	fwrite(&bfType_w, 1, sizeof(WORD), fpw);

	//bmpInfoHeader.biSizeImage = width * bmpInfoHeader.biHeight;

	//写入文件头和信息头
	fwrite(&bmpFileHeader, 1, sizeof(tagBITMAPFILEHEADER), fpw);
	fwrite(&bmpInfoHeader, 1, sizeof(tagBITMAPINFOHEADER), fpw);

	//保存调色板数据
	for (unsigned int i = 0; i < bmpInfoHeader.biClrUsed; i++)
	{
		fwrite(&RGBPalette[i].rgbBlue, 1, sizeof(BYTE), fpw);
		fwrite(&RGBPalette[i].rgbGreen, 1, sizeof(BYTE), fpw);
		fwrite(&RGBPalette[i].rgbRed, 1, sizeof(BYTE), fpw);
		fwrite(&(RGBPalette[i].rgbReserved), 1, sizeof(BYTE), fpw);
	}
	//保存像素数据
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
		{
			fwrite(&imageData[i][j].blue, 1, sizeof(BYTE), fpw);
			fwrite(&imageData[i][j].green, 1, sizeof(BYTE), fpw);
			fwrite(&imageData[i][j].red, 1, sizeof(BYTE), fpw);
			//fwrite(&(RGBPalette[i].rgbReserved), 1, sizeof(BYTE), fpw);
		}
		for (int j = bmpInfoHeader.biWidth * 3; j < width; ++j)
		{
			fwrite(&emptyBytes[i][j - bmpInfoHeader.biWidth * 3], sizeof(BYTE), 1, fpw);
		}
	}
	cout << "图片保存成功" << endl;
	fclose(fpw);
}

//保存8位图
void save8BitBmpImage(const char* fileName)
{
	//保存图片信息写入bmp文件
	FILE* fileOutput;
	if ((fileOutput = fopen(fileName, "wb")) == NULL)
	{
		cout << "create the bmp file error!" << endl;
		return;
	}
	WORD bfType_w = 0x4d42;
	fwrite(&bfType_w, 1, sizeof(WORD), fileOutput);

	bmpInfoHeader.biBitCount = 8;
	width = (bmpInfoHeader.biWidth + 3) / 4 * 4;
	bmpInfoHeader.biSizeImage = width * bmpInfoHeader.biHeight;
	bmpInfoHeader.biClrUsed = 256;
	for (int i = 0; i < height; i++)
	{
		emptyBytes[i] = new BYTE[width - bmpInfoHeader.biWidth];
	}

	//加入调色板，要修改bmpInfoHeader,修改bmpFileHeader
	bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * 4L; //加调色板
	bmpFileHeader.bfSize = bmpFileHeader.bfOffBits + bmpInfoHeader.biSizeImage;

	//写入文件头和信息头
	fwrite(&bmpFileHeader, 1, sizeof(BITMAPFILEHEADER), fileOutput);
	fwrite(&bmpInfoHeader, 1, sizeof(BITMAPINFOHEADER), fileOutput);

	//初始化调色板
	RGBPalette = new RGBQUAD[256];
	for (int i = 0; i < 256; i++)
	{
		RGBPalette[i].rgbBlue = i;
		RGBPalette[i].rgbGreen = i;
		RGBPalette[i].rgbRed = i;
		RGBPalette[i].rgbReserved = i;
	}
	//保存调色板数据
	for (unsigned int i = 0; i < 256; i++)
	{
		fwrite(&RGBPalette[i].rgbBlue, 1, sizeof(BYTE), fileOutput);
		fwrite(&RGBPalette[i].rgbGreen, 1, sizeof(BYTE), fileOutput);
		fwrite(&RGBPalette[i].rgbRed, 1, sizeof(BYTE), fileOutput);
		fwrite(&(RGBPalette[i].rgbReserved), 1, sizeof(BYTE), fileOutput);
	}
	//保存像素数据
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
		{
			fwrite(&imageData[i][j].blue, 1, sizeof(BYTE), fileOutput);
		}
		for (int j = bmpInfoHeader.biWidth; j < width; ++j)
		{
			fwrite(&emptyBytes[i][j - bmpInfoHeader.biWidth], sizeof(BYTE), 1, fileOutput);
		}
	}
	cout << "图片保存成功" << endl;
	fclose(fileOutput);
}

//对RGB三个分量进行分离
void separateBmp(const char* fileInName, const char* fileROutName, const char* fileGOutName, const char* fileBOutName)
{
	loadBmpImageInfo(fileInName);
	//修改图片信息
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
		{
			imageData[i][j].blue = 0;
			imageData[i][j].green = 0;
		}
	}
	saveBmpImage(fileROutName);

	loadBmpImageInfo(fileInName);
	//修改图片信息
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
		{
			imageData[i][j].blue = 0;
			imageData[i][j].red = 0;
		}
	}
	saveBmpImage(fileGOutName);

	loadBmpImageInfo(fileInName);
	//修改图片信息
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
		{
			imageData[i][j].green = 0;
			imageData[i][j].red = 0;
		}
	}
	saveBmpImage(fileBOutName);
}

//24位图灰度化
void greyscale()
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
		{
			imageData[i][j].blue = imageData[i][j].green = imageData[i][j].red =
				0.299 * imageData[i][j].red + 0.587 * imageData[i][j].green + 0.114 * imageData[i][j].blue;
		}
	}
}

//8位灰度图反色
void reverseGrayscaleColor(const char* fileInName, const char* fileOutName) {
	loadBmpImageInfo(fileInName);

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
		{
			imageData[i][j].blue = 255 - imageData[i][j].blue;
		}
	}

	save8BitBmpImage(fileOutName);
}

//直方图处理模块

void getBmpHistogram(int greyscaleValue[], int size)
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
		{
			int num = (int)imageData[i][j].blue;
			greyscaleValue[num]++;
		}
	}
}

void histogramEqualization(int greyscaleValue[], int size)
{
	int nktemp[256] = {};

	for (int i = 0; i < 256; ++i)
	{
		int j = 0;
		while (j <= i)
		{
			nktemp[i] += greyscaleValue[j];
			++j;
		}
	}

	//计算得到sk与sk',sk'为新的灰度级 
	float sk[256] = {};
	int skInt[256] = {};
	for (int i = 0; i < 256; ++i)
	{
		sk[i] = (float)nktemp[i] / (bmpInfoHeader.biWidth * bmpInfoHeader.biHeight);
		skInt[i] = (int)(255 * sk[i] + 0.5);
	}

	//通过sk'重新计算得到n(sk)
	for (int i = 0; i < 256; ++i)
	{
		greyscaleValue[i] = 0;
	}

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
		{
			int num = (int)imageData[i][j].blue;
			imageData[i][j].blue = skInt[num];
			greyscaleValue[skInt[num]] ++;
		}
	}
	/*for (int i = 0; i < 256; ++i)
	{
		cout << greyscaleValue[i] << " ";
	}
	cout << endl;*/
}

void saveImageHistogram(const char* fileName, int greyscaleValue[])
{
	//保存图片信息写入bmp文件
	FILE* fileOutput;
	if ((fileOutput = fopen(fileName, "wb")) == NULL)
	{
		cout << "create the bmp file error!" << endl;
		return;
	}
	WORD bfType_w = 0x4d42;
	fwrite(&bfType_w, 1, sizeof(WORD), fileOutput);

	bmpInfoHeader.biBitCount = 8;
	bmpInfoHeader.biWidth = 256;
	bmpInfoHeader.biHeight = 256;
	bmpInfoHeader.biClrUsed = 256;
	width = 256;

	//加入调色板后,修改bmpInfoHeader,修改bmpFileHeader
	bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * 4L;//加调色板
	bmpFileHeader.bfSize = bmpFileHeader.bfOffBits + bmpInfoHeader.biSizeImage;

	for (int i = 0; i < 256; i++)
	{
		greyscaleValue[i] = greyscaleValue[i] * 256 / (bmpInfoHeader.biWidth * bmpInfoHeader.biHeight);
		//cout << greyscaleValue[i] << endl;
	}
	int image[256][256] = {};
	for (int i = 0; i < 256; ++i)
	{
		for (int j = 0; j < 256; ++j)
		{
			image[i][j] = greyscaleValue[j] > i ? 0 : 255;
		}
	}

	//写入文件头和信息头
	fwrite(&bmpFileHeader, 1, sizeof(tagBITMAPFILEHEADER), fileOutput);
	fwrite(&bmpInfoHeader, 1, sizeof(tagBITMAPINFOHEADER), fileOutput);

	//初始化调色板
	RGBPalette = new RGBQUAD[256];
	for (int i = 0; i < 256; i++)
	{
		RGBPalette[i].rgbBlue = i;
		RGBPalette[i].rgbGreen = i;
		RGBPalette[i].rgbRed = i;
		RGBPalette[i].rgbReserved = i;
	}
	//保存调色板数据
	for (unsigned int i = 0; i < 256; i++)
	{
		fwrite(&RGBPalette[i].rgbBlue, 1, sizeof(BYTE), fileOutput);
		fwrite(&RGBPalette[i].rgbGreen, 1, sizeof(BYTE), fileOutput);
		fwrite(&RGBPalette[i].rgbRed, 1, sizeof(BYTE), fileOutput);
		fwrite(&RGBPalette[i].rgbReserved, 1, sizeof(BYTE), fileOutput);
	}
	//保存像素数据
	for (int i = 0; i < bmpInfoHeader.biHeight; ++i)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
		{
			fwrite(&image[i][j], 1, sizeof(BYTE), fileOutput);
		}
	}
	cout << "图片保存成功" << endl;
	fclose(fileOutput);
}

//空间域滤波模块
//边缘扩展
void borderPointDeal(const char* fileName)
{
	//载入图片
	loadBmpImageInfo(fileName);

	//加入左，下边界
	int newWidth = width + 1;
	int newHeight = height + 1;

	//初始化新图像数据
	newImageData = new IMAGEDATA * [newHeight];
	for (int i = 0; i < newHeight; i++)
	{
		newImageData[i] = new IMAGEDATA[newWidth];
	}

	//加边界
	newImageData[0][0].blue = imageData[0][0].blue;

	for (int i = 0; i < width; i++)
	{
		if (i < bmpInfoHeader.biWidth)
		{
			newImageData[0][i + 1].blue = imageData[0][i].blue;
		}
		else
		{
			newImageData[0][i + 1].blue = emptyBytes[0][i];
		}
	}

	for (int j = 0; j < height; j++)
	{
		newImageData[j + 1][0].blue = imageData[j][0].blue;
	}

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
		{
			newImageData[i + 1][j + 1].blue = imageData[i][j].blue;
		}
		for (int j = bmpInfoHeader.biWidth; j < width; ++j)
		{
			newImageData[i + 1][j + 1].blue = emptyBytes[i][j - bmpInfoHeader.biWidth];
		}
	}

}

void averageFilter()
{
	for (int i = 1; i < height; ++i)
	{
		for (int j = 1; j < bmpInfoHeader.biWidth + 1; ++j)
		{
			imageData[i - 1][j - 1].blue = (newImageData[i + 1][j - 1].blue + newImageData[i + 1][j].blue + newImageData[i + 1][j + 1].blue +
				newImageData[i][j - 1].blue + newImageData[i][j].blue + newImageData[i][j + 1].blue +
				newImageData[i - 1][j - 1].blue + newImageData[i - 1][j].blue + newImageData[i - 1][j + 1].blue) / 9;
		}
	}
}

BYTE findMidValue(int i, int j)
{
	BYTE tempImageData[9] = { newImageData[i + 1][j - 1].blue , newImageData[i + 1][j].blue , newImageData[i + 1][j + 1].blue ,
				newImageData[i][j - 1].blue , newImageData[i][j].blue , newImageData[i][j + 1].blue ,
				newImageData[i - 1][j - 1].blue , newImageData[i - 1][j].blue , newImageData[i - 1][j + 1].blue };
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8 - i; j++)
		{
			if (tempImageData[j] > tempImageData[j + 1])
			{
				swap(tempImageData[j], tempImageData[j + 1]);
			}
		}
	}

	return tempImageData[4];
}

void midFilter()
{
	for (int i = 1; i < height; ++i)
	{
		for (int j = 1; j < bmpInfoHeader.biWidth + 1; ++j)
		{
			imageData[i - 1][j - 1].blue = findMidValue(i, j);
		}
	}
}

//图像变换模块
void ImageScale(const char* fileInName, const char* fileOutName)
{
	loadBmpImageInfo(fileInName);

	float scaleX;
	float scaleY;

	scaleX = 2;
	scaleY = 2;

	int newbiWidth = bmpInfoHeader.biWidth * scaleX;
	int newbiHeight = bmpInfoHeader.biHeight * scaleY;

	int newWidth = (newbiWidth * bmpInfoHeader.biBitCount / 8 + 3) / 4 * 4;

	IMAGEDATA** tempImageData;	//临时图像数据
	tempImageData = new IMAGEDATA * [newbiHeight];
	for (int i = 0; i < newbiHeight; i++)
	{
		tempImageData[i] = new IMAGEDATA[newbiWidth];
	}

	BYTE** tempEmptyData;	//临时空字节数据
	tempEmptyData = new BYTE * [newbiHeight];
	for (int i = 0; i < newbiHeight; i++)
	{
		tempEmptyData[i] = new BYTE[newWidth - newbiWidth * bmpInfoHeader.biBitCount / 8];
	}

	if (scaleX > 1)
	{
		int x0, x1;
		int y0, y1;

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				x0 = i * scaleY;
				y0 = j * scaleX;
				x1 = (i + 1) * scaleY;
				y1 = (j + 1) * scaleX;

				//丢掉越界的点
				if (x0 < 0 || y0 < 0 || x0 >= newbiHeight || y0 >= newbiWidth)
				{
					continue;
				}

				for (int m = x0; m < x1; m++)
				{
					for (int n = y0; n < y1; n++)
					{
						tempImageData[m][n] = imageData[i][j];
					}
				}

			}
		}
	}
	else if (scaleX < 1)
	{
		int x0 = 1 / scaleY;
		int y0 = 1 / scaleX;

		for (int i = 0, m = 0; i < height; i = i + x0, m++)
		{
			for (int j = 0, n = 0; j < width; j = j + y0, n++)
			{
				tempImageData[m][n] = imageData[i][j];
			}
		}
	}

	//更改原信息头
	bmpInfoHeader.biSizeImage = newWidth * newbiHeight;
	bmpInfoHeader.biWidth = newbiWidth;
	bmpInfoHeader.biHeight = newbiHeight;

	bmpFileHeader.bfSize = bmpInfoHeader.biSizeImage + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + bmpInfoHeader.biClrUsed * sizeof(RGBQUAD);

	//更改原imagedata的值
	width = newWidth;
	height = newbiHeight;

	//初始化图像数据
	imageData = new IMAGEDATA * [height];
	for (int i = 0; i < height; i++)
	{
		imageData[i] = new IMAGEDATA[bmpInfoHeader.biWidth];
	}
	//空的字节
	emptyBytes = new BYTE * [height];
	for (int i = 0; i < height; i++)
	{
		emptyBytes[i] = new BYTE[width - bmpInfoHeader.biWidth * bmpInfoHeader.biBitCount / 8];
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			imageData[i][j] = tempImageData[i][j];
		}
	}

	save8BitBmpImage(fileOutName);
}

void ImageTranslate(const char* fileInName, const char* fileOutName)
{
	loadBmpImageInfo(fileInName);

	int deltaX;
	int deltaY;

	deltaX = 100;
	deltaY = 100;

	IMAGEDATA** tempImageData;	//临时图像数据
	tempImageData = new IMAGEDATA * [height];
	for (int i = 0; i < height; i++)
	{
		tempImageData[i] = new IMAGEDATA[bmpInfoHeader.biWidth];
	}

	int x0;
	int y0;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			x0 = i + deltaY;
			y0 = j + deltaX;

			//丢掉越界的点
			if (x0 < 0 || y0 < 0 || x0 >= bmpInfoHeader.biHeight || y0 >= bmpInfoHeader.biWidth)
			{
				continue;
			}

			tempImageData[x0][y0] = imageData[i][j];
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			imageData[i][j] = tempImageData[i][j];
		}
	}

	save8BitBmpImage(fileOutName);
}

void ImageMirrorHorizontal(const char* fileInName, const char* fileOutName)
{
	loadBmpImageInfo(fileInName);

	IMAGEDATA** tempImageData;	//临时图像数据
	tempImageData = new IMAGEDATA * [height];
	for (int i = 0; i < height; i++)
	{
		tempImageData[i] = new IMAGEDATA[bmpInfoHeader.biWidth];
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			tempImageData[i][j] = imageData[bmpInfoHeader.biWidth - i - 1][j];
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			imageData[i][j] = tempImageData[i][j];
		}
	}

	save8BitBmpImage(fileOutName);
}

void ImageMirrorVertical(const char* fileInName, const char* fileOutName)
{
	loadBmpImageInfo(fileInName);

	IMAGEDATA** tempImageData;	//临时图像数据
	tempImageData = new IMAGEDATA * [height];
	for (int i = 0; i < height; i++)
	{
		tempImageData[i] = new IMAGEDATA[bmpInfoHeader.biWidth];
	}

	for (int j = 0; j < width; j++)
	{
		for (int i = 0; i < height; i++)
		{
			tempImageData[i][j] = imageData[i][bmpInfoHeader.biHeight - j - 1];
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			imageData[i][j] = tempImageData[i][j];
		}
	}

	save8BitBmpImage(fileOutName);
}

void ImageRotate(const char* fileInName, const char* fileOutName)
{
	loadBmpImageInfo(fileInName);

	int rotateAngle;
	/*cout << "请输入旋转的角度：" << endl;
	cin >> rotateAngle;*/

	rotateAngle = 45;

	//转化为弧度
	double angle = 1.0 * rotateAngle * PI / 180;
	int midX = bmpInfoHeader.biWidth / 2;
	int midY = bmpInfoHeader.biHeight / 2;

	IMAGEDATA** tempImageData;	//临时图像数据
	tempImageData = new IMAGEDATA * [height];
	for (int i = 0; i < height; i++)
	{
		tempImageData[i] = new IMAGEDATA[bmpInfoHeader.biWidth];
	}

	////绕原点旋转
	//int x0 = 0;
	//int y0 = 0;

	////绕中心点旋转
	//int x1 = 0;
	//int y1 = 0;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			//绕原点旋转
			/*x0 = i * cos(angle) + j * sin(angle);
			y0 = -i * sin(angle) + j * cos(angle);*/

			////绕中心点旋转
			//x1 = i - midY;
			//y1 = j - midX;

			//x0 = x1 * cos(angle) + y1 * sin(angle) + midY;
			//y0 = -x1 * sin(angle) + y1 * cos(angle) + midX;

			////丢掉越界的点
			//if (x0 < 0 || y0 < 0 || x0 >= bmpInfoHeader.biHeight || y0 >= bmpInfoHeader.biWidth)
			//{
			//	continue;
			//}

			//tempImageData[x0][y0] = imageData[i][j];

			int x1 = i - midY;
			int y1 = j - midX;

			//线性插值
			double x2 = x1 * cos(angle) - y1 * sin(angle) + midY;
			double y2 = x1 * sin(angle) + y1 * cos(angle) + midX;

			if (x2 < 0 || y2<0 || x2>bmpInfoHeader.biHeight || y2 >= bmpInfoHeader.biWidth)
			{
				continue;
			}

			const int pixelX = x2;
			const int pixelY = y2;

			const double distanceX = x2 - pixelX;
			const double distanceY = y2 - pixelY;

			int pixelXnext = pixelX + 1;
			int pixelYnext = pixelY + 1;

			if (pixelXnext >= bmpInfoHeader.biHeight)
			{
				pixelXnext = bmpInfoHeader.biHeight - 1;
			}
			if (pixelYnext >= bmpInfoHeader.biWidth)
			{
				pixelYnext = bmpInfoHeader.biWidth - 1;
			}

			int fx0 = (int)(imageData[pixelX][pixelY].blue + distanceX * (imageData[pixelXnext][pixelY].blue - imageData[pixelX][pixelY].blue));
			int fx1 = (int)(imageData[pixelX][pixelYnext].blue + distanceX * (imageData[pixelXnext][pixelYnext].blue - imageData[pixelX][pixelYnext].blue));
			int fxy = (int)(fx0 + distanceY * (fx1 - fx0));

			tempImageData[i][j].blue = fxy;
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			imageData[i][j] = tempImageData[i][j];
		}
	}

	save8BitBmpImage(fileOutName);
}

//基于区域的分割--区域增长
void RegionalInit(const char* fileInName)
{
	loadBmpImageInfo(fileInName);

	unRegion = new BYTE * [height];
	for (int i = 0; i < height; i++)
	{
		unRegion[i] = new BYTE[bmpInfoHeader.biWidth];
	}
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; j++)
		{
			unRegion[i][j] = 0;
		}
	}
}

//改进前版本
void RegionalGrowth(int seedX, int seedY, int threshold)
{
	int Dx[8] = { 0,0,1,-1,1,1,-1,-1 };
	int Dy[8] = { 1,-1,0,0,-1,1,1,-1 };

	int start = 0;
	int end = 0;

	int* growQueX = new int[bmpInfoHeader.biWidth * bmpInfoHeader.biHeight];
	int* growQueY = new int[bmpInfoHeader.biWidth * bmpInfoHeader.biHeight];

	growQueX[end] = seedX;
	growQueY[end] = seedY;

	while (start <= end)
	{
		const int currentX = growQueX[start];
		const int currentY = growQueY[start];

		for (int i = 0; i < 8; i++)
		{
			const int xx = currentX + Dx[i];
			const int yy = currentY + Dy[i];

			if (xx < bmpInfoHeader.biWidth && xx >= 0 &&
				yy < bmpInfoHeader.biHeight && yy >= 0 && unRegion[yy][xx] == 0 &&
				abs(imageData[yy][xx].blue - imageData[currentY][currentX].blue) < threshold)
			{
				end++;
				growQueX[end] = xx;
				growQueY[end] = yy;

				unRegion[yy][xx] = 1;
			}
		}

		start++;
	}
}

//改进后版本
void RegionalGrowth(int seedX, int seedY, int threshold, int color)
{
	int Dx[8] = { 0,0,1,-1,1,1,-1,-1 };
	int Dy[8] = { 1,-1,0,0,-1,1,1,-1 };

	int start = 0;
	int end = 0;

	int* growQueX = new int[bmpInfoHeader.biWidth * bmpInfoHeader.biHeight];
	int* growQueY = new int[bmpInfoHeader.biWidth * bmpInfoHeader.biHeight];

	growQueX[end] = seedX;
	growQueY[end] = seedY;

	while (start <= end)
	{
		const int currentX = growQueX[start];
		const int currentY = growQueY[start];

		for (int i = 0; i < 8; i++)
		{
			const int xx = currentX + Dx[i];
			const int yy = currentY + Dy[i];

			if (xx < bmpInfoHeader.biWidth && xx >= 0 &&
				yy < bmpInfoHeader.biHeight && yy >= 0 && unRegion[yy][xx] == 0 &&
				abs(imageData[yy][xx].blue - imageData[currentY][currentX].blue) < threshold)
			{
				end++;
				growQueX[end] = xx;
				growQueY[end] = yy;

				unRegion[yy][xx] = color;
			}
		}

		start++;
	}
}

void RegionalSave(const char* fileOutName)
{
	for (int i = 0; i < bmpInfoHeader.biHeight; i++)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; j++)
		{
			switch (unRegion[i][j])
			{
			case 1:
				imageData[i][j].blue = 80;
				break;
			case 2:
				imageData[i][j].blue = 160;
				break;
			case 3:
				imageData[i][j].blue = 255;
				break;
			default:
				imageData[i][j].blue = 0;
				break;
			}
		}
	}

	save8BitBmpImage(fileOutName);
}

//基于区域的分割--区域合并
Region* RegionalDivision(const Region& r, Region* alloc, int i)
{
	if (r.wEndIndex - r.wBeginIndex <= i || r.hEndIndex - r.hBeginIndex <= i) {
		return nullptr;
	}

	alloc[0].wBeginIndex = r.wBeginIndex;
	alloc[0].wEndIndex = r.wBeginIndex + (r.wEndIndex - r.wBeginIndex) / 2;
	alloc[0].hBeginIndex = r.hBeginIndex;
	alloc[0].hEndIndex = r.hBeginIndex + (r.hEndIndex - r.hBeginIndex) / 2;

	alloc[1].wBeginIndex = r.wBeginIndex + (r.wEndIndex - r.wBeginIndex) / 2;
	alloc[1].wEndIndex = r.wEndIndex;
	alloc[1].hBeginIndex = r.hBeginIndex;
	alloc[1].hEndIndex = r.hBeginIndex + (r.hEndIndex - r.hBeginIndex) / 2;

	alloc[2].wBeginIndex = r.wBeginIndex;
	alloc[2].wEndIndex = r.wBeginIndex + (r.wEndIndex - r.wBeginIndex) / 2;
	alloc[2].hBeginIndex = r.hBeginIndex + (r.hEndIndex - r.hBeginIndex) / 2;
	alloc[2].hEndIndex = r.hEndIndex;

	alloc[3].wBeginIndex = r.wBeginIndex + (r.wEndIndex - r.wBeginIndex) / 2;
	alloc[3].wEndIndex = r.wEndIndex;
	alloc[3].hBeginIndex = r.hBeginIndex + (r.hEndIndex - r.hBeginIndex) / 2;
	alloc[3].hEndIndex = r.hEndIndex;

	return alloc;
}

double getAver(const Region& r)
{
	int count = 0, result = 0;
	for (int i = r.hBeginIndex; i < r.hEndIndex; i++)
	{
		for (int j = r.wBeginIndex; j < r.wEndIndex; j++)
		{
			result += imageData[i][j].blue;
			count++;
		}
	}

	return static_cast<double>(result) / count;
}

//改进前版本
void RegionalMerge(const char* fileInName, int threadhold, const char* fileOutName, int i)
{
	loadBmpImageInfo(fileInName);

	Region origin;
	origin.wBeginIndex = 0;
	origin.wEndIndex = bmpInfoHeader.biWidth;
	origin.hBeginIndex = 0;
	origin.hEndIndex = bmpInfoHeader.biHeight;

	vector<Region> qTList;
	vector<Region> qTList2;
	queue<Region> queRegion;

	Region* rList = new Region[4];

	queRegion.push(origin);
	int start = 0, end = 0;
	while (start <= end)
	{
		const Region curRegion = queRegion.front();
		queRegion.pop();

		Region* rQList = RegionalDivision(curRegion, rList, i);

		for (int i = 0; i < 4; i++)
		{
			if (rQList != nullptr)
			{
				if (getAver(rList[i]) > threadhold)
				{
					queRegion.push(rList[i]);
					end++;
				}
				else if (getAver(rList[i]) > threadhold - 10)
				{
					qTList2.push_back(rList[i]);
				}
				else
				{
					qTList.push_back(rList[i]);
				}
			}
		}

		start++;
	}

	delete[] rList;

	IMAGEDATA** tempImageData = new IMAGEDATA * [height];
	for (int i = 0; i < height; i++)
	{
		tempImageData[i] = new IMAGEDATA[bmpInfoHeader.biWidth];
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; j++)
		{
			tempImageData[i][j].blue = 0;
		}
	}

	for (auto& r : qTList)
	{
		for (int i = r.hBeginIndex; i < r.hEndIndex; i++)
		{
			for (int j = r.wBeginIndex; j < r.wEndIndex; j++)
			{
				tempImageData[i][j].blue = 1;
			}
		}
	}

	for (auto& r : qTList2)
	{
		for (int i = r.hBeginIndex; i < r.hEndIndex; i++)
		{
			for (int j = r.wBeginIndex; j < r.wEndIndex; j++)
			{
				tempImageData[i][j].blue = 2;
			}
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; j++)
		{
			if (tempImageData[i][j].blue == 1)
			{
				imageData[i][j].blue = 255;
			}
			else if (tempImageData[i][j].blue == 2)
			{
				imageData[i][j].blue = 125;
			}
			else
			{
				imageData[i][j].blue = 0;
			}
		}
	}

	save8BitBmpImage(fileOutName);
}

bool isDivision(const Region& r)
{
	int Iaverage = static_cast<int> (getAver(r));
	for (int i = r.hBeginIndex; i < r.hEndIndex; i++)
	{
		for (int j = r.wBeginIndex; j < r.wEndIndex; j++)
		{
			if (abs(Iaverage - imageData[i][j].blue) > 20)
			{
				return true;
			}
		}
	}

	return false;
}

//改进后版本
void RegionalMerge(const char* fileInName, const char* fileOutName, int i)
{
	loadBmpImageInfo(fileInName);

	Region origin;
	origin.wBeginIndex = 0;
	origin.wEndIndex = bmpInfoHeader.biWidth;
	origin.hBeginIndex = 0;
	origin.hEndIndex = bmpInfoHeader.biHeight;

	vector<Region> qTList;
	queue<Region> queRegion;

	Region* rList = new Region[4];

	queRegion.push(origin);
	int start = 0, end = 0;
	while (start <= end)
	{
		const Region curRegion = queRegion.front();
		queRegion.pop();

		Region* rQList = RegionalDivision(curRegion, rList, i);

		for (int i = 0; i < 4; i++)
		{
			if (rQList != nullptr)
			{
				if (isDivision(rList[i]))
				{
					queRegion.push(rList[i]);
					end++;
				}
				else
				{
					qTList.push_back(rList[i]);
				}
			}
		}

		start++;
	}

	delete[] rList;

	IMAGEDATA** tempImageData = new IMAGEDATA * [height];
	for (int i = 0; i < height; i++)
	{
		tempImageData[i] = new IMAGEDATA[bmpInfoHeader.biWidth];
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; j++)
		{
			tempImageData[i][j].blue = 0;
		}
	}

	for (auto& r : qTList)
	{
		for (int i = r.hBeginIndex; i < r.hEndIndex; i++)
		{
			for (int j = r.wBeginIndex; j < r.wEndIndex; j++)
			{
				tempImageData[i][j].blue = static_cast<int> (getAver(r));
			}
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; j++)
		{
			imageData[i][j] = tempImageData[i][j];
		}
	}

	save8BitBmpImage(fileOutName);
}

//边缘检测 -- Prewitt
void Prewitt(const char* fileInName, int threshold, const char* fileOutName)
{
	loadBmpImageInfo(fileInName);

	IMAGEDATA** tempImageData = new IMAGEDATA * [height];
	for (int i = 0; i < height; i++)
	{
		tempImageData[i] = new IMAGEDATA[bmpInfoHeader.biWidth];
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; j++)
		{
			tempImageData[i][j].blue = 0;
		}
	}

	for (int i = 1; i < height - 1; i++)
	{
		for (int j = 1; j < bmpInfoHeader.biWidth - 1; j++)
		{
			const int dx = imageData[i + 1][j - 1].blue + imageData[i + 1][j].blue + imageData[i + 1][j + 1].blue
				- imageData[i - 1][j - 1].blue - imageData[i - 1][j].blue - imageData[i - 1][j + 1].blue;

			const int dy = imageData[i - 1][j + 1].blue + imageData[i][j + 1].blue + imageData[i - 1][j + 1].blue
				- imageData[i + 1][j - 1].blue - imageData[i][j - 1].blue - imageData[i - 1][j - 1].blue;

			int la = dx > dy ? dx : dy;
			if (la > threshold)
			{
				tempImageData[i][j].blue = 1;
			}
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; j++)
		{
			imageData[i][j].blue = tempImageData[i][j].blue == 1 ? 255 : 0;
		}
	}

	save8BitBmpImage(fileOutName);
}

//边缘检测 -- Sobel
void Sobel(const char* fileInName, int threshold, const char* fileOutName)
{
	loadBmpImageInfo(fileInName);

	IMAGEDATA** tempImageData = new IMAGEDATA * [height];
	for (int i = 0; i < height; i++)
	{
		tempImageData[i] = new IMAGEDATA[bmpInfoHeader.biWidth];
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; j++)
		{
			tempImageData[i][j].blue = 0;
		}
	}

	for (int i = 1; i < height - 1; i++)
	{
		for (int j = 1; j < bmpInfoHeader.biWidth - 1; j++)
		{
			const int gx = imageData[i - 1][j - 1].blue * -1 + imageData[i][j - 1].blue * -2 + imageData[i + 1][j - 1].blue * -1
				+ imageData[i - 1][j + 1].blue * 1 + imageData[i][j + 1].blue * 2 + imageData[i + 1][j + 1].blue * 1;

			const int gy = imageData[i + 1][j - 1].blue * -1 + imageData[i + 1][j].blue * -2 + imageData[i + 1][j + 1].blue * -1
				+ imageData[i - 1][j - 1].blue * 1 + imageData[i - 1][j].blue * 2 + imageData[i - 1][j + 1].blue * 1;

			const double g = sqrt(gx * gx + gy * gy);
			if (g > threshold)
			{
				tempImageData[i][j].blue = 1;
			}
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; j++)
		{
			imageData[i][j].blue = tempImageData[i][j].blue == 1 ? 255 : 0;
		}
	}

	save8BitBmpImage(fileOutName);
}

//边缘检测 -- LOG
double** getGaussianKernel(int size, double sqrSigma)
{
	double** gaus = new double* [size];
	for (int i = 0; i < size; i++)
	{
		gaus[i] = new double[size];
	}

	double pi = 4.0 * atan(1.0);
	int center = size / 2;
	double sum = 0;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			gaus[i][j] = (1 / (2 * pi * sqrSigma)) * exp(-((1 - center) * (1 - center) + (j - center) * (j - center)) / (2 * sqrSigma));
			sum += gaus[i][j];
		}

	}

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			gaus[i][j] /= sum;
		}
	}

	return gaus;
}

//改进后版本
void LOG(const char* fileInName, int threshold, const char* fileOutName)
{
	loadBmpImageInfo(fileInName);

	IMAGEDATA** tempImageData = new IMAGEDATA * [height];
	for (int i = 0; i < height; i++)
	{
		tempImageData[i] = new IMAGEDATA[bmpInfoHeader.biWidth];
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; j++)
		{
			tempImageData[i][j].blue = 0;
		}
	}

	for (int i = 2; i < height - 2; i++)
	{
		for (int j = 2; j < bmpInfoHeader.biWidth - 2; j++)
		{
			const int g = imageData[i - 2][j].blue * -1
				+ imageData[i - 1][j - 1].blue * -1 + imageData[i - 1][j].blue * -2 + imageData[i - 1][j + 1].blue * -1
				+ imageData[i][j - 2].blue * -1 + imageData[i][j - 1].blue * -2 + imageData[i][j].blue * 16 + imageData[i][j + 1].blue * -2 + imageData[i][j + 1].blue * -1
				+ imageData[i + 1][j - 1].blue * -1 + imageData[i + 1][j].blue * -2 + imageData[i + 1][j + 1].blue * -1
				+ imageData[i + 2][j].blue * -1;

			if (g > threshold)
			{
				tempImageData[i][j].blue = 1;
			}
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; j++)
		{
			imageData[i][j].blue = tempImageData[i][j].blue == 1 ? 255 : 0;
		}
	}

	save8BitBmpImage(fileOutName);
}

//Hough变换 -- 直线检测
//改进前版本
void Hough(const char* fileInName, const char* fileOutName)
{
	loadBmpImageInfo(fileInName);

	//存储新的图像,先将所有像素初始化为黑色
	newImageData = new IMAGEDATA * [height];
	for (int i = 0; i < height; i++)
	{
		newImageData[i] = new IMAGEDATA[bmpInfoHeader.biWidth];
	}
	for (int i = 0; i < bmpInfoHeader.biHeight; ++i)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
		{
			newImageData[i][j].blue = 0;
		}
	}

	int maxR = (int)pow(bmpInfoHeader.biWidth * bmpInfoHeader.biWidth + height * height, 0.5);//最大R

	cout << maxR << endl;

	int* R = new int[maxR + 1];//R的范围时0到maxR
	for (int i = 0; i < maxR; ++i)
		R[i] = i;
	double angles[901];//角度范围是0到90
	for (int i = 0; i < 901; ++i)
		angles[i] = (double)i / 10;
	for (int r = 0; r < maxR; ++r)
	{
		for (int k = 0; k < 901; ++k)
		{
			int count = 0;//统计在这条直线上的点的个数
			vector<_Point>  line;
			bool first_find = false;
			_Point first = _Point{ 0, 0 };
			_Point last = _Point{ 0, 0 };
			for (int i = 0; i < bmpInfoHeader.biHeight; ++i)//对每个r和k的组合，都要遍历整个图像所有像素一遍
			{
				for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
				{
					if (abs((double)i * cos(angles[k] * PI / 180) + (double)j * sin(angles[k] * PI / 180) - r) <= 0.2)//r = x*cos + y*sin
					{
						line.push_back(_Point{ i, j });
						if (255 == (int)imageData[i][j].blue)
						{
							++count;
							if (!first_find)
							{
								first_find = true;
								first = _Point{ i, j };
							}
							else last = _Point{ i, j };
						}
					}
				}
			}
			if (count >= 30)
			{
				for (_Point point : line)
				{
					if (!(point.x >= first.x ^ point.x <= last.x))
						newImageData[point.x][point.y].blue = 255;
				}
			}
			line.clear();
		}
		cout << "R:" << r << endl;
	}
	delete[] imageData;
	imageData = newImageData;
	save8BitBmpImage(fileOutName);
}
//改进后版本
void HoughTransform(const char* fileInName, const char* fileOutName)
{
	loadBmpImageInfo(fileInName);

	//存储新的图像,先将所有像素初始化为黑色
	newImageData = new IMAGEDATA * [height];
	for (int i = 0; i < height; i++)
	{
		newImageData[i] = new IMAGEDATA[bmpInfoHeader.biWidth];
	}
	for (int i = 0; i < bmpInfoHeader.biHeight; ++i)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
		{
			newImageData[i][j].blue = 0;
		}
	}

	int maxR = (int)pow(bmpInfoHeader.biWidth * bmpInfoHeader.biWidth + height * height, 0.5);//最大R
	int* R = new int[maxR + 1];//R的范围时0到maxR
	for (int i = 0; i < maxR; ++i)
		R[i] = i;
	double angles[901]; //角度范围是0到90
	for (int i = 0; i < 901; ++i)
		angles[i] = (double)i / 10;

	cout << "Hough 执行中 ... " << endl;

	for (int k = 0; k < 901; ++k)//角度相同，r相近的线中找极大值
	{
		int maxCount = 0;
		vector<_Point>  maxLine;
		_Point ml_first = _Point{ 0, 0 };
		_Point ml_last = _Point{ 0, 0 };
		bool findMax = false;

		for (int r = 0; r < maxR; ++r)//对每个组合都统计点个数，找连着的线中点最多的线
		{
			int count = 0;//统计在这条直线上的点的个数
			vector<_Point>  line;
			bool _findFirst = false;
			_Point first = _Point{ 0, 0 };
			_Point last = _Point{ 0, 0 };
			for (int i = 0; i < bmpInfoHeader.biHeight; i++)//对每个r和k的组合，都要遍历整个图像所有像素一遍
			{
				for (int j = 0; j < bmpInfoHeader.biWidth; j++)
				{
					if (abs((double)i * cos(angles[k] * PI / 180) + (double)j * sin(angles[k] * PI / 180) - r) <= 0.2)             //r = x*cos + y*sin
					{
						line.push_back(_Point{ i, j });
						if ((int)imageData[i][j].blue == 255)
						{
							++count;
							if (!_findFirst)
							{
								_findFirst = true;
								first = _Point{ i, j };
							}
							else last = _Point{ i, j };
						}
					}
				}
			}
			if (count >= 30)
			{
				if (findMax && count <= maxCount)
				{
					//继续
				}
				else //否则都修改当前的线为最长的线
				{
					maxLine = line;
					ml_first = first;
					ml_last = last;
					findMax = true;
					maxCount = count;
				}
			}
			else if (findMax)
			{
				findMax = false;
				for (_Point point : maxLine)
				{
					if (!(point.x >= ml_first.x ^ point.x <= ml_last.x))
						newImageData[point.x][point.y].blue = 255;
				}
			}
			line.clear();
		}
		if (findMax)
		{
			for (_Point point : maxLine)
			{
				if (!(point.x >= ml_first.x ^ point.x <= ml_last.x))
					newImageData[point.x][point.y].blue = 255;
			}
		}
		//cout << "K:" << k << endl;
	}
	delete[] imageData;
	imageData = newImageData;

	cout << "Hough 执行结束" << endl;

	save8BitBmpImage(fileOutName);
}


//区域标记与轮廓提取 -- 连通域分析
void RecursiveMark(int rheight, int rwidth, int markVal)
{
	if (rheight < 0 || rheight > bmpInfoHeader.biHeight - 1 || rwidth <0 || rwidth>bmpInfoHeader.biWidth - 1)
	{
		return;
	}

	if (mark[rheight][rwidth] != -1)
	{
		return;
	}

	if (imageData[rheight][rwidth].blue == markVal)
	{
		mark[rheight][rwidth] = record;
	}
	else
	{
		return;
	}

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if (rheight + i < 0 || rheight + i>bmpInfoHeader.biHeight - 1 || rwidth + j<0 || rwidth + j>bmpInfoHeader.biWidth - 1)
			{
				continue;
			}
			if (i == 0 && j == 0)
			{
				continue;
			}
			RecursiveMark(rheight + i, rwidth + j, imageData[rheight][rwidth].blue);
		}
	}
}

void ConnectedDomain(const char* fileInName, const char* fileOutName)
{
	loadBmpImageInfo(fileInName);

	mark = new int* [height];
	for (int i = 0; i < height; i++)
	{
		mark[i] = new int[bmpInfoHeader.biWidth];
	}
	for (int i = 0; i < bmpInfoHeader.biHeight; ++i)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
		{
			mark[i][j] = -1;
		}
	}

	//cout << "test = " << static_cast<int>(imageData[0][0].blue) << endl;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; j++)
		{
			if (mark[i][j] == -1)
			{
				record++;
				RecursiveMark(i, j, imageData[i][j].blue);
			}
		}
	}

	RecursiveMark(10, 10, imageData[10][10].blue);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; j++)
		{
			imageData[i][j].blue = mark[i][j] * 40;
		}
	}
	save8BitBmpImage(fileOutName);
}

//区域标记与轮廓提取 -- 轮廓提取
bool isBorder(int rheight, int rwidth, int markVal)
{
	if (markVal == 255)
	{
		return false;
	}

	bool temp = false;

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if (rheight + i < 0 || rheight + i>bmpInfoHeader.biHeight - 1 || rwidth + j<0 || rwidth + j>bmpInfoHeader.biWidth - 1)
			{
				continue;
			}
			if (i == 0 && j == 0)
			{
				continue;
			}
			if (imageData[rheight + i][rwidth + j].blue != markVal)
			{
				temp = true;
			}
		}
	}

	return temp;
}

void ContourExtraction(const char* fileInName, const char* fileOutName)
{
	loadBmpImageInfo(fileInName);

	mark = new int* [height];
	for (int i = 0; i < height; i++)
	{
		mark[i] = new int[bmpInfoHeader.biWidth];
	}
	for (int i = 0; i < bmpInfoHeader.biHeight; ++i)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; ++j)
		{
			mark[i][j] = -1;
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; j++)
		{
			if (isBorder(i, j, imageData[i][j].blue))
			{
				mark[i][j] = 1;
			}
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < bmpInfoHeader.biWidth; j++)
		{
			imageData[i][j].blue = mark[i][j] == 1 ? 0 : 255;
		}
	}

	save8BitBmpImage(fileOutName);
}


void Tcut() {//阈值分割
	float T;
	cout << "输入阈值" << endl;
	cin >> T;
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	FILE* pfin, * pfout1, * pfout2;
	fopen_s(&pfin, "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\yzfg\\lena.bmp", "rb");
	fopen_s(&pfout1, "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\yzfg\\T-cut.bmp", "wb");
	fopen_s(&pfout2, "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\yzfg\\t-cut-histogram.bmp", "wb");
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pfin);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pfin);

	int height = infoHeader.biHeight, width = infoHeader.biWidth;
	int byteWidth = (width * infoHeader.biBitCount / 8 + 3) / 4 * 4;
	int size = byteWidth * height;

	unsigned char* img = new unsigned char[size];
	unsigned char* newimg = new unsigned char[size];
	memset(newimg, 0, sizeof(unsigned char) * size);

	int count[256];
	int maxvalue = 0;
	memset(count, 0, 256 * sizeof(int));
	int histogramwidth = 256;

	RGBQUAD rgbquad[256];
	fread(rgbquad, sizeof(RGBQUAD), 256, pfin);
	fread(img, sizeof(unsigned char), size, pfin);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (img[i * width + j] >= T) {
				newimg[i * width + j] = 255;
			}
			else newimg[i * width + j] = 0;

			count[img[i * width + j]]++;
		}

	}
	for (int i = 0; i < 256; i++) {
		count[i] /= 10;
		if (maxvalue < count[i])
		{
			maxvalue = count[i];
		}
	}
	unsigned char* histogram = new unsigned char[maxvalue * histogramwidth];
	for (int i = 0; i < 256; i++) {
		if (i >= T - 2 && i <= T + 2) {
			count[i] = maxvalue;
		}
		while (count[i]--)
		{
			if (i >= T - 2 && i <= T + 2) {
				histogram[count[i] * histogramwidth + i] = 128;
			}

			else histogram[count[i] * histogramwidth + i] = 0;
		}
	}

	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pfout1);
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pfout1);
	fwrite(rgbquad, 4 * 256, 1, pfout1);
	fwrite(newimg, sizeof(unsigned char), size, pfout1);


	infoHeader.biWidth = 256;
	infoHeader.biHeight = maxvalue;
	infoHeader.biSizeImage = 256 * maxvalue;
	fileHeader.bfSize = 1078 + infoHeader.biSizeImage;

	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pfout2);
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pfout2);
	fwrite(rgbquad, 4 * 256, 1, pfout2);
	fwrite(histogram, sizeof(unsigned char), maxvalue * histogramwidth, pfout2);

	fclose(pfin);
	fclose(pfout1);
	fclose(pfout2);
	delete[] img;
	delete[] histogram;
	delete[] newimg;
}


void iteratorcut() {//迭代分割
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	FILE* pfin, * pfout, * pfout2;
	fopen_s(&pfin, "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\yzfg\\lena.bmp", "rb");
	fopen_s(&pfout, "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\yzfg\\iterator-cut.bmp", "wb");
	fopen_s(&pfout2, "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\yzfg\\iterator-cut-histogram.bmp", "wb");
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pfin);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pfin);

	int height = infoHeader.biHeight, width = infoHeader.biWidth;
	width = (width * infoHeader.biBitCount / 8 + 3) / 4 * 4;
	int size = width * height;

	unsigned char* img = new unsigned char[size];
	unsigned char* newimg = new unsigned char[size];
	int* gray = new int[size];
	memset(newimg, 0, sizeof(unsigned char) * size);

	int count[256];
	int maxvalue = 0;
	memset(count, 0, 256 * sizeof(int));
	int histogramwidth = 256;


	RGBQUAD rgbquad[256];
	fread(rgbquad, sizeof(RGBQUAD), 256, pfin);
	fread(img, sizeof(unsigned char), size, pfin);

	for (int i = 0; i < size; i++) {
		gray[i] = img[i];
	}
	sort(gray, gray + size);
	int T = gray[size / 2];
	while (1)
	{
		float right = 0, left = 0;
		int leftsize = 0, rightsize = 0;
		for (int i = 0; i < size; i++) {
			if (img[i] >= T) {
				right += img[i];
				rightsize++;
			}
			else {
				left += img[i];
				leftsize++;
			}
		}

		float T1 = (left / leftsize + right / rightsize) / 2;
		if (abs(T1 - T) <= 1) {
			T = T1;
			break;
		}
		T = T1;
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (img[i * width + j] >= T) {
				newimg[i * width + j] = 255;

			}
			else newimg[i * width + j] = 0;

			count[img[i * width + j]]++;
		}
	}

	for (int i = 0; i < 256; i++) {
		count[i] /= 10;
		if (maxvalue < count[i])
		{
			maxvalue = count[i];
		}
	}
	unsigned char* histogram = new unsigned char[maxvalue * histogramwidth];
	for (int i = 0; i < 256; i++) {
		if (i >= T - 2 && i <= T + 2) {
			count[i] = maxvalue;
		}
		while (count[i]--)
		{
			if (i >= T - 2 && i <= T + 2) {
				histogram[count[i] * histogramwidth + i] = 128;
			}
			else {
				histogram[count[i] * histogramwidth + i] = 0;
			}

		}
	}

	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pfout);
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pfout);
	fwrite(&rgbquad, 4 * 256, 1, pfout);
	fwrite(newimg, sizeof(unsigned char), size, pfout);



	infoHeader.biWidth = 256;
	infoHeader.biHeight = maxvalue;
	infoHeader.biSizeImage = 256 * maxvalue;
	fileHeader.bfSize = 1078 + infoHeader.biSizeImage;
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pfout2);
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pfout2);
	fwrite(&rgbquad, 4 * 256, 1, pfout2);
	fwrite(histogram, sizeof(unsigned char), maxvalue * histogramwidth, pfout2);

	fclose(pfin);
	fclose(pfout);
	fclose(pfout2);
	delete[] img;
	delete[] histogram;
	delete[] newimg;
}

void otsu() {//otsu阈值分割
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	FILE* pfin, * pfout, * pfout2;
	fopen_s(&pfin, "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\yzfg\\lena.bmp", "rb");
	fopen_s(&pfout, "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\yzfg\\otsucut.bmp", "wb");
	fopen_s(&pfout2, "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\yzfg\\otsucuthistogram.bmp", "wb");
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pfin);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pfin);

	int height = infoHeader.biHeight, width = infoHeader.biWidth;
	width = (width * infoHeader.biBitCount / 8 + 3) / 4 * 4;
	int size = width * height;

	unsigned char* img = new unsigned char[size];
	unsigned char* newimg = new unsigned char[size];
	memset(newimg, 0, sizeof(unsigned char) * size);

	int count[256];
	int maxvalue = 0;
	memset(count, 0, 256 * sizeof(int));
	int histogramwidth = 256;


	RGBQUAD rgbquad[256];
	fread(rgbquad, sizeof(RGBQUAD), 256, pfin);
	fread(img, sizeof(unsigned char), size, pfin);
	int T;
	int t = 0;
	float s0 = 0;
	//w点在类里的概率 u类的平均灰度 类间方差s=w1*w2(u1-u2)²;
	for (T = 0; T < 255; T++) {
		float w1 = 0, w2 = 0;
		float u1 = 0, u2 = 0;
		int size1 = 0, size2 = 0;
		for (int i = 0; i < size; i++) {
			if (img[i] <= T) {
				w1 += (float)1 / size;
				u1 += img[i];
				size1++;
			}
			else {
				w2 += (float)1 / size;
				u2 += img[i];
				size2++;
			}
		}
		u1 = u1 / size1;
		u2 = u2 / size2;
		float s = w1 * w2 * (u1 - u2) * (u1 - u2);
		if (s > s0) {
			s0 = s;
			t = T;
		}
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			newimg[i * width + j] = img[i * width + j] >= t ? 255 : 0;
			count[img[i * width + j]]++;
		}
	}

	for (int i = 0; i < 256; i++) {
		count[i] /= 10;
		if (maxvalue < count[i])
		{
			maxvalue = count[i];
		}
	}
	unsigned char* histogram = new unsigned char[maxvalue * histogramwidth];
	for (int i = 0; i < 256; i++) {
		if (i >= t - 2 && i <= t + 2) {
			count[i] = maxvalue;
		}
		while (count[i]--)
		{
			if (i >= T - 2 && i <= T + 2) {
				histogram[count[i] * histogramwidth + i] = 128;
			}
			else histogram[count[i] * histogramwidth + i] = 0;
		}
	}

	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pfout);
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pfout);
	fwrite(&rgbquad, 4 * 256, 1, pfout);
	fwrite(newimg, sizeof(unsigned char), size, pfout);

	infoHeader.biWidth = 256;
	infoHeader.biHeight = maxvalue;
	infoHeader.biSizeImage = 256 * maxvalue;
	fileHeader.bfSize = 1078 + infoHeader.biSizeImage;
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pfout2);
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pfout2);
	fwrite(&rgbquad, 4 * 256, 1, pfout2);
	fwrite(histogram, sizeof(unsigned char), maxvalue * histogramwidth, pfout2);

	fclose(pfin);
	fclose(pfout);
	fclose(pfout2);
	delete[] img;
	delete[] histogram;
	delete[] newimg;
}




int main()
{
	//程序正常运行的文件路径
	//对RGB进行分离
	const char* originalFileName = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\1\\rgb.bmp";
	const char* separateRFileName = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\1\\separateRed.bmp";
	const char* separateGFileName = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\1\\separateGreen.bmp";
	const char* separateBFileName = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\1\\separateBlue.bmp";

	//24位图灰度化
	const char* fileName = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\1\\rgb.bmp";
	const char* greyFileName = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\1\\Grey.bmp";

	//8位图反色
	const char* preGreyFileName = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\1\\Grey.bmp";
	const char* reverseGreyFileName = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\1\\unGrey.bmp";

	//直方图
	const char* lightFileName = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\2\\dim.bmp";
	const char* savelightHisto = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\2\\dimLightHisto.bmp";
	const char* saveLightEqualBmp = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\2\\dimLightEqualBmp.bmp";
	const char* lightEqualBmp = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\2\\lightEqualBmp.bmp";

	//平均滤波
	const char* noiseFileName = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\3\\noise2.bmp";
	const char* averageNoiseFileName = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\3\\Noiseave.bmp";

	//中值滤波
	const char* midNoiseFileName = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\3\\lenaMidNoise8.bmp";

	// 图像变换
	const char* lena8bitsGreyFile = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\4\\lena.bmp";
	const char* HorizontalMirrorFile = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\4\\lenaHorizontalMirror.bmp";
	const char* VerticalMirrorFile = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\4\\lenaVerticalMirror.bmp";
	const char* RotateFile = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\4\\lenaRotate.bmp";
	const char* TranslateFile = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\4\\lenaTranslate.bmp";
	const char* ScaleFile = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\4\\lenaScale.bmp";

	//基于区域的阈值分割
	const char* defectGrey6File = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\6\\lena.bmp";
	const char* defectGreyFile = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\6\\lena.bmp";
	const char* RegionalGrowthFile = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\6\\RegionalGrowth.bmp";
	const char* RegionalDivisionFile = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\6\\RegionalDivision.bmp";
	const char* RegionalDivision4File = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\6\\RegionalDivision4.bmp";
	const char* RegionalDivision8File = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\6\\RegionalDivision8.bmp";
	const char* RegionalDivision16File = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\6\\RegionalDivision16.bmp";

	//边缘检测
	const char* detectionGreyFile = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\7\\lena.bmp";
	const char* edgePrewittFile = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\7\\prewitt.bmp";
	const char* edgeSobelFile = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\7\\sobel.bmp";;
	const char* edgeLOGFile = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\7\\LOG.bmp";;

	//区域标记与轮廓提取
	const char* connectedDomainGreyFile = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\9\\lena.bmp";
	const char* connectedDomainAnaGreyFile = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\9\\connectedDomain.bmp";

	const char* contourGreyFile = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\9\\lena.bmp";
	const char* contourExtractionGreyFile = "D:\\Cherchez\\Microsoft Visual Studio\\Documents\\Repos\\PICProg\\PICProg\\workspace\\9\\contourExtraction.bmp";

	int choice = -1;
	bool flag = false;
	Menu menu;
	while (choice != 10)
	{
		if (!flag)
		{
			system("cls");
			menu.PrintMenu();
			cin >> choice;
		}
		switch (choice)
		{
		case 1:
			separateBmp(originalFileName, separateRFileName, separateGFileName, separateBFileName);
			loadBmpImageInfo(fileName);
			greyscale();
			save8BitBmpImage(greyFileName);
			reverseGrayscaleColor(preGreyFileName, reverseGreyFileName);
			break;
		case 2:
			loadBmpImageInfo(lightFileName);
			getBmpHistogram(greyscaleValue, 256);
			saveImageHistogram(savelightHisto, greyscaleValue);
			loadBmpImageInfo(lightFileName);
			getBmpHistogram(greyscaleValue, 256);
			histogramEqualization(greyscaleValue, 256);
			save8BitBmpImage(lightEqualBmp);
			saveImageHistogram(saveLightEqualBmp, greyscaleValue);
			break;
		case 3:
			borderPointDeal(noiseFileName);
			averageFilter();
			save8BitBmpImage(averageNoiseFileName);
			borderPointDeal(noiseFileName);
			midFilter();
			save8BitBmpImage(midNoiseFileName);
			break;
		case 4:
			ImageScale(lena8bitsGreyFile, ScaleFile);
			ImageTranslate(lena8bitsGreyFile, TranslateFile);
			ImageMirrorHorizontal(lena8bitsGreyFile, HorizontalMirrorFile);
			ImageMirrorVertical(lena8bitsGreyFile, VerticalMirrorFile);
			ImageRotate(lena8bitsGreyFile, RotateFile);
			break;
		case 5:
			/*Tcut();
			iteratorcut();
			otsu();*/
			break;
		case 6:
			RegionalInit(defectGrey6File);
			RegionalGrowth(0, 0, 3, 1);
			RegionalGrowth(90, 141, 2, 2);
			RegionalGrowth(200, 210, 10, 3);
			RegionalSave(RegionalGrowthFile);

			RegionalMerge(defectGreyFile, RegionalDivisionFile, 1);
			RegionalMerge(defectGreyFile, RegionalDivision4File, 4);
			RegionalMerge(defectGreyFile, RegionalDivision8File, 8);
			RegionalMerge(defectGreyFile, RegionalDivision16File, 16);
			break;
		case 7:
			Prewitt(detectionGreyFile, 100, edgePrewittFile);
			Sobel(detectionGreyFile, 100, edgeSobelFile);
			LOG(detectionGreyFile, 127, edgeLOGFile);
			break;
		case 8:

			break;
		case 9:
			ConnectedDomain(connectedDomainGreyFile, connectedDomainAnaGreyFile);
			ContourExtraction(contourGreyFile, contourExtractionGreyFile);
			break;
		case 10:
			choice = -1;
			if (flag)
			{
				flag = false;
			}
			else
			{
				flag = true;
			}
			break;
		default:
			break;
		}
		if (flag)
		{
			choice++;
		}
	}

	//释放内存
	delete[] imageData;
	delete[] emptyBytes;
	delete[] RGBPalette;
}
