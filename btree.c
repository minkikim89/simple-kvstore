#include <stdlib.h>
#include <string.h>
#include "btree.h"
#include "types.h"

#define MAX_KEY_LEN 32
#define MAX_ITEM_NUM 3

/* optype */
#define INSERT_OP 0
#define SEARCH_OP 1
#define DELETE_OP 2

/* internal structures */
typedef struct _btree_item {
  void     *ptr; /* nval + value */
  uint32_t  nkey;
  char      key[MAX_KEY_LEN];
  //char      key[0];
} btree_item;

typedef struct _btree_node {
  uint8_t  is_leaf;
  uint8_t  reserved[4];
  uint16_t num_items;
  btree_item items[MAX_ITEM_NUM];
} btree_node;

typedef struct _btree {
  btree_node *root;
} btree;

btree bt;

/* common functions */
static int
key_compare(char *key1, int key1_len, char *key2, int key2_len)
{
  int cmp;
  if (key1_len < key2_len) {
    cmp = memcmp(key1, key2, key1_len);
    if (cmp == 0) cmp = -1;
  } else if (key1_len > key2_len) {
    cmp = memcmp(key1, key2, key2_len);
    if (cmp == 0) cmp = 1;
  } else {
    assert(key1_len == key2_len);
    cmp = memcmp(key1, key2, key1_len);
  }
  return cmp;
}

/* internal btree node functions */
static btree_node*
alloc_internal_node()
{
  btree_node *node = malloc(sizeof(btree_node));
  memset(node, 0, sizeof(btree_node));
  return node;
}

static void
free_internal_node(btree_node *node)
{
  free(node);
}

static int lookup_internal_node(btree_node *node, char *key, int key_len)
{
  assert(!node->is_leaf);

  int cmp;
  int s, m, e;

  s = 0;
  m = 0;
  e = node->num_items - 1;
  while (s <= e) {
    m = (s + e) / 2;
    cmp = key_compare(key, key_len, node->items[m].key, node->items[m].nkey);

    if (cmp == 0) return m;
    if (cmp > 0)  s = m + 1;
    else          e = m - 1;
  }

  return e;
}

static int insert_internal_node(btree_node *node, int index, char *key, int key_len, void *ptr)
{
  int i;
  for (i = node->num_items; i > index; i--) {
    node->items[i] = node->items[i-1];
  }
  btree_item *bitem = &node->items[index];
  bitem->ptr = ptr;
  bitem->nkey = key_len;
  memcpy(&bitem->key, key, key_len);
  node->num_items++;
  return 0;
}

static int delete_internal_node(btree_node *node, int index)
{
  int i;
  for (i = index; i < node->num_items-1; i++) {
    node->items[i] = node->items[i+1];
  }
  node->num_items--;
  return 0;
}

static int split_internal_node(btree_node *parent_node, char *key, int key_len, int index, btree_node **curr_node)
{
  btree_node *node = *curr_node;
  btree_node *new_node = alloc_internal_node();

  int i;
  int half_size = node->num_items/2;
  int move_count = node->num_items - half_size;

  for (i = 0; i < move_count; i++) {
    new_node->items[i] = node->items[half_size+i];
  }
  node->num_items -= move_count;
  new_node->num_items += move_count;

  char *next_key = new_node->items[0].key;
  int next_key_len = new_node->items[0].nkey;

  insert_internal_node(parent_node, index, key, key_len, new_node);
  int cmp = key_compare(key, key_len, next_key, next_key_len);
  if (cmp >= 0) *curr_node = new_node;
  return 0;
}

