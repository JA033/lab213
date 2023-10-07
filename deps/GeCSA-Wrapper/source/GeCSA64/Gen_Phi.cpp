/*============================================
# Filename: Gen_Phi.cpp
# Ver 2.0 2020-10-10
# Copyright (C) 2020 PengLong HongweiHuo
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the
License.
#
# Description:
=============================================*/
#include "Gen_Phi.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <math.h>
#include <time.h>
#include <memory>

#define MAXNUM 999999999
using namespace std;

struct Tables {
    std::unique_ptr<u16 []> zerostable;
    std::unique_ptr<u16 []> decodevaluenum;
    std::unique_ptr<u16 []> decodebitsnum;
    std::unique_ptr<u16 []> decoderesult;

    Tables() {
        const u32 tablewidth = 16;
        zerostable = std::unique_ptr<u16 []>(new u16[1u << tablewidth]);
        decodevaluenum = std::unique_ptr<u16 []>(new u16[1u << tablewidth]);
        decodebitsnum = std::unique_ptr<u16 []>(new u16[1u << tablewidth]);
        decoderesult = std::unique_ptr<u16 []>(new u16[1u << tablewidth]);
    }
};

u64 GetBits_Gamma(u32 *sequence, i64 position, i64 num) {
    u64 anchor = position >> 5;
    u64 temp1 = sequence[anchor];
    u32 temp2 = sequence[anchor + 1];
    u32 temp3 = sequence[anchor + 2];
    i32 overloop = ((anchor + 2) << 5) - (position + num);
    temp1 = (temp1 << 32) + temp2;
    if (overloop < 0) {
        temp1 = (temp1 << (-overloop)) + (temp3 >> (32 + overloop));
        return temp1 & ((1ull << num) - 1);
    } else
        return (temp1 >> overloop) & ((1ull << num) - 1);
}

i64 ZerosRun_Gamma(u32 *sequence, u16 *zerostable, i64 &position) {
    i64 y = 0;
    i64 D = 16;
    i64 x = GetBits_Gamma(sequence, position, D);
    i64 w = y = zerostable[x];
    while (y == D) {
        position = position + D;
        x = GetBits_Gamma(sequence, position, D);
        y = zerostable[x];
        w = w + y;
    }
    position = position + y;
    return w;
}

i64 DecodeGamma(u32 *sequence, u16 *zerostable, i64 &position, i64 &x) {
    i64 a = ZerosRun_Gamma(sequence, zerostable, position);
    x = GetBits_Gamma(sequence, position, a + 1);
    position = position + a + 1;
    return 2 * a + 1;
}

struct GammaTables : Tables {
    GammaTables() {
        const u32 tablewidth = 16;
        u16 *R = zerostable.get();
        for (integer i = 0; i < tablewidth; i++)
            for (integer j = (1 << i); j < (2 << i); j++)
                R[j] = tablewidth - 1 - i;
        R[0] = tablewidth;
        u16 *Rn = this->decodevaluenum.get();
        u16 *Rb = this->decodebitsnum.get();
        u16 *Rx = this->decoderesult.get();
        u32 tablesize = (1 << tablewidth);
        u32 B[3] = {0xffffffff, 0xffffffff, 0xffffffff};
        i64 b = 0;
        i64 num = 0;
        i64 x = 0;
        i64 d = 0;
        i64 preb = 0;

        for (u32 i = 0; i < tablesize; i++) {
            B[0] = (i << 16);
            b = 0;
            num = 0;
            x = 0;
            d = 0;
            while (1) {
                DecodeGamma(B, R, b, d);
                if (b > tablewidth)
                    break;
                x = x + d;
                num++;
                preb = b;
            }
            Rn[i] = num;
            Rb[i] = preb;
            Rx[i] = x;
        }
    }
};

u64 GetBits_Delta(u32 *sequence, i64 position, i64 num) {
    u64 anchor = position >> 5;
    u64 temp1 = sequence[anchor];
    u32 temp2 = sequence[anchor + 1];
    u32 temp3 = sequence[anchor + 2];
    i32 overloop = ((anchor + 2) << 5) - (position + num);
    temp1 = (temp1 << 32) + temp2;
    if (overloop < 0) {
        temp1 = (temp1 << (-overloop)) + (temp3 >> (32 + overloop));
        return temp1 & ((1ull << num) - 1);
    } else
        return (temp1 >> overloop) & ((1ull << num) - 1);
}

i64 ZerosRun_Delta(u32 *sequence, u16 *zerostable, i64 &position) {
    i64 y = 0;
    i64 D = 16;
    u64 x = GetBits_Delta(sequence, position, D);
    i64 w = y = zerostable[x];
    while (y == D) {
        position = position + D;
        x = GetBits_Delta(sequence, position, D);
        y = zerostable[x];
        w = w + y;
    }
    position = position + y;
    return w;
}

