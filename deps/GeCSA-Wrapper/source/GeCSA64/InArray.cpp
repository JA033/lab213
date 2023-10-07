/*============================================
# Filename: InArray.cpp
# Ver 2.0 2020-10-10
# Copyright (C) 2020 PengLong HongweiHuo
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the
License.
#
# Description:
=============================================*/
#include "InArray.h"
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

InArray::~InArray(void) {
    if (data)
        delete[] data;
    if (rankflag) {
        delete Md;
        delete Sd;
        delete SBd;
        delete Bd;
    }
}

InArray::InArray() {
    data = nullptr;
    rankflag = 0;
}

InArray::InArray(u64 len, u32 size) {
    rankflag = 0;
    this->datanum = len;
    this->datawidth = size;
    u64 totlesize = datanum * datawidth;
    if (totlesize % 32 == 0)
        totlesize = totlesize / 32 + 1;
    else
        totlesize = (totlesize / 32) + 2;
    this->data = new u32[totlesize + 1];
    memset(data, 0, 4 * totlesize);
    mask = (u64)pow(2, datawidth) - 1;
}
u64 InArray::GetrankMemorySize() {
    return Md->GetMemorySize() + Sd->GetMemorySize() + SBd->GetMemorySize() +
           Bd->GetMemorySize();
}
int ablog(int x) {
    int ans = 0;
    x--;
    while (x > 0) {
        ans++;
        x = x >> 1;
    }
    return ans;
}

u64 InArray::GetValue(u64 index) {
    if (index > datanum - 1) {
        cerr << "InArray:GetValue: index out of boundary" << endl;
        __builtin_trap();
        exit(0);
    }
    u64 anchor = (index * datawidth) >> 5;
    u64 temp1 = data[anchor];
    u32 temp2 = data[anchor + 1];
    u32 temp3 = data[anchor + 2];
    i32 overloop = ((anchor + 2) << 5) - (index + 1) * datawidth;
    temp1 = (temp1 << 32) + temp2;
    if (overloop < 0) {
        temp1 = (temp1 << (-overloop)) + (temp3 >> (32 + overloop));
        return temp1 & mask;
    } else
        return (temp1 >> overloop) & mask;
}

void InArray::SetValue(u64 index, u64 v) {
    if (index > datanum - 1) {
        cerr << "InArray:index out of boundary" << endl;
        exit(0);
    } else if (v > mask) {
        cerr << "InArray:value is out of boundary" << endl;
        exit(0);
    } else {
        u64 value = v;
        u64 anchor = (index * datawidth) >> 5;
        u64 temp1 = data[anchor];
        u32 temp2 = data[anchor + 1];
        temp1 = (temp1 << 32) + temp2;
        i32 overloop = ((anchor + 2) << 5) - (index + 1) * datawidth;
        if (overloop < 0) {
            value = (value >> (-overloop));
            temp1 = temp1 + value;
            data[anchor + 1] = (temp1 & (0xffffffff));
            data[anchor] = (temp1 >> 32) & (0xffffffff);
            data[anchor + 2] = (v << (32 + overloop)) & (0xffffffff);
        } else {
            value = (value << overloop);
            temp1 = temp1 + value;
            data[anchor + 1] = (temp1 & (0xffffffff));
            data[anchor] = (temp1 >> 32) & (0xffffffff);
        }
    }
}

u64 InArray::GetNum() { return datanum; }

u64 InArray::GetMemorySize() { return (datanum * datawidth) / 8; }

u32 InArray::GetDataWidth() { return datawidth; }

i64 InArray::write(savekit &s) {
    if (!rankflag) {
        s.writeu64(datanum);
        s.writeu32(datawidth);
        i64 len = (datanum * datawidth);
        if (len % 32 == 0)
            len = len / 32;
        else
            len = len / 32 + 1;
        s.writei64(len);
        s.writeu32array(data, len);
        return 1;
    } else {
        s.writeinteger(Blength);
        s.writeinteger(SBlength);
        s.writei64(block_len);
        s.writei64(super_block_len);
        s.writeu64(datanum);
        Md->write(s);
        Sd->write(s);
        SBd->write(s);
        Bd->write(s);
        return 1;
    }
}

i64 InArray::load(loadkit &s, int flag) {
    if (!flag) {
        s.loadu64(datanum);
        s.loadu32(datawidth);
        i64 len = 0;
        s.loadi64(len);
        data = new u32[len];
        s.loadu32array(data, len);
        mask = (((u64)1 << datawidth) - 1);
        return 1;
    } else {
        s.loadinteger(Blength);
        s.loadinteger(SBlength);
        s.loadi64(block_len);
        s.loadi64(super_block_len);
        s.loadu64(datanum);
        Md = new InArray();
        Md->load(s, 0);
        Sd = new InArray();
        Sd->load(s, 0);
        SBd = new InArray();
        SBd->load(s, 0);
        Bd = new InArray();
        Bd->load(s, 0);
        rankflag = 1;
        return 1;
    }
}

