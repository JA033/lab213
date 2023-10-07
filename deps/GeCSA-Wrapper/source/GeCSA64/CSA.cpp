// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

/*============================================
# Filename: CSA.cpp
# Ver 2.0 2020-10-10
# Copyright (C) 2020 PengLong HongweiHuo
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the
License.
#
# Description:
=============================================*/
#include "CSA.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include <stdlib.h>

using namespace std;
#define SearchKind 2
#define TimesCount 5000
#define TimesLocate 5000

bool compare(const pair<i64, u64> x, const pair<i64, u64> y) {
    return x.second > y.second;
}

integer CSA::save(FILE *file) {
    savekit s;
    s.setfile(file);

    s.writeu64(198809102511);
    s.writei64(n);
    s.writeinteger(alphabetsize);

    s.writeinteger(SL);
    s.writeinteger(L);
    s.writeinteger(D);
    s.writeinteger(RD);

    SAL->write(s);
    RankL->write(s);
    ArrD->write(s);

    s.writeinteger(256);
    s.writei64array(code, 256);
    s.writeinteger(alphabetsize + 1);
    s.writei64array(start, alphabetsize + 1);
    s.writeinteger(alphabetsize);
    s.writei64array(incode, alphabetsize);
    Phi0->write(s);
    s.setfile();
    return 0;
}

integer CSA::save(const char *indexfile) {
    savekit s(indexfile);

    s.writeu64(198809102511);
    s.writei64(n);
    s.writeinteger(alphabetsize);

    s.writeinteger(SL);
    s.writeinteger(L);
    s.writeinteger(D);
    s.writeinteger(RD);

    SAL->write(s);
    RankL->write(s);
    ArrD->write(s);

    s.writeinteger(256);
    s.writei64array(code, 256);
    s.writeinteger(alphabetsize + 1);
    s.writei64array(start, alphabetsize + 1);
    s.writeinteger(alphabetsize);
    s.writei64array(incode, alphabetsize);
    Phi0->write(s);
    s.close();
    return 0;
}

integer CSA::load(FILE *file) {
    loadkit s;
    s.setfile(file);

    u64 magicnum;
    s.loadu64(magicnum);
    if (magicnum != 198809102511) {
        cerr << "Not a CSA file" << endl;
        exit(0);
    }
    s.loadi64(this->n);
    s.loadinteger(this->alphabetsize);
    s.loadinteger(SL);
    s.loadinteger(L);
    s.loadinteger(D);
    s.loadinteger(RD);
    SAL = new InArray();
    SAL->load(s, 0);
    RankL = new InArray();
    RankL->load(s, 0);
    ArrD = new InArray();
    ArrD->load(s, 1);
    integer len = 0;
    s.loadinteger(len);
    this->code = new i64[len];
    s.loadi64array(this->code, len);
    s.loadinteger(len);
    this->start = new i64[len];
    s.loadi64array(this->start, len);
    s.loadinteger(len);
    this->incode = new i64[len];
    s.loadi64array(this->incode, len);
    Phi0 = new Phi();
    Phi0->load(s);
    s.setfile();
    return 0;
}

integer CSA::load(const char *indexfile) {
    loadkit s(indexfile);
    u64 magicnum;
    s.loadu64(magicnum);
    if (magicnum != 198809102511) {
        cerr << "Not a CSA file" << endl;
        exit(0);
    }
    s.loadi64(this->n);
    s.loadinteger(this->alphabetsize);
    s.loadinteger(SL);
    s.loadinteger(L);
    s.loadinteger(D);
    s.loadinteger(RD);
    SAL = new InArray();
    SAL->load(s, 0);
    RankL = new InArray();
    RankL->load(s, 0);
    ArrD = new InArray();
    ArrD->load(s, 1);
    integer len = 0;
    s.loadinteger(len);
    this->code = new i64[len];
    s.loadi64array(this->code, len);
    s.loadinteger(len);
    this->start = new i64[len];
    s.loadi64array(this->start, len);
    s.loadinteger(len);
    this->incode = new i64[len];
    s.loadi64array(this->incode, len);
    Phi0 = new Phi();
    Phi0->load(s);
    s.close();
    return 0;
}

