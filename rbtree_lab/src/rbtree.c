#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nilNode = (node_t *)calloc(1, sizeof(node_t));                 

  // 앞으로 사용할 닐 노드를 정의하기
  nilNode -> color = RBTREE_BLACK;
  p -> root = nilNode;
  p -> nil = nilNode;

  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}

// 우회전 연산을 한 뒤, 부모 노드를 반환
void *rotate_right(rbtree *t, node_t *base){
  // 왼쪽 자식은 새로운 부모 노드가 됨
  // 세로운 루트의 오른쪽 서브트리를, 기준 노드의 왼쪽에 붙임
  node_t *new_parent = base -> left;
  base -> left = new_parent -> right;

  // 오른족 서브트리의 부모는 새로운 루트
  // 단, 오른쪽서브트리가 Nil 노드가 아닐 때만
  if (new_parent -> right != t -> nil){
    new_parent -> right -> parent = base;
  }

  // 새로운 부모의 부모는, 기준 노드의 부모
  new_parent -> parent = base -> parent;

  // 기존 노드의 부모의 자식에 새로운 부모 할당
  if (base -> parent == t -> nil){
    t -> root = new_parent;
  } else if (base == base -> parent -> right){
    base -> parent -> right = new_parent;
  } else {
    base -> parent -> left = new_parent;
  }

  // 기준 노드를 새로운 부모의 오른쪽에 붙임
  new_parent -> right = base;
  base -> parent = new_parent;
}

// 좌회전 연산을 한 뒤, 부모 노드를 반환
void *rotate_left(rbtree *t, node_t *base){
  // 오른쪽 자식은 새로운 부모 노드가 됨
  // 세로운 루트의 왼쪽 서브트리를, 기준 노드의 오른쪽에 붙임
  node_t *new_parent = base -> right;
  base -> right = new_parent -> left;

  // 왼쪽 서브트리의 부모는 새로운 루트
  // 단, 왼쪽 서브트리가 Nil 노드가 아닐 때만
  if (new_parent -> left != t -> nil){
    new_parent -> left -> parent = base;
  }

  // 새로운 부모의 부모는, 기준 노드의 부모
  new_parent -> parent = base -> parent;

  // 기존 노드의 부모의 자식에 새로운 부모 할당
  if (base -> parent == t -> nil){
    t -> root = new_parent;
  } else if (base == base -> parent -> left){
    base -> parent -> left = new_parent;
  } else {
    base -> parent -> right = new_parent;
  }

  // 기준 노드를 새로운 부모의 왼쪽에 붙임
  new_parent -> left = base;
  base -> parent = new_parent;
}

