#include <windows.h>
#include "debug.h"
#include "scale_pattern.h"
#include "blt.h"


void blt_clean(
    unsigned char* dst,
    int dst_x,
    int dst_y,
    int dst_w,
    int dst_h,
    int dst_p,
    unsigned char* src,
    int src_x,
    int src_y,
    int src_p,
    int bpp)
{
    int bytes_pp = bpp / 8;

    size_t size = dst_w * bytes_pp;

    src += (src_x * bytes_pp) + (src_p * src_y);
    dst += (dst_x * bytes_pp) + (dst_p * dst_y);

    for (int i = 0; i < dst_h; i++)
    {
        memcpy(dst, src, size);

        src += src_p;
        dst += dst_p;
    }
}

void blt_overlap(
    unsigned char* dst,
    int dst_x,
    int dst_y,
    int dst_w,
    int dst_h,
    int dst_p,
    unsigned char* src,
    int src_x,
    int src_y,
    int src_p,
    int bpp)
{
    int bytes_pp = bpp / 8;

    size_t size = dst_w * bytes_pp;

    src += (src_x * bytes_pp) + (src_p * src_y);
    dst += (dst_x * bytes_pp) + (dst_p * dst_y);

    if (dst_y > src_y)
    {
        src += src_p * dst_h;
        dst += dst_p * dst_h;

        for (int i = dst_h; i-- > 0;)
        {
            src -= src_p;
            dst -= dst_p;

            memmove(dst, src, size);
        }
    }
    else
    {
        for (int i = 0; i < dst_h; i++)
        {
            memmove(dst, src, size);

            src += src_p;
            dst += dst_p;
        }
    }
}

void blt_colorkey(
    unsigned char* dst,
    int dst_x,
    int dst_y,
    int dst_w,
    int dst_h,
    int dst_p,
    unsigned char* src,
    int src_x,
    int src_y,
    int src_p,
    unsigned int key_low,
    unsigned int key_high,
    int bpp)
{
    int bytes_pp = bpp / 8;

    size_t s_a = (src_p / bytes_pp) - dst_w;
    size_t d_a = (dst_p / bytes_pp) - dst_w;

    src += (src_x * bytes_pp) + (src_p * src_y);
    dst += (dst_x * bytes_pp) + (dst_p * dst_y);

    if (bpp == 8)
    {
        unsigned char key_l = (unsigned char)key_low;
        unsigned char key_h = (unsigned char)key_high;

        if (key_l == key_h)
        {
            for (int y = 0; y < dst_h; y++)
            {
                for (int x = 0; x < dst_w; x++)
                {
                    unsigned char c = *src++;

                    if (c != key_l)
                    {
                        *dst = c;
                    }

                    dst++;
                }

                src += s_a;
                dst += d_a;
            }
        }
        else
        {
            for (int y = 0; y < dst_h; y++)
            {
                for (int x = 0; x < dst_w; x++)
                {
                    unsigned char c = *src++;

                    if (c < key_l || c > key_h)
                    {
                        *dst = c;
                    }

                    dst++;
                }

                src += s_a;
                dst += d_a;
            }
        }
    }
    else if (bpp == 16)
    {
        unsigned short key_l = (unsigned short)key_low;
        unsigned short key_h = (unsigned short)key_high;

        unsigned short* d = (unsigned short*)dst;
        unsigned short* s = (unsigned short*)src;

        if (key_l == key_h)
        {
            for (int y = 0; y < dst_h; y++)
            {
                for (int x = 0; x < dst_w; x++)
                {
                    unsigned short c = *s++;

                    if (c != key_l)
                    {
                        *d = c;
                    }

                    d++;
                }

                s += s_a;
                d += d_a;
            }
        }
        else
        {
            for (int y = 0; y < dst_h; y++)
            {
                for (int x = 0; x < dst_w; x++)
                {
                    unsigned short c = *s++;

                    if (c < key_l || c > key_h)
                    {
                        *d = c;
                    }

                    d++;
                }

                s += s_a;
                d += d_a;
            }
        }
    }
    else if (bpp == 32)
    {
        unsigned int key_l = (unsigned int)key_low;
        unsigned int key_h = (unsigned int)key_high;

        unsigned int* d = (unsigned int*)dst;
        unsigned int* s = (unsigned int*)src;

        if (key_l == key_h)
        {
            for (int y = 0; y < dst_h; y++)
            {
                for (int x = 0; x < dst_w; x++)
                {
                    unsigned int c = *s++;

                    if (c != key_l)
                    {
                        *d = c;
                    }

                    d++;
                }

                s += s_a;
                d += d_a;
            }
        }
        else
        {
            for (int y = 0; y < dst_h; y++)
            {
                for (int x = 0; x < dst_w; x++)
                {
                    unsigned int c = *s++;

                    if (c < key_l || c > key_h)
                    {
                        *d = c;
                    }

                    d++;
                }

                s += s_a;
                d += d_a;
            }
        }
    }
}