i64 DecodeDelta(u32 *sequence, u16 *zerostable, i64 &position, i64 &value) {
    i64 pos = position;
    i64 x = ZerosRun_Delta(sequence, zerostable, position);
    i64 v = GetBits_Delta(sequence, position, x + 2);
    position = position + x + 2;

    if (v > 32) {
        position = (1 << 30);
        return (1 << 30);
    }
    value = (1 << (v - 1)) + GetBits_Delta(sequence, position, v - 1);
    position = position + v - 1;
    return position - pos;
}

struct DeltaTables : Tables {
    DeltaTables() {
        const u32 tablewidth = 16;
        u16 *R = zerostable.get();
        for (integer i = 0; i < tablewidth; i++)
            for (integer j = (1 << i); j < (2 << i); j++)
                R[j] = tablewidth - 1 - i;
        R[0] = tablewidth;
        u16 *Rn = decodevaluenum.get();
        u16 *Rb = decodebitsnum.get();
        u16 *Rx = decoderesult.get();
        u32 tablesize = (1 << tablewidth);
        u32 B[4] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
        i64 b = 0;
        i64 d = 0;
        i64 num = 0;
        i64 preb = 0;
        i64 x = 0;
        for (u32 i = 0; i < tablesize; i++) {
            B[0] = (i << 16);
            b = num = x = d = 0;
            while (1) {
                DecodeDelta(B, R, b, d);
                if (b > 16)
                    break;
                if (d % 2 == 0) {
                    x = x + d / 2;
                    num = num + d / 2;
                } else {
                    x = x + (d + 1) / 2;
                    num++;
                }
                preb = b;
            }
            Rn[i] = num;
            Rx[i] = x;
            Rb[i] = preb;
        }
    }
};

static GammaTables &GetGammaTables() {
    static GammaTables tables;
    return tables;
}

static DeltaTables &GetDeltaTables() {
    static DeltaTables tables;
    return tables;
}

GEN_Phi::GEN_Phi() {}

GEN_Phi::GEN_Phi(parmaters *csa, i64 *phivalue, double multi) {

    this->phivalue = phivalue;
    this->tablewidth = 16;

    this->alpsize = csa->alphabetsize;
    this->n = csa->n;
    this->a = csa->SL;
    this->b = csa->L;
    this->index = 0;
    this->multi = multi;

    GammaTables &gammatables = GetGammaTables();
    this->zerostable_gamma = gammatables.zerostable.get();
    this->decodevaluenum_gamma = gammatables.decodevaluenum.get();
    this->decodebitsnum_gamma = gammatables.decodebitsnum.get();
    this->decoderesult_gamma = gammatables.decoderesult.get();

    DeltaTables &deltatables = GetDeltaTables();
    this->zerostable = deltatables.zerostable.get();
    this->decodevaluenum = deltatables.decodevaluenum.get();
    this->decodebitsnum = deltatables.decodebitsnum.get();
    this->decoderesult = deltatables.decoderesult.get();

    this->SamplingAndCoding(csa); // 压缩phi


#if defined(GECSA_TEST_CSA_PHI)
    std::cout << "-- Start testing phi-array random accessing.\n";
    for (i64 i = 0; i < n; ++i) {
        u64 ref = phivalue[i];
        u64 val = this->GetValue(i);
        if (val != ref) {
            std::cout << "---- Error: index " << i << " (" << val << ") "
                      << "ref = " << ref << "\n";
            break; 
        }
    }
    std::cout << "-- Finished testing phi-array random accessing.\n";
#endif

    delete[] phivalue;
}

GEN_Phi::~GEN_Phi(void) {
    delete[] superoffset;
    delete samples;
    delete offsects;
    delete[] sequence;
}

void GEN_Phi::Append_Gamma(i64 x) {
    u64 y = x;
    i64 zeronums = blogsize(y) - 1;
    index = index + zeronums;
    i64 valuewidth = zeronums + 1;
    i64 anchor = (index >> 5);
    u64 temp1 = sequence[anchor];
    u32 temp2 = sequence[anchor + 1];
    temp1 = (temp1 << 32) + temp2;

    i64 overloop = ((anchor + 2) << 5) - index - valuewidth;
    if (overloop < 0) {
        u64 value2 = (y >> (-overloop));
        temp1 = temp1 + value2;
        sequence[anchor + 1] = (temp1 & (0xffffffff));
        sequence[anchor] = (temp1 >> 32) & (0xffffffff);
        sequence[anchor + 2] = (y << (32 + overloop)) & (0xffffffff);

    } else {
        y = (y << overloop);
        temp1 = temp1 + y;
        sequence[anchor + 1] = (temp1 & (0xffffffff));
        sequence[anchor] = (temp1 >> 32) & (0xffffffff);
    }
    index = index + valuewidth;
}