void fixDoubleRed(rbtree *t, node_t *newNode){
  // 부모가 RED일 때 반복
  node_t *newParent, *newGP, *newUncle;

  while ((newParent = newNode -> parent)-> color == RBTREE_RED){
    // 삼촌 찾기
    newGP = newParent -> parent;
    if (newGP -> left == newParent){
      newUncle = newGP -> right;
    } else {
      newUncle = newGP -> left;
    }

    // 삼촌이 red인 경우
    if (newUncle -> color == RBTREE_RED){
      // 조부모를 red로, 부모/삼촌을 black으로
      newParent -> color = RBTREE_BLACK;
      newUncle -> color = RBTREE_BLACK;
      newGP -> color = RBTREE_RED;

      // 조부모 다시 체크
      newNode = newGP;
    } else {
      color_t temp;
      // 삼촌이 black인 경우
      if (newParent -> right == newNode && newGP -> right == newParent){
        // 부모와 조부모의 색을 바꿈
        temp = newParent -> color;
        newParent -> color = newGP -> color;
        newGP -> color = temp;

        // 조부모 기준 좌회전
        rotate_left(t, newGP);

      } else if (newParent -> left == newNode && newGP -> left == newParent){

        // 부모와 조부모의 색을 바꿈
        temp = newParent -> color;
        newParent -> color = newGP -> color;
        newGP -> color = temp;

        // 조부모 기준 우회전
        rotate_right(t, newGP);

      } else if (newParent -> right == newNode && newGP -> left == newParent){
        // 부모 기준 좌회전
        rotate_left(t, newParent);

      } else if (newParent -> left == newNode && newGP -> right == newParent) {
        // 이건 조건문 안 써도 되지만 명확히 하기 위해 그냥 써둠
        // 부모 기준 우회전
        rotate_right(t, newParent);
      }
    }
  }
  // 루트 노드를 BLACK으로
  t -> root -> color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // 예외 처리
  if (t == NULL) return NULL;

  // 넣을 위치를 찾기
  node_t *curr, *prev;  // 현재 노드, 직전 노드
  int isRight = -1;      // 루트노드면 -1, 오른쪽자식이면 1, 왼쪽자식이면 0
  prev = t -> nil;
  curr = t -> root;

  while (curr != t -> nil){
    if (curr -> key <= key){
      // 삽입할 값보다 작거나 같으면 오른쪽으로
      prev = curr;
      curr = curr -> right;
      isRight = 1;
    } else {
      // 삽입할 값보다 크면 왼쪽으로
      prev = curr;
      curr = curr -> left;
      isRight = 0;
    }
  }

  // 찾았으면 노드를 만들고 넣어 주기
  node_t *newNode = (node_t *)calloc(1, sizeof(node_t));
  newNode -> color = RBTREE_RED;  // 삽입되는 색은 RED이여야 함
  newNode -> left = t -> nil;       // 왼쪽자식은 nil
  newNode -> right = t -> nil;      // 오른쪽자식은 nil
  newNode -> parent = prev;         // 부모는 prev
  newNode -> key = key;

  if (isRight == -1){
    t -> root = newNode;
  }  
  if (isRight == 1){
    prev -> right = newNode;
  } else {
    prev -> left = newNode;
  }

  // 부모, 자식이 연속으로 RED일 수도 있으니 Fix
  fixDoubleRed(t, newNode);

  return newNode;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // 예외 처리
  if (t == NULL) return NULL;

  node_t *curr = t -> root; // 현재 노드
  
  while (curr != t -> nil){
    if (curr -> key < key){
      // 찾는 값보다 작으면 오른쪽으로
      curr = curr -> right;
    } else if (curr -> key > key){
      // 찾는 값보다 크면 왼쪽으로
      curr = curr -> left;
    } else {
      // 값을 찾으면 노드를 리턴
      return curr;
    }
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

void fixDoubleBlack(const rbtree *t, node_t *dbNode){

}

int rbtree_erase(rbtree *t, node_t *erase){
  // 예외 처리
  if (t == NULL || erase == NULL) return 0;

  if ((erase -> left == t -> nil) && (erase -> right == t -> nil)){
    // 삭제할 노드의 자식이 없는 경우
    if (erase -> parent == t -> nil){
      t -> root = t -> nil;
    } else if (erase -> parent -> right == erase) {
      erase -> parent -> right = t -> nil;
    } else {
      erase -> parent -> left = t -> nil;
    } 
    free(erase);
  } else if ((erase -> left == t -> nil)){
    // 삭제할 노드가 오른쪽 자식만 있는 경우
    if (erase -> parent == t -> nil){
      t -> root = erase -> right;
    } else if (erase -> parent -> right == erase) {
      erase -> parent -> right = erase -> right;
    } else {
      erase -> parent -> left = erase -> right;
    }
    erase -> right -> parent = erase -> parent;
    free(erase);
  } else if ((erase -> right == t -> nil)){
    // 삭제할 노드가 왼쪽 자식만 있는 경우
    if (erase -> parent == t -> nil){
      t -> root = erase -> left;
    } else if (erase -> parent -> right == erase) {
        erase -> parent = erase -> left;
    } else {
      erase -> parent = erase -> left;
    }
    erase -> left -> parent = erase -> parent;
    free(erase);
  } else{
    // 양쪽 자식이 있는 경우- 찾아라 계승자
    node_t *successor = erase -> right;
    while (successor -> left != t -> nil){
      successor = successor -> left;
    }
    int newValue = successor -> key;

    // 계승자 노드를 삭제
    rbtree_erase(t, successor);
    erase -> key = newValue;
    }
    return 1;
  }

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}

void inorder(const rbtree *t, node_t *p){
  if (p == t -> nil) return;
  
  inorder(t, p -> left);
  printf("%d(%d) ", p -> key, p -> color);
  inorder(t, p -> right);
}
void preorder(const rbtree *t, node_t *p){
  if (p == t -> nil) return;
  
  printf("%d(%d) ", p -> key, p -> color);
  preorder(t, p -> left);
  preorder(t, p -> right);
}
void postorder(const rbtree *t, node_t *p){
  if (p == t -> nil) return;
  
  postorder(t, p -> left);
  postorder(t, p -> right);
  printf("%d(%d) ", p -> key, p -> color);
  
}

int main(void){
  rbtree *t = new_rbtree();
  int nums[] = {50, 20, 10, 40, 30, 70, 60, 80, 90};

  for (int i = 0; i < 9; i++){
    rbtree_insert(t, nums[i]);
    preorder(t, t -> root);
    printf("\n");
  }

  for (int i = 0; i < 9; i++){
    rbtree_erase(t, t -> root);
    preorder(t, t -> root);
    printf("\n");
  }
  
  // printf("%d\n", n -> key);
}