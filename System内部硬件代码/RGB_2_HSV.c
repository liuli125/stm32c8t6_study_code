#include "RGB_2_HSV.h" 

/*  RGB转换为HSV
    Red 红，Green 绿， Blue 蓝
    Hue 色相（0-360）, Saturation 饱和度（0.0-1.0）, Value 亮度（0.0-1.0）
    通过调整色相数值，可以单变量实现颜色变化/循环
*/

float Max(float a, float b, float c)    //求最大值
{
	float m;
    m = a < b ? a : b;
    return (m < c ? m : c); 
}

float Min(float a, float b, float c)    //求最小值
{
    float m;
    m = a > b ? a : b;
    return (m > c ? m : c);
}

void RGB_to_HSV(uint8_t red, uint8_t green, uint8_t blue, float *h, float *s, float *v)
{
    float r, g, b;
    float max, min, delta;
    r = (float)red /255;
    g = (float)green /255;
    b = (float)blue /255;
    max = Max(r, g, b);
    min = Min(r, g, b);
    delta = max - min;
    //计算Hue
    if(delta == 0){
        *h = 0;
    }
    else if(max == r){
        *h = 60 * ((g - b)/ delta);
    }
    else if(max == g){
        *h = (60 * ((b - r)/ delta )) + 120;
    }else if(max == b){
        *h = (60 * ((r - g)/ delta )) + 240;
    }
    
    if(*h < 0){
        *h = *h + 360;
    }
    //计算Saturation
    if (max == 0){
        *s = 0;
    }
    else{
        *s = delta/max;
    }
    //计算Value
    *v = max;
}

void HSV_to_RGB(uint16_t Hue, float s, float v, uint8_t *r, uint8_t *g, uint8_t *b)
{
	Hue = Hue % 360;    // H范围限定为0~360
	if (s < 0.0f) s = 0.0f;
    if (s > 1.0f) s = 1.0f;  // S范围限定为0.0~1.0
    if (v < 0.0f) v = 0.0f;
    if (v > 1.0f) v = 1.0f;  // V范围限定为0.0~1.0
    float h, f, p, q, t;
	
	h = Hue/60.0f;
    uint8_t i = (uint8_t)h;
    f = h - i;
    p = v * (1 - s);
    q = v * (1 - f * s);
    t = v * (1 - s * (1 - f));
    if(s == 0){
        *r = *g = *b = (uint8_t)(v * 255.0f + 0.5f);
    }else{
        switch(i)
        {
            case 0:
                *r = (int)(255.0 * v + 0.5);
                *g = (int)(255.0 * t + 0.5);
                *b = (int)(255.0 * p + 0.5);
                break;
            case 1:
                *r = (int)(255.0 * q + 0.5);
                *g = (int)(255.0 * v + 0.5);
                *b = (int)(255.0 * p + 0.5);
                break;
            case 2:
                *r = (int)(255.0 * p + 0.5);
                *g = (int)(255.0 * v + 0.5);
                *b = (int)(255.0 * t + 0.5);
                break;
            case 3:
                *r = (int)(255.0 * p + 0.5);
                *g = (int)(255.0 * q + 0.5);
                *b = (int)(255.0 * v + 0.5);
                break;
            case 4:
                *r = (int)(255.0 * t + 0.5);
                *g = (int)(255.0 * p + 0.5);
                *b = (int)(255.0 * v + 0.5);
                break;
            case 5:
                *r = (int)(255.0 * v + 0.5);
                *g = (int)(255.0 * p + 0.5);
                *b = (int)(255.0 * q + 0.5);
                break;
        }
    }
}





