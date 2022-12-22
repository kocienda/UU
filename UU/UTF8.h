//
//  UTF8.h
//
//  License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1999-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*
*   created on: 1999sep13
*   created by: Markus W. Scherer
*/

#ifndef UU_UTF8_H
#define UU_UTF8_H

/* internal definitions ----------------------------------------------------- */

#define U8_COUNT_TRAIL_BYTES(leadByte) \
    (U8_IS_LEAD(leadByte) ? \
        ((uint8_t)(leadByte)>=0xe0)+((uint8_t)(leadByte)>=0xf0)+1 : 0)

#define U8_COUNT_TRAIL_BYTES_UNSAFE(leadByte) \
    (((uint8_t)(leadByte)>=0xc2)+((uint8_t)(leadByte)>=0xe0)+((uint8_t)(leadByte)>=0xf0))

#define U8_MASK_LEAD_BYTE(leadByte, countTrailBytes) ((leadByte)&=(1<<(6-(countTrailBytes)))-1)

#define U8_LEAD3_T1_BITS "\x20\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x10\x30\x30"

#define U8_IS_VALID_LEAD3_AND_T1(lead, t1) (U8_LEAD3_T1_BITS[(lead)&0xf]&(1<<((uint8_t)(t1)>>5)))

#define U8_LEAD4_T1_BITS "\x00\x00\x00\x00\x00\x00\x00\x00\x1E\x0F\x0F\x0F\x00\x00\x00\x00"

#define U8_IS_VALID_LEAD4_AND_T1(lead, t1) (U8_LEAD4_T1_BITS[(uint8_t)(t1)>>4]&(1<<((lead)&7)))

/* single-code point definitions -------------------------------------------- */

#define U8_IS_SINGLE(c) (((c)&0x80)==0)

#define U8_IS_LEAD(c) ((uint8_t)((c)-0xc2)<=0x32)
// 0x32=0xf4-0xc2

#define U8_IS_TRAIL(c) ((int8_t)(c)<-0x40)

#define U8_LENGTH(c) \
    ((uint32_t)(c)<=0x7f ? 1 : \
        ((uint32_t)(c)<=0x7ff ? 2 : \
            ((uint32_t)(c)<=0xd7ff ? 3 : \
                ((uint32_t)(c)<=0xdfff || (uint32_t)(c)>0x10ffff ? 0 : \
                    ((uint32_t)(c)<=0xffff ? 3 : 4)\
                ) \
            ) \
        ) \
    )

#define U8_MAX_LENGTH 4

/* definitions with forward iteration --------------------------------------- */

#define U8_NEXT_UNSAFE(s, i, c) { \
    (c)=(uint8_t)(s)[(i)++]; \
    if(!U8_IS_SINGLE(c)) { \
        if((c)<0xe0) { \
            (c)=(((c)&0x1f)<<6)|((s)[(i)++]&0x3f); \
        } else if((c)<0xf0) { \
            /* no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (UChar) */ \
            (c)=(UChar)(((c)<<12)|(((s)[i]&0x3f)<<6)|((s)[(i)+1]&0x3f)); \
            (i)+=2; \
        } else { \
            (c)=(((c)&7)<<18)|(((s)[i]&0x3f)<<12)|(((s)[(i)+1]&0x3f)<<6)|((s)[(i)+2]&0x3f); \
            (i)+=3; \
        } \
    } \
}

#define U8_NEXT(s, i, length, c) U8_INTERNAL_NEXT_OR_SUB(s, i, length, c, 0xfffd)

#define U8_NEXT_OR_FFFD(s, i, length, c) U8_INTERNAL_NEXT_OR_SUB(s, i, length, c, 0xfffd)

