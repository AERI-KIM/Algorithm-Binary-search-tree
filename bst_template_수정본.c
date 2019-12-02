/* Binary search tree using doubly-linked lists
 * COMP319 Algorithms, Fall 2019
 * School of Electronics Engineering, Kyungpook National University
 * Instructor: Gil-Jin Jang
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define MEASURE_TIME	// to measure time
#define max(a,b)    (((a)>(b))? (a):(b))


/////////////////////////////////////////////////////////////
// DATA STRUCTURE:
// binary tree node definition using doubly linked lists
// key is a string of a fixed length
// KEYLENGTH	length of the key
// BULK_SIZE	key is hidden in the "BULK"
//	to retrieve key, we have to dig into the "BULK"
//	so accessing key takes average "BULK_SIZE"/2 searches
//	this is INTENTIONALLY to add extra overhead for search
//	your program should reduce the number of key accesses at your best
/////////////////////////////////////////////////////////////
#define KEYLENGTH	3
#define BULK_SIZE	4096
//#define BULK_SIZE	65536
struct BTNode {
  char bulk[BULK_SIZE];	// null character to be added
  struct BTNode *left, *right;	// binary tree: left and right children
};

int get_height(struct BTNode *bst);
/////////////////////////////////////////////////////////////
// GIVEN: functions for binary tree node
// name and parameters only
// implementations are moved after "main" function
/////////////////////////////////////////////////////////////

const char* getkey(struct BTNode *a);
// return value: char array of KEYLENGTH+1 (+1 for '\0' character)
//  key is hidden in "bulk", so use the following function to
//  read key string of length KEYLENGTH
//  it will make BULK_SIZE/2 searches on average
//  so try to use it as rarely as possible

int setkey(struct BTNode *a, const char kw[]);
// return value: 0 for failure (NULL a), 1 for success
//  the following function hides a string "kw" of KEYLENGTH
//  by randomly selecting the location to save key

int copykey(struct BTNode *dst, struct BTNode *src)
{ return setkey(dst,getkey(src)); }
// copies the key of one node to the other
// very simple, single line, so implementation is given here

int comparekey(struct BTNode *a, struct BTNode *b);
// return value: (by character comparison)
//  -1 if a's key < b's key
//  0 if a's key == b's key
//  +1 if a's key > b's key
//  may be needed for binary search tree search and build-up

struct BTNode *generate_btnode(const char kw[]);
// return value: pointer to a single BTNode (left/right are NULL)
//  generates a node for binary tree

void free_bt_recursive (struct BTNode *bt);
// frees a binary tree

struct BTNode *copy_bt_recursive (struct BTNode *bt);
// return value: pointer to the root of the copy of the given binary tree "bt"

struct BTNode *insert_left_bcnode(
    struct BTNode *parent, struct BTNode *newPtr );
//  adds a node to the left of a BTNode parent
//  it will be used to generate a left-half binary tree
//  (LHBT, all rights are NULL)
// pre-condition: left pointer to the new node should be NULL
// to store the left pointer to the parent node
// return value: parent if the given parent is not NULL; newPtr if parent NULL

struct BTNode *readkeys_textfile_LHBT( const char infile[], int *pN );
// File I/O: read key words from the given file
// and generate a binary tree which is left-half
// (all right children are NULL)

/////////////////////////////////////////////////////////////
// FILL 1: generate a binary search tree using insertion
/////////////////////////////////////////////////////////////
struct BTNode *insert_to_BST_leaf(struct BTNode *bst, struct BTNode *newPtr)
{
  if ( bst == NULL ) return newPtr;	// new bst as the input node
  else if ( newPtr == NULL ) return bst;	// nothing to add
  else {
    if ( comparekey(bst, newPtr) < 0 ) {
      bst->right = insert_to_BST_leaf(bst->right, newPtr);
    }
    else {
      bst->left = insert_to_BST_leaf(bst->left, newPtr);
    }
  }
  return bst;
}

struct BTNode *generate_BST_by_insertion(struct BTNode *lhbt)
{

    struct BTNode* bst = NULL;
    struct BTNode* tmp = NULL;

    while(lhbt != NULL){
        tmp = lhbt;
        lhbt = lhbt->left;
        tmp->left = NULL;
        bst = insert_to_BST_leaf(bst, tmp);
    }

    return bst;


    /*
    1. lhbt의 처음 노드를 root로 삼는다.
    2. lhbt->left를 이용해 다음 노드로 이동한다.
    3. insert_to_BST_leaf를 호출해서 노드를 삽입한다.
    4. lhbt가 NULL이 될 때까지 반복한다.
    */
}

