#ifndef __PROD_H__
#define __PROD_H__

/*****************
 * define profiles
 *****************/
#if defined(PPT)
    #define KEYING_ON 1
    #define KEYING_OFF 0

    #define TXING_ON 1
    #define TXING_OFF 0

    #define TXING_AS_MUTE 0
#elif defined(PPM)
    #define KEYING_ON 1
    #define KEYING_OFF 0

    #define TXING_ON 1
    #define TXING_OFF 0

    #define TXING_AS_MUTE 1
#elif defined(PNT)
    #define KEYING_ON 1
    #define KEYING_OFF 0

    #define TXING_ON 0
    #define TXING_OFF 1

    #define TXING_AS_MUTE 0
#elif defined(PNM)
    #define KEYING_ON 1
    #define KEYING_OFF 0

    #define TXING_ON 0
    #define TXING_OFF 1

    #define TXING_AS_MUTE 1
#elif defined(NNT)
    #define KEYING_ON 0
    #define KEYING_OFF 1

    #define TXING_ON 0
    #define TXING_OFF 1

    #define TXING_AS_MUTE 0
#else
    #error "Wrong profile!"
#endif

#endif // __PROD_H__
