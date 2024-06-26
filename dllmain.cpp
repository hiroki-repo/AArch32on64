﻿// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"
#include "stdlib.h"
#include "windows.h"

#pragma warning(disable : 4081);
#pragma warning(disable : 4309);
#pragma warning(disable : 4312);
#pragma warning(disable : 4838);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

struct AA32Reg {
    UINT64 NZCV;
    UINT64 VREG64[32];
    UINT32 REG32[16];
    UINT32 TranslatedCodeFlag[1048576][2];
    void* TranslatedCodePtr[1048576][2];
};

static unsigned long CRC32Table[256] = {
    0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9,
    0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005,
    0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61,
    0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
    0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9,
    0x5F15ADAC, 0x5BD4B01B, 0x569796C2, 0x52568B75,
    0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011,
    0x791D4014, 0x7DDC5DA3, 0x709F7B7A, 0x745E66CD,
    0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039,
    0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5,
    0xBE2B5B58, 0xBAEA46EF, 0xB7A96036, 0xB3687D81,
    0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D,
    0xD4326D90, 0xD0F37027, 0xDDB056FE, 0xD9714B49,
    0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
    0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1,
    0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D,

    0x34867077, 0x30476DC0, 0x3D044B19, 0x39C556AE,
    0x278206AB, 0x23431B1C, 0x2E003DC5, 0x2AC12072,
    0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16,
    0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA,
    0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE,
    0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02,
    0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1, 0x53DC6066,
    0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
    0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E,
    0xBFA1B04B, 0xBB60ADFC, 0xB6238B25, 0xB2E29692,
    0x8AAD2B2F, 0x8E6C3698, 0x832F1041, 0x87EE0DF6,
    0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A,
    0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E,
    0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2,
    0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686,
    0xD5B88683, 0xD1799B34, 0xDC3ABDED, 0xD8FBA05A,

    0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637,
    0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,
    0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F,
    0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53,
    0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47,
    0x36194D42, 0x32D850F5, 0x3F9B762C, 0x3B5A6B9B,
    0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF,
    0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623,
    0xF12F560E, 0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7,
    0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B,
    0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F,
    0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
    0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7,
    0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B,
    0x9B3660C6, 0x9FF77D71, 0x92B45BA8, 0x9675461F,
    0x8832161A, 0x8CF30BAD, 0x81B02D74, 0x857130C3,

    0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640,
    0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C,
    0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8,
    0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24,
    0x119B4BE9, 0x155A565E, 0x18197087, 0x1CD86D30,
    0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
    0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088,
    0x2497D08D, 0x2056CD3A, 0x2D15EBE3, 0x29D4F654,
    0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0,
    0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C,
    0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18,
    0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4,
    0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0,
    0x9ABC8BD5, 0x9E7D9662, 0x933EB0BB, 0x97FFAD0C,
    0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668,
    0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4,
};

// メモリのCRC32コードを計算
extern unsigned long getMemCRC32(unsigned long crc32, unsigned const char buff[], size_t size)
{
    while (size != 0) {
        crc32 = CRC32Table[(crc32 >> (32 - CHAR_BIT)) ^ *buff] ^ (crc32 << CHAR_BIT);
        buff++;
        size--;
    }
    return crc32;
}

typedef UINT32 typeofthunkinprg(void*,void*,void*);

__declspec(dllexport) AA32Reg *initaa32reg() { return (AA32Reg*)malloc(sizeof(AA32Reg)); }

void setexecasprg(void* prm_0,UINT64 prm_1) { DWORD tmp; VirtualProtect(prm_0, prm_1, 0x40, &tmp); }
void resetcache4prg(void* prm_0, UINT64 prm_1) { FlushInstructionCache(GetCurrentProcess(), prm_0, prm_1); }