#define U8_INTERNAL_NEXT_OR_SUB(s, i, length, c, sub) { \
    (c)=(uint8_t)(s)[(i)++]; \
    if(!U8_IS_SINGLE(c)) { \
        uint8_t __t = 0; \
        if((i)!=(length) && \
            /* fetch/validate/assemble all but last trail byte */ \
            ((c)>=0xe0 ? \
                ((c)<0xf0 ?  /* U+0800..U+FFFF except surrogates */ \
                    U8_LEAD3_T1_BITS[(c)&=0xf]&(1<<((__t=(s)[i])>>5)) && \
                    (__t&=0x3f, 1) \
                :  /* U+10000..U+10FFFF */ \
                    ((c)-=0xf0)<=4 && \
                    U8_LEAD4_T1_BITS[(__t=(s)[i])>>4]&(1<<(c)) && \
                    ((c)=((c)<<6)|(__t&0x3f), ++(i)!=(length)) && \
                    (__t=(s)[i]-0x80)<=0x3f) && \
                /* valid second-to-last trail byte */ \
                ((c)=((c)<<6)|__t, ++(i)!=(length)) \
            :  /* U+0080..U+07FF */ \
                (c)>=0xc2 && ((c)&=0x1f, 1)) && \
            /* last trail byte */ \
            (__t=(s)[i]-0x80)<=0x3f && \
            ((c)=((c)<<6)|__t, ++(i), 1)) { \
        } else { \
            (c)=(sub);  /* ill-formed*/ \
        } \
    } \
}

#define U8_APPEND_UNSAFE(s, i, c) { \
    uint32_t __uc=(c); \
    if(__uc<=0x7f) { \
        (s)[(i)++]=(uint8_t)__uc; \
    } else { \
        if(__uc<=0x7ff) { \
            (s)[(i)++]=(uint8_t)((__uc>>6)|0xc0); \
        } else { \
            if(__uc<=0xffff) { \
                (s)[(i)++]=(uint8_t)((__uc>>12)|0xe0); \
            } else { \
                (s)[(i)++]=(uint8_t)((__uc>>18)|0xf0); \
                (s)[(i)++]=(uint8_t)(((__uc>>12)&0x3f)|0x80); \
            } \
            (s)[(i)++]=(uint8_t)(((__uc>>6)&0x3f)|0x80); \
        } \
        (s)[(i)++]=(uint8_t)((__uc&0x3f)|0x80); \
    } \
}

#define U8_APPEND(s, i, capacity, c, isError) { \
    uint32_t __uc=(c); \
    if(__uc<=0x7f) { \
        (s)[(i)++]=(uint8_t)__uc; \
    } else if(__uc<=0x7ff && (i)+1<(capacity)) { \
        (s)[(i)++]=(uint8_t)((__uc>>6)|0xc0); \
        (s)[(i)++]=(uint8_t)((__uc&0x3f)|0x80); \
    } else if((__uc<=0xd7ff || (0xe000<=__uc && __uc<=0xffff)) && (i)+2<(capacity)) { \
        (s)[(i)++]=(uint8_t)((__uc>>12)|0xe0); \
        (s)[(i)++]=(uint8_t)(((__uc>>6)&0x3f)|0x80); \
        (s)[(i)++]=(uint8_t)((__uc&0x3f)|0x80); \
    } else if(0xffff<__uc && __uc<=0x10ffff && (i)+3<(capacity)) { \
        (s)[(i)++]=(uint8_t)((__uc>>18)|0xf0); \
        (s)[(i)++]=(uint8_t)(((__uc>>12)&0x3f)|0x80); \
        (s)[(i)++]=(uint8_t)(((__uc>>6)&0x3f)|0x80); \
        (s)[(i)++]=(uint8_t)((__uc&0x3f)|0x80); \
    } else { \
        (isError)=TRUE; \
    } \
}

#define U8_FWD_1_UNSAFE(s, i) { \
    (i)+=1+U8_COUNT_TRAIL_BYTES_UNSAFE((s)[i]); \
}

#define U8_FWD_1(s, i, length) { \
    uint8_t __b=(s)[(i)++]; \
    if(U8_IS_LEAD(__b) && (i)!=(length)) { \
        uint8_t __t1=(s)[i]; \
        if((0xe0<=__b && __b<0xf0)) { \
            if(U8_IS_VALID_LEAD3_AND_T1(__b, __t1) && \
                    ++(i)!=(length) && U8_IS_TRAIL((s)[i])) { \
                ++(i); \
            } \
        } else if(__b<0xe0) { \
            if(U8_IS_TRAIL(__t1)) { \
                ++(i); \
            } \
        } else /* c>=0xf0 */ { \
            if(U8_IS_VALID_LEAD4_AND_T1(__b, __t1) && \
                    ++(i)!=(length) && U8_IS_TRAIL((s)[i]) && \
                    ++(i)!=(length) && U8_IS_TRAIL((s)[i])) { \
                ++(i); \
            } \
        } \
    } \
}