void blt_colorkey_mirror_stretch(
    unsigned char* dst,
    int dst_x,
    int dst_y,
    int dst_w,
    int dst_h,
    int dst_p,
    unsigned char* src,
    int src_x,
    int src_y,
    int src_w,
    int src_h,
    int src_p,
    unsigned int key_low,
    unsigned int key_high,
    BOOL mirror_up_down,
    BOOL mirror_left_right,
    int bpp)
{
    int bytes_pp = bpp / 8;

    int dst_surf_w = dst_p / bytes_pp;
    int src_surf_w = src_p / bytes_pp;

    float scale_w = (float)src_w / dst_w;
    float scale_h = (float)src_h / dst_h;

    if (bpp == 8)
    {
        unsigned char key_l = (unsigned char)key_low;
        unsigned char key_h = (unsigned char)key_high;

        for (int y = 0; y < dst_h; y++)
        {
            int scaled_y = (int)(y * scale_h);

            if (mirror_up_down)
                scaled_y = src_h - 1 - scaled_y;

            int src_row = src_surf_w * (scaled_y + src_y);
            int dst_row = dst_surf_w * (y + dst_y);

            for (int x = 0; x < dst_w; x++)
            {
                int scaled_x = (int)(x * scale_w);

                if (mirror_left_right)
                    scaled_x = src_w - 1 - scaled_x;

                unsigned char c = ((unsigned char*)src)[scaled_x + src_x + src_row];

                if (c < key_l || c > key_h)
                {
                    ((unsigned char*)dst)[x + dst_x + dst_row] = c;
                }
            }
        }
    }
    else if (bpp == 16)
    {
        unsigned short key_l = (unsigned short)key_low;
        unsigned short key_h = (unsigned short)key_high;

        for (int y = 0; y < dst_h; y++)
        {
            int scaled_y = (int)(y * scale_h);

            if (mirror_up_down)
                scaled_y = src_h - 1 - scaled_y;

            int src_row = src_surf_w * (scaled_y + src_y);
            int dst_row = dst_surf_w * (y + dst_y);

            for (int x = 0; x < dst_w; x++)
            {
                int scaled_x = (int)(x * scale_w);

                if (mirror_left_right)
                    scaled_x = src_w - 1 - scaled_x;

                unsigned short c = ((unsigned short*)src)[scaled_x + src_x + src_row];

                if (c < key_l || c > key_h)
                {
                    ((unsigned short*)dst)[x + dst_x + dst_row] = c;
                }
            }
        }
    }
    else if (bpp == 32)
    {
        unsigned int key_l = (unsigned int)key_low;
        unsigned int key_h = (unsigned int)key_high;

        for (int y = 0; y < dst_h; y++)
        {
            int scaled_y = (int)(y * scale_h);

            if (mirror_up_down)
                scaled_y = src_h - 1 - scaled_y;

            int src_row = src_surf_w * (scaled_y + src_y);
            int dst_row = dst_surf_w * (y + dst_y);

            for (int x = 0; x < dst_w; x++)
            {
                int scaled_x = (int)(x * scale_w);

                if (mirror_left_right)
                    scaled_x = src_w - 1 - scaled_x;

                unsigned int c = ((unsigned int*)src)[scaled_x + src_x + src_row];

                if (c < key_l || c > key_h)
                {
                    ((unsigned int*)dst)[x + dst_x + dst_row] = c;
                }
            }
        }
    }
}