void GEN_Phi::Test() {
    integer i = 0;
    integer k = 0;
    for (i = 0; i < n; i++) {
        if (phivalue[i] != GetValue(i))
            k++;
    }
    if (k == 0)
        cout << "Phi is right" << endl;
    else
        cout << "Phi is Sorry" << endl;
}
void GEN_Phi::Testforgetphivalue() {

    int c = 20;
    for (i64 i = 0; i < n; i++) {
        if (this->GetValue(i) != phivalue[i]) {
            cout << "i is: " << i << (int)this->method->GetValue(i / b) << " "
                 << this->GetValue(i) << " " << phivalue[i] << endl;
            c--;
            if (!c)
                exit(0);
        }
    }
}

i64 GEN_Phi::blogsize(i64 x) {

    i64 len = 0;
    while (x > 0) {
        x = x >> 1;
        len++;
    }
    return len;
}

u64 GEN_Phi::GetBits(i64 position, i64 num) {
    u64 anchor = position >> 5;
    u64 temp1 = sequence[anchor];
    u32 temp2 = sequence[anchor + 1];
    u32 temp3 = sequence[anchor + 2];
    i32 overloop = ((anchor + 2) << 5) - (position + num);
    temp1 = (temp1 << 32) + temp2;
    if (overloop < 0) {
        temp1 = (temp1 << (-overloop)) + (temp3 >> (32 + overloop));
        return temp1 & ((1ull << num) - 1);
    } else
        return (temp1 >> overloop) & ((1ull << num) - 1);
}

i64 GEN_Phi::DecodeDelta(i64 &position, i64 &value) {
    i64 pos = position;
    i64 x = this->ZerosRun(position);
    i64 v = GetBits(position, x + 2);
    position = position + x + 2;

    if (v > 32) {
        position = (1 << 30);
        return (1 << 30);
    }
    value = (1 << (v - 1)) + GetBits(position, v - 1);
    position = position + v - 1;
    return position - pos;
}

i64 GEN_Phi::DecodeDeltaBlock(i64 position, i64 base, i64 num) {
    i64 i = 0;
    i64 value = 0;
    while (i < num) {
        DecodeDelta2(position, value);
        if (value % 2 == 0) {
            if (i + value / 2 >= num)
                return (base + num - i) % n;
            base = (base + value / 2) % n;
            i = i + value / 2;
        } else {
            base = (base + (value + 1) / 2) % n;
            i++;
        }
    }
    return base;
}

i64 GEN_Phi::DecodeDelta2(i64 &position, i64 &value) {
    i64 pos = position;
    i64 x = this->ZerosRun(position);
    i64 v = GetBits(position, x + 2);
    position = position + x + 2;
    value = ((uint64_t)1 << (v - 1)) + GetBits(position, v - 1);
    position = position + v - 1;
    return position - pos;
}

i64 GEN_Phi::ZerosRun(i64 &position) {
    i64 y = 0;
    i64 D = this->tablewidth;
    u64 x = GetBits(position, D);
    i64 w = y = zerostable[x];
    while (y == D) {
        position = position + D;
        x = GetBits(position, D);
        y = zerostable[x];
        w = w + y;
    }
    position = position + y;
    return w;
}

i64 GEN_Phi::deltasize(i64 v) {
    i64 x = blogsize(v);
    i64 pre = 2 * blogsize(x) - 2;
    return pre + x - 1;
}

void GEN_Phi::Append(i64 x) {

    i64 size = blogsize(x);

    u64 y = size;
    i64 zeronums = blogsize(size) - 2;
    index = index + zeronums;
    i64 valuewidth = zeronums + 2;
    i64 anchor = (index >> 5);
    u64 temp1 = sequence[anchor];
    u32 temp2 = sequence[anchor + 1];
    temp1 = (temp1 << 32) + temp2;
    i64 overloop = ((anchor + 2) << 5) - index - valuewidth;
    if (overloop < 0) {
        u64 value2 = (y >> (-overloop));
        temp1 = temp1 + value2;
        sequence[anchor + 1] = (temp1 & (0xffffffff));
        sequence[anchor] = (temp1 >> 32) & (0xffffffff);
        sequence[anchor + 2] = (y << (32 + overloop)) & (0xffffffff);

    } else {
        y = (y << overloop);
        temp1 = temp1 + y;
        sequence[anchor + 1] = (temp1 & (0xffffffff));
        sequence[anchor] = (temp1 >> 32) & (0xffffffff);
    }
    index = index + valuewidth;

    valuewidth = size - 1;
    y = x;
    y = (y ^ ((uint64_t)1 << valuewidth));
    anchor = (index >> 5);
    temp1 = sequence[anchor];
    temp2 = sequence[anchor + 1];
    temp1 = (temp1 << 32) + temp2;
    overloop = ((anchor + 2) << 5) - index - valuewidth;
    if (overloop < 0) {
        u64 value2 = (y >> (-overloop));
        temp1 = temp1 + value2;
        sequence[anchor + 1] = (temp1 & (0xffffffff));
        sequence[anchor] = (temp1 >> 32) & (0xffffffff);
        sequence[anchor + 2] = (y << (32 + overloop)) & (0xffffffff);

    } else {
        y = (y << overloop);
        temp1 = temp1 + y;
        sequence[anchor + 1] = (temp1 & (0xffffffff));
        sequence[anchor] = (temp1 >> 32) & (0xffffffff);
    }
    index = index + valuewidth;
}