static inline u64 div_ceil(u64 a, u64 b) {
    return a == 0 ? 0 : (a - 1) / b + 1;
}

static inline u32 bit_width(u64 x) {
    return 64 - __builtin_clzll(x);
}

void InArray::constructrank(u64 data_num, u64 *positions, u64 num, int b_length, int sb_length) {
    rankflag = 1;
    this->datanum = data_num;
    this->Blength = ablog(b_length);
    this->SBlength = ablog(sb_length);

    const u32 b_width = this->Blength;
    //const u32 sb_width = this->SBlength;
    
    const u64 n_blocks = div_ceil(data_num, b_length);
    const u64 n_sblocks = div_ceil(n_blocks, sb_length);

    this->block_len = n_blocks;
    this->super_block_len = n_sblocks;

    // Aliasing
    InArray *&blocks = Bd;
    InArray *&sblocks = SBd;
    InArray *&block_flags = Md;
    InArray *&offsets = Sd;

    blocks = new InArray(n_blocks + 1, bit_width(b_length * sb_length + 1));
    sblocks = new InArray(n_sblocks, bit_width(num + 1));
    block_flags = new InArray(n_blocks, 1);
    offsets = new InArray(num, b_width);

    u64 *position_end = positions + num;
    u64 *position_iter = positions;
    u64 block_num1_acc = 0;
    u64 sblock_num1_acc = 0;

    for (i64 i = 0; i < (i64) n_blocks - 1; ++i) {
        if (i % sb_length == 0) {
            u64 sblock_idx = i / sb_length;
            sblocks->SetValue(sblock_idx, sblock_num1_acc);
            block_num1_acc = 0;
        }
        
        u64 block_start = i * b_length;
        u64 block_end = (i + 1) * b_length;
        u64 *iter = std::lower_bound(position_iter, position_end, block_end);
        
        u64 num1 = iter - position_iter;
        blocks->SetValue(i, block_num1_acc);
        block_num1_acc += num1;
        sblock_num1_acc += num1;

        if (num1 != 0)
            block_flags->SetValue(i, true);

        while (position_iter != iter) {
            const u64 j = position_iter - positions;
            offsets->SetValue(j, *position_iter - block_start);
            ++position_iter;
        }
    }

    u64 block_idx = n_blocks - 1;

    if (block_idx % sb_length == 0) {
        u64 sblock_idx = block_idx / sb_length;
        sblocks->SetValue(sblock_idx, sblock_num1_acc);
        block_num1_acc = 0;
    }

    u64 block_start = block_idx * b_length;
    u64 num1 = position_end - position_iter;
    blocks->SetValue(block_idx, block_num1_acc);
    block_num1_acc += num1;
    blocks->SetValue(block_idx + 1, block_num1_acc);

    if (num1 != 0)
        block_flags->SetValue(block_idx, true);

    while (position_iter != position_end) {
        const u64 j = position_iter - positions;
        offsets->SetValue(j, *position_iter - block_start);
        ++position_iter;
    }
}

void InArray::constructrank(int Blength, int SBlength) {
    rankflag = 1;
    this->Blength = ablog(Blength);
    this->SBlength = ablog(SBlength);
    i64 totlesize = datanum * datawidth;
    if (totlesize % 32 == 0)
        totlesize = totlesize / 32;
    else
        totlesize = (totlesize / 32) + 1;
    u64 sum1 = 0;
    for (int i = 0; i < totlesize; i++)
        sum1 += (u32)__builtin_popcount(data[i]);
    u64 Mdm;
    if (datanum * datawidth % Blength == 0)
        Mdm = datanum * datawidth / Blength;
    else
        Mdm = datanum * datawidth / Blength + 1;
    this->block_len = Mdm;
    Md = new InArray(Mdm, 1);
    Sd = new InArray(sum1, ablog(Blength));
    SBd = new InArray(((datanum * datawidth / Blength + 1) / SBlength + 2),
                      ablog(sum1 + 1));
    Bd = new InArray(Mdm + 2, ablog(Blength * (SBlength - 1) + 1));
    u64 Sdcount = 0;
    int tem;
    u64 now = 0;
    for (u64 i = 0; i < Mdm; i++) {
        int flag = 0;
        for (int j = 0; j < Blength; j++) {
            now = i * Blength + j;
            if (now >= datanum)
                break;
            tem = GetValue(now);
            if (tem == 1) {
                flag = 1;
                Sd->SetValue(Sdcount, j);
                Sdcount++;
            }
        }
        if (flag != 0)
            Md->SetValue(i, 1);
        else
            Md->SetValue(i, 0);
    }
    i64 SBnum;
    if (Mdm % SBlength == 0)
        SBnum = Mdm / SBlength + 1;
    else
        SBnum = Mdm / SBlength + 2;
    this->super_block_len = SBnum;
    i64 SBsum1 = 0;
    now = 0;
    for (i64 i = 0; i < SBnum; i++) {
        SBd->SetValue(i, SBsum1);
        i64 Bdsum1 = 0;
        for (i64 j = 0; j < SBlength; j++) {
            if (now >= datanum)
                break;
            Bd->SetValue(i * SBlength + j, Bdsum1);
            for (i64 k = 0; k < Blength; k++) {
                now = i * (SBlength * Blength) + j * Blength + k;
                if (now >= datanum)
                    break;
                else {
                    int tem = GetValue(now);
                    if (tem == 1) {
                        SBsum1++;
                        Bdsum1++;
                    }
                }
            }
        }
    }
}