CSA::CSA(const uchar *str, u64 len, integer L, integer D) {
    this->SL = L * 18;
    this->L = L;
    this->D = D;
    this->RD = D * 16;
    this->code = new i64[256];
    for (integer i = 0; i < 256; i++)
        code[i] = 0;

    this->Blength = 256;
    this->alphabetsize = 0;

    // this->n = strlen(reinterpret_cast<const char *>(str));
    this->n = len;
    T = new uchar[this->n + 1];
    memcpy(T, str, n);
    T[n] = '\0';

    statics(T);
    int64_t *SA = new int64_t[n];
    divsufsort64(T, SA, n);
    parmaters p = {alphabetsize, n, SL, L, start, lastchar, SA, T, code};
    CreateSupportStructer(&p);

    if (SA != NULL)
        delete[] SA;
    if (T != NULL) {
        delete[] T;
        T = nullptr;
    }
}

CSA::CSA(const char *sourcefile, integer L, integer D) {
    this->SL = L * 18;
    this->L = L;
    this->D = D;
    this->RD = D * 16;
    this->code = new i64[256];
    for (integer i = 0; i < 256; i++)
        code[i] = 0;

    this->Blength = 256;
    this->alphabetsize = 0;
    T = GetFile(sourcefile);
    statics(T);
    int64_t *SA = new int64_t[n];
    divsufsort64(T, SA, n);
    parmaters p = {alphabetsize, n, SL, L, start, lastchar, SA, T, code};
    CreateSupportStructer(&p);

    if (SA != NULL)
        delete[] SA;
    if (T != NULL)
        delete[] T;

    /*string filepath(sourcefile);
    string indexname = filepath + to_string(L) + to_string(D) + ".geindex";
    if(FileExist(indexname.data())){
            load(indexname.data());
            cout<<"index load"<<endl;
    }else{
            this->SL=L*18;
            this->L =L;
            this->D =D;
            this->RD=D*16;
            this->code =new i64[256];
            for(integer i=0;i<256;i++)
                    code[i]=0;

    this->Blength=256;
            this->alphabetsize =0;
            T=GetFile(sourcefile);
            statics(T);
            int64_t *SA = new int64_t[n];
            divsufsort64(T, SA, n);
            parmaters p={alphabetsize,n,SL,L,start,lastchar,SA,T,code};
            CreateSupportStructer(&p);
            save(indexname.data());
            if(SA!=NULL)
                    delete [] SA;
            if(T!=NULL)
                    delete [] T;
    }*/
}

i64 CSA::getN() { return n; }

PhiStats CSA::getPhiStats() { return {}; }

/*PhiStats CSA::getPhiStats() {
        PhiStats stats;
        GEN_Phi *impl = (GEN_Phi *) Phi0->getImpl();
        stats.all1 = impl->numall1;
        stats.bv = impl->numbv;
        stats.gamma = impl->numgamma;
        stats.rldelta = impl->numrldelta;
        stats.totalblocks = stats.all1 + stats.bv + stats.gamma + stats.rldelta;
        stats.bv_size = Phi0->getEcodeSpace(1);
        stats.gamma_size = Phi0->getEcodeSpace(0);
        stats.rldelta_size = Phi0->getEcodeSpace(3);
        return stats;
}*/

integer CSA::getAlphabetsize() { return this->alphabetsize; }

CSA::~CSA(void) {
    delete SAL;
    delete RankL;
    delete[] start;
    delete Phi0;
    delete[] code;
    delete[] incode;
    if (ArrD != NULL)
        delete ArrD;
    // if(T!=NULL)
    //	delete [] T;
}