i64 GEN_Phi::GetValue(i64 index) {
    i64 position = 0;
    return GetValue(index, position);
}

i64 GEN_Phi::GetValue(i64 index, i64 &position) {
    i64 index_b = index / b;
    i64 base = samples->GetValue(index_b);
    int overloop = index % b;
    if (overloop == 0)
        return base;
    position = superoffset[index / a] + offsects->GetValue(index_b);
    int m = this->method->GetValue(index_b);
    switch (m) {
    case gamma: {
        i64 x = 0;
        i64 d = 0;

        i64 i = 0;
        i64 p = 0;
        i64 num = 0;
        i64 zero = 0;
        while (i < overloop) {
            p = this->GetBits_Gamma(position, 16);
            num = this->decodevaluenum_gamma[p];
            if (num == 0) {
                zero = this->zerostable_gamma[p];
                if (zero == 16)
                    this->Decodegamma(position, d);
                else
                    this->Decodegamma(position, d, zero);
                x = (x + d) % n;
                i++;
            } else {
                if (i + num > overloop)
                    break;
                i = i + num;
                position = position + this->decodebitsnum_gamma[p];
                x = (x + this->decoderesult_gamma[p]) % n;
            }
        }

        for (; i < overloop; i++) {
            this->Decodegamma(position, d);
            x = x + d;
            x = x % n;
        }
        return (base + x) % n;
    }
    case bv: {
        u64 value = 0;
        int k = 0;
        int num1 = 0;
        i64 count = 0;
        while (num1 < overloop) {
            value = GetBits_Gamma(position, 32);
            k = popcnt(value);
            num1 += k;
            position += 32;
            count++;
        }
        if (num1 >= overloop) {
            num1 -= k;
            position -= 32;
            count--;
        }
        i64 off = select_in_word_big(value, overloop - num1) - 32;
        return (base + 32 * count + off) % n;
    }
    case rldelta: {
        return DecodeDeltaBlock(position, base, overloop);
    }
    case allone: {
        return (base + overloop) % n;
    }
    default:
        cout << "get value method erro" << endl;
        break;
    }
}
void GEN_Phi::SamplingAndCoding(parmaters *csa) {
    i64 i, j;

    i64 pre = 0;
    i64 totallen = 0;
    i64 maxlen = 0;
    i64 len = 0;
    i64 x = n / a;
    method = new InArray(n / b + 1, 2);

    i64 len_g = 0;
    i64 len_bv = 0;
    i64 len_all = 0;
    i64 len_rldelta = 0;
    i64 num_g = 0;
    i64 num_bv = 0;
    i64 num_all = 0;
    i64 num_rldelta = 0;
    i64 gap = 0;
    for (i = 0; i < x + 1; i++) {
        for (j = i * a; j < (i + 1) * a && j < n; j++) {
            if (j % b == 0) {
                pre = phivalue[j];
                if ((j / b + 1) * b - 1 < n) {
                    len_bv = phivalue[(j / b + 1) * b - 1] - phivalue[j];
                } else {
                    len_bv = MAXNUM;
                }
                len_g = 0;
                len_all = 0;
                len_rldelta = 0;
                continue;
            }
            gap = phivalue[j] - pre;
            if (gap < 0) {
                gap = n + gap;
                len_bv = MAXNUM;
            }
            if (gap == 1) {
                len_all++;
            } else {
                if (len_all != 0) {
                    len_rldelta = len_rldelta + deltasize(2 * len_all);
                }
                len_rldelta = len_rldelta + deltasize(2 * gap - 1);
                len_all = 0;
            }
            len_g = len_g + blogsize(gap) * 2 - 1;
            pre = phivalue[j];

            // Fixme: these encoders have some bugs.
            //len_bv = MAXNUM;
            len_rldelta = MAXNUM;

            if (j + 1 == n) {
                method->SetValue(j / b, gamma);
                len += len_g;
                num_g++;
            } else {
                if ((j + 1) % b == 0) {

                    if (len_all > 0) {
                        len_rldelta = len_rldelta + deltasize(2 * len_all);
                    }
                    if (len_all == (b - 1)) {
                        method->SetValue(j / b, allone);
                        num_all++;
                        len += 0;
                    } else if ((len_g * this->multi) < len_bv &&
                               len_g < len_rldelta) {
                        method->SetValue(j / b, gamma);
                        len += len_g;
                        num_g++;
                    } else if ((len_rldelta * this->multi) < len_bv) {
                        method->SetValue(j / b, rldelta);
                        len += len_rldelta;
                        num_rldelta++;
                    } else {
                        method->SetValue(j / b, bv);
                        len += len_bv;
                        num_bv++;
                    }
                }
            }
        }

        if (len > maxlen)
            maxlen = len;

        totallen = totallen + len;
        len = 0;
    }

    this->lenofsequence = totallen / 32 + 3;
    this->sequence = new u32[lenofsequence];
    for (i = 0; i < lenofsequence; i++)
        sequence[i] = 0;
    this->offsects = new InArray(n / b + 1, blogsize(maxlen));
    samples = new InArray(n / b + 1, blogsize(n));

    this->lenofsuperoffset = n / a + 1;
    superoffset = new i64[lenofsuperoffset];

#ifdef TESTDATA
    cout << endl << "ecode block ratio" << endl;
    cout << "block_gamma: "
         << 1.0 * num_g / (num_g + num_bv + num_all + num_rldelta) << endl;
    cout << "block_bv: "
         << 1.0 * num_bv / (num_g + num_bv + num_all + num_rldelta) << endl;
    cout << "block_allone: "
         << 1.0 * num_all / (num_g + num_bv + num_all + num_rldelta) << endl;
    cout << "block_rldelta: "
         << 1.0 * num_rldelta / (num_g + num_bv + num_all + num_rldelta)
         << endl;
#endif
    bvsum = 0;
    gamasum = 0;
    rldatasum = 0;
    i = 0;
    j = 0;

    gap = 0;
    i64 index1 = 0;
    i64 index2 = 0;
    i64 len1 = 0;
    i64 len2 = 0;
    i64 m = 0;
    u64 value = 0;
    i64 sam = 0;
    u32 u = 1;
    i64 runs = 0;

    for (i64 i = 0; i < n; i++) {
        assert(len1 == this->index);

        if (i % a == 0) {
            len2 = len1;
            superoffset[index2] = len2;
            index2++;
        }
        if (i % b == 0) {
            samples->SetValue(index1, phivalue[i]);
            offsects->SetValue(index1, len1 - len2);
            index1++;
            pre = phivalue[i];
            sam = phivalue[i];

            m = method->GetValue(i / b);
            if (m > 3 || m < 0)
                cout << "WRONG " << method << " " << i / b << endl;
            continue;
        }
        gap = phivalue[i] - pre;
        if (gap < 0)
            gap = n + gap;
        pre = phivalue[i];

        switch (m) {
        case gamma: {
            len1 = len1 + blogsize(gap) * 2 - 1;
            gamasum += blogsize(gap) * 2 - 1;
            Append_Gamma(gap);
            break;
        }
        case bv: {
            len1 = len1 + phivalue[i] - sam;
            bvsum += phivalue[i] - sam;
            this->index += (phivalue[i] - sam);
            sam = phivalue[i];
            u = (u << (31 - (len1 - 1) % 32));
            this->sequence[(len1 - 1) / 32] |= u;
            u = 1;
            break;
        }
        case rldelta: {
            if (gap == 1) {
                runs++;
                if (((i + 1) % b == 0) || (i + 1) == n) {
                    len1 = len1 + deltasize(2 * runs);
                    rldatasum += deltasize(2 * runs);
                    this->Append(2 * runs);
                    runs = 0;
                }
            } else {
                if (runs != 0) {
                    len1 = len1 + deltasize(2 * runs);
                    rldatasum += deltasize(2 * runs);
                    this->Append(2 * runs);
                }

                len1 = len1 + deltasize(2 * gap - 1);
                rldatasum += deltasize(2 * gap - 1);
                this->Append(2 * gap - 1);
                runs = 0;
            };

            break;
        }
        case allone:
            break;
        default:
            cout << "erro method" << endl;
            break;
        }
    }
}
i64 GEN_Phi::GetMemorySize() {
    return samples->GetMemorySize() + this->offsects->GetMemorySize() +
           this->lenofsequence * 4 + this->lenofsuperoffset * 8 +
           method->GetMemorySize();
}
i64 GEN_Phi::GetOneBit(i64 i) {
    i64 anchor = i / 32;
    i64 position = i % 32;
    return (sequence[anchor] & 1 << (31 - position)) >> (31 - position);
}