/////////////////////////////////////////////////////////////
// FILL 2: PRINT
/////////////////////////////////////////////////////////////
int print_LHBT(FILE *fp, struct BTNode *lhbt)
  // prints left-half binary tree
  // ___-___-___
  // INPUT
  //   fp: file pointer for the output file, stdout for monitor output
  //   lhbt: left-half binary tree (right pointers are all null)
  // RETURNs number of NODES in the list
{
  int num_nodes;

  num_nodes=0;
  while ( lhbt ) {
    if ( lhbt->right != NULL ) {	// check if left-half
      fprintf(stderr,"Non-left-half binary tree for %s\n",__FUNCTION__);
      break;
    }
    else {
      fprintf(fp, "%s", getkey(lhbt));
      if ( lhbt->left != NULL ) fprintf(fp, "-");
    }
    num_nodes++;
    lhbt = lhbt->left;
  }

  fprintf(fp, "\n");	// change the line at the end

  return num_nodes;
}

int count_node=0; //node 개수를 세기위해 전역변수로 선언
int print_BST_sortedorder(FILE *fp, struct BTNode *bst, int level)
  // prints a binary search tree nodes by a single line
  // in a SORTED ORDER
  // (hint: inorder traversal)
  // INPUT
  //   fp: file pointer for the output file, stdout for monitor output
  //   bst: root node of the BST, should satisfy the property of
  //      binary search tree, left <= center < right
  //   level: level of the root node, starting from 0 (empty)
  //      if it is unnecessary, do not have to use it
  // RETURNs number of NODES in the list
{
  /* GJ: you may fill out the *FILL* lines, or completely rewrite */
  //int count=0;	// to count the number of nodes


  if ( bst != NULL ) {
    level++;	// root node of the current subtree exists

    print_BST_sortedorder(fp, bst->left,level);

    // center node
    fprintf(fp, "%s ",getkey(bst));
    count_node++;

    print_BST_sortedorder(fp, bst->right,level);

  }

  // change the line once - only at the bst node
  if ( level <= 1 ) fprintf(fp, "\n");

  return count_node;
}

int print_BST_right_center_left(FILE *fp, struct BTNode *bst, int level)
  // prints a binary search tree, rotated by 270 degrees
  // Note: key's length is fixed to KEYLENGTH, so there are
  // (KEYLENGTH+1)*level spaces. For examples,
  //         999
  //     777
  //         555
  // 333
  //     222
  //         111
  // INPUT
  //   (same as print_BST_sortedorder)
  // RETURNs HEIGHT-1 of the printed tree (2 in the above example)
  //   (hint: printing order is right -> center -> left
  //    carefully count the number of spaces)
{
  int hr, hl;
  if ( bst != NULL ) {

    level++;
    hr = print_BST_right_center_left(fp, bst->right,level);


    // center node
    level--;
    for(int i=0; i<level; i++) fprintf(fp, "    ");
    fprintf(fp, "%s \n",getkey(bst));


    level++;
    hl = print_BST_right_center_left(fp, bst->left,level);

    return (hl > hr) ? hl+1:hr+1;
  }
  else return 0;

  //return get_height(bst);
}

int get_height(struct BTNode *bst){
    int h=0;
    if(bst != NULL)
        h = 1 + max(get_height(bst->left), get_height(bst->right));

    return h;
}

    /*
        1. 오른쪽 -> 센터 -> 왼쪽 출력
        2. indention만큼 들여써서 값을 출력
        3. 한 번 내려갈때마다 level++ =>right, left 호출
        4. 한 번 출력할 때마다 다음줄로 넘어가기
        5. 한 번 올라갈때마다 level-- 현재 노드를 출력하고 나면 다시 올라가기
        6. 높이는 함수를 만들어서 구한다.
    */