integer InArray::getD(u64 i) {
    u64 Mdnum = i >> Blength;
    if (Md->GetValue(Mdnum) == 0)
        return 0;
    else {
        u64 SBnum = Mdnum >> SBlength;
        u64 Sbdbefore = SBd->GetValue(SBnum);
        u64 Sdnowi = Sbdbefore + Bd->GetValue(Mdnum);
        u64 Sdnow = Sd->GetValue(Sdnowi);
        if (((Mdnum << Blength) + Sdnow) > i)
            return 0;
        else if (((Mdnum << Blength) + Sdnow) == i)
            return 1;
        else {
            u64 temnum1 = 0;
            if (Mdnum != Md->GetNum() - 1) {
                u64 nextBd = Bd->GetValue(Mdnum + 1);
                if (nextBd == 0)
                    temnum1 = SBd->GetValue(SBnum + 1) - Sbdbefore -
                              Bd->GetValue(Mdnum);
                else
                    temnum1 = nextBd - Bd->GetValue(Mdnum);
            } else {
                temnum1 = Sd->GetNum() - Sdnowi;
            }
            for (u64 j = 1; j < temnum1; j++) {
                Sdnow = Sd->GetValue(Sdnowi + j);
                if (((Mdnum << Blength) + Sdnow) > i)
                    return 0;
                else if (((Mdnum << Blength) + Sdnow) == i)
                    return 1;
            }
            return 0;
        }
    }
}

u64 InArray::rank(u64 i){
    u64 Mdnum=i>>Blength;
    u64 SBnum=Mdnum>>SBlength;
    u64 Sbdbefore=SBd->GetValue(SBnum);
    u64 Sdnowi=Sbdbefore+Bd->GetValue(Mdnum);
    while(1)
    {
        u64 Sdnow=Sd->GetValue(Sdnowi);
        if(((Mdnum<<Blength)+Sdnow)==i)break;
        Sdnowi++;
    }
    return Sdnowi+1;
}  

u64 InArray::rankGeneral(u64 i) {
    u64 Mdnum = i >> Blength;
    u64 SBnum = Mdnum >> SBlength;
    u64 Sbdbefore = SBd->GetValue(SBnum);
    u64 Sdnowi = Sbdbefore + Bd->GetValue(Mdnum);
    
    u64 nxtMdnum = Mdnum + 1;
    u64 nxtSBnum = nxtMdnum >> SBlength;
    u64 nxtSbdbefore = SBd->GetValue(nxtSBnum);
    u64 nxtSdnowi = nxtSbdbefore + Bd->GetValue(nxtMdnum);

    while (1) {
        if (Sdnowi >= nxtSdnowi)
            break;
        u64 Sdnow = Sd->GetValue(Sdnowi);
        u64 idx = ((Mdnum << Blength) + Sdnow);
        if (idx == i)
            return Sdnowi + 1;
        if (idx > i)
            break;
        Sdnowi++;
    }
    return Sdnowi;
}

u64 InArray::select(u64 i) {
    if (i <= 0 || i > datanum) {
        cout << "select out boundary" << endl;
        exit(EXIT_FAILURE);
    }

    u64 l = 1, h = super_block_len - 1;
    u64 spj = blockbinarySearch(SBd, i, l, h);
    u64 k1 = i - SBd->GetValue(spj);
    l = spj << this->SBlength;
    h = ((spj + 1) << this->SBlength) - 1;

    if (h >= this->block_len) {
        h = this->block_len - 1;
    }
    u64 bpj = blockbinarySearch(Bd, k1, l + 1, h);

    return (bpj << this->Blength) + Sd->GetValue(i - 1);
}

u64 InArray::blockbinarySearch(InArray *B, u64 i, u64 l, u64 r) {
    if (B->GetValue(l) == i)
        return l - 1;
    if (B->GetValue(r) == i) {
        while (B->GetValue(r) == i) {
            r--;
        }
        return r;
    }
    if (B->GetValue(r) < i)
        return r;
    while (l <= r) {
        u64 midpos = (l + r) >> 1;
        if (midpos == l) {
            if (B->GetValue(l) >= i) {
                return l - 1;
            } else {
                return l;
            }
        }
        u64 mid = B->GetValue(midpos);
        if (mid < i)
            l = midpos + 1;
        else if (mid >= i)
            r = midpos;
    }
    return 0;
}
