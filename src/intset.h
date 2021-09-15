/*
 * Copyright (c) 2009-2012, Pieter Noordhuis <pcnoordhuis at gmail dot com>
 * Copyright (c) 2009-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __INTSET_H
#define __INTSET_H
#include <stdint.h>

typedef struct intset {
    
    // 编码方式
    uint32_t encoding;

    // 集合包含的元素数量
    // length 属性记录了整数集合包含的元素数量，也即是 contents 数组的长度。
    uint32_t length;

    // 保存元素的数组
    // contents 数组是整数集合的底层实现:整数集合的每个元素都是 contents 数组的一个数组项(item)，
    // 各个项在数组中按值的大小从小到大有序地排列，并且数组中不包含任何重复项。
    int8_t contents[];

} intset;

// 创建一个新的压缩列表
// T = O(1)
intset *intsetNew(void);
// 将给定元素添加到整数集合里面
// T = O(N)
intset *intsetAdd(intset *is, int64_t value, uint8_t *success);
// 从整数集合中移除给定元素
// T = O(N)
intset *intsetRemove(intset *is, int64_t value, int *success);
// 检查给定值是否存在于集合
// 因为底层数组有序，查找可以通过二分查找法来进行，所以复杂度为 O(logN)
uint8_t intsetFind(intset *is, int64_t value);
// 从整数集合中随机返回一个元素
// T = O(1)
int64_t intsetRandom(intset *is);
// 取出底层数组在给定索引上的元素
// T = O(1)
uint8_t intsetGet(intset *is, uint32_t pos, int64_t *value);
// 返回整数集合包含的元素个数
// T = O(1)
uint32_t intsetLen(intset *is);
// 返回整数集合占用的内存字节数
// T = O(1)
size_t intsetBlobLen(intset *is);

#endif // __INTSET_H