i64 GEN_Phi::Decodegamma(i64 &position, i64 &x) {
    i64 a = this->ZerosRun_Gamma(position);
    x = this->GetBits_Gamma(position, a + 1);
    position = position + a + 1;
    return 2 * a + 1;
}

i64 GEN_Phi::Decodegamma(i64 &position, i64 &x, i64 zero) {
    x = this->GetBits_Gamma(position + zero, zero + 1);
    position += (2 * zero + 1);
    return 2 * zero + 1;
}

i64 GEN_Phi::ZerosRun_Gamma(i64 &position) {
    i64 y = 0;
    i64 D = this->tablewidth;
    i64 x = this->GetBits_Gamma(position, D);
    i64 w = y = this->zerostable_gamma[x];
    while (y == D) {
        position = position + D;
        x = this->GetBits_Gamma(position, D);
        y = this->zerostable_gamma[x];
        w = w + y;
    }
    position = position + y;
    return w;
}

u64 GEN_Phi::GetBits_Gamma(i64 position, i64 num) {

    u64 anchor = position >> 5;
    u64 temp1 = sequence[anchor];
    u32 temp2 = sequence[anchor + 1];
    u32 temp3 = sequence[anchor + 2];
    i32 overloop = ((anchor + 2) << 5) - (position + num);
    temp1 = (temp1 << 32) + temp2;
    if (overloop < 0) {
        temp1 = (temp1 << (-overloop)) + (temp3 >> (32 + overloop));
        return temp1 & ((1ull << num) - 1);
    } else
        return (temp1 >> overloop) & ((1ull << num) - 1);
}