int left=0;
int print_BST_1(FILE *fp, struct BTNode *bst, int level)
/*
  // prints a binary search tree, rotated by 270 degrees, with less lines
  //  1) center and right are in the same line
  //  2) left subtree is below the center
  //  3) right is connected by '/' and left by '+'
  // Note: key's length is fixed to KEYLENGTH,
  // so left and right begins at (KEYLENGTH+1)*level+1
  // 333/777/999
  //        +555
  //    +222
  //        +111
  // (or)
  // 100/400/800/900
  //                +900
  //            +800
  //                +500/700
  //    +000
  // INPUT and OUPUT
  //   (same as print_BST_right_center_left)
  //   (hint: printing order is center -> right -> left)
  */
{
    int hl, hr;
    if ( bst != NULL ){

        level++;
        fprintf(fp, "%s",getkey(bst));


        if(bst->right!=NULL){ //right가 있다면, /출력
            fprintf(fp, "/");
            hr = print_BST_1(fp, bst->right, level);
        }
        else hr = 0;

        if((bst->right==NULL && bst->left==NULL)|| bst->right==NULL) //다음 문장
            fprintf(fp, "\n");

        if(bst->left!=NULL){ //left가 있다면 빈칸과 + 출력
            for(int i=0; i<4*level-1; i++) fprintf(fp, " ");
            fprintf(fp, "+");
            hl = print_BST_1(fp, bst->left, level);
            level--;
        }
        else hl = 0;

        return (hl > hr) ? hl+1:hr+1;
    }
    else return 0;

}

int print_BST_2(FILE *fp, struct BTNode *bst, int level, int has_bar[])
/*
  // same as print_BST_1 except vertical line from center to left
  // 100/400/800/900
  //    |       |   +900
  //    |       +800
  //    |           +500/700
  //    +000
  // Hint: stack or some extra variables may help.
  //       static variable can be used as well
  //       You may add additional parameter to the function if necessary
  */
{
    int hl, hr;
    if ( bst != NULL ){

        level++;
        fprintf(fp, "%s",getkey(bst));


        if(bst->right!=NULL){ //right가 있다면, /출력
            if(bst->left!=NULL) //right와 left를 둘 다 가지고 있는 경우
                has_bar[level]=1;
            else has_bar[level]=0;
            fprintf(fp, "/");
            hr = print_BST_2(fp, bst->right, level, has_bar);
        }
        else hr = 0;

        if((bst->right==NULL && bst->left==NULL)|| bst->right==NULL) //띄어쓰기
            fprintf(fp, "\n");

        if(bst->left!=NULL){ //left가 있다면 빈칸과 + 출력
            for(int i=1; i<level; i++){
                if(has_bar[i]==1)
                    fprintf(fp, "   |");
                else if(has_bar[i]==0)
                    fprintf(fp, "    ");
            }
            has_bar[level] = 0;
            fprintf(fp, "   +");
            hl = print_BST_2(fp, bst->left, level, has_bar);
            level--;
        }
        else hl = 0;

        return (hl > hr) ? hl+1:hr+1;
    }
    else return 0;

}

    /*
        1. right 호출 전에 right와 left 둘 다 가지고 있는지 판단
            1-1. 둘 다 가지고 있으면, has_bar[]=1
            1-2. 아닌 경우에는 has_bar[]=0
        2. right 호출
        3. \n 결정
        4. left 출력;
            4-1. level-1만큼 | or " " 결정해서 출력
            4-2. "   "출력
            4-3. left 함수 호출
    */


/////////////////////////////////////////////////////////////
// FILL 3: Conversion of an BST to a complete BST
/////////////////////////////////////////////////////////////
int inorder(struct BTNode* tmp[], int pos, struct BTNode* bst, int numNodes){
    if(bst){
        pos = inorder(tmp, pos, bst->left, numNodes);
        tmp[pos++] = bst;
        pos = inorder(tmp, pos, bst->right, numNodes);
        return pos;
    }
    else return pos;
}


struct BTNode **BST_to_sortedarray(struct BTNode *bst, int numNodes)
{
    struct BTNode **tmp;

    tmp = (struct BTNode**) malloc(sizeof(struct BTNode*)*numNodes);
    inorder(tmp, 0, bst, numNodes);
    return tmp;
}

int fill_sortedarray_to_completeArrayBST(struct BTNode **sortedArray, int k,
                                          struct BTNode **completeArrayBST, int i, int numNodes)
{
    if ( i <= numNodes ) {
        k = fill_sortedarray_to_completeArrayBST(sortedArray, k, completeArrayBST, i*2, numNodes);
        completeArrayBST[i] = sortedArray[k++];
        k = fill_sortedarray_to_completeArrayBST(sortedArray, k, completeArrayBST, i*2+1, numNodes);
        return k;
    }
    return k;
}


