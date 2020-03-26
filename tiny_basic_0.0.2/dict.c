
#include<stdio.h>
struct nlist
{                       /* table entry: */
    struct nlist *next; /* next entry in chain */
    char *name; /* defined name */
    char *defn; /* replacement text */
};
#define HASHSIZE 101
static struct nlist *hashtab[HASHSIZE]; /* pointer table */
/* hash: form hash value for string s */
unsigned hash(char *s)
{
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}
/* lookup: look for s in hashtab */
struct nlist *lookup(char *s)
{
    struct nlist *np;
    for (np = hashtab[hash(s)]; np != NULL; np = np->next)
        if (strcmp(s, np->name) == 0)
            return np; /* found */
    return NULL; /* not found */
}
char *strdup(char *);
/* install: put (name, defn) in hashtab */
struct nlist *install(char *name, char *defn)

{
    struct nlist *np;
    unsigned hashval;
    if ((np = lookup(name)) == NULL)
    { /* not found */
    np = (struct nlist *)malloc(sizeof(*np));
        if (np == NULL || (np->name = strdup(name)) == NULL)
            return NULL;
        hashval = hash(name);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    }
    else                        /* already there */
    free((void *)np->defn); /*free previous defn */
    if ((np->defn = strdup(defn)) == NULL)
        return NULL;
    return np;
}
char *strdup(char *s) /* make a duplicate of s */

{
    char *p;
    p = (char *)malloc(strlen(s) + 1); /* +1 for ’\0’ */
    if (p != NULL)
        strcpy(p, s);
    return p;
}
/*dic opreation interface */
void dic_put(char *key, char *value)
{
    install(key, value);
}
char *dic_value_for_key(char *key)
{
    char *ret = "";
    struct nlist *np = lookup(key);
    if (np != NULL)
    {
        ret = np->defn;
    }
    return ret;
}


#include <stdio.h>
#include <stdlib.h>
 
#define  MAX    26
 
typedef struct TrieNode
{
	int nCount;  // 该节点前缀 出现的次数
	struct TrieNode *next[MAX]; //该节点的后续节点
} TrieNode;
 
TrieNode Memory[1000000]; 
int allocp = 0;
 
//初始化一个节点。
TrieNode * createTrieNode()
{
	TrieNode * tmp = &Memory[allocp++];
	tmp->nCount = 1;
	for (int i = 0; i < MAX; i++)
		tmp->next[i] = NULL;
	return tmp;
}
 
void insertTrie(TrieNode * * pRoot, char * str)
{
	TrieNode * tmp = *pRoot;
	int i = 0, k;
	while (str[i])
	{
		k = str[i] - 'a'; 
		if (tmp->next[k])
		{
			tmp->next[k]->nCount++;
		}
		else
		{
			tmp->next[k] = createTrieNode();
		}
 
		tmp = tmp->next[k];
		i++; 
	}
 
}
 
int searchTrie(TrieNode * root, char * str)
{
	if (root == NULL)
		return 0;
	TrieNode * tmp = root;
	int i = 0, k;
	while (str[i])
	{
		k = str[i] - 'a';
		if (tmp->next[k])
		{
			tmp = tmp->next[k];
		}
		else
			return 0;
		i++;
	}
	return tmp->nCount; 
}


