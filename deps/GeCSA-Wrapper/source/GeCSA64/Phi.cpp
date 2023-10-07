// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

/*================:============================
# Filename: Phi.cpp
# Ver 2.0 2020-10-10
# Copyright (C) 2020 PengLong HongweiHuo
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the
License.
#
# Description:
=============================================*/
#include "Phi.h"
#define MAXNUM 99999999

Phi::Phi() {
    type = 0;
    phi = NULL;
}
Phi::Phi(parmaters *csa) {

    phivalue = this->PreSpace(csa);

    /*
    int64_t gap_1_num = 0,gap_2_3_num = 0,gap_4_15_num = 0,gap_16_num = 0;
    int64_t gap = 0;
    for (int64_t i = 1; i < n; i++)
    {
            gap = phivalue[i]-phivalue[i-1];
            if(gap == 1){
                    gap_1_num++;
            }else if(gap > 1 && gap < 4){
                    gap_2_3_num++;
            }else if( gap > 3 && gap < 16){
                    gap_4_15_num++;
            }else{
                    gap_16_num++;
            }
    }
    cout<< "1: "<< gap_1_num <<" "<<gap_1_num * 1.0 / n << endl;
    cout<< "2-3: "<< gap_2_3_num <<" "<<gap_2_3_num * 1.0 / n << endl;
    cout<< "3-15: "<< gap_4_15_num <<" "<<gap_4_15_num * 1.0 / n << endl;
    cout<< "16~: "<< gap_16_num <<" "<<gap_16_num * 1.0 / n << endl;

    */

    int64_t numof1 = 0;
    int64_t run_1_num = 0;
    int64_t len_runs_1 = 0;
    int64_t len_1 = 0;
    for (int64_t i = 1; i < n; i++) {
        if ((phivalue[i] - phivalue[i - 1]) == 1) {
            numof1++;
            len_1++;
        } else {
            if (len_1 != 0) {
                len_runs_1 += len_1;
                run_1_num++;
                len_1 = 0;
            }
        }
    }

#ifdef TESTDATA
    cout << "gap test" << endl;
    cout << "1-gap-num: " << numof1 << endl;
    cout << "1_ratio: " << numof1 * (1.0) / n << endl;
    cout << "len_all_runs: " << len_runs_1 << endl;
    cout << "average_runs: " << len_runs_1 * (1.0) / run_1_num << endl;
/////////
#endif

    double multi = 1.0;
    if ((numof1 * (1.0) / n) < 0.9)
        multi = 1.8;

    this->T = csa->T;
    this->code = csa->code;
    this->start = csa->start;
    this->lastchar = csa->lastchar;
    randchar(T);
    delete[] csa->SA;
    csa->SA = NULL;

    phi = new GEN_Phi(csa, phivalue, multi);
}

i64 Phi::GetValue(i64 i) { return phi->GetValue(i); }

i64 Phi::RightBoundary(i64 r, i64 L, i64 R) {
    return phi->RightBoundary(r, L, R);
}

i64 Phi::LeftBoundary(i64 l, i64 L, i64 R) {
    return phi->LeftBoundary(l, L, R);
}

i64 Phi::Size() { return phi->GetMemorySize(); }

integer Phi::write(savekit &s) {
    for (int i = 0; i < 256; i++) {
        s.writei64array(doublec[i], 256);
    }

    s.writeinteger(range_len);
    s.writei64array(range, range_len);
    phi->write(s);
    return 1;
}

integer Phi::load(loadkit &s) {
    this->doublec = new i64 *[256];
    for (int i = 0; i < 256; i++)
        doublec[i] = new i64[256];
    for (int i = 0; i < 256; i++) {
        s.loadi64array(doublec[i], 256);
    }

    s.loadinteger(range_len);
    this->range = new i64[range_len];
    s.loadi64array(range, range_len);

    phi = new GEN_Phi();
    phi->load(s);
    return 0;
}

Phi::~Phi() {
    if (phi != NULL)
        delete phi;
    phi = NULL;
}

i64 Phi::blogsize(i64 x) {
    i64 len = 0;
    while (x > 0) {
        x = (x >> 1);
        len++;
    }
    return len;
}

i64 Phi::puredelta(i64 v) {
    i64 x = blogsize(v);
    i64 pre = 2 * blogsize(x) - 1;
    return pre + x - 1;
}