static int merge_internal_node(btree_node *parent_node, int index, btree_node *curr_node)
{
  int i;
  int sibling_index;
  btree_node *sibling_node;

  if (index < parent_node->num_items - 1) {
    /* merge with right node */
    sibling_index = index + 1;
    sibling_node = parent_node->items[sibling_index].ptr;
    if (curr_node->num_items + sibling_node->num_items < MAX_ITEM_NUM) {
      for (i = 0; i < sibling_node->num_items; i++) {
        curr_node->items[curr_node->num_items+i] = sibling_node->items[i];
      }
      curr_node->num_items += sibling_node->num_items;
      sibling_node->num_items = 0;
      delete_internal_node(parent_node, sibling_index);
      free_internal_node(sibling_node);
    } else {
      //TODO
    }
  } else {
    /* merge with left node */
    sibling_index = index - 1;
    sibling_node = parent_node->items[sibling_index].ptr;
    if (curr_node->num_items + sibling_node->num_items < MAX_ITEM_NUM) {
      for (i = 0; i < curr_node->num_items; i++) {
        sibling_node->items[sibling_node->num_items+i] = curr_node->items[i];
      }
      sibling_node->num_items += curr_node->num_items;
      curr_node->num_items = 0;
      delete_internal_node(parent_node, index);
      free_internal_node(curr_node);
    } else {
      //TODO
    }
  }
  return 0;
}

static btree_node*
alloc_leaf_node()
{
  btree_node *node = malloc(sizeof(btree_node));
  memset(node, 0, sizeof(btree_node));
  node->is_leaf = true;
  return node;
}

static void
free_leaf_node(btree_node *node)
{
  free(node);
}


static int lookup_leaf_node(btree_node *node, char *key, int key_len)
{
  assert(node->is_leaf);

  int cmp;
  int s, m, e;

  s = 0;
  m = 0;
  e = node->num_items - 1;
  while (s <= e) {
    m = (s + e) / 2;
    cmp = key_compare(key, key_len, node->items[m].key, node->items[m].nkey);

    if (cmp == 0) return m;
    if (cmp > 0)  s = m + 1;
    else          e = m - 1;
  }

  return -1;
}

static int insert_leaf_node(btree_node *node, char *key, int key_len, void *ptr)
{
  int cmp;
  int s, m, e;

  s = 0;
  m = 0;
  e = node->num_items-1;
  while (s <= e) {
    m = (s + e) / 2;
    cmp = key_compare(key, key_len, node->items[m].key, node->items[m].nkey);

    if (cmp == 0) break;
    if (cmp > 0)  s = m + 1;
    else          e = m - 1;
  }

  if (s <= e) {
    // already exists
    btree_item *bitem = &node->items[m];
    bitem->ptr = ptr;
    bitem->nkey = key_len;
    memcpy(&bitem->key, key, key_len);
    node->num_items++;
  } else {
    int i;
    for (i = node->num_items; i > s; i--) {
      node->items[i] = node->items[i-1];
    }
    btree_item *bitem = &node->items[s];
    bitem->ptr = ptr;
    bitem->nkey = key_len;
    memcpy(&bitem->key, key, key_len);
    node->num_items++;
  }

  return 0;
}

static int delete_leaf_node(btree_node *node, int index)
{
  int i;
  for (i = index; i < node->num_items-1; i++) {
    node->items[i] = node->items[i+1];
  }
  node->num_items--;
  return 0;
}

static int split_leaf_node(btree_node *parent_node, char *key, int key_len, int index, btree_node **curr_node)
{
  bool new_root = false;
  if (parent_node == NULL) {
    new_root = true;
    bt.root = alloc_internal_node();
    parent_node = bt.root;
  }
  btree_node *node = *curr_node;
  btree_node *new_node = alloc_leaf_node();

  int i;
  int half_size = node->num_items/2;
  int move_count = node->num_items - half_size;

  for (i = 0; i < move_count; i++) {
    new_node->items[i] = node->items[half_size+i];
  }
  node->num_items -= move_count;
  new_node->num_items += move_count;

  char *next_key = new_node->items[0].key;
  int next_key_len = new_node->items[0].nkey;

  if (new_root) {
    insert_internal_node(parent_node, index, NULL, 0, node);
  }
  insert_internal_node(parent_node, index+1, next_key, next_key_len, new_node);
  int cmp = key_compare(key, key_len, next_key, next_key_len);
  if (cmp >= 0) *curr_node = new_node;
  return 0;
}