void CSA::counting(const uchar *Pattern, i64 &num) {
    i64 L = 0;
    i64 R = 0;
    u64 m = strlen((const char *)Pattern);
    Search2(Pattern, m, L, R);
    num = R - L + 1;
}

void CSA::counting(const uchar *pattern, u64 m, i64 &num) {
    i64 L = 0;
    i64 R = 0;
    Search2(pattern, m, L, R);
    num = R - L + 1;
}

void CSA::statics(uchar *T) {
    for (i64 i = 0; i < n; i++) {
        code[T[i]]++;
    }

    for (integer i = 0; i < 256; i++)
        if (code[i] != 0)
            this->alphabetsize++;
    this->start = new i64[this->alphabetsize + 1];
    this->start[this->alphabetsize] = n;
    this->start[0] = 0;
    i64 k = 1;
    i64 pre = 0;
    for (integer i = 0; i < 256; i++)
        if (code[i] != 0) {
            start[k] = pre + code[i];
            pre = start[k];
            k++;
        }
    this->incode = new i64[this->alphabetsize];
    k = 0;
    for (integer i = 0; i < 256; i++)
        if (code[i] != 0) {
            code[i] = k;
            incode[k] = i;
            k++;
        } else
            code[i] = -1;
    lastchar = T[n - 1];
}

uchar *CSA::GetFile(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        cout << "Be sure that the file is available" << endl;
        exit(0);
    }
    fseeko(fp, 0, SEEK_END);
    this->n = ftello(fp);
    uchar *T = new uchar[n];
    fseeko(fp, 0, SEEK_SET);
    cout << "the size of file is " << n << " Bytes " << endl;
    i64 e = 0;
    e = fread(T, sizeof(uchar), n, fp);
    if (e != n) {
        cout << "read the file failed!" << endl;
        exit(0);
    }
    fclose(fp);

    return T;
}

i64 CSA::sizeInByte() {
    return (SAL->GetMemorySize() + Phi0->Size() + RankL->GetMemorySize() +
            ArrD->GetrankMemorySize());
}
i64 CSA::sizeInByteForLocate() {
    return (SAL->GetMemorySize() + Phi0->Size() + ArrD->GetrankMemorySize());
}

i64 CSA::sizeInByteForCount() { return Phi0->Size(); }

double CSA::compressRatio() {

#ifdef TESTDATA
    cout << "space_gamma_phi: " << compressRatioforEcodeInPhi(0) << endl;
    cout << "space_bv_phi: " << compressRatioforEcodeInPhi(1) << endl;
    cout << "space_allone_phi: " << compressRatioforEcodeInPhi(2) << endl;
    cout << "space_rldelta_phi: " << compressRatioforEcodeInPhi(3) << endl;
#endif
    cout << "compressRatioForAll: " << sizeInByte() / (1.0 * n) << endl;
    return 0;
}

double CSA::compressRatioForCount() { return sizeInByteForCount() / (1.0 * n); }
double CSA::compressRatioForLocate() {
    return sizeInByteForLocate() / (1.0 * n);
}