void blt_colorfill(
    unsigned char* dst,
    int dst_x,
    int dst_y,
    int dst_w,
    int dst_h,
    int dst_p,
    unsigned int color,
    int bpp)
{
    int bytes_pp = bpp / 8;

    size_t size = dst_w * bytes_pp;

    dst += (dst_x * bytes_pp) + (dst_p * dst_y);

    if (bpp == 8)
    {
        for (int i = 0; i < dst_h; i++)
        {
            memset(dst, color, size);
            dst += dst_p;
        }
    }
    else if (bpp == 16)
    {
        if ((color & 0xFF) == ((color >> 8) & 0xFF))
        {
            for (int i = 0; i < dst_h; i++)
            {
                memset(dst, color, size);
                dst += dst_p;
            }
        }
        else
        {
            unsigned short* first_row = (unsigned short*)dst;

            for (int x = 0; x < dst_w; x++)
            {
                first_row[x] = (unsigned short)color;
            }

            for (int i = 1; i < dst_h; i++)
            {
                dst += dst_p;
                memcpy(dst, first_row, size);
            }
        }
    }
    else if (bpp == 32)
    {
        if ((color & 0xFF) == ((color >> 8) & 0xFF) &&
            (color & 0xFF) == ((color >> 16) & 0xFF) &&
            (color & 0xFF) == ((color >> 24) & 0xFF))
        {
            for (int i = 0; i < dst_h; i++)
            {
                memset(dst, color, size);
                dst += dst_p;
            }
        }
        else
        {
            unsigned int* first_row = (unsigned int*)dst;

            for (int x = 0; x < dst_w; x++)
            {
                first_row[x] = color;
            }

            for (int i = 1; i < dst_h; i++)
            {
                dst += dst_p;
                memcpy(dst, first_row, size);
            }
        }
    }
}

void blt_rgb565_to_rgba8888(
    unsigned int* dst,
    int dst_x,
    int dst_y,
    int dst_w,
    int dst_h,
    int dst_p,
    unsigned short* src,
    int src_x,
    int src_y,
    int src_p)
{
    size_t s_a = (src_p / sizeof(src[0])) - dst_w;
    size_t d_a = (dst_p / sizeof(dst[0])) - dst_w;

    src += (src_x * sizeof(src[0])) + (src_p * src_y);
    dst += (dst_x * sizeof(dst[0])) + (dst_p * dst_y);

    for (int y = 0; y < dst_h; y++)
    {
        for (int x = 0; x < dst_w; x++)
        {
            unsigned short pixel = *src++;

            BYTE r = ((pixel & 0xF800) >> 11) << 3;
            BYTE g = ((pixel & 0x07E0) >> 5) << 2;
            BYTE b = ((pixel & 0x001F)) << 3;

            *dst++ = (0xFF << 24) | (b << 16) | (g << 8) | r;
        }

        src += s_a;
        dst += d_a;
    }
}

void blt_bgra8888_to_rgba8888(
    unsigned int* dst,
    int dst_x,
    int dst_y,
    int dst_w,
    int dst_h,
    int dst_p,
    unsigned int* src,
    int src_x,
    int src_y,
    int src_p)
{
    size_t s_a = (src_p / sizeof(src[0])) - dst_w;
    size_t d_a = (dst_p / sizeof(dst[0])) - dst_w;

    src += (src_x * sizeof(src[0])) + (src_p * src_y);
    dst += (dst_x * sizeof(dst[0])) + (dst_p * dst_y);

    for (int y = 0; y < dst_h; y++)
    {
        for (int x = 0; x < dst_w; x++)
        {
            unsigned int pixel = *src++;

            BYTE r = pixel >> 16;
            BYTE g = pixel >> 8;
            BYTE b = pixel;

            *dst++ = (0xFF << 24) | (b << 16) | (g << 8) | r;
        }

        src += s_a;
        dst += d_a;
    }
}