struct BTNode *BST_to_completeBST(struct BTNode *bst, int numNodes)
  // convert a BST to complete BST (minimum height, filling in left first)
  // INPUT
  //   bst: root node of the BST, should satisfy the property of
  //      binary search tree, left <= center < right
  //   numNodes: number of nodes in the bst
  //      if not necessary in your implementation, do not have to use it
  // RETURNs a COMPLETE BST
  // (hint: using extra memory (arrays or lists) may help,
  //  array's rule for parent-child, sorted list, etc.)
{
    struct BTNode **sortedArray, **completeArrayBST;
    int i;

    sortedArray = BST_to_sortedarray(bst, numNodes);
    completeArrayBST = (struct BTNode**) malloc(sizeof(struct BTNode*)*(numNodes+1));
    fill_sortedarray_to_completeArrayBST(sortedArray, 0, completeArrayBST, 1, numNodes);
    for (i=1; i<=numNodes ; i++) completeArrayBST[i]->left = completeArrayBST[i]->right = NULL;
    for (i=1; i<=numNodes ; i++) {
        if ( i*2 <= numNodes ) completeArrayBST[i]->left = completeArrayBST[i*2];
        if ( i*2+1 <= numNodes ) completeArrayBST[i]->right = completeArrayBST[i*2+1];
    }
    return completeArrayBST[1];
    /*
        1. 빈 포인터 배열을 만든다.
        2. 빈 포인터 배열에 트리를 넣는다.
            2-1. inoder traverse이용
        3. 포인터 배열을 completeBST 구조에 맞게 정렬한다.
            3-1. 배열 인덱스 사용하기
        4. 정렬된 포인터 배열을 bst에 집어넣는다.
            4-1. 배열 인덱스와 bst의 관계식을 이용해서 직접 넣어주기 // 왼쪽, 가운데, 오른쪽
    */

}





/////////////////////////////////////////////////////////////
// FILL 4: generate binary search tree from a left-half binary tree
// using quick sort
/////////////////////////////////////////////////////////////

struct BTNode* extract_root(struct BTNode* lhbt){
    if(lhbt->right != NULL) return NULL;
    struct BTNode* root, next;
    root = lhbt;
    lhbt = lhbt->left;
    root->left = NULL;
    return root;
}

struct BTNode* add_to_tail_BST(struct BTNode* h, struct BTNode* t, struct BTNode* x){
    if(h==NULL && t==NULL){
        h = t = x;
    }
    else{
        t->left = x;
        t->right=NULL;
        t = x; //tail을 x로 업데이트
    }
    return h;
}
struct BTNode* quicksort_partition(struct BTNode* lhbt)
{
    struct BTNode *q, *x;
    q = extract_root(lhbt); // 피봇으로 삼을 노드를 빼낸다.
    struct BTNode *lh, *rh, *lt, *rt; // subtree들의 head, tail 선언.
    lh=rh=lt=rt=NULL;

    while(lhbt!=NULL){ // 기존 lhbt에서 떼낼 노드가 없을 때까지
        x = extract_root(lhbt); // q에 붙일 노드를 떼낸다.

//        fprintf(fp, "hello");
        if(getkey(x)<=getkey(q)){ // pivot과 붙일 노드의 값 비교
            q->left = add_to_tail_BST(lh, lt, x); // pivot의 왼쪽에 붙이기
        }
        else{
            q->right = add_to_tail_BST(rh, rt, x); //pivot의 오른쪽에 붙이기
        }
        //printf("hello");
    }

    return q;
}

struct BTNode* generate_BST_quicksort_basic(struct BTNode *lhbt)
  // gerate a BST using quick sort algorithm
  // the resultant tree should be identical to generate_BST_by_insertion
{
    if(lhbt!=NULL && lhbt->left!=NULL)
    {
        struct BTNode* q;
        q = quicksort_partition(lhbt);
        q->left = generate_BST_quicksort_basic(q->left);
        q->right = generate_BST_quicksort_basic(q->right);

        return q;
    }
    else return lhbt;
}

  /*
    1. 제일 처음 노드를 pivot으로 두기
    2. pivot을 기준으로 pivot보다 작은 것들은 left, pivot보다 큰 것들은 right으로 옮긴다.
    // 필요없을지도?3. left, right정렬
        3-1. 각각 BST로 만들어 준다.
    4. 이 과정 반복
        4-1. 언제까지 반복? bst->left==NULL일 때까지

  */


/*
struct BTNode *generate_BST_quicksort_advanced(struct BTNode *lhbt)
  // challenge: try to reduce the height using quick sort algorithm
{
  FILL
}
*/