void CSA::CreateSupportStructer(parmaters *csa) {
    i64 i = 0;
    i64 j = 0;

    integer step1 = D;
    SAL = new InArray(n / step1 + 2, blog(n / step1 + 1));
    RankL = new InArray(n / step1 + 2, blog(n / step1 + 1));
    ArrD = new InArray(n, 1);

    i64 bitOffset = 0;

    for (i = 0; i < n; i++) {
        if (csa->SA[i] % step1 == 0) {
            SAL->SetValue(j, csa->SA[i] / step1);
            j++;
            ArrD->SetValue(i, 1);
        } else {
            ArrD->SetValue(i, 0);
        }
        bitOffset++;
        if (bitOffset == 64) {
            bitOffset = 0;
        }
    }

    ArrD->constructrank(256, 16);

#if defined(GECSA_TEST_CSA_DMAP)
    std::cout << "-- Start testing D-map random accessing.\n";
    for (i64 i = 0; i < n; ++i) {
        bool ref = csa->SA[i] % step1 == 0;
        bool val = ArrD->getD(i);
        if (val != ref) {
            std::cout << "---- Error: index " << i << " (" << val << ") "
                      << "ref = " << ref << "\n";
            break; 
        }
    }
    std::cout << "-- Finished testing D-map random accessing.\n";

    std::cout << "-- Start testing D-map rank1.\n";
    u64 rank_sum = 0;
    for (i64 i = 0; i < n; ++i) {
        if (ArrD->GetValue(i)) {
            rank_sum += ArrD->GetValue(i);
            u64 ref = rank_sum;
            u64 val = ArrD->rank(i);
            if (val != ref) {
                std::cout << "---- Error: index " << i << " (" << val << ") "
                        << "ref = " << ref << "\n";
                break; 
            }
        }
    }
    std::cout << "-- Finished testing D-map rank1.\n";
#endif

    for (i64 i = 0; i < n; i++) {
        if (csa->SA[i] % step1 == 0) {
            RankL->SetValue(csa->SA[i] / step1, ArrD->rank(i));
        }
    }
    Phi0 = new Phi(csa);
}

void CSA::Search2(const uchar *Pattern, u64 m, i64 &L, i64 &R, i64 hintL, i64 hintR) {
    u64 len = m;

    if (len == 0) {
        L = 1;
        R = 0;
        return;
    }


    i64 Left = 0, Right = 0;

    Left = hintL;
    Right = hintR;

    i64 l0 = 0;
    i64 r0 = 0;

    for (integer i = len - 1; i >= 0; i--) {
        uchar c = Pattern[i];
        i64 coding = code[c];
        if (coding < 0) {
            Left = 1;
            Right = 0;
            break;
        }
        l0 = start[coding];
        r0 = start[coding + 1] - 1;
        if (coding == code[lastchar])
            l0++;
        Right = Phi0->RightBoundary(Right, l0, r0);
        Left = Phi0->LeftBoundary(Left, l0, r0);
        if (Left > Right) {
            Left = 1;
            Right = 0;
            break;
        }
    }
    L = Left;
    R = Right;
}

void CSA::Search2(const uchar *Pattern, u64 m, i64 &L, i64 &R) {
    u64 len = m;

    if (len == 0) {
        L = 1;
        R = 0;
        return;
    }

    unsigned char c = Pattern[len - 1];
    uchar doublec = Pattern[len - 2];
    i64 k = Phi0->doublec[doublec - '\0'][c - '\0'];
    i64 Left = 0, Right = 0;
    i64 coding = 0;
    if (m > 1 && k > -1) {
        Left = Phi0->range[k * 2];
        Right = Phi0->range[2 * k + 1];
        len--;
    } else {
        coding = code[c];
        if (coding < 0 || coding > alphabetsize - 1) {
            L = 1;
            R = 0;
            return;
        }
        Left = start[coding];
        Right = start[coding + 1] - 1;
    }
    i64 l0 = 0;
    i64 r0 = 0;

    for (integer i = len - 2; i >= 0; i--) {
        c = Pattern[i];
        coding = code[c];
        if (coding < 0) {
            Left = 1;
            Right = 0;
            break;
        }
        l0 = start[coding];
        r0 = start[coding + 1] - 1;
        if (coding == code[lastchar])
            l0++;
        Right = Phi0->RightBoundary(Right, l0, r0);
        Left = Phi0->LeftBoundary(Left, l0, r0);
        if (Left > Right) {
            Left = 1;
            Right = 0;
            break;
        }
    }
    L = Left;
    R = Right;
    return;
}

