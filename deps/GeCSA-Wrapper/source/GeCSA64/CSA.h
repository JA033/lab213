// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

/*============================================
# Filename: CSA.h
# Ver 2.0 2020-10-10
# Copyright (C) 2020 PengLong HongweiHuo
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the
License.
#
# Description:
  compressed suffix array,a self-index structure.
  CSA(filename) will build a csa-index for a document nameed filename.
                it's useful for text data,but not for multi-media data.
=============================================*/
#pragma once
#include "BaseClass.h"
#include "InArray.h"
#include "Phi.h"
#include "divsufsort/divsufsort64.h"
#include "loadkit.h"
#include "parmaters.h"
#include "savekit.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <vector>
using namespace std;

struct PhiStats {
    u64 all1 = 0;
    u64 rldelta = 0;
    u64 gamma = 0;
    u64 bv = 0;
    u64 totalblocks = 0;
    u64 all1_size = 0;
    u64 rldelta_size = 0;
    u64 gamma_size = 0;
    u64 bv_size = 0;
};

class CSA {
public:
    CSA(){};
    CSA(const uchar *str, u64 n, integer L, integer D);
    CSA(const char *sourcefile, integer L = 256, integer D = 256);
    ~CSA();

    // bool existential(const uchar * Pattern);
    void counting(const uchar *Pattern, i64 &num);
    void counting(const uchar *pattern, u64 m, i64 &num);

    void Search2(const uchar *Pattern, u64 m, i64 &L, i64 &R);
    void Search2(const uchar *Pattern, u64 m, i64 &L, i64 &R, i64 hintL, i64 hintR);
    void SearchExtend(i64 &L, i64 &R, i64 baseL, i64 baseR, uchar c);
    void newEnumerative2(i64 l, i64 r, i64 *pos);
    i64 GetPos(i64 index);

    i64 *locating(const uchar *Pattern, i64 &num);

    i64 locating(const uchar *pattern, u64 m, i64 *out);

    uchar *extracting(i64 start, integer len);
    i64 extracting(i64 start, i64 len, uchar *out);

    integer save(const char *indexfile);
    integer save(FILE *file);
    integer load(const char *indexfile);
    integer load(FILE *file);

    i64 getPhi(i64 index) { return Phi0->GetValue(index); }

    integer getAlphabetsize();
    i64 getN();
    i64 sizeInByte();
    i64 sizeInByteForLocate();
    i64 sizeInByteForCount();
    double compressRatio();
    double compressRatioForCount();
    double compressRatioForLocate();
    integer getL() { return L; }
    integer getD() { return D; }

    PhiStats getPhiStats();
    i64 getPhiSizeInBytes() { return Phi0->Size(); }
    i64 getSaSamplesSizeInBytes() { return SAL->GetMemorySize() / 2; }
    i64 getIsaSamplesSizeInBytes() { return SAL->GetMemorySize() / 2; }
    i64 getDmapSizeInBytes() { return ArrD->GetrankMemorySize(); }

    i64 n;

    void TestForCount(int count = 5000);
    void TestForExtract(int count = 5000);
    void TestForLocate(int count = 5000);
    void TestForIndex(int index_time = 5000);

    uchar *T;
    integer lookup(integer i);

    double compressRatioforEcodeInPhi(int type);
    double compressRatioforEcodeInAll(int type);
    double compressRationforSA();
    double compressRationforRank();
    double compressRationforRSD();
    double compressRatioforallsample();

    i64 LocalCharacter(i64 i);
    i64 Character(i64 i);
    i64 AlphabetOffset(i64 i);

    InArray *GetArrD();
    InArray *GetSAL();
    InArray *GetRankL();

private:
    CSA(const CSA &);
    CSA &operator=(const CSA &right);

    void CreateSupportStructer(parmaters *csa);

    i64 newInverse(i64 i);
    i64 newPhi_list(i64 i, i64 &Lpre, i64 &Rpre);

    void Search(const uchar *Pattern, i64 &L, i64 &R);

    uchar *GetFile(const char *filename);
    void statics(uchar *T);

    u64 blog(i64 x);

    void SplitString(const string &s, vector<string> &v, const string &c);
    int FileExist(const char *fname);

    integer SL, L, D, RD, NewD;
    InArray *SAL;
    InArray *RankL;
    Phi *Phi0;
    i64 *code;
    i64 *incode;
    integer alphabetsize;
    i64 *start;
    uchar lastchar;
    integer Blength;

    InArray *ArrD;
};