/////////////////////////////////////////////////////////////
// main function
/////////////////////////////////////////////////////////////
#define MAXLINE	1024
int main()
{
  int numWords;	// number of words
  //int wordLen;	// word length: number of characters per word
  struct BTNode *root, *bst1, *bst2;
  int numNodes, lev;	// level of the tree

  /* for file name, max length 1023 including path */
  char line[MAXLINE];
  char infile[MAXLINE], outfile[MAXLINE];
  FILE *fp;

#ifdef MEASURE_TIME
  clock_t start, end;
  double cpu_time_used;
#endif

  /* input file name given by keyboard */
  memset(line,0,sizeof(char)*MAXLINE);	// clear the buffer
  fprintf(stderr,"Input file name? ");
  fgets(line,MAXLINE,stdin);
  if ( strlen(line) == 0 || sscanf(line,"%s",infile) != 1 ) {
    fprintf(stderr,"cannot read input file name from '%s'\n",line);
    exit(0);
  }

  /* output file name: enter for standard out */
  memset(line,0,sizeof(char)*MAXLINE);	// clear the buffer
  fprintf(stderr,"Output file name? ");
  fgets(line,MAXLINE,stdin);
  if ( strlen(line) == 0 || sscanf(line,"%s",outfile) != 1 ) {
    fprintf(stderr,"cannot read output file name from '%s'\n",line);
    fprintf(stderr,"output to stdout\n");
    fp = stdout;
    memset(outfile,0,sizeof(char)*MAXLINE);	// clear the buffer
  }
  else {
    /* open output file pointer */
    fp = fopen(outfile,"w");
    if ( fp == NULL ) {
      fprintf(stderr, "cannot open file '%s' for write\n",outfile);
      fprintf(stderr,"output to stdout\n");
      fp = stdout;
    }
  }

  /* read text file of integers:
   * number_of_intergers integer1 integer2 ...
   * then convert it to a linked list */
  root = readkeys_textfile_LHBT(infile, &numWords);

#ifdef MEASURE_TIME
  start = clock();
#endif

  if ( root != NULL ) {

    // prints input
    fprintf(fp,"=====================================\n");
    numNodes = print_LHBT(fp, root);
    fprintf(fp,"total %d nodes\n",numNodes);

    // BST construction by simple insertion
    // keep root unchanged
    bst1 = generate_BST_by_insertion(copy_bt_recursive(root));

    fprintf(fp,"=====================================\n");
    numNodes = print_BST_sortedorder(fp,bst1,0);
    fprintf(fp,"total %d nodes (sorted)\n",numNodes);
    fprintf(fp,"=====================================\n");
    lev = print_BST_right_center_left(fp,bst1,0);
    fprintf(fp,"BST height %d\n",lev);
    fprintf(fp,"=====================================\n");
    lev = print_BST_1(fp,bst1,0);
    fprintf(fp,"BST height %d\n",lev);
    fprintf(fp,"=====================================\n");

    int has_bar[BULK_SIZE];
    lev = print_BST_2(fp,bst1,0, has_bar);
    fprintf(fp,"BST height %d\n",lev);
    fprintf(fp,"=====================================\n");

    int has_bar_2[BULK_SIZE];
    bst1 = BST_to_completeBST(bst1,numNodes);
    lev = print_BST_2(fp,bst1,0, has_bar_2);
    fprintf(fp,"Complete BST height %d\n",lev);
    fprintf(fp,"=====================================\n");

   // BST using quick sort, pivot as left, basic
 /*   int has_bar_3[BULK_SIZE];
    bst2 = generate_BST_quicksort_basic(copy_bt_recursive(root));
    lev = print_BST_2(fp,bst2,0, has_bar_3);
    fprintf(fp,"BST by QUICKSORT, height %d\n",lev);
    fprintf(fp,"=====================================\n");

    // BST using quick sort, advanced, to reduce height
    bst2 = generate_BST_quicksort_advanced(copy_bt_recursive(root));
    lev = print_BST_2(fp,bst2,0);
    fprintf(fp,"BST by QUICKSORT (advanced), height %d\n",lev);
    fprintf(fp,"=====================================\n");
*/
    free_bt_recursive(root);
    free_bt_recursive(bst1);
    free_bt_recursive(bst2);
  }

#ifdef MEASURE_TIME
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  fprintf(fp,"TIME %.5f seconds\n",cpu_time_used);
#endif

  if ( fp != NULL && fp != stdout ) fclose(fp);
  return 0;
}


/////////////////////////////////////////////////////////////
// implementation: functions for binary tree node
/////////////////////////////////////////////////////////////