void CSA::SearchExtend(i64 &L, i64 &R, i64 baseL, i64 baseR, uchar c) {
    i64 coding = code[c];
    i64 Left = baseL, Right = baseR;
    if (coding < 0) {
        L = 1;
        R = 0;
        return;
    }
    i64 l0 = start[coding];
    i64 r0 = start[coding + 1] - 1;
    if (coding == code[lastchar])
        l0++;
    Right = Phi0->RightBoundary(Right, l0, r0);
    Left = Phi0->LeftBoundary(Left, l0, r0);
    if (Left > Right) {
        L = 1;
        R = 0;
        return;
    }
    L = Left;
    R = Right;
}

integer CSA::lookup(integer i) {
    integer D = this->D;
    integer step = 0;
    while (i % D != 0) {
        i = Phi0->GetValue(i);
        step++;
    }
    i = i / D;
    return (n + SAL->GetValue(i) - step) % n;
}

i64 CSA::newInverse(i64 st) {
    integer D = this->D;
    i64 st1 = st / D;
    i64 i1 = RankL->GetValue(st1);
    i64 i = ArrD->select(i1);
    i64 step = st % D;
    for (i64 j = 0; j < step; j++) {
        i = Phi0->GetValue(i);
    }
    return i;
}

uchar *CSA::extracting(i64 i, integer len) {
    if (i + len > n) {
        cout << "overshoot the length of the Doc." << endl;
        return NULL;
    }
    uchar *p = new uchar[len + 1];
    p[len] = '\0';
    i64 k = 0;
    i = this->newInverse(i);

    i64 Lpre = -1, Rpre = -1;
    integer j = 0;
    while (j < len) {
        if (Lpre <= i && i < Rpre) {
            p[j] = p[j - 1];
        } else {
            k = this->newPhi_list(i, Lpre, Rpre);
            p[j] = this->Character(k);
        }
        i = Phi0->GetValue(i);
        j++;
    }

    return p;
}

i64 CSA::extracting(i64 i, i64 len, uchar *p) {
    if (i + len > n) {
        cout << "overshoot the length of the Doc." << endl;
        return 0;
    }

    i64 k = 0;
    i = this->newInverse(i);

    i64 Lpre = -1, Rpre = -1;
    integer j = 0;
    while (j < len) {
        if (Lpre <= i && i < Rpre) {
            p[j] = p[j - 1];
        } else {
            k = this->newPhi_list(i, Lpre, Rpre);
            p[j] = this->Character(k);
        }
        i = Phi0->GetValue(i);
        j++;
    }
    return len;
}

i64 CSA::newPhi_list(i64 i, i64 &Lpre, i64 &Rpre) {
    i64 l = 0;
    i64 r = this->alphabetsize;
    i64 m = 0;
    while (l < r) {
        m = (l + r) / 2;
        if (start[m] <= i)
            l = m + 1;
        else
            r = m;
    }
    Lpre = start[r - 1];
    Rpre = start[r];
    return r - 1;
}

i64 CSA::LocalCharacter(i64 i) { return code[i]; }

i64 CSA::Character(i64 i) { return incode[i]; }

i64 CSA::AlphabetOffset(i64 c) {
    return c <= alphabetsize ? start[c] : -1;
}

InArray *CSA::GetArrD() {
    return ArrD;;
}

InArray *CSA::GetSAL() {
    return SAL;
}

InArray *CSA::GetRankL() {
    return RankL;
}

u64 CSA::blog(i64 x) {
    u64 ans = 0;
    while (x > 0) {
        ans++;
        x = x >> 1;
    }
    return ans;
}

i64 *CSA::locating(const uchar *Pattern, i64 &num) {
    i64 L = 0;
    i64 R = 0;

    u64 m = strlen((const char *)Pattern);

    this->Search2(Pattern, m, L, R);
    num = R - L + 1;
    if (L > R)
        return NULL;
    i64 *pos = new i64[num];
    newEnumerative2(L, R, pos);
    return pos;
}