void blt_stretch(
    unsigned char* dst_buf,
    int dst_x,
    int dst_y,
    int dst_w,
    int dst_h,
    int dst_p,
    unsigned char* src_buf,
    int src_x,
    int src_y,
    int src_w,
    int src_h,
    int src_p,
    int bpp)
{
    /* Linear scaling using integer math
    * Since the scaling pattern for x will aways be the same, the pattern itself gets pre-calculated
    * and stored in an array.
    * Y scaling pattern gets calculated during the blit loop
    */
    unsigned int x_ratio = (unsigned int)((src_w << 16) / dst_w) + 1;
    unsigned int y_ratio = (unsigned int)((src_h << 16) / dst_h) + 1;

    unsigned int s_src_x, s_src_y;
    unsigned int dest_base, source_base;

    scale_pattern* pattern = malloc((dst_w + 1) * (sizeof(scale_pattern)));
    int pattern_idx = 0;
    unsigned int last_src_x = 0;

    if (pattern != NULL)
    {
        pattern[pattern_idx] = (scale_pattern){ ONCE, 0, 0, 1 };

        /* Build the pattern! */
        int x;
        for (x = 1; x < dst_w; x++) {
            s_src_x = (x * x_ratio) >> 16;
            if (s_src_x == last_src_x)
            {
                if (pattern[pattern_idx].type == REPEAT || pattern[pattern_idx].type == ONCE)
                {
                    pattern[pattern_idx].type = REPEAT;
                    pattern[pattern_idx].count++;
                }
                else if (pattern[pattern_idx].type == SEQUENCE)
                {
                    pattern_idx++;
                    pattern[pattern_idx] = (scale_pattern){ REPEAT, x, s_src_x, 1 };
                }
            }
            else if (s_src_x == last_src_x + 1)
            {
                if (pattern[pattern_idx].type == SEQUENCE || pattern[pattern_idx].type == ONCE)
                {
                    pattern[pattern_idx].type = SEQUENCE;
                    pattern[pattern_idx].count++;
                }
                else if (pattern[pattern_idx].type == REPEAT)
                {
                    pattern_idx++;
                    pattern[pattern_idx] = (scale_pattern){ ONCE, x, s_src_x, 1 };
                }
            }
            else
            {
                pattern_idx++;
                pattern[pattern_idx] = (scale_pattern){ ONCE, x, s_src_x, 1 };
            }
            last_src_x = s_src_x;
        }
        pattern[pattern_idx + 1] = (scale_pattern){ END, 0, 0, 0 };


        /* Do the actual blitting */
        int bytes_pp = bpp / 8;
        int dst_surf_w = dst_p / bytes_pp;
        int src_surf_w = src_p / bytes_pp;

        int count = 0;
        int y;

        for (y = 0; y < dst_h; y++) {

            dest_base = dst_x + dst_surf_w * (y + dst_y);

            s_src_y = (y * y_ratio) >> 16;

            source_base = src_x + src_surf_w * (s_src_y + src_y);

            pattern_idx = 0;
            scale_pattern* current = &pattern[pattern_idx];

            if (bpp == 8)
            {
                unsigned char* d, * s, v;
                unsigned char* src = (unsigned char*)src_buf;
                unsigned char* dst = (unsigned char*)dst_buf;

                do {
                    switch (current->type)
                    {
                    case ONCE:
                        dst[dest_base + current->dst_index] =
                            src[source_base + current->src_index];
                        break;

                    case REPEAT:
                        d = (dst + dest_base + current->dst_index);
                        v = src[source_base + current->src_index];

                        count = current->count;
                        while (count-- > 0)
                            *d++ = v;

                        break;

                    case SEQUENCE:
                        d = dst + dest_base + current->dst_index;
                        s = src + source_base + current->src_index;

                        memcpy((void*)d, (void*)s, current->count);
                        break;

                    case END:
                    default:
                        break;
                    }

                    current = &pattern[++pattern_idx];
                } while (current->type != END);
            }
            else if (bpp == 16)
            {
                unsigned short* d, * s, v;
                unsigned short* src = (unsigned short*)src_buf;
                unsigned short* dst = (unsigned short*)dst_buf;

                do {
                    switch (current->type)
                    {
                    case ONCE:
                        dst[dest_base + current->dst_index] =
                            src[source_base + current->src_index];
                        break;

                    case REPEAT:
                        d = (dst + dest_base + current->dst_index);
                        v = src[source_base + current->src_index];

                        count = current->count;
                        while (count-- > 0)
                            *d++ = v;

                        break;

                    case SEQUENCE:
                        d = dst + dest_base + current->dst_index;
                        s = src + source_base + current->src_index;

                        memcpy((void*)d, (void*)s, current->count * bytes_pp);
                        break;

                    case END:
                    default:
                        break;
                    }

                    current = &pattern[++pattern_idx];
                } while (current->type != END);
            }
            else if (bpp == 32)
            {
                unsigned int* d, * s, v;
                unsigned int* src = (unsigned int*)src_buf;
                unsigned int* dst = (unsigned int*)dst_buf;

                do {
                    switch (current->type)
                    {
                    case ONCE:
                        dst[dest_base + current->dst_index] =
                            src[source_base + current->src_index];
                        break;

                    case REPEAT:
                        d = (dst + dest_base + current->dst_index);
                        v = src[source_base + current->src_index];

                        count = current->count;
                        while (count-- > 0)
                            *d++ = v;

                        break;

                    case SEQUENCE:
                        d = dst + dest_base + current->dst_index;
                        s = src + source_base + current->src_index;

                        memcpy((void*)d, (void*)s, current->count * bytes_pp);
                        break;

                    case END:
                    default:
                        break;
                    }

                    current = &pattern[++pattern_idx];
                } while (current->type != END);
            }
        }
        free(pattern);
    }

}
