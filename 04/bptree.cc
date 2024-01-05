#include "bptree.h"
#include <vector>
#include <sys/time.h>

struct timeval
cur_time(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return t;
}

void
print_tree_core(NODE *n)
{
	printf("["); 
	for (int i = 0; i < n->nkey; i++) {
		if (!n->isLeaf) print_tree_core(n->chi[i]); 
		printf("%d", n->key[i]); 
		if (i != n->nkey-1 && n->isLeaf) putchar(' ');
	}
	if (!n->isLeaf) print_tree_core(n->chi[n->nkey]);
	printf("]");
}

void
print_tree(NODE *node)
{
	print_tree_core(node);
	printf("\n"); fflush(stdout);
}

NODE *
find_leaf(NODE *node, int key)
{
	int kid;

	if (node->isLeaf) return node;
	for (kid = 0; kid < node->nkey; kid++) {
		if (key < node->key[kid]) break;
	}

	return find_leaf(node->chi[kid], key);
}

NODE *
insert_in_leaf(NODE *leaf, int key, DATA *data)
{
	int i;
	if (key < leaf->key[0]) {
		for (i = leaf->nkey; i > 0; i--) {
			leaf->chi[i] = leaf->chi[i-1] ;
			leaf->key[i] = leaf->key[i-1] ;
		} 
		leaf->key[0] = key;
		leaf->chi[0] = (NODE *)data;
	}
	else {
		for (i = 0; i < leaf->nkey; i++) {
			if (key < leaf->key[i]) break;
		}
		for (int j = leaf->nkey; j > i; j--) {		
			leaf->chi[j] = leaf->chi[j-1] ;
			leaf->key[j] = leaf->key[j-1] ;
		}

		/* CodeQuiz */
		leaf->key[i] = key;
		leaf->chi[i] = (NODE *)data;
	}
	leaf->nkey++;

	return leaf;
}

NODE *
alloc_leaf(NODE *parent)
{
	NODE *node;
	if (!(node = (NODE *)calloc(1, sizeof(NODE)))) ERR;
	node->isLeaf = true;
	node->parent = parent;
	node->nkey = 0;

	return node;
}

NODE *
alloc_node(NODE *parent)
{
	NODE *node;
	if (!(node = (NODE *)calloc(1, sizeof(NODE)))) ERR;
	node->isLeaf = false;
	node->parent = parent;
	node->nkey = 0;

	return node;
}

// NODEを新しいTEMPにコピーする
TEMP *
create_temp_from_node(NODE *node)
{
	TEMP *temp;
	temp = (TEMP *)malloc(sizeof(TEMP));
	for (int i = 0; i < node->nkey; i++) {
		temp->chi[i] = node->chi[i];
		temp->key[i] = node->key[i];
	}
	temp->chi[node->nkey] = node->chi[node->nkey];
	temp->nkey = node->nkey;
	return temp;
}

// LeafとしてのTEMP(元々chiに空きがある)にchild(key,data)を挿入する
TEMP *
insert_child_in_leaf_temp(TEMP *leaf, int key, DATA *data)
{
	int i;
	if (key < leaf->key[0]) {
		for (i = leaf->nkey; i > 0; i--) {
			leaf->chi[i] = leaf->chi[i-1] ;
			leaf->key[i] = leaf->key[i-1] ;
		} 
		leaf->key[0] = key;
		leaf->chi[0] = (NODE *)data;
	}
	else {
		for (i = 0; i < leaf->nkey; i++) {
			if (key < leaf->key[i]) break;
		}
		for (int j = leaf->nkey; j > i; j--) {
			leaf->chi[j] = leaf->chi[j-1] ;
			leaf->key[j] = leaf->key[j-1] ;
		}

		leaf->key[i] = key;
		leaf->chi[i] = (NODE *)data;
	}
	leaf->nkey++;

	return leaf;
}

// NODEとしてのTEMP(元々chiに空きがない)にchild(key,node)を挿入する
TEMP *
insert_child_in_temp(TEMP *parent, int key, NODE *node)
{
	int i;
	if (key < parent->key[0]) {
		for (i = parent->nkey; i > 0; i--) {
			parent->chi[i+1] = parent->chi[i] ;
			parent->key[i] = parent->key[i-1] ;
		} 
		parent->key[0] = key;
		parent->chi[1] = node;
	}
	else {
		for (i = 0; i < parent->nkey; i++) {
			if (key < parent->key[i]) break;
		}
		for (int j = parent->nkey; j > i; j--) {		
			parent->chi[j+1] = parent->chi[j] ;
			parent->key[j] = parent->key[j-1] ;
		}

		parent->key[i] = key;
		parent->chi[i+1] = node;
	}

	parent->nkey++;

	return parent;
}