void* transcode32to64(UINT32 Addr) {
    UINT32 Addrtmp = Addr & 0xFFFFFFFE;
    UINT32 Opctmp = 0;
    UINT8 Condtmp = 0x0e;
    UINT32 Opctmp2;
    UINT32 Opctmp3;
    UINT32 Opctmp3_rsb;
    UINT32 Opctmp4;
    /*
add x15,x15,4
movz x18,0x0000
movk x18,0x0000,LSL 16
add w15,w15,w18
and x16,x15,0xfffffffe
and x17,x15,0xfffffffc
beq conditon1
b condition0
conditon1:
nop
nop
condition0:
*/
    char JITTMP[] = { 0xEF ,0x11 ,0x00 ,0x91 ,0x12 ,0x00 ,0x80 ,0xD2 ,0x12 ,0x00 ,0xA0 ,0xF2 ,0xEF ,0x01 ,0x12 ,0x0B ,0xF0 ,0x79 ,0x7F ,0x92 ,0xF1 ,0x75 ,0x7E ,0x92 ,0x40 ,0x00 ,0x00 ,0x54 ,0x03 ,0x00 ,0x00 ,0x14 ,0x1F ,0x20 ,0x03 ,0xD5 ,0x1F ,0x20 ,0x03 ,0xD5 };

    char* ARMCode32on64 = (char*)malloc(4096 * sizeof(JITTMP));

    for (int cnt = 0; cnt < 4096; cnt++) {
        *(UINT32*)(JITTMP + 32) = 0xD503201F;
        *(UINT32*)(JITTMP + 36) = 0xD503201F;
        *(UINT32*)(JITTMP + 4) = 0xD2800012;
        *(UINT32*)(JITTMP + 8) = 0xF2A00012;
        *(UINT32*)(JITTMP + 12) = 0x0B1201EF;
        Opctmp3 = (((Opctmp >> 12) & 0xF) << 0) | (((Opctmp >> 16) & 0xF) << 5) | (((Opctmp >> 25) & 0x1) << 24) | (((Opctmp >> 25) & 0x1) ? ((Opctmp&0xFFF)<<10) : ((Opctmp&0xF)<<16)) | (((Opctmp >> 20) & 1) << 29);
        Opctmp3_rsb = (((Opctmp >> 12) & 0xF) << 0) | ((18) << 5) | 0 | (((Opctmp >> 16) & 0xF) << 16) | (((Opctmp >> 20) & 1) << 29);
        Condtmp = 0x0e;
        if (Addr & 1) {
            Opctmp = *(UINT16*)(Addrtmp);
            Addrtmp += 2;
        }
        else {
            Opctmp = *(UINT32*)(Addrtmp);
            Addrtmp += 4;
            Condtmp = (Opctmp >> 28) & 0xF;
            switch ((Opctmp >> 26) & 3) {
            case 0:
                if (((Opctmp >> 25) & 0x1) == 0 && ((Opctmp >> 4) & 0xF) == 0x9) {
                    switch ((Opctmp >> 23) & 3) {
                    case 2:
                        if (((Opctmp >> 20) & 3) == 2) {
                            *(UINT32*)(JITTMP + 32) = 0x2A0003EF | ((Opctmp & 0xF) << 16);
                            *(UINT32*)(JITTMP + 36) = 0xD65F03C0;
                        }
                        else {
                            *(UINT32*)(JITTMP + 32) = 0xb9400000 | ((Opctmp & 0xF) << 5) | ((Opctmp >> 12) & 0xF);
                            *(UINT32*)(JITTMP + 36) = 0xb9000000 | ((Opctmp & 0xF) << 5) | ((Opctmp >> 16) & 0xF);
                        }
                        break;
                    }
                }
                else {
                    switch ((Opctmp >> 21) & 0xF) {
                    case 0:
                        Opctmp2 = 0x0a000000 | Opctmp3;
                        break;
                    case 1:
                        Opctmp2 = 0x4a000000 | Opctmp3;
                        break;
                    case 2:
                        Opctmp2 = 0x4b000000 | Opctmp3;
                        break;
                    case 3:
                        Opctmp4 = (Opctmp & 0xFFF);
                        if (((Opctmp >> 25) & 0x1) == 0) {
                            *(UINT32*)(JITTMP + 4) = 0xD2800012 | ((Opctmp4 >> (16 * 0)) & 0xFFFF) << 5;
                            *(UINT32*)(JITTMP + 8) = 0xF2A00012 | ((Opctmp4 >> (16 * 1)) & 0xFFFF) << 5;
                            *(UINT32*)(JITTMP + 12) = 0xD503201F;
                        }
                        else {
                            *(UINT32*)(JITTMP + 4) = 0xD503201F;
                            *(UINT32*)(JITTMP + 8) = 0xD503201F;
                            *(UINT32*)(JITTMP + 12) = (0x2A0003F2 | (((Opctmp & 0xF) << 16)));
                        }
                        Opctmp2 = 0x4b000000 | Opctmp3_rsb;
                        break;
                    case 4:
                        Opctmp2 = 0x0b000000 | Opctmp3;
                        break;
                    case 5:
                        Opctmp2 = 0x1a000000 | Opctmp3;
                        break;
                    case 6:
                        Opctmp2 = 0x5a000000 | Opctmp3;
                        break;
                    case 7:
                        Opctmp4 = (Opctmp & 0xFFF);
                        if (((Opctmp >> 25) & 0x1) == 0) {
                            *(UINT32*)(JITTMP + 4) = 0xD2800012 | ((Opctmp4 >> (16 * 0)) & 0xFFFF) << 5;
                            *(UINT32*)(JITTMP + 8) = 0xF2A00012 | ((Opctmp4 >> (16 * 1)) & 0xFFFF) << 5;
                            *(UINT32*)(JITTMP + 12) = 0xD503201F;
                        }
                        else {
                            *(UINT32*)(JITTMP + 4) = 0xD503201F;
                            *(UINT32*)(JITTMP + 8) = 0xD503201F;
                            *(UINT32*)(JITTMP + 12) = (0x2A0003F2 | (((Opctmp & 0xF) << 16)));
                        }
                        Opctmp2 = 0x5a000000 | Opctmp3_rsb;
                        break;
                    case 8:
                        Opctmp2 = 0x6a00001f | Opctmp3;
                        break;
                    case 9:
                        Opctmp2 = 0x4a00001f | Opctmp3;
                        break;
                    case 10:
                        Opctmp2 = 0x6b00001f | Opctmp3;
                        break;
                    case 11:
                        Opctmp2 = 0x2b00001f | Opctmp3;
                        break;
                    case 12:
                        Opctmp2 = 0x2a000000 | Opctmp3;
                        break;
                    case 13:
                        Opctmp2 = 0x2a0000e0 | Opctmp3;
                        break;
                    case 14:
                        Opctmp2 = 0x0a200000 | Opctmp3;
                        break;
                    case 15:
                        Opctmp2 = 0x2a2003e0 | Opctmp3;
                        break;
                    }
                }
                break;
            case 2:
                if (((Opctmp >> 25) & 0x1) == 0) {
                }
                else {
                    Opctmp4 = (Opctmp & 0x800000) ? (0xFF000000 | (Opctmp & 0xFFFFFF)) : (Opctmp & 0xFFFFFF);
                    Opctmp4 *= 4;
                    *(UINT32*)(JITTMP + 4) = 0xD2800012 | ((Opctmp4 >> (16 * 0)) & 0xFFFF) << 5;
                    *(UINT32*)(JITTMP + 8) = 0xF2A00012 | ((Opctmp4 >> (16 * 1)) & 0xFFFF) << 5;
                    if (((Opctmp >> 24) & 0x1)) { *(UINT32*)(JITTMP + 32) = 0x2A0F03EE; }
                    *(UINT32*)(JITTMP + 36) = 0xD65F03C0;
                }
                break;
            }
        }
        *(UINT32*)(JITTMP + 36) = Opctmp2;
        if (Condtmp == 0x0e) { 
            *(UINT32*)(JITTMP + 24) = 0xD503201F; *(UINT32*)(JITTMP + 28) = 0xD503201F;
        }
        else {
            *(UINT32*)(JITTMP + 24) = 0x54000040; *(UINT32*)(JITTMP + 28) = 0x14000002;
            *(UINT8*)(JITTMP + 24) = ((Condtmp & 0xF) | ((*(UINT8*)(JITTMP + 24)) & 0xF0));
        }
        memcpy(ARMCode32on64 + (sizeof(JITTMP) * cnt), JITTMP, sizeof(JITTMP));
    }

    setexecasprg(ARMCode32on64,4096 * sizeof(JITTMP));
    resetcache4prg(ARMCode32on64, 4096 * sizeof(JITTMP));
    return ARMCode32on64;
}

