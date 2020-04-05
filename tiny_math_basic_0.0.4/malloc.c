#include <stdio.h>
//#include "tiny_basic_interpreter.h"
#define MEM_ALLOC 1
#if defined(MEM_ALLOC) && MEM_ALLOC
#define alloc_printf printf
#else
#define alloc_printf(argv, ...)
#endif
#define MEM_SIZE 2 * 1024 /*内存池的大小 2 KBytes*/

static char mem[MEM_SIZE]; /*定义用来内存分配的数组*/

#define MEM_START &mem[0]      /*定义内存池的首地址*/
#define MEM_END &mem[MEM_SIZE] /*定义内存池的尾地址*/

enum USE_STA
{ /*定义内存块的使用状态(UNUSED 未使用)，(USED 已使用)*/
  UNUSED = 0,
  USED
};

#pragma pack(1)
typedef struct mem_block
{                              /*定义内存管理块的数据结构*/
    void *mem_ptr;             /*当前内存块的内存地址*/
    struct mem_block *nxt_ptr; /*下一个内存管理块的地址*/
    unsigned int mem_size;     /*当前内存块的大小*/
    enum USE_STA mem_sta;      /*当前内存块的状态*/
} mem_block;
#pragma pack()

#define BLK_SIZE sizeof(mem_block)     /*内存管理块的大小*/
#define HEAD_NODE (MEM_END - BLK_SIZE) /*头内存管理块的地址*/

static signed char mem_init_flag = -1; /*内存分配系统初始化的标志(-1 未初始化),(1 已初始化)*/

void * memset(void *s, int c, size_t n)
{
    unsigned char *p = (unsigned char *)s; //强制转换成字符型指针

    while (n > 0)
    {
        *p++ = (unsigned char)c;
        n--;
    }
    return s;
}

void mem_init(void)
{
    mem_block *node;

    memset(mem, 0x00UL, sizeof(mem));

    node = (mem_block *)HEAD_NODE;
    node->mem_ptr = MEM_START;
    node->nxt_ptr = (mem_block *)HEAD_NODE;
    node->mem_size = MEM_SIZE - BLK_SIZE;
    node->mem_sta = UNUSED;

    mem_init_flag = 1;
}

void *malloc(unsigned nbytes)
{
    unsigned int suit_size = 0xFFFFFFFFUL;
    mem_block *head_node = NULL, *tmp_node = NULL, *suit_node = NULL;

    if (nbytes == 0)
    {
        alloc_printf("illegal parameter\n");
        return NULL;
    }
    if (mem_init_flag < 0)
    {
        alloc_printf("please initialize first\n");
        mem_init();
    }

    head_node = tmp_node = (mem_block *)HEAD_NODE;
    while (1)
    {
        if (tmp_node->mem_sta == UNUSED)
        {
            if (nbytes <= tmp_node->mem_size && tmp_node->mem_size < suit_size)
            {
                suit_node = tmp_node;
                suit_size = suit_node->mem_size;
            }
        }
        tmp_node = tmp_node->nxt_ptr;
        if (tmp_node == head_node)
        {
            if (suit_node == NULL)
            {
                alloc_printf("NULL\r\n");
                return NULL;
            }
            break;
        }
    }

    if (nbytes <= suit_node->mem_size && (nbytes + BLK_SIZE) >= suit_node->mem_size)
    {
        suit_node->mem_sta = USED;
        return suit_node->mem_ptr;
    }
    else if (suit_node->mem_size > (nbytes + BLK_SIZE))
    {
        tmp_node = suit_node->mem_ptr;
        tmp_node = (mem_block *)((unsigned char *)tmp_node + nbytes);
        tmp_node->mem_ptr = suit_node->mem_ptr;
        tmp_node->nxt_ptr = suit_node->nxt_ptr;
        tmp_node->mem_size = nbytes;
        tmp_node->mem_sta = USED;

        suit_node->mem_ptr = (mem_block *)((unsigned char *)tmp_node + BLK_SIZE);
        suit_node->nxt_ptr = tmp_node;
        suit_node->mem_size -= (nbytes + BLK_SIZE);
        suit_node->mem_sta = UNUSED;

        return tmp_node->mem_ptr;
    }
    else
    {
        alloc_printf("%s,size err!\r\n", __FUNCTION__);
    }

    return NULL;
}
void free(void *ap)
{
    mem_block *head_node, *tmp_node, *nxt_node;

    if (ap == NULL)
        return;
    if (mem_init_flag < 0)
    {
        return;
    }
    head_node = tmp_node = (mem_block *)HEAD_NODE;
    while (1)
    {
        if (tmp_node->mem_ptr == ap)
        {
            if (tmp_node->mem_sta != UNUSED)
            {
                tmp_node->mem_sta = UNUSED;
                break;
            }
            else
            {
                alloc_printf("ap:0x%08x 已经释放,无需再次释放\r\n", ap);
                return;
            }
        }

        tmp_node = tmp_node->nxt_ptr;
        if (tmp_node == head_node)
        {
            alloc_printf("%s,can not found ap!\r\n", __FUNCTION__);
            return;
        }
    }

AGAIN:
    head_node = tmp_node = (mem_block *)HEAD_NODE;
    while (1)
    {
        nxt_node = tmp_node->nxt_ptr;
        if (nxt_node == head_node)
        {
            break;
        }
        if (tmp_node->mem_sta == UNUSED && nxt_node->mem_sta == UNUSED)
        {
            tmp_node->mem_ptr = nxt_node->mem_ptr;
            tmp_node->nxt_ptr = nxt_node->nxt_ptr;
            tmp_node->mem_size += nxt_node->mem_size + BLK_SIZE;
            tmp_node->mem_sta = UNUSED;

            goto AGAIN;
        }
        tmp_node = nxt_node;
    }
}