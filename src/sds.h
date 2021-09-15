/* SDSLib, A C dynamic strings library
 *
 * Copyright (c) 2006-2010, Salvatore Sanfilippo <antirez at gmail dot com>
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

#ifndef __SDS_H
#define __SDS_H

/*
 * 最大预分配长度
 */
#define SDS_MAX_PREALLOC (1024*1024)

#include <sys/types.h>
#include <stdarg.h>

/*
 * 类型别名，用于指向 sdshdr 的 buf 属性
 */
typedef char *sds;

/*
 * 保存字符串对象的结构
 */
struct sdshdr {

    // 记录 buf 数组中已使用字节的数量
    // 等于 SDS 所保存字符串的长度
    int len;

    // 记录 buf 数组中未使用字节的数量
    int free;

    // 字节数组，用于保存字符串
    char buf[];
};

/*
 * 返回 sds 实际保存的字符串的长度
 *
 * T = O(1)
 */
static inline size_t sdslen(const sds s) {
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->len;
}

/*
 * 返回 sds 可用空间的长度
 *
 * T = O(1)
 */
static inline size_t sdsavail(const sds s) {
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->free;
}

sds sdsnewlen(const void *init, size_t initlen);
// 创建一个包含给定 C 字符串的 SDS。时间复杂度O(N)，N 为给定 C 字符串的长度
sds sdsnew(const char *init);
// 创建一个不包含任何内容的空 SDS。时间复杂度O(1)
sds sdsempty(void);
// 返回 SDS 的已使用空间字节数。这个值可以通过读取 SDS 的 len 属性来 直接获得，复杂度为 0(1)
size_t sdslen(const sds s);
// 创建一个给定 SDS 的副本(copy)。时间复杂度O(N)，N 为给定 SDS 的长度
sds sdsdup(const sds s);
// 释放给定的 SDS。时间复杂度(N)，N 为被释放 SDS 的长度
void sdsfree(sds s);
// 返回 SDS 的未使用空间字节数。这个值可以通过读取 SDS 的 free 属性来直接获得，复杂度为 0(1)
size_t sdsavail(const sds s);
// 用空字符将 SDS 扩展至给定长度。O(N)，N 为扩展新增的字节数
sds sdsgrowzero(sds s, size_t len);
// 将长度为 len 的字符串 t 追加到 sds 的字符串末尾。O(N)
sds sdscatlen(sds s, const void *t, size_t len);
// 将给定 C 字符串拼接到 SDS 字符串的末尾。O(N), N 为被拼接 C 字符串的长度
sds sdscat(sds s, const char *t);
// 将给定 SDS 字符串拼接到另 一个 SDS 字符串 的末尾。O(N)，N 为被拼接 SDS 字符串的长度
sds sdscatsds(sds s, const sds t);
// 将字符串 t 的前 len 个字符复制到 sds s 当中，并在字符串的最后添加终结符。
sds sdscpylen(sds s, const char *t, size_t len);
// 将给定的 C 字符串复制到 SDS 里面，覆盖SDS 原有的字符串。O(N)，N为被复制 C 字符串的长度
sds sdscpy(sds s, const char *t);

sds sdscatvprintf(sds s, const char *fmt, va_list ap);
#ifdef __GNUC__
sds sdscatprintf(sds s, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
#else
sds sdscatprintf(sds s, const char *fmt, ...);
#endif

sds sdscatfmt(sds s, char const *fmt, ...);
// 接受一个 SDS 和一个 C 字符串作为参数，从SDS 中移除所有在 C 字符串中出现过的字符
sds sdstrim(sds s, const char *cset);
// 保留 SDS 给定区间内的数据，不在区间内的数据会被覆盖或清除。O(N)，N 为被保留数据的字节数
void sdsrange(sds s, int start, int end);
void sdsupdatelen(sds s);
// 清空 SDS 保存的字符串内容。因为惰性空间释放策略，复杂度为 0(1)
void sdsclear(sds s);
// 对比两个 SDS 字符串是否相同。O(N)，N 为两个 SDS 中较短的那个 SDS 的长度
int sdscmp(const sds s1, const sds s2);
sds *sdssplitlen(const char *s, int len, const char *sep, int seplen, int *count);
void sdsfreesplitres(sds *tokens, int count);
void sdstolower(sds s);
void sdstoupper(sds s);
sds sdsfromlonglong(long long value);
sds sdscatrepr(sds s, const char *p, size_t len);
sds *sdssplitargs(const char *line, int *argc);
sds sdsmapchars(sds s, const char *from, const char *to, size_t setlen);
sds sdsjoin(char **argv, int argc, char *sep);

/* Low level functions exposed to the user API */
sds sdsMakeRoomFor(sds s, size_t addlen);
void sdsIncrLen(sds s, int incr);
sds sdsRemoveFreeSpace(sds s);
size_t sdsAllocSize(sds s);

#endif