i64 CSA::locating(const uchar *pattern, u64 m, i64 *out) {
    i64 L = 0;
    i64 R = 0;
    this->Search2(pattern, m, L, R);
    i64 num = R - L + 1;
    if (L > R)
        return 0;
    newEnumerative2(L, R, out);
    return num;
}

i64 CSA::GetPos(i64 index) {
    i64 i = index;
    i64 step = 0;
    while (ArrD->getD(i) != 1) {
        i = Phi0->GetValue(i);
        ++step;
    }
    i = ArrD->rank(i);
    i64 x = SAL->GetValue(i - 1) * D - step;
    return x < 0 ? x + n : x;
}

void CSA::newEnumerative2(i64 L, i64 R, i64 *pos) {
    integer D = this->D;
    InArray *SAL = this->SAL;
    i64 *distance = new i64[R - L + 1];
    i64 *pred = new i64[R - L + 1];
    i64 f = 0;
    i64 step = 0;
    i64 q = 0;
    i64 s = 0;
    i64 i = 0;

    for (i64 w = 0; w < R - L + 1; w++) {
        pos[w] = -1;
        pred[w] = -1;
    }

    for (i64 j = L; j <= R; j++) {
        f = 0;
        i = j;
        step = 0;
        while (ArrD->getD(i) != 1) {
            i = this->Phi0->GetValue(i);
            step++;
            if (L <= i && i <= R) {
                distance[j - L] = step;
                pred[i - L] = j;
                f = 1;
                break;
            }
        }
        if (f == 0) {
            i = ArrD->rank(i);
            pos[j - L] = SAL->GetValue(i - 1) * D - step;
            if (pos[j - L] < 0)
                pos[j - L] = pos[j - L] + n;
        }
    }

    for (i64 j = L; j <= R; j++) {
        if (pos[j - L] != -1) {
            q = j;
            while (pred[q - L] != -1) {
                s = pos[q - L];
                i = pred[q - L];
                step = distance[i - L];
                pos[i - L] = s - step;
                pred[q - L] = -1;
                q = i;
            }
        }
    }
    delete[] pred;
    delete[] distance;
}

void CSA::TestForCount(int index_time) {

    i64 pos;

    int len = 20;

    clock_t start, finish;
    uchar **Pattern_arr = new uchar *[index_time];
    for (int i = 0; i < index_time; i++) {

        pos = rand() % (this->n - len - 1);
        Pattern_arr[i] = extracting(pos, len);
    }
    long long count_back = 0;
    long long count_sum = 0;
    cout << "count test  " << index_time << " times" << endl;
    for (int i = 0; i < index_time; i++) {

        start = clock();
        this->counting(Pattern_arr[i], count_back);
        finish = clock();
        count_sum += finish - start;
    }
    cout << "average count time:"
         << count_sum * 1.0 / (index_time * CLOCKS_PER_SEC) * 1000000 << endl;

    for (int i = 0; i < index_time; i++) {
        delete[] Pattern_arr[i];
    }
    delete[] Pattern_arr;
}

void CSA::TestForExtract(int index_time) {
    i64 pos;

    int len = 20;
    cout << "extract test  " << index_time << " times" << endl;
    clock_t start, finish;
    long long extract_sum = 0;

    uchar **Pattern_arr = new uchar *[index_time];
    for (int i = 0; i < index_time; i++) {

        pos = rand() % (this->n - len - 1);
        start = clock();
        Pattern_arr[i] = extracting(pos, len);
        finish = clock();
        extract_sum += finish - start;
    }
    cout << "average extract time:"
         << extract_sum * 1.0 / (index_time * CLOCKS_PER_SEC) * 1000000 << endl;

    for (int i = 0; i < index_time; i++) {
        delete[] Pattern_arr[i];
    }
    delete[] Pattern_arr;
}