__declspec(dllexport) UINT64 execa32ona64(UINT32 Addr, AA32Reg *Reg) {
    AA32Reg* Regtmp = Reg;
    UINT32 Addrtmp = Addr;
/*
stp d0,d1,[sp,#-16]
stp d2,d3,[sp,#-16]
stp d4,d5,[sp,#-16]
stp d6,d7,[sp,#-16]
stp d8,d9,[sp,#-16]
stp d10,d11,[sp,#-16]
stp d12,d13,[sp,#-16]
stp d14,d15,[sp,#-16]
stp d16,d17,[sp,#-16]
stp d18,d19,[sp,#-16]
stp d20,d21,[sp,#-16]
stp d22,d23,[sp,#-16]
stp d24,d25,[sp,#-16]
stp d26,d27,[sp,#-16]
stp d28,d29,[sp,#-16]
stp d30,d31,[sp,#-16]
stp x8,x9,[sp,#-16]
stp x10,x11,[sp,#-16]
stp x12,x13,[sp,#-16]
stp x14,x15,[sp,#-16]
stp x16,x17,[sp,#-16]
stp x18,x19,[sp,#-16]
stp x20,x21,[sp,#-16]
stp x22,x23,[sp,#-16]
stp x24,x25,[sp,#-16]
stp x26,x27,[sp,#-16]
stp x28,x29,[sp,#-16]
mov x20,x0
mov x19,x1
mov x21,x2
ldr w0,[x19,#66*4]
ldr w1,[x19,#67*4]
ldr w2,[x19,#68*4]
ldr w3,[x19,#69*4]
ldr w4,[x19,#70*4]
ldr w5,[x19,#71*4]
ldr w6,[x19,#72*4]
ldr w7,[x19,#73*4]
ldr w8,[x19,#74*4]
ldr w9,[x19,#75*4]
ldr w10,[x19,#76*4]
ldr w11,[x19,#77*4]
ldr w12,[x19,#78*4]
ldr w13,[x19,#79*4]
ldr w14,[x19,#80*4]
ldr w15,[x19,#81*4]
ldr d0,[x19,#1*8]
ldr d1,[x19,#2*8]
ldr d2,[x19,#3*8]
ldr d3,[x19,#4*8]
ldr d4,[x19,#5*8]
ldr d5,[x19,#6*8]
ldr d6,[x19,#7*8]
ldr d7,[x19,#8*8]
ldr d8,[x19,#9*8]
ldr d9,[x19,#10*8]
ldr d10,[x19,#11*8]
ldr d11,[x19,#12*8]
ldr d12,[x19,#13*8]
ldr d13,[x19,#14*8]
ldr d14,[x19,#15*8]
ldr d15,[x19,#16*8]
ldr d16,[x19,#17*8]
ldr d17,[x19,#18*8]
ldr d18,[x19,#19*8]
ldr d19,[x19,#20*8]
ldr d20,[x19,#21*8]
ldr d21,[x19,#22*8]
ldr d22,[x19,#23*8]
ldr d23,[x19,#24*8]
ldr d24,[x19,#25*8]
ldr d25,[x19,#26*8]
ldr d26,[x19,#27*8]
ldr d27,[x19,#28*8]
ldr d28,[x19,#29*8]
ldr d29,[x19,#30*8]
ldr d30,[x19,#31*8]
ldr d31,[x19,#32*8]
ldr x22,[x19,#0]
msr NZCV,x22
br x20
*/
    char Thunkin[] = { 0xE0 ,0x07 ,0x3F ,0x6D ,0xE2 ,0x0F ,0x3F ,0x6D ,0xE4 ,0x17 ,0x3F ,0x6D ,0xE6 ,0x1F ,0x3F ,0x6D ,0xE8 ,0x27 ,0x3F ,0x6D ,0xEA ,0x2F ,0x3F ,0x6D ,0xEC ,0x37 ,0x3F ,0x6D ,0xEE ,0x3F ,0x3F ,0x6D ,0xF0 ,0x47 ,0x3F ,0x6D ,0xF2 ,0x4F ,0x3F ,0x6D ,0xF4 ,0x57 ,0x3F ,0x6D ,0xF6 ,0x5F ,0x3F ,0x6D ,0xF8 ,0x67 ,0x3F ,0x6D ,0xFA ,0x6F ,0x3F ,0x6D ,0xFC ,0x77 ,0x3F ,0x6D ,0xFE ,0x7F ,0x3F ,0x6D ,0xE8 ,0x27 ,0x3F ,0xA9 ,0xEA ,0x2F ,0x3F ,0xA9 ,0xEC ,0x37 ,0x3F ,0xA9 ,0xEE ,0x3F ,0x3F ,0xA9 ,0xF0 ,0x47 ,0x3F ,0xA9 ,0xF2 ,0x4F ,0x3F ,0xA9 ,0xF4 ,0x57 ,0x3F ,0xA9 ,0xF6 ,0x5F ,0x3F ,0xA9 ,0xF8 ,0x67 ,0x3F ,0xA9 ,0xFA ,0x6F ,0x3F ,0xA9 ,0xFC ,0x77 ,0x3F ,0xA9 ,0xF4 ,0x03 ,0x00 ,0xAA ,0xF3 ,0x03 ,0x01 ,0xAA ,0xF5 ,0x03 ,0x02 ,0xAA ,0x60 ,0x0A ,0x41 ,0xB9 ,0x61 ,0x0E ,0x41 ,0xB9 ,0x62 ,0x12 ,0x41 ,0xB9 ,0x63 ,0x16 ,0x41 ,0xB9 ,0x64 ,0x1A ,0x41 ,0xB9 ,0x65 ,0x1E ,0x41 ,0xB9 ,0x66 ,0x22 ,0x41 ,0xB9 ,0x67 ,0x26 ,0x41 ,0xB9 ,0x68 ,0x2A ,0x41 ,0xB9 ,0x69 ,0x2E ,0x41 ,0xB9 ,0x6A ,0x32 ,0x41 ,0xB9 ,0x6B ,0x36 ,0x41 ,0xB9 ,0x6C ,0x3A ,0x41 ,0xB9 ,0x6D ,0x3E ,0x41 ,0xB9 ,0x6E ,0x42 ,0x41 ,0xB9 ,0x6F ,0x46 ,0x41 ,0xB9 ,0x60 ,0x06 ,0x40 ,0xFD ,0x61 ,0x0A ,0x40 ,0xFD ,0x62 ,0x0E ,0x40 ,0xFD ,0x63 ,0x12 ,0x40 ,0xFD ,0x64 ,0x16 ,0x40 ,0xFD ,0x65 ,0x1A ,0x40 ,0xFD ,0x66 ,0x1E ,0x40 ,0xFD ,0x67 ,0x22 ,0x40 ,0xFD ,0x68 ,0x26 ,0x40 ,0xFD ,0x69 ,0x2A ,0x40 ,0xFD ,0x6A ,0x2E ,0x40 ,0xFD ,0x6B ,0x32 ,0x40 ,0xFD ,0x6C ,0x36 ,0x40 ,0xFD ,0x6D ,0x3A ,0x40 ,0xFD ,0x6E ,0x3E ,0x40 ,0xFD ,0x6F ,0x42 ,0x40 ,0xFD ,0x70 ,0x46 ,0x40 ,0xFD ,0x71 ,0x4A ,0x40 ,0xFD ,0x72 ,0x4E ,0x40 ,0xFD ,0x73 ,0x52 ,0x40 ,0xFD ,0x74 ,0x56 ,0x40 ,0xFD ,0x75 ,0x5A ,0x40 ,0xFD ,0x76 ,0x5E ,0x40 ,0xFD ,0x77 ,0x62 ,0x40 ,0xFD ,0x78 ,0x66 ,0x40 ,0xFD ,0x79 ,0x6A ,0x40 ,0xFD ,0x7A ,0x6E ,0x40 ,0xFD ,0x7B ,0x72 ,0x40 ,0xFD ,0x7C ,0x76 ,0x40 ,0xFD ,0x7D ,0x7A ,0x40 ,0xFD ,0x7E ,0x7E ,0x40 ,0xFD ,0x7F ,0x82 ,0x40 ,0xFD ,0x76 ,0x02 ,0x40 ,0xF9 ,0x16 ,0x42 ,0x1B ,0xD5 ,0x80 ,0x02 ,0x1F ,0xD6 };

/*
mrs x22,NZCV
str x22,[x19,#0]
str d0,[x19,#1*8]
str d1,[x19,#2*8]
str d2,[x19,#3*8]
str d3,[x19,#4*8]
str d4,[x19,#5*8]
str d5,[x19,#6*8]
str d6,[x19,#7*8]
str d7,[x19,#8*8]
str d8,[x19,#9*8]
str d9,[x19,#10*8]
str d10,[x19,#11*8]
str d11,[x19,#12*8]
str d12,[x19,#13*8]
str d13,[x19,#14*8]
str d14,[x19,#15*8]
str d15,[x19,#16*8]
str d16,[x19,#17*8]
str d17,[x19,#18*8]
str d18,[x19,#19*8]
str d19,[x19,#20*8]
str d20,[x19,#21*8]
str d21,[x19,#22*8]
str d22,[x19,#23*8]
str d23,[x19,#24*8]
str d24,[x19,#25*8]
str d25,[x19,#26*8]
str d26,[x19,#27*8]
str d27,[x19,#28*8]
str d28,[x19,#29*8]
str d29,[x19,#30*8]
str d30,[x19,#31*8]
str d31,[x19,#32*8]
str w0,[x19,#66*4]
str w1,[x19,#67*4]
str w2,[x19,#68*4]
str w3,[x19,#69*4]
str w4,[x19,#70*4]
str w5,[x19,#71*4]
str w6,[x19,#72*4]
str w7,[x19,#73*4]
str w8,[x19,#74*4]
str w9,[x19,#75*4]
str w10,[x19,#76*4]
str w11,[x19,#77*4]
str w12,[x19,#78*4]
str w13,[x19,#79*4]
str w14,[x19,#80*4]
str w15,[x19,#81*4]
mov x0,x15
ldp x28,x29,[sp,#16]
ldp x26,x27,[sp,#16]
ldp x24,x25,[sp,#16]
ldp x22,x23,[sp,#16]
ldp x20,x21,[sp,#16]
ldp x18,x19,[sp,#16]
ldp x16,x17,[sp,#16]
ldp x14,x15,[sp,#16]
ldp x12,x13,[sp,#16]
ldp x10,x11,[sp,#16]
ldp x8,x9,[sp,#16]
ldp d30,d31,[sp,#16]
ldp d28,d29,[sp,#16]
ldp d26,d27,[sp,#16]
ldp d24,d25,[sp,#16]
ldp d22,d23,[sp,#16]
ldp d20,d21,[sp,#16]
ldp d18,d19,[sp,#16]
ldp d16,d17,[sp,#16]
ldp d14,d15,[sp,#16]
ldp d12,d13,[sp,#16]
ldp d10,d11,[sp,#16]
ldp d8,d9,[sp,#16]
ldp d6,d7,[sp,#16]
ldp d4,d5,[sp,#16]
ldp d2,d3,[sp,#16]
ldp d0,d1,[sp,#16]
ret
*/
    char Thunkout[] = { 0x16 ,0x42 ,0x3B ,0xD5 ,0x76 ,0x02 ,0x00 ,0xF9 ,0x60 ,0x06 ,0x00 ,0xFD ,0x61 ,0x0A ,0x00 ,0xFD ,0x62 ,0x0E ,0x00 ,0xFD ,0x63 ,0x12 ,0x00 ,0xFD ,0x64 ,0x16 ,0x00 ,0xFD ,0x65 ,0x1A ,0x00 ,0xFD ,0x66 ,0x1E ,0x00 ,0xFD ,0x67 ,0x22 ,0x00 ,0xFD ,0x68 ,0x26 ,0x00 ,0xFD ,0x69 ,0x2A ,0x00 ,0xFD ,0x6A ,0x2E ,0x00 ,0xFD ,0x6B ,0x32 ,0x00 ,0xFD ,0x6C ,0x36 ,0x00 ,0xFD ,0x6D ,0x3A ,0x00 ,0xFD ,0x6E ,0x3E ,0x00 ,0xFD ,0x6F ,0x42 ,0x00 ,0xFD ,0x70 ,0x46 ,0x00 ,0xFD ,0x71 ,0x4A ,0x00 ,0xFD ,0x72 ,0x4E ,0x00 ,0xFD ,0x73 ,0x52 ,0x00 ,0xFD ,0x74 ,0x56 ,0x00 ,0xFD ,0x75 ,0x5A ,0x00 ,0xFD ,0x76 ,0x5E ,0x00 ,0xFD ,0x77 ,0x62 ,0x00 ,0xFD ,0x78 ,0x66 ,0x00 ,0xFD ,0x79 ,0x6A ,0x00 ,0xFD ,0x7A ,0x6E ,0x00 ,0xFD ,0x7B ,0x72 ,0x00 ,0xFD ,0x7C ,0x76 ,0x00 ,0xFD ,0x7D ,0x7A ,0x00 ,0xFD ,0x7E ,0x7E ,0x00 ,0xFD ,0x7F ,0x82 ,0x00 ,0xFD ,0x60 ,0x0A ,0x01 ,0xB9 ,0x61 ,0x0E ,0x01 ,0xB9 ,0x62 ,0x12 ,0x01 ,0xB9 ,0x63 ,0x16 ,0x01 ,0xB9 ,0x64 ,0x1A ,0x01 ,0xB9 ,0x65 ,0x1E ,0x01 ,0xB9 ,0x66 ,0x22 ,0x01 ,0xB9 ,0x67 ,0x26 ,0x01 ,0xB9 ,0x68 ,0x2A ,0x01 ,0xB9 ,0x69 ,0x2E ,0x01 ,0xB9 ,0x6A ,0x32 ,0x01 ,0xB9 ,0x6B ,0x36 ,0x01 ,0xB9 ,0x6C ,0x3A ,0x01 ,0xB9 ,0x6D ,0x3E ,0x01 ,0xB9 ,0x6E ,0x42 ,0x01 ,0xB9 ,0x6F ,0x46 ,0x01 ,0xB9 ,0xE0 ,0x03 ,0x0F ,0xAA ,0xFC ,0x77 ,0x41 ,0xA9 ,0xFA ,0x6F ,0x41 ,0xA9 ,0xF8 ,0x67 ,0x41 ,0xA9 ,0xF6 ,0x5F ,0x41 ,0xA9 ,0xF4 ,0x57 ,0x41 ,0xA9 ,0xF2 ,0x4F ,0x41 ,0xA9 ,0xF0 ,0x47 ,0x41 ,0xA9 ,0xEE ,0x3F ,0x41 ,0xA9 ,0xEC ,0x37 ,0x41 ,0xA9 ,0xEA ,0x2F ,0x41 ,0xA9 ,0xE8 ,0x27 ,0x41 ,0xA9 ,0xFE ,0x7F ,0x41 ,0x6D ,0xFC ,0x77 ,0x41 ,0x6D ,0xFA ,0x6F ,0x41 ,0x6D ,0xF8 ,0x67 ,0x41 ,0x6D ,0xF6 ,0x5F ,0x41 ,0x6D ,0xF4 ,0x57 ,0x41 ,0x6D ,0xF2 ,0x4F ,0x41 ,0x6D ,0xF0 ,0x47 ,0x41 ,0x6D ,0xEE ,0x3F ,0x41 ,0x6D ,0xEC ,0x37 ,0x41 ,0x6D ,0xEA ,0x2F ,0x41 ,0x6D ,0xE8 ,0x27 ,0x41 ,0x6D ,0xE6 ,0x1F ,0x41 ,0x6D ,0xE4 ,0x17 ,0x41 ,0x6D ,0xE2 ,0x0F ,0x41 ,0x6D ,0xE0 ,0x07 ,0x41 ,0x6D ,0xC0 ,0x03 ,0x5F ,0xD6 };

    setexecasprg(Thunkin, sizeof(Thunkin));
    setexecasprg(Thunkout, sizeof(Thunkout));
    resetcache4prg(Thunkin, sizeof(Thunkin));
    resetcache4prg(Thunkout, sizeof(Thunkout));

    unsigned long crc32 = 0;

    while ((Addrtmp&0xFFFFFFFE)!=0xFFFFFFFE) {
        if (Regtmp->TranslatedCodePtr[(Addrtmp >> 12) & 0xFFFFF][Addrtmp & 1] == nullptr) {
            Regtmp->TranslatedCodePtr[(Addrtmp >> 12) & 0xFFFFF][Addrtmp & 1] = transcode32to64(Addrtmp);
            crc32 = 0xFFFFFFFF;
            crc32 = getMemCRC32(crc32, (unsigned char*)(Addrtmp), 4096);
            crc32 ^= 0xFFFFFFFF;
            Regtmp->TranslatedCodeFlag[(Addrtmp >> 12) & 0xFFFFF][Addrtmp & 1] = crc32;
        }
        Addrtmp = ((typeofthunkinprg*)(&Thunkin))(((&Regtmp->TranslatedCodePtr[(Addrtmp >> 12) & 0xFFFFF][Addrtmp & 1]) + (((Addrtmp & 0xFFF) >> (1 + ((Addr & 1) == 0))) * 40)), Regtmp, Thunkout);
        if (Regtmp->TranslatedCodePtr[(Addrtmp >> 12) & 0xFFFFF][Addrtmp & 1] != nullptr) {
            crc32 = 0xFFFFFFFF;
            crc32 = getMemCRC32(crc32, (unsigned char*)(Addrtmp), 4096);
            crc32 ^= 0xFFFFFFFF;
            if (crc32 != Regtmp->TranslatedCodeFlag[(Addrtmp >> 12) & 0xFFFFF][Addrtmp & 1]) { free(Regtmp->TranslatedCodePtr[(Addrtmp >> 12) & 0xFFFFF][Addrtmp & 1]); }
        }
    }

    return ((UINT64)(Regtmp->REG32[0])<<(32*0))|((UINT64)(Regtmp->REG32[1])<<(32*1));
}