i64 *Phi::PreSpace(parmaters *csa) {

    i64 i, j;
    this->n = csa->n;
    i64 *phivalue = new i64[n];
    i64 *temp = new i64[csa->alphabetsize + 1];
    for (i64 i = 0; i < csa->alphabetsize + 1; i++)
        temp[i] = csa->start[i];
    i64 index = temp[csa->code[csa->lastchar]];
    temp[csa->code[csa->lastchar]]++;
    i64 h = 0;
    unsigned char c = 0;
    i64 pos = 0;
    for (i64 i = 0; i < n; i++) {
        pos = csa->SA[i];
        if (pos == 0) {
            h = i;
            PrimayIndex = i;
            continue;
        }
        c = csa->T[pos - 1];
        phivalue[temp[csa->code[c]]++] = i;
    }
    phivalue[index] = h;

    delete[] temp;

    i64 pre = 0;
    int a = csa->SL;
    int b = csa->L;
    i64 x = n / a;
    i64 gap = 0;
    i64 len_bv = 0;
    i64 len_g = 0;
    i64 num_bv = 0;

    i64 num_g = 0;
    i64 num1 = 0;
    i64 runsflag = 0;
    i64 one_runs = 0;

    for (i = 0; i < x + 1; i++) {
        for (j = i * a; j < (i + 1) * a && j < n; j++) {
            if (j % b == 0) {
                len_g = 0;
                pre = phivalue[j];
                if ((j / b + 1) * b < n)
                    len_bv = phivalue[(j / b + 1) * b] - phivalue[j];
                continue;
            }
            gap = phivalue[j] - pre;
            if (gap < 0) {
                gap = n + gap;
                len_bv = MAXNUM;
            }

            len_g = len_g + 2 * blogsize(gap) - 1;

            if (gap == 1) {
                num1++;
                if (runsflag == 0) {
                    one_runs++;
                    runsflag++;
                } else
                    runsflag++;
            } else
                runsflag = 0;

            pre = phivalue[j];

            if ((j + 1) % b == 0 || (j + 1) == n) {

                if (len_bv <= len_g)
                    num_bv++;
            }
        }
    }

    return phivalue;
}

void Phi::randchar(uchar *T) {
    i64 times = 20000;
    i64 pos = 0;
    i64 num = 0;
    this->mini = 200;
    this->doublec = new i64 *[256];
    for (i64 i = 0; i < 256; i++)
        doublec[i] = new i64[256];
    for (i64 i = 0; i < 256; i++)
        for (i64 j = 0; j < 256; j++)
            doublec[i][j] = -1;
    for (i64 i = 0; i < times; i++) {
        pos = rand() % (this->n - 21);
        doublec[T[pos] - '\0'][T[pos + 1] - '\0']++;
    }

    for (i64 i = 0; i < 256; i++) {
        for (i64 j = 0; j < 256; j++) {
            if (doublec[i][j] > (mini - 1))
                doublec[i][j] = num++;
            else
                doublec[i][j] = -1;
        }
    }
    this->range = new i64[num * 2];
    this->range_len = num * 2;
    num = 0;
    for (i64 i = 0; i < 256; i++)
        for (i64 j = 0; j < 256; j++)
            if (doublec[i][j] >= 0) {
                num = doublec[i][j];
                range[2 * num] = left(i, j);
                range[2 * num + 1] = right(i, j);
            }
}

i64 Phi::left(i64 i, i64 j) {
    i64 coding = this->code[j + '\0'];
    i64 L = start[coding];
    i64 R = start[coding + 1] - 1;

    coding = code[i + '\0'];
    i64 Left = start[coding];
    i64 Right = start[coding + 1] - 1;
    if (coding == code[lastchar])
        Left++;
    for (i64 k = Left; k <= Right; k++)
        if (phivalue[k] >= L) {
            Left = k;
            break;
        }
    return Left;
}
i64 Phi::right(i64 i, i64 j) {
    i64 coding = this->code[j + '\0'];
    i64 L = start[coding];
    i64 R = start[coding + 1] - 1;

    coding = code[i + '\0'];
    i64 Left = start[coding];
    i64 Right = start[coding + 1] - 1;
    if (coding == code[lastchar])
        Left++;
    i64 k = Left;
    while (phivalue[k] <= R) {
        if (k == Right)
            return Right;
        k++;
    }
    Right = k - 1;
    return Right;
}

i64 Phi::getEcodeSpace(int type) { return phi->sizeofEcode(type); }