void CSA::TestForLocate(int index_time) {
    i64 pos;
    int len = 20;
    clock_t start, finish;
    uchar **Pattern_arr = new uchar *[index_time];
    for (int i = 0; i < index_time; i++) {
        pos = rand() % (this->n - len - 1);
        Pattern_arr[i] = extracting(pos, len);
    }
    long long locate_sum = 0;
    long long locate_back = 0;
    cout << "locate test  " << index_time << " times" << endl;
    for (int i = 0; i < index_time; i++) {

        long long *newtem = NULL;
        start = clock();
        newtem = this->locating(Pattern_arr[i], locate_back);
        finish = clock();

        locate_sum += finish - start;
        delete[] newtem;
    }

    cout << "average  locate time:"
         << locate_sum * 1.0 / (index_time * CLOCKS_PER_SEC) * 1000000 << endl;

    for (int i = 0; i < index_time; i++) {
        delete[] Pattern_arr[i];
    }
    delete[] Pattern_arr;
}

void CSA::TestForIndex(int index_time) {
    // extract
    i64 pos;

    int len = 20;
    cout << "extract test  " << index_time << " times" << endl;
    clock_t start, finish;
    long long extract_sum = 0;

    uchar **Pattern_arr = new uchar *[index_time];
    for (int i = 0; i < index_time; i++) {

        pos = rand() % (this->n - len - 1);
        start = clock();
        Pattern_arr[i] = extracting(pos, len);
        finish = clock();
        extract_sum += finish - start;
    }
    cout << "average_extract time: "
         << extract_sum * 1.0 / (index_time * CLOCKS_PER_SEC) * 1000000 << endl;

    long long locate_sum = 0;
    long long locate_back = 0;
    cout << "locate test  " << index_time << " times" << endl;
    for (int i = 0; i < index_time; i++) {

        long long *newtem = NULL;
        start = clock();
        newtem = this->locating(Pattern_arr[i], locate_back);
        finish = clock();

        locate_sum += finish - start;
        delete[] newtem;
    }

    cout << "average_locate time: "
         << locate_sum * 1.0 / (index_time * CLOCKS_PER_SEC) * 1000000 << endl;

    long long count_back = 0;
    long long count_sum = 0;
    cout << "count test  " << index_time << " times" << endl;
    for (int i = 0; i < index_time; i++) {

        start = clock();
        this->counting(Pattern_arr[i], count_back);
        finish = clock();
        count_sum += finish - start;
    }
    cout << "average_count time :"
         << count_sum * 1.0 / (index_time * CLOCKS_PER_SEC) * 1000000 << endl;

    for (int i = 0; i < index_time; i++) {
        delete[] Pattern_arr[i];
    }
    delete[] Pattern_arr;
}

double CSA::compressRatioforEcodeInPhi(int type) {
    return Phi0->getEcodeSpace(type) * 1.0 / Phi0->Size();
}
double CSA::compressRatioforEcodeInAll(int type) {
    return Phi0->getEcodeSpace(type) * 1.0 / sizeInByte();
}

double CSA::compressRationforSA() {
    return (SAL->GetMemorySize() + ArrD->GetrankMemorySize()) * 1.0 /
           sizeInByte();
}

double CSA::compressRationforRank() {
    return (RankL->GetMemorySize() + ArrD->GetrankMemorySize()) * 1.0 /
           sizeInByte();
}
double CSA::compressRationforRSD() {
    return (ArrD->GetrankMemorySize()) * 1.0 / sizeInByte();
}
double CSA::compressRatioforallsample() {
    return (SAL->GetMemorySize() + RankL->GetMemorySize() +
            ArrD->GetrankMemorySize()) *
           1.0 / sizeInByte();
}

void CSA::SplitString(const string &s, vector<string> &v, const string &c) {
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (string::npos != pos2) {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
        v.push_back(s.substr(pos1));
}

int CSA::FileExist(const char *fname) { return (access(fname, 0) == 0); }