i64 GEN_Phi::RightBoundary(i64 pr, i64 l, i64 r) {
    i64 ans = 0;
    i64 SL = this->a;
    i64 L = this->b;
    i64 lb = (l + L - 1) / L;
    i64 rb = r / L;
    i64 b = lb - 1;
    i64 m = 0;
    i64 x = 0;
    while (lb <= rb) {
        m = (lb + rb) >> 1;
        x = this->samples->GetValue(m);
        if (x == pr) {
            b = m;
            break;
        }
        if (x < pr) {
            b = m;
            lb = m + 1;
        } else
            rb = m - 1;
    }
    if (b < 0)
        return 0;
    x = this->samples->GetValue(b);
    ans = l - 1;
    if (r > (b + 1) * L - 1)
        r = (b + 1) * L - 1;
    m = b * L;

    i64 methods = this->method->GetValue(b);

    switch (methods) {
    case rldelta: {
        i64 d = 0;
        i64 position =
            this->superoffset[m / SL] + this->offsects->GetValue(m / L);
        i64 p = 0;
        i64 num = 0;
        i64 bits = 0;
        i64 zero = 0;

        bool loop = false;
        while (x <= pr && m < r) {
            loop = true;
            p = this->GetBits(position, 16);
            num = this->decodevaluenum[p];
            if (num == 0) {
                bits = DecodeDelta2(position, d); // hzt
                if (d % 2 == 0) {
                    m = m + d / 2;
                    x = (x + d / 2) % n;
                } else {
                    m++;
                    x = (x + (d + 1) / 2) % n;
                }
            } else {
                m = m + num;
                position = position + this->decodebitsnum[p];
                x = (x + this->decoderesult[p]) % n;
            }
        }
        if (loop) {
            if (num != 0) {
                x = (n + x - this->decoderesult[p]) % n;
                position = position - this->decodebitsnum[p];
                m = m - num;
            } else {
                if (d % 2 == 0) {
                    m = m - d / 2;
                    x = (x - d / 2 + n) % n;
                } else {
                    m = m - 1;
                    x = (n + x - (d + 1) / 2) % n;
                }
                position = position - bits;
            }
        }

        i64 run = 0;
        while (1) {
            if (m >= l && x > pr)
                break;
            ans = m;
            m++;
            if (m > r)
                break;
            if (run > 0) {
                x = (x + 1) % n;
                run--;
            } else {
                DecodeDelta2(position, d);
                if (d % 2 == 0) {
                    run = d / 2 - 1;
                    x = (x + 1) % n;
                } else
                    x = (x + (d + 1) / 2) % n;
            }
        }
        return ans;
        break;
    }
    case gamma: {
        i64 d = 0;
        i64 position =
            this->superoffset[m / SL] + this->offsects->GetValue(m / L);
        while (m < l) {
            this->Decodegamma(position, d);
            x = (x + d) % n;
            m++;
        }

        i64 p = 0;
        i64 num = 0;
        i64 bits = 0;
        i64 zero = 0;
        bool loop = false;
        while (x <= pr && m < r) {
            loop = true;
            p = this->GetBits_Gamma(position, 16);
            num = this->decodevaluenum_gamma[p];
            if (num == 0) {
                m++;
                zero = this->zerostable_gamma[p];
                if (zero == 16)
                    bits = this->Decodegamma(position, d);
                else
                    bits = this->Decodegamma(position, d, zero);
                x = x + d;
            } else {
                m = m + num;
                position = position + this->decodebitsnum_gamma[p];
                x = mod(x + this->decoderesult_gamma[p]);
            }
        }

        if (loop) {
            if (num != 0) {
                x = mod(x - this->decoderesult_gamma[p]);
                position = position - this->decodebitsnum_gamma[p];
                m = m - num;
            } else {
                m = m - 1;
                x = mod(x - d);
                position = position - bits;
            }
        }
        while (1) {
            if (x > pr)
                break;
            ans = m;
            m++;
            if (m > r)
                break;
            Decodegamma(position, d);
            x = (x + d) % n;
        }
        return ans;
    }

    case bv: {
        i64 position =
            this->superoffset[m / SL] + this->offsects->GetValue(m / L);
        i64 u = this->superoffset[(m / L + 1) * L / SL] +
                this->offsects->GetValue(m / L + 1) - position;

        if (x + u <= pr) {
            ans = r;
            return ans;
        }

        if (pr == x) {
            if (m >= l) {
                ans = m;
                return ans;
            } else
                return ans;
        }

        i64 pos = position;
        u64 value = 0;
        i64 k = 0;
        i64 target = pr - x + position;
        while (pos < target) {
            value = GetBits_Gamma(pos, 32);
            k = popcnt(value);
            m += k;
            pos += 32;
        }
        if (pos > target) {
            m -= k;
            pos -= 32;
        }

        if (pos == target) {
            if (m >= l) {
                ans = m;
                return ans;
            } else {
                ans = l - 1;
                return ans;
            }
        }

        value = GetBits_Gamma(pos, target - pos);
        k = popcnt(value);
        m += k;

        if (m >= l) {
            ans = m;
            return ans;
        } else {
            ans = l - 1;
            return ans;
        }
    }
    case allone: {
        if (x > pr)
            return ans;
        if (pr == x) {
            if (m >= l) {
                ans = m;
                return ans;
            } else
                return ans;
        }
        int target = pr - x;
        if (m + target >= r) {
            return r;
        }
        m += target;
        if (m >= l) {
            ans = m;
            return ans;
        } else
            return ans;
    }
    default:
        cout << "right boundary method erro" << endl;
        break;
    }
    return ans;
}

