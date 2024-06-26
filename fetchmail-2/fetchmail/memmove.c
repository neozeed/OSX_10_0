
char *memmove(char *dst, register char *src, register int n)
{
    register char *svdst;

    if ((dst > src) && (dst < src + n)) 
    {
        src += n;
        for (svdst = dst + n; n-- > 0; )
            *--svdst = *--src;
    }
    else
    {
        for (svdst = dst; n-- > 0; )
            *svdst++ = *src++;
    }
    return dst;
}
