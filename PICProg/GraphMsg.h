#pragma once
#pragma once
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef long LONG;


typedef struct tagBITMAPFILEHEADER
{
	//WORD   	bfType; //文件类型，必须是“BM”,在文件中直接使用
	DWORD	bfSize; //文件大小，包含文件头的大小
	WORD	bfReserved1; //保留字
	WORD	bfReserved2; //保留字
	DWORD	bfOffBits; //从文件头到实际位图数据的偏移字节数
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
	DWORD   biSize; //该结构的长度，为40
	LONG	biWidth; //图像宽度
	LONG	biHeight; //图像高度
	WORD	biPlanes; //位平面数，必须为1
	WORD	biBitCount; //颜色位数，
	DWORD   biCompression; //是否压缩
	DWORD   biSizeImage; //实际位图数据占用的字节数
	LONG	biXPelsPerMeter;//目标设备水平分辨率
	LONG	biYPelsPerMeter;//目标设备垂直分辨率
	DWORD   biClrUsed;//实际使用的颜色数
	DWORD   biClrImportant;//图像中重要的颜色数
}BITMAPINFOHEADER;

typedef struct tagRGBQUAD
{
	BYTE  rgbBlue;   	 //该颜色的蓝色分量
	BYTE  rgbGreen;	 //该颜色的绿色分量
	BYTE  rgbRed;		 //该颜色的红色分量
	BYTE  rgbReserved;	 //保留值
} RGBQUAD;

typedef struct tagIMAGEDATA
{
	BYTE blue;
	BYTE green;
	BYTE red;
}IMAGEDATA;

typedef struct Region
{
	int wBeginIndex = 0, wEndIndex = 0, hBeginIndex = 0, hEndIndex = 0;
	bool Q = false;
};

typedef struct Point
{
	int x;
	int y;
}_Point;

BITMAPFILEHEADER bmpFileHeader;
BITMAPINFOHEADER bmpInfoHeader;
RGBQUAD* RGBPalette;
long width, height;
IMAGEDATA** imageData;	//图像数据
IMAGEDATA** newImageData;	//新图像数据
BYTE** emptyBytes;		//补全的空字节
int greyscaleValue[256] = {};	//灰度值
BYTE** unRegion;		//阈值增长
int** mark;			//标记