i64 GEN_Phi::LeftBoundary(i64 pl, i64 l, i64 r) {
    i64 ans = 0;
    i64 SL = this->a;
    i64 L = this->b;
    i64 lb = (l + L - 1) / L;
    i64 rb = r / L;
    i64 b = rb + 1;
    i64 m = 0;
    i64 x = 0;

    while (lb <= rb) {
        m = (lb + rb) >> 1;
        x = this->samples->GetValue(m);
        if (x == pl) {
            b = m;
            break;
        }
        if (x > pl) {
            b = m;
            rb = m - 1;
        } else
            lb = m + 1;
    }
    if (b <= 0)
        return 0;
    x = this->samples->GetValue(b - 1);
    if (r > b * L - 1)
        r = b * L - 1;
    ans = r + 1;
    m = (b - 1) * L;

    i64 methods = this->method->GetValue(b - 1);

    switch (methods) {
    case rldelta: {
        i64 d = 0;
        i64 position =
            this->superoffset[m / SL] + this->offsects->GetValue(m / L);
        i64 p = 0;
        i64 num = 0;
        i64 bits = 0;
        i64 zero = 0;

        bool loop = false;
        while (x <= pl && m < r) {
            loop = true;
            p = this->GetBits(position, 16);
            num = this->decodevaluenum[p];
            if (num == 0) {
                bits = DecodeDelta2(position, d); // hzt
                if (d % 2 == 0) {
                    m = m + d / 2;
                    x = (x + d / 2) % n;
                } else {
                    m++;
                    x = (x + (d + 1) / 2) % n;
                }
            } else {
                m = m + num;
                position = position + this->decodebitsnum[p];
                x = (x + this->decoderesult[p]) % n;
            }
        }
        if (loop) {
            if (num != 0) {
                x = (n + x - this->decoderesult[p]) % n;
                position = position - this->decodebitsnum[p];
                m = m - num;
            } else {
                if (d % 2 == 0) {
                    m = m - d / 2;
                    x = (x - d / 2 + n) % n;
                } else {
                    m = m - 1;
                    x = (n + x - (d + 1) / 2) % n;
                }
                position = position - bits;
            }
        }
        i64 run = 0;
        while (1) {
            if (m >= l && x > pl)
                break;
            ans = m;
            m++;
            if (m > r)
                break;
            if (run > 0) {
                x = (x + 1) % n;
                run--;
            } else {
                DecodeDelta2(position, d);
                if (d % 2 == 0) {
                    run = d / 2 - 1;
                    x = (x + 1) % n;
                } else
                    x = (x + (d + 1) / 2) % n;
            }
        }
        return ans;
        break;
    }
    case gamma: {
        i64 position =
            this->superoffset[m / SL] + this->offsects->GetValue(m / L);
        i64 d = 0;
        while (m < l) {
            this->Decodegamma(position, d);
            x = (x + d) % n;
            m++;
        }

        i64 p = 0;
        i64 num = 0;
        i64 bits = 0;
        bool loop = false;
        while (x < pl && m < r) {
            loop = true;
            p = this->GetBits_Gamma(position, 16);
            num = this->decodevaluenum_gamma[p];
            if (num != 0) {
                m = m + num;
                position = position + this->decodebitsnum_gamma[p];
                x = (x + this->decoderesult_gamma[p]) % n;

            } else {
                m++;
                bits = this->Decodegamma(position, d);
                x = x + d;
            }
        }
        if (loop) {
            if (num != 0) {
                x = mod(x - this->decoderesult_gamma[p]);
                position = position - this->decodebitsnum_gamma[p];
                m = m - num;
            } else {
                m = m - 1;
                x = mod(x - d);
                position = position - bits;
            }
        }

        while (1) {
            if (x >= pl) {
                ans = m;
                break;
            }
            m++;
            if (m > r)
                break;
            Decodegamma(position, d);
            x = (x + d) % n;
        }

        return ans;
    }

    case bv: {

        i64 position =
            this->superoffset[m / SL] + this->offsects->GetValue(m / L);
        i64 u = this->superoffset[(m / L + 1) * L / SL] +
                this->offsects->GetValue(m / L + 1) - position;

        if (x + u < pl)
            return ans;

        if (pl == x) {
            if (m >= l) {
                ans = m;
                return ans;
            } else {
                ans = l;
                return ans;
            }
        }

        i64 pos = position;
        u64 value = 0;
        i64 k = 0;
        i64 target = pl - x + position;
        i64 lastbit = 0;
        while (pos < target) {
            value = GetBits_Gamma(pos, 32);
            k = popcnt(value);
            m += k;
            pos += 32;
        }
        if (pos > target) {
            m -= k;
            pos -= 32;
        }

        if (pos == target) {
            lastbit = (value & 0x0001);
            if (lastbit == 0)
                m++;
            if (m >= l) {
                ans = m;
                return ans;
            } else {
                ans = l;
                return ans;
            }
        }

        value = GetBits_Gamma(pos, target - pos);
        k = popcnt(value);
        m += k;
        lastbit = (value & 0x0001);
        if (lastbit == 0)
            m++;
        if (m >= l) {
            ans = m;
            return ans;
        } else {
            ans = l;
            return ans;
        }
    }
    case allone: {
        int tagert = pl - x;
        if ((m + tagert) > r) {
            ans = r + 1;
            return ans;
        }
        if (pl == x) {
            if (m >= l) {
                ans = m;
                return ans;
            } else {
                ans = l;
                return ans;
            }
        }
        m += tagert;
        if (m >= l) {
            ans = m;
            return ans;
        } else {
            ans = l;
            return ans;
        }
    }
    default:
        cout << "left boundary methods erro" << endl;
        break;
    }

    return ans;
}