// NODEのkeyとchildを空にする
void
clean_up_node(NODE *node)
{
	for (int i = 0; i < N-1; i++) {
		node->key[i] = 0;
	}
	for (int i = 0; i < N; i++) {
		node->chi[i] = NULL;
	}
	node->nkey = 0;
}

// TEMPを2つのLeafに分割する
void
copy_to_two_leaves(TEMP *temp, NODE *left, NODE *right)
{
	int middle = (temp->nkey) / 2;
	for (int i = 0; i < temp->nkey; i++) {
		if (i < middle) {
			left->chi[i] = temp->chi[i];
			left->key[i] = temp->key[i];
		}
		else {
			right->chi[i-middle] = temp->chi[i];
			right->key[i-middle] = temp->key[i];
		}
	}
	left->nkey = middle;
	right->nkey = temp->nkey - middle;
}

// TEMPを2つのNODEに分割する
void
copy_to_two_nodes(TEMP *temp, NODE *left, NODE *right)
{
	int middle = (N+1)/2;
	// rightは1番左が親に行く
	for (int i = 0; i < temp->nkey-1; i++) {
		if (i < middle) {
			left->key[i] = temp->key[i];
		}
		else {
			right->key[i-middle] = temp->key[i+1];
		}
	}
	for (int i = 0; i < temp->nkey+1; i++) {
		if (i < middle+1) {
			left->chi[i] = temp->chi[i];
		}
		else {
			right->chi[i-middle-1] = temp->chi[i];
		}
	}
	left->nkey = middle;
	right->nkey = temp->nkey - middle - 1;
}

// parentの空いている所にchild(key,node)を挿入する
NODE *
insert_child_in_parent(NODE *parent, int key, NODE *node)
{
	int i;
	if (key < parent->key[0]) {
		for (i = parent->nkey; i > 0; i--) {
			parent->chi[i+1] = parent->chi[i] ;
			parent->key[i] = parent->key[i-1] ;
		} 
		parent->key[0] = key;
		parent->chi[1] = node;
	}
	else {
		for (i = 0; i < parent->nkey; i++) {
			if (key < parent->key[i]) break;
		}
		for (int j = parent->nkey; j > i; j--) {		
			parent->chi[j+1] = parent->chi[j] ;
			parent->key[j] = parent->key[j-1] ;
		}

		parent->key[i] = key;
		parent->chi[i+1] = node;
	}

	parent->nkey++;

	return parent;
}

void
insert_in_parent(NODE *left, int key, NODE *right)
{
	if (left == Root) {
		NODE *parent = alloc_node(NULL);
		parent->key[0] = key;
		parent->nkey = 1;
		parent->chi[0] = left;
		parent->chi[1] = right;
		parent->isLeaf = false;

		Root = parent;
		left->parent = Root;
		right->parent = Root;
		return;
	}

	NODE *parent = left->parent;

	if (parent->nkey < N-1) {
		insert_child_in_parent(parent, key, right);
	}
	else {
		// TODO: Parentが埋まっている時の処理
		TEMP *temp = create_temp_from_node(parent);

		insert_child_in_temp(temp, key, right);

		clean_up_node(parent);

		NODE *parent_right = alloc_node(parent->parent);

		copy_to_two_nodes(temp, parent, parent_right);

        return insert_in_parent(parent, temp->key[(N+1)/2], parent_right);
	}
}

void 
insert(int key, DATA *data)
{
	NODE *leaf;

	if (Root == NULL) {
		leaf = alloc_leaf(NULL);
		Root = leaf;
	}
	else {
		leaf = find_leaf(Root, key);
	}
	if (leaf->nkey < (N-1)) {
		insert_in_leaf(leaf, key, data);
	}
	else { // split
		// future work
		TEMP *temp = create_temp_from_node(leaf);
		insert_child_in_leaf_temp(temp, key, data);
		NODE *next = alloc_leaf(leaf->parent);
		clean_up_node(leaf);
		copy_to_two_leaves(temp, leaf, next);
		insert_in_parent(leaf, next->key[0], next);
	}
}

void
init_root(void)
{
	Root = NULL;
}

int 
interactive()
{
  int key;

  std::cout << "Key: ";
  std::cin >> key;

  return key;
}

int
main(int argc, char *argv[])
{
  struct timeval begin, end;

	init_root();

	printf("-----Insert-----\n");
	begin = cur_time();
  while (true) {
		insert(interactive(), NULL);
    print_tree(Root);
  }
	end = cur_time();

	return 0;
}
