#include "stdafx.h"
#include "rgbyuv.h"

#define  _RGB565   0
#define  _RGB24    1
#define  _RGB32    2

typedef void RGB_IN(unsigned int& r, unsigned int& g, unsigned int& b, const unsigned char* s);
typedef void RGB_OUT(unsigned char* d,const unsigned int r, const unsigned int g, const unsigned int b);

static inline unsigned int bitcopy_n(unsigned int a, int n)
{
    int mask;
    mask = (1 << n) - 1;
    return (a & (0xff & ~mask)) | ((-((a >> n) & 1)) & mask);
}

static inline void RGB_IN_565(unsigned int& r, unsigned int& g, unsigned int& b, const unsigned char* s)
{
    unsigned int v = ((const unsigned short *)(s))[0];
    r = bitcopy_n(v >> (11 - 3), 3);//r
    g = bitcopy_n(v >> (5 - 2), 2);
    b = bitcopy_n(v << 3, 3);//b
}


static inline void RGB_OUT_565(unsigned char* d,const unsigned int r, const unsigned int g, const unsigned int b)
{
    ((unsigned short *)(d))[0] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

static inline void RGB_IN_24(unsigned int& r, unsigned int& g, unsigned int& b, const unsigned char* s)
{
    //b = (s)[0];
	r = (s)[0];
    g = (s)[1];
    //r = (s)[2];
	b = (s)[2];
}


static inline void RGB_OUT_24(unsigned char* d,const unsigned int r, const unsigned int g, const unsigned int b)
{
    //(d)[0] = b;
	(d)[0] = r;
    (d)[1] = g;
   // (d)[2] = r;
	(d)[2] = b;
}

static inline void RGB_IN_32(unsigned int& r, unsigned int& g, unsigned int& b, const unsigned char* s)
{
    unsigned int v = ((const unsigned int *)(s))[0];
   // r = (v >> 16) & 0xff;
	 b = (v >> 16) & 0xff;
    g = (v >> 8) & 0xff;
    //b = v & 0xff;
	r = v & 0xff;
}

static inline void RGB_OUT_32(unsigned char* d, const unsigned int r, const unsigned int g, const unsigned int b)
{
	unsigned int a =0;
    ((unsigned int *)(d))[0] = (a << 24) | (r << 16) | (g << 8) | b;
}

#define LIMIT(x) (unsigned char) (((x > 0xffffff) ? 0xff0000 : ((x <= 0xffff) ? 0 : x & 0xff0000)) >> 16)
#define rgbtoyuv(b, g, r, y, u, v) \
	y=(BYTE)(((int)30*r  +(int)59*g +(int)11*b)/100); \
	u=(BYTE)(((int)-17*r  -(int)33*g +(int)50*b+12800)/100); \
	v=(BYTE)(((int)50*r  -(int)42*g -(int)8*b+12800)/100); \


void Rgb565ToRgb24(unsigned char* dst, const unsigned char* src,int width, int height)
{
    int j, y;
    unsigned int r, g , b;

    for(y=0;y<height;y++)
    {
        for(j = 0;j < width; j++)
        {
            RGB_IN_565(r, g, b, src);
            RGB_OUT_24(dst,r,g,b);
            dst += 3;
            src += 2;
        }
    }
}

void Rgb24ToRgb565(unsigned char* dst, const unsigned char* src,int width, int height)
{
    int j, y;
    unsigned int r, g , b;

    for(y=0;y<height;y++)
    {
        for(j = 0;j < width; j++)
        {
            RGB_IN_24(r, g, b, src);
            RGB_OUT_565(dst,r,g,b);
            dst += 2;
            src += 3;
        }
    }
}

static int YUV420PtoRGB(const BYTE * srcFrameBuffer,BYTE * dstFrameBuffer,int srcFrameWidth,int srcFrameHeight,int rgbType)
{
    int  rgbIncrement =3;
    RGB_OUT*   rgbOutFunc=RGB_OUT_24;
    if(rgbType == _RGB565)
    {
        rgbIncrement =2;
        rgbOutFunc = RGB_OUT_565;
    }
	else if(rgbType == _RGB32)
	{
		rgbIncrement =4;
		rgbOutFunc = RGB_OUT_32;
	}

    BYTE          *dstImageFrame;
	unsigned int   nbytes    = srcFrameWidth*srcFrameHeight;
	const BYTE    *yplane    = srcFrameBuffer;           		// 1 byte Y (luminance) for each pixel
	const BYTE    *uplane    = yplane+nbytes;              	// 1 byte U for a block of 4 pixels
	const BYTE    *vplane    = uplane+(nbytes >> 2);       	// 1 byte V for a block of 4 pixels
	unsigned int   pixpos[4] = { 0, 1, srcFrameWidth, srcFrameWidth+1 };
	unsigned int   x, y, p;
	
	long     int   yvalue;
	long     int   cr, cb, rd, gd, bd;
	long     int   l, r, g, b;
	
    BYTE     *p_dstTemp = new BYTE[srcFrameWidth*rgbIncrement];
	if (!p_dstTemp)
       return 0;

    
    dstImageFrame = dstFrameBuffer;
	
	for (y = 0; y < srcFrameHeight; y += 2)
	{
		for (x = 0; x < srcFrameWidth; x += 2)
		{
			// The RGB value without luminance
			cr = *uplane-128;
			cb = *vplane-128;
	
			rd =  106986*cb;
			gd = -26261*cr  +  -54496*cb ;
			bd = 135221*cr;
			
			// Add luminance to each of the 4 pixels
			for (p = 0; p < 4; p++)
			{
				yvalue = *(yplane+pixpos[p])-16;
				if (yvalue < 0) yvalue = 0;
				
				l = 76310*yvalue;
				
				r = l+rd;
				g = l+gd;
				b = l+bd;
				
                rgbOutFunc(dstImageFrame + rgbIncrement*pixpos[p],LIMIT(r),LIMIT(g),LIMIT(b));
			}
			
			yplane += 2;
			dstImageFrame += rgbIncrement*2;
			
			uplane++;
			vplane++;
		}
		
		yplane += srcFrameWidth;
		
        memcpy(p_dstTemp,dstImageFrame-srcFrameWidth*rgbIncrement,srcFrameWidth*rgbIncrement);
        memcpy(dstFrameBuffer+(srcFrameHeight-y-2)*srcFrameWidth*rgbIncrement,dstImageFrame,srcFrameWidth*rgbIncrement);
		memcpy(dstFrameBuffer+(srcFrameHeight-y-1)*srcFrameWidth*rgbIncrement,p_dstTemp,srcFrameWidth*rgbIncrement);
		dstImageFrame = dstFrameBuffer;
	}
	
	delete []p_dstTemp;
	return srcFrameWidth*srcFrameHeight*3;

}
int YUV420PtoRGB24(const BYTE * srcFrameBuffer,BYTE * dstFrameBuffer,int srcFrameWidth,int srcFrameHeight)
{
	return YUV420PtoRGB(srcFrameBuffer,dstFrameBuffer,srcFrameWidth,srcFrameHeight,_RGB24);
}

int YUV420PtoRGB565(const BYTE * srcFrameBuffer,BYTE * dstFrameBuffer,int srcFrameWidth,int srcFrameHeight)
{
	return YUV420PtoRGB(srcFrameBuffer,dstFrameBuffer,srcFrameWidth,srcFrameHeight,_RGB565);
}

int  YUV420PtoRGB32(const BYTE * srcFrameBuffer,BYTE * dstFrameBuffer,int srcFrameWidth,int srcFrameHeight)
{
    return YUV420PtoRGB(srcFrameBuffer,dstFrameBuffer,srcFrameWidth,srcFrameHeight,_RGB32);
}

static int RGBToYUV420P(const BYTE * rgb,BYTE * yuv,int srcFrameWidth,int srcFrameHeight,int rgbType)
{
    int  rgbIncrement =3;
    RGB_IN*   rgbInFunc=RGB_IN_24;
    if(rgbType == _RGB565)
    {
        rgbIncrement =2;
        rgbInFunc = RGB_IN_565;
    }
	else if(rgbType == _RGB32)
    {
        rgbIncrement =4;
        rgbInFunc = RGB_IN_32;
    }


    unsigned int r,g ,b;
	const unsigned planeSize = srcFrameWidth*srcFrameHeight;
	const unsigned halfWidth = srcFrameWidth >> 1;
	
	// get pointers to the data
	BYTE * yplane  = yuv;
	BYTE * uplane  = yuv + planeSize;
	BYTE * vplane  = yuv + planeSize + (planeSize >> 2);
	const BYTE * rgbIndex = rgb;
	
	for (unsigned y = 0; y < srcFrameHeight; y++) 
	{
		BYTE * yline  = yplane + (y * srcFrameWidth);
		BYTE * uline  = uplane + ((y >> 1) * halfWidth);
		BYTE * vline  = vplane + ((y >> 1) * halfWidth);
		
        rgbIndex = rgb + (srcFrameWidth*(srcFrameHeight-1-y)*rgbIncrement);
		for (unsigned x = 0; x < srcFrameWidth; x+=2) 
		{
            (*rgbInFunc)(r, g, b, rgbIndex);
            rgbtoyuv(r,g,b,*yline, *uline, *vline);
			rgbIndex += rgbIncrement;
			yline++;
			
            (*rgbInFunc)(r, g, b, rgbIndex);
            rgbtoyuv(r,g,b,*yline, *uline, *vline);
			rgbIndex += rgbIncrement;
			yline++;
			
            uline++;
			vline++;
		}
	}
	
	return srcFrameWidth *srcFrameHeight +srcFrameWidth *srcFrameHeight/4 ;
}


int RGB24toYUV420P(const BYTE * rgb,BYTE * yuv,int srcFrameWidth,int srcFrameHeight) 
{
    return RGBToYUV420P(rgb,yuv,srcFrameWidth,srcFrameHeight,_RGB24);
}

int RGB565toYUV420P(const BYTE * rgb,BYTE * yuv,int srcFrameWidth,int srcFrameHeight) 
{
    return RGBToYUV420P(rgb,yuv,srcFrameWidth,srcFrameHeight,_RGB565);
}

int  RGB32toYUV420P(const BYTE * rgb,BYTE * yuv,int srcFrameWidth,int srcFrameHeight)
{
    return RGBToYUV420P(rgb,yuv,srcFrameWidth,srcFrameHeight,_RGB32);
}

bool RGB32ToRGB24(const BYTE * srgb,BYTE * drgb,int srcFrameWidth,int srcFrameHeight)
{
	long dew = srcFrameWidth*3;// ((((24 * srcFrameWidth) + 31) / 32) * 4);
	long sew = srcFrameWidth*4;//((((32 * srcFrameWidth) + 31) / 32) * 4);
	BYTE* psBase = (BYTE*)srgb;
	BYTE * pdBase = drgb;
	BYTE *pSrc = NULL;
	BYTE *pDst = NULL;
	for (int y = 0; y < srcFrameHeight; y ++)
	{
		pSrc = psBase + y*sew;
		pDst = pdBase + y*dew;
		for (int x = 0; x < srcFrameWidth; x ++)
		{
			pDst[0]=pSrc[0];
			pDst[1]=pSrc[1];
			pDst[2]=pSrc[2];
			pSrc += 4;
			pDst += 3;
		}
	}
	return true;
}
bool RGB24ToRGB32(const BYTE * srgb,BYTE * drgb,int srcFrameWidth,int srcFrameHeight)
{
	long sew = srcFrameWidth*3;//((((24 * srcFrameWidth) + 31) / 32) * 4);
	long dew = srcFrameWidth*4;//((((32 * srcFrameWidth) + 31) / 32) * 4);
	BYTE* psBase = (BYTE*)srgb;
	BYTE * pdBase = drgb;
	BYTE *pSrc = NULL;
	BYTE *pDst = NULL;

	for (int y = 0; y < srcFrameHeight; y ++)
	{
		pSrc = psBase + y*sew;
		pDst = pdBase + y*dew;
		for (int x = 0; x < srcFrameWidth; x ++)
		{
			pDst[0]=pSrc[0];
			pDst[1]=pSrc[1];
			pDst[2]=pSrc[2];
			pDst[0]=0;
			pSrc += 3;
			pDst += 4;
		}
	}

	return true;
}