#define U8_FWD_N_UNSAFE(s, i, n) { \
    int32_t __N=(n); \
    while(__N>0) { \
        U8_FWD_1_UNSAFE(s, i); \
        --__N; \
    } \
}

#define U8_FWD_N(s, i, length, n) { \
    int32_t __N=(n); \
    while(__N>0 && ((i)<(length) || ((length)<0 && (s)[i]!=0))) { \
        U8_FWD_1(s, i, length); \
        --__N; \
    } \
}

#define U8_SET_CP_START_UNSAFE(s, i) { \
    while(U8_IS_TRAIL((s)[i])) { --(i); } \
}

#define U8_TRUNCATE_IF_INCOMPLETE(s, start, length) \
    if((length)>(start)) { \
        uint8_t __b1=s[(length)-1]; \
        if(U8_IS_SINGLE(__b1)) { \
            /* common ASCII character */ \
        } else if(U8_IS_LEAD(__b1)) { \
            --(length); \
        } else if(U8_IS_TRAIL(__b1) && ((length)-2)>=(start)) { \
            uint8_t __b2=s[(length)-2]; \
            if(0xe0<=__b2 && __b2<=0xf4) { \
                if(__b2<0xf0 ? U8_IS_VALID_LEAD3_AND_T1(__b2, __b1) : \
                        U8_IS_VALID_LEAD4_AND_T1(__b2, __b1)) { \
                    (length)-=2; \
                } \
            } else if(U8_IS_TRAIL(__b2) && ((length)-3)>=(start)) { \
                uint8_t __b3=s[(length)-3]; \
                if(0xf0<=__b3 && __b3<=0xf4 && U8_IS_VALID_LEAD4_AND_T1(__b3, __b2)) { \
                    (length)-=3; \
                } \
            } \
        } \
    }

/* definitions with backward iteration -------------------------------------- */

#define U8_PREV_UNSAFE(s, i, c) { \
    (c)=(uint8_t)(s)[--(i)]; \
    if(U8_IS_TRAIL(c)) { \
        uint8_t __b, __count=1, __shift=6; \
\
        /* c is a trail byte */ \
        (c)&=0x3f; \
        for(;;) { \
            __b=(s)[--(i)]; \
            if(__b>=0xc0) { \
                U8_MASK_LEAD_BYTE(__b, __count); \
                (c)|=(UChar32)__b<<__shift; \
                break; \
            } else { \
                (c)|=(UChar32)(__b&0x3f)<<__shift; \
                ++__count; \
                __shift+=6; \
            } \
        } \
    } \
}

#define U8_BACK_1_UNSAFE(s, i) { \
    while(U8_IS_TRAIL((s)[--(i)])) {} \
}

#define U8_BACK_1(s, start, i) { \
    if(U8_IS_TRAIL((s)[--(i)])) { \
        (i)=utf8_back1SafeBody(s, start, (i)); \
    } \
}

#define U8_BACK_N_UNSAFE(s, i, n) { \
    int32_t __N=(n); \
    while(__N>0) { \
        U8_BACK_1_UNSAFE(s, i); \
        --__N; \
    } \
}

#define U8_BACK_N(s, start, i, n) { \
    int32_t __N=(n); \
    while(__N>0 && (i)>(start)) { \
        U8_BACK_1(s, start, i); \
        --__N; \
    } \
}

#define U8_SET_CP_LIMIT_UNSAFE(s, i) { \
    U8_BACK_1_UNSAFE(s, i); \
    U8_FWD_1_UNSAFE(s, i); \
}

#define U8_SET_CP_LIMIT(s, start, i, length) { \
    if((start)<(i) && ((i)<(length) || (length)<0)) { \
        U8_BACK_1(s, start, i); \
        U8_FWD_1(s, i, length); \
    } \
}

#endif  // UU_UTF8_H
