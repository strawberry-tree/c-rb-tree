#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nilNode = (node_t *)calloc(1, sizeof(node_t));  
  
  // (TODO) 나도 널 처리 해야겠다.
  if (p == NULL || nilNode == NULL) return NULL;

  // 앞으로 사용할 닐 노드를 정의하기
  nilNode -> color = RBTREE_BLACK;
  p -> root = nilNode;
  p -> nil = nilNode;

  return p;
}

void free_nodes(const rbtree *t, node_t *curr){
  if (curr == t -> nil) return;
  free_nodes(t, curr -> left);
  free_nodes(t, curr -> right);
  free(curr);
}

void delete_rbtree(rbtree *t) {
  free_nodes(t, t -> root);
  free(t -> nil);
  free(t);
}

// 우회전 연산을 한 뒤, 부모 노드를 반환
void rotate_right(rbtree *t, node_t *base){
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
void rotate_left(rbtree *t, node_t *base){
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
        newNode = newNode -> left;

      } else if (newParent -> left == newNode && newGP -> right == newParent) {
        // 이건 조건문 안 써도 되지만 명확히 하기 위해 그냥 써둠
        // 부모 기준 우회전
        rotate_right(t, newParent);
        newNode = newNode -> right;
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
  } else if (isRight == 1){
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
  // 왼쪽으로만 이동
  node_t *minNode = t -> root;

  while (minNode -> left != t -> nil){
    minNode = minNode -> left;
  }
  return minNode;
}

node_t *rbtree_max(const rbtree *t) {
  // 오른쪽으로만 이동
  node_t *maxNode = t -> root;

  while (maxNode -> right != t -> nil){
    maxNode = maxNode -> right;
  }
  return maxNode;
}

void fixDoubleBlack(rbtree *t, node_t *dbNode){
  while (dbNode != t -> root && dbNode -> color == RBTREE_BLACK){
    node_t *repParent = dbNode -> parent;
    node_t *sibling;
    if (repParent -> left == dbNode){
      sibling = repParent -> right;
    } else {
      sibling = repParent -> left;
    }
    if (sibling -> color == RBTREE_RED){
      // CASE 1
      // 부모와 형제의 색 바꾸기
      color_t temp;
      temp = repParent -> color;
      repParent -> color = sibling -> color;
      sibling -> color = temp;

      // 부모 기준 회전
      if (repParent -> left == dbNode){
        rotate_left(t, repParent);
      } else {
        rotate_right(t, repParent);
      }
    } else if (sibling -> left -> color == RBTREE_BLACK && sibling -> right -> color == RBTREE_BLACK){
      // CASE 2
      // 형제를 Red로 변경
      sibling -> color = RBTREE_RED;
      dbNode = repParent;
    } else if (repParent -> left == dbNode && sibling -> right -> color == RBTREE_BLACK){
      // CASE 3 -> DB가 왼쪽 자식
      // 오른쪽 형제와 형제의 왼쪽 자녀의 색 바꾸기
      color_t temp;
      temp = sibling -> color;
      sibling -> color = sibling -> left -> color;
      sibling -> left -> color = temp;

      // 오른쪽 형제 기준 우회전
      rotate_right(t, sibling);
    } else if (repParent -> right == dbNode && sibling -> left -> color == RBTREE_BLACK){
      // CASE 3 -> DB가 오른쪽 자식
      // 왼쪽 형제와 형제의 오른쪽 자녀의 색 바꾸기
      color_t temp;
      temp = sibling -> color;
      sibling -> color = sibling -> right -> color;
      sibling -> right -> color = temp;

      // 왼쪽 형제 기준 좌회전
      rotate_left(t, sibling);
      
    } else if (repParent -> left == dbNode){
      // CASE 4 -> DB가 왼쪽 자식
      // 색 바꾸기
      sibling -> color = repParent -> color;
      sibling -> right -> color = RBTREE_BLACK;
      repParent -> color = RBTREE_BLACK;

      // 부모 기준 좌회전
      rotate_left(t, repParent);

      // 관찰하는 노드를 루트노드로 
      dbNode = t -> root;

    } else {
      // CASE 4 -> DB가 오른쪽 자식
      // 색 바꾸기
      sibling -> color = repParent -> color;
      sibling -> left -> color = RBTREE_BLACK;
      repParent -> color = RBTREE_BLACK;

      // 부모 기준 우회전
      rotate_right(t, repParent);

      // while문을 종료
      dbNode = t -> root;
    }
  }
  // 타깃을 블랙으로 변경
  dbNode -> color = RBTREE_BLACK;
  
}


int rbtree_erase(rbtree *t, node_t *erase){
  // 예외 처리
  if (t == NULL || erase == NULL) return 0;

  color_t eraseColor; // 삭제된 색
  node_t *repNode = t -> nil; // 삭제된 위치의 노드

  if ((erase -> left == t -> nil) && (erase -> right == t -> nil)){
    // 삭제할 노드의 자식이 없는 경우
    if (erase -> parent == t -> nil){
      t -> root = t -> nil;
    } else if (erase -> parent -> right == erase) {
      erase -> parent -> right = t -> nil;
    } else {
      erase -> parent -> left = t -> nil;
    }
    t -> nil -> parent = erase -> parent;
    eraseColor = erase -> color;
    free(erase);
  } else if ((erase -> left == t -> nil)){
    repNode = erase -> right;
    // 삭제할 노드가 오른쪽 자식만 있는 경우
    if (erase -> parent == t -> nil){
      t -> root = erase -> right;
    } else if (erase -> parent -> right == erase) {
      erase -> parent -> right = erase -> right;
    } else {
      erase -> parent -> left = erase -> right;
    }
    erase -> right -> parent = erase -> parent;
    eraseColor = erase -> color;
    free(erase);
  } else if ((erase -> right == t -> nil)){
    repNode = erase -> left;
    // 삭제할 노드가 왼쪽 자식만 있는 경우
    if (erase -> parent == t -> nil){
      t -> root = erase -> left;
    } else if (erase -> parent -> right == erase) {
        erase -> parent -> right = erase -> left;
    } else {
      erase -> parent -> left= erase -> left;
    }
    erase -> left -> parent = erase -> parent;
    eraseColor = erase -> color;
    free(erase);
  } else {
    // 양쪽 자식이 있는 경우- 찾아라 계승자
    node_t *successor = erase -> left;
    while (successor -> right != t -> nil){
      successor = successor -> right;
    }
    repNode = successor -> left;
    
    eraseColor = successor -> color;
  
    if (successor -> parent == t -> nil){
      t -> root = repNode;
    } else if (successor -> parent -> left == successor){
      successor -> parent -> left = repNode;
      
    } else if (successor -> parent -> right == successor){
      successor -> parent -> right = repNode;
    }
    repNode -> parent = successor -> parent;

    erase -> key = successor -> key;
    free(successor); 
  }
  if (eraseColor == RBTREE_BLACK){
    fixDoubleBlack(t, repNode);
  }
  return 1;
}

key_t *inorder(const rbtree *t, key_t *arr, key_t *target, node_t *curr){
  if (curr == t -> nil) return arr;
  arr = inorder(t, arr, target, curr -> left);
  if (arr < target){
    *arr = curr -> key;
    arr += 1;
  }
  arr = inorder(t, arr, target, curr -> right);
  return arr;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  inorder(t, arr, arr + n, t -> root);
  return 0;
}
