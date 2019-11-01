#ifndef __BTREE_H__
#define __BTREE_H__
int insert_index(char *key, int key_len, void *ptr);
int search_index(char *key, int key_len, void **ptr);
int delete_index(char *key, int key_len);
#endif