const char* getkey(struct BTNode *a)
{
  int i;
  for (i=0; i<BULK_SIZE-KEYLENGTH; i++) {
    if ( a->bulk[i] != '\0' ) return a->bulk+i;
  }
  return NULL;	// not found
}

int setkey(struct BTNode *a, const char kw[])
{
  int pos;
  if ( a != NULL ) {
    // fill with 0
    memset(a->bulk,0,sizeof(char)*BULK_SIZE);

    // find position randomly to store KEYLENGTH+1 characters
    pos = rand()%(BULK_SIZE-KEYLENGTH);
    if ( kw != NULL ) memcpy(a->bulk+pos,kw,sizeof(char)*KEYLENGTH);
    a->bulk[pos+KEYLENGTH] = '\0';	// to make it a C string

    // success
    return 1;
  }
  else return 0;
}

struct BTNode *generate_btnode(const char kw[])
{
  struct BTNode *tmp;

  tmp = (struct BTNode*)malloc(sizeof(struct BTNode));
  setkey(tmp,kw);

  // initial left and right children for the generated leaf node
  tmp->left = tmp->right = NULL;

  return tmp;
}

void free_bt_recursive (struct BTNode *bt)
{
  if ( bt != NULL ) {
    free_bt_recursive(bt->left);
    free_bt_recursive(bt->right);
    free(bt);
  }
}

struct BTNode *copy_bt_recursive (struct BTNode *bt)
{
  struct BTNode *dup;

  if ( bt != NULL ) {
    dup = (struct BTNode*) malloc(sizeof(struct BTNode));
    copykey(dup,bt);
    dup->left = copy_bt_recursive(bt->left);
    dup->right = copy_bt_recursive(bt->right);
  }
  else dup = NULL;
  return dup;
}

struct BTNode *insert_left_bcnode(struct BTNode *parent, struct BTNode *newPtr)
{
  if ( parent == NULL ) return newPtr;	// no parent
  else if ( newPtr == NULL ) return parent;	// Nothing to add
  else if ( newPtr->left != NULL ) {
    fprintf(stderr,"cannot add a node with non-null left tree\n");
    return parent;
  }
  else {
    newPtr->left = parent->left;
    parent->left=newPtr;
    return newPtr;	// returning new node as a new parent
  }
}

// static: internal use only
static int _compare_n_char(const char a[], const char b[], int L)
{
  int i;
  for (i=0; i<L; i++) {
    if ( a[i] < b[i] ) return -1;
    else if ( a[i] > b[i] ) return 1;
    else continue;	// to next character
  }
  return 0;
}

int comparekey(struct BTNode *a, struct BTNode *b)
{
  return _compare_n_char(getkey(a),getkey(b), KEYLENGTH);
}

/////////////////////////////////////////////////////////////
// File I/O
/////////////////////////////////////////////////////////////
struct BTNode *readkeys_textfile_LHBT( const char infile[], int *pN )
  // read key words from the given file
  // and generate a binary tree which is skewed to the left
  // (all right children are NULL)
{
  struct BTNode *root, *cur, *tmp;
  char word[1024];
  FILE *fp;
  int i;

  // check for input file name
  if ( infile == NULL ) {
    fprintf(stderr, "NULL file name\n");
    return NULL;
  }

  // check for file existence
  fp = fopen(infile,"r");
  if ( !fp ) {
    fprintf(stderr, "cannot open file %s\n",infile);
    return NULL;
  }

  // check for number of keys
  if ( fscanf(fp, "%d", pN) != 1 || *pN <= 0 ) {
    fprintf(stderr, "File %s: ",infile);
    fprintf(stderr, "number of keys cannot be read or or wrong\n");
    fclose(fp);
    return NULL;
  }

  /*
  // check for number of characters per key
  if ( fscanf(fp, "%d", pL) != 1 || *pL <= 0 ) {
    fprintf(stderr, "File %s: ",infile);
    fprintf(stderr, "number of characters per key cannot be read or or wrong\n");
    fclose(fp);
    return NULL;
  }
  */

  // reading keys
  root = cur = tmp = NULL;
  for (i=0; i<(*pN); i++) {
    if ( fscanf(fp, "%s", word) != 1 ) {
      fprintf(stderr, "cannot read a word at %d/%d\n",i+1,(*pN));
      *pN = i;	// number of read keys so far
      break;
    }
    else {
      //check_and_correct_word(word, KEYLENGTH);

      // generate a new node
      tmp = generate_btnode(word);

      if ( root == NULL ) root = cur = tmp;
      else cur = insert_left_bcnode(cur,tmp);
    }
  }

  return root;
}

