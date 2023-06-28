#pragma once
#pragma once
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef long LONG;


typedef struct tagBITMAPFILEHEADER
{
	//WORD   	bfType; //�ļ����ͣ������ǡ�BM��,���ļ���ֱ��ʹ��
	DWORD	bfSize; //�ļ���С�������ļ�ͷ�Ĵ�С
	WORD	bfReserved1; //������
	WORD	bfReserved2; //������
	DWORD	bfOffBits; //���ļ�ͷ��ʵ��λͼ���ݵ�ƫ���ֽ���
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
	DWORD   biSize; //�ýṹ�ĳ��ȣ�Ϊ40
	LONG	biWidth; //ͼ����
	LONG	biHeight; //ͼ��߶�
	WORD	biPlanes; //λƽ����������Ϊ1
	WORD	biBitCount; //��ɫλ����
	DWORD   biCompression; //�Ƿ�ѹ��
	DWORD   biSizeImage; //ʵ��λͼ����ռ�õ��ֽ���
	LONG	biXPelsPerMeter;//Ŀ���豸ˮƽ�ֱ���
	LONG	biYPelsPerMeter;//Ŀ���豸��ֱ�ֱ���
	DWORD   biClrUsed;//ʵ��ʹ�õ���ɫ��
	DWORD   biClrImportant;//ͼ������Ҫ����ɫ��
}BITMAPINFOHEADER;

typedef struct tagRGBQUAD
{
	BYTE  rgbBlue;   	 //����ɫ����ɫ����
	BYTE  rgbGreen;	 //����ɫ����ɫ����
	BYTE  rgbRed;		 //����ɫ�ĺ�ɫ����
	BYTE  rgbReserved;	 //����ֵ
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
IMAGEDATA** imageData;	//ͼ������
IMAGEDATA** newImageData;	//��ͼ������
BYTE** emptyBytes;		//��ȫ�Ŀ��ֽ�
int greyscaleValue[256] = {};	//�Ҷ�ֵ
BYTE** unRegion;		//��ֵ����
int** mark;			//���