i64 GEN_Phi::mod(i64 x) {
    if (x < 0)
        return x + n;
    else
        return x % n;
}

i64 GEN_Phi::sizeofEcode(int type) {
    if (type == bv) {
        return bvsum / 8;
    } else if (type == gamma) {
        return gamasum / 8;
    } else if (type == rldelta) {
        return rldatasum / 8;
    } else {
        return 0;
    }
}

integer GEN_Phi::write(savekit &s) {
    s.writei64(n);
    s.writeinteger(a);
    s.writeinteger(b);
    s.writeinteger(tablewidth);
    s.writei64(lenofsuperoffset);
    s.writei64array(superoffset, lenofsuperoffset);

    offsects->write(s);
    samples->write(s);
    method->write(s);

    s.writei64(lenofsequence);
    s.writeu32array(sequence, lenofsequence);

    return 1;
}

integer GEN_Phi::load(loadkit &s) {
    s.loadi64(this->n);
    s.loadinteger(this->a);
    s.loadinteger(this->b);
    s.loadinteger(tablewidth);

    GammaTables &gammatables = GetGammaTables();
    this->zerostable_gamma = gammatables.zerostable.get();
    this->decodevaluenum_gamma = gammatables.decodevaluenum.get();
    this->decodebitsnum_gamma = gammatables.decodebitsnum.get();
    this->decoderesult_gamma = gammatables.decoderesult.get();

    DeltaTables &deltatables = GetDeltaTables();
    this->zerostable = deltatables.zerostable.get();
    this->decodevaluenum = deltatables.decodevaluenum.get();
    this->decodebitsnum = deltatables.decodebitsnum.get();
    this->decoderesult = deltatables.decoderesult.get();

    s.loadi64(lenofsuperoffset);
    superoffset = new i64[lenofsuperoffset];
    s.loadi64array(superoffset, lenofsuperoffset);

    offsects = new InArray();
    offsects->load(s, 0);

    samples = new InArray();
    samples->load(s, 0);

    method = new InArray();
    method->load(s, 0);

    s.loadi64(lenofsequence);
    sequence = new u32[lenofsequence];
    s.loadu32array(sequence, lenofsequence);
    return 1;
}