static int merge_leaf_node(btree_node *parent_node, int index, btree_node *curr_node)
{
  int i;
  int sibling_index;
  btree_node *sibling_node;

  if (index < parent_node->num_items - 1) {
    /* merge with right node */
    sibling_index = index + 1;
    sibling_node = parent_node->items[sibling_index].ptr;
    if (curr_node->num_items + sibling_node->num_items < MAX_ITEM_NUM) {
      for (i = 0; i < sibling_node->num_items; i++) {
        curr_node->items[curr_node->num_items+i] = sibling_node->items[i];
      }
      curr_node->num_items += sibling_node->num_items;
      sibling_node->num_items = 0;
      delete_leaf_node(parent_node, sibling_index);
      free_leaf_node(sibling_node);
    } else {
      //TODO
    }
  } else {
    /* merge with left node */
    sibling_index = index - 1;
    sibling_node = parent_node->items[sibling_index].ptr;
    if (curr_node->num_items + sibling_node->num_items < MAX_ITEM_NUM) {
      for (i = 0; i < curr_node->num_items; i++) {
        sibling_node->items[sibling_node->num_items+i] = curr_node->items[i];
      }
      sibling_node->num_items += curr_node->num_items;
      curr_node->num_items = 0;
      delete_leaf_node(parent_node, index);
      free_leaf_node(curr_node);
    } else {
      //TODO
    }
  }
  return 0;
}

/* index functions */
static btree_node *find_leaf_node(char *key, int key_len, int optype)
{
  if (bt.root == NULL) {
    return NULL;
  }

  btree_node *curr_node, *next_node;
  curr_node = bt.root;
  next_node = NULL;
  if (curr_node->is_leaf) {
    if (curr_node->num_items == MAX_ITEM_NUM) {
        split_leaf_node(NULL, key, key_len, 0, &curr_node);
    }
    return curr_node;
  }

  int index;
  while (!curr_node->is_leaf) {
    index = lookup_internal_node(curr_node, key, key_len);
    next_node = curr_node->items[index].ptr;
    if (optype == INSERT_OP && next_node->num_items == MAX_ITEM_NUM) {
      if (next_node->is_leaf) {
        split_leaf_node(curr_node, key, key_len, index, &next_node);
      } else {
        split_internal_node(curr_node, key, key_len, index, &next_node);
      }
    } else if (optype == DELETE_OP && curr_node->num_items == 1) {
      if (next_node->is_leaf) {
        merge_leaf_node(curr_node, index, next_node);
      } else {
        merge_internal_node(curr_node, index, next_node);
      }
    }
    curr_node = next_node;
  }

  return curr_node;
}

int search_index(char *key, int key_len, void **ptr)
{
  btree_node *node = bt.root;

  node = find_leaf_node(key, key_len, SEARCH_OP);
  if (node == NULL) {
    return -1;
  }

  int index = lookup_leaf_node(node, key, key_len);

  if (index == -1) {
    return -1;
  }
  *ptr = node->items[index].ptr;
  return 0;
}

int insert_index(char *key, int key_len, void *ptr)
{
  btree_node *node = find_leaf_node(key, key_len, INSERT_OP);
  if (node == NULL) {
    /* root is NULL */
    node = alloc_leaf_node();
    bt.root = node;
  }

  insert_leaf_node(node, key, key_len, ptr);
  return 0;
}

int delete_index(char *key, int key_len)
{
  btree_node *node = find_leaf_node(key, key_len, DELETE_OP);
  int index = lookup_leaf_node(node, key, key_len);
  delete_leaf_node(node, index);
  return 0;
}
