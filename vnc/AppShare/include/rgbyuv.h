#ifndef RGB_YUV_CONVERT_H
#define RGB_YUV_CONVERT_H

void Rgb565ToRgb24(unsigned char* dst, const unsigned char* src,int width, int height);
void Rgb24ToRgb565(unsigned char* dst, const unsigned char* src,int width, int height);
int  YUV420PtoRGB565(const BYTE * srcFrameBuffer,BYTE * dstFrameBuffer,int srcFrameWidth,int srcFrameHeight);
int  YUV420PtoRGB24(const BYTE * srcFrameBuffer,BYTE * dstFrameBuffer,int srcFrameWidth,int srcFrameHeight);
int  YUV420PtoRGB32(const BYTE * srcFrameBuffer,BYTE * dstFrameBuffer,int srcFrameWidth,int srcFrameHeight);
int  RGB24toYUV420P(const BYTE * rgb,BYTE * yuv,int srcFrameWidth,int srcFrameHeight);
int  RGB565toYUV420P(const BYTE * rgb,BYTE * yuv,int srcFrameWidth,int srcFrameHeight);
int  RGB32toYUV420P(const BYTE * rgb,BYTE * yuv,int srcFrameWidth,int srcFrameHeight);

bool RGB32ToRGB24(const BYTE * srgb,BYTE * drgb,int srcFrameWidth,int srcFrameHeight);
bool RGB24ToRGB32(const BYTE * srgb,BYTE * drgb,int srcFrameWidth,int srcFrameHeight);

#endif
