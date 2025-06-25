#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  if (p == NULL) return NULL;       // 할당 실패 

  node_t *nilNode = (node_t *)calloc(1, sizeof(node_t));  
  if (nilNode == NULL) return NULL;  // 할당 실패 

  // 앞으로 사용할 닐 노드를 정의하기
  nilNode -> color = RBTREE_BLACK;
  
  p -> nil = nilNode;
  p -> root = p -> nil;
  p -> nil -> left = p -> nil;
  p -> nil -> right = p -> nil;
  p -> nil -> parent = p -> nil;

  return p;
}

void free_nodes(const rbtree *t, node_t *curr){
  // 예외 처리
  if (t == NULL || curr == NULL) return;

  // 후위 순회와 같은 방식으로, 자식 노드를 재귀적으로 free하고, 자신을 free
  if (curr == t -> nil) return;
  free_nodes(t, curr -> left);
  free_nodes(t, curr -> right);
  free(curr);
}

void delete_rbtree(rbtree *t) {
  // 예외 처리
  if (t == NULL) return;

  // nil 노드를 포함한 모든 노드를 free하고, rbtree도 free
  free_nodes(t, t -> root);
  free(t -> nil);
  free(t);
}

// 우회전 연산을 한 뒤, 부모 노드를 반환
void rotate_right(rbtree *t, node_t *base){
  // 예외 처리
  if (t == NULL || base == NULL) return;

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

  // 기존 노드의 부모의 자식에 새로운 부모 연결
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
  // 예외 처리
  if (t == NULL || base == NULL) return;

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

void swap_color(node_t *a, node_t *b){
  // 예외 처리
  if (a == NULL || b == NULL) return;

  // 두 노드의 색 바꾸기
  color_t temp = a -> color;
  a -> color = b -> color;
  b -> color = temp;
}

void insert_fix(rbtree *t, node_t *newNode){
  // 예외 처리
  if (t == NULL || newNode == NULL) return;

  // 부모가 RED일 때 반복
  node_t *newParent, *newGP, *newUncle; // 부모, 조부모, 삼촌

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
      // 삼촌이 black인 경우
      if (newParent -> right == newNode && newGP -> right == newParent){
        swap_color(newParent, newGP);   // 부모와 조부모의 색을 바꿈
        rotate_left(t, newGP);          // 조부모 기준 좌회전
      } else if (newParent -> left == newNode && newGP -> left == newParent){
        swap_color(newParent, newGP);   // 부모와 조부모의 색을 바꿈
        rotate_right(t, newGP);         // 조부모 기준 우회전
      } else if (newParent -> right == newNode && newGP -> left == newParent){
        rotate_left(t, newParent);
        newNode = newNode -> left;      // 부모 기준 좌회전
      } else {
        rotate_right(t, newParent);
        newNode = newNode -> right;     // 부모 기준 우회전
      }
    }
  }
  // 루트 노드를 BLACK으로
  t -> root -> color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // 예외 처리
  if (t == NULL) return NULL;

  // 노드 만들기
  node_t *newNode = (node_t *)calloc(1, sizeof(node_t));
  if (newNode == NULL) return NULL;

  // 넣을 위치를 찾기
  node_t *curr, *prev;  // 현재 노드, 직전 노드
  prev = t -> nil;
  curr = t -> root;

  while (curr != t -> nil){
    prev = curr;
    if (curr -> key <= key){
      // 삽입할 값보다 작거나 같으면 오른쪽으로
      curr = curr -> right;
    } else {
      // 삽입할 값보다 크면 왼쪽으로
      curr = curr -> left;
    }
  }

  // 찾았으면 노드 넣어 주기
  newNode -> color = RBTREE_RED;  // 삽입되는 색은 RED이여야 함
  newNode -> left = t -> nil;       // 왼쪽자식은 nil
  newNode -> right = t -> nil;      // 오른쪽자식은 nil
  newNode -> parent = prev;         // 부모는 prev
  newNode -> key = key;

  // 부모 노드와 삽입된 노드 연결
  if (prev == t -> nil){
    t -> root = newNode;
  } else if (prev -> key <= key){
    prev -> right = newNode;
  } else {
    prev -> left = newNode;
  }

  // 부모, 자식이 연속으로 RED일 수도 있으니 Fix
  insert_fix(t, newNode);

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

  // 찾았는데 없으면 NULL 반환
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // 예외 처리
  if (t == NULL || t -> root == t -> nil) return NULL;

  // 왼쪽으로만 이동
  node_t *minNode = t -> root;

  while (minNode -> left != t -> nil){
    minNode = minNode -> left;
  }
  return minNode;
}

node_t *rbtree_max(const rbtree *t) {
  // 예외 처리
  if (t == NULL || t -> root == t -> nil) return NULL;

  // 오른쪽으로만 이동
  node_t *maxNode = t -> root;

  while (maxNode -> right != t -> nil){
    maxNode = maxNode -> right;
  }
  return maxNode;
}

void delete_fix(rbtree *t, node_t *dbNode){
  // 예외 처리
  if (t == NULL || dbNode == NULL) return;

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
      swap_color(repParent, sibling);       // 부모와 형제의 색 바꾸기
      
      if (repParent -> left == dbNode){
        rotate_left(t, repParent);          // 부모 기준 좌회전
      } else {
        rotate_right(t, repParent);         // 부모 기준 우회전
      }

    } else if (sibling -> left -> color == RBTREE_BLACK && sibling -> right -> color == RBTREE_BLACK){
      // CASE 2
      sibling -> color = RBTREE_RED;          // 형제를 Red로 변경
      dbNode = repParent;

    } else if (repParent -> left == dbNode && sibling -> right -> color == RBTREE_BLACK){
      // CASE 3 -> DB가 왼쪽 자식
      swap_color(sibling, sibling -> left);   // 오른쪽 형제와 형제의 왼쪽 자녀의 색 바꾸기  
      rotate_right(t, sibling);               // 오른쪽 형제 기준 우회전

    } else if (repParent -> right == dbNode && sibling -> left -> color == RBTREE_BLACK){
      // CASE 3 -> DB가 오른쪽 자식
      swap_color(sibling, sibling -> right);  // 왼쪽 형제와 형제의 오른쪽 자녀의 색 바꾸기
      rotate_left(t, sibling);                // 왼쪽 형제 기준 좌회전

    } else {
      // CASE 4 -> DB가 왼쪽 자식
      // 색 바꾸기                              // 부모와 형제의 색 바꾸기
      swap_color(sibling, repParent);
      if (repParent -> left == dbNode){
        sibling -> right -> color = RBTREE_BLACK;
        rotate_left(t, repParent);              // 부모 기준 좌회전
      } else {
        sibling -> left -> color = RBTREE_BLACK;
        rotate_right(t, repParent);             // 부모 기준 우회전
      }
      dbNode = t -> root;                     // 관찰하는 노드를 루트노드로 
    }
  }
  // 타깃을 블랙으로 변경
  dbNode -> color = RBTREE_BLACK;
}

void change_connection(rbtree *t, node_t *erase, node_t *repNode){
  // 예외처리
  if (t == NULL || erase == NULL || repNode == NULL) return;

  // erase의 자리에 repNode를 연결하고, repNode의 부모를 erase의 부모로 설정
  if (erase -> parent == t -> nil){
    t -> root = repNode;
  } else if (erase -> parent -> right == erase) {
    erase -> parent -> right = repNode;
  } else {
    erase -> parent -> left = repNode;
  }
  repNode -> parent = erase -> parent;

}

int rbtree_erase(rbtree *t, node_t *erase){
  // 예외 처리
  if (t == NULL || erase == NULL || erase == t -> nil) return 1;

  node_t *repNode;                        // 삭제된 위치를 대체할 노드
  color_t eraseColor = erase -> color;    // 삭제된 색

  if (erase -> left != t -> nil && erase -> right != t -> nil){
    // 양쪽 자식이 있는 경우- 찾아라 계승자
    node_t *predecessor = erase -> left;    // 계승자 노드 (왼쪽 서브트리에서 가장 큰 값)
    while (predecessor -> right != t -> nil){
      predecessor = predecessor -> right;
    }
    eraseColor = predecessor -> color;      // 계승자 노드의 색 저장
    repNode = predecessor -> left;          // 계승자 노드의 왼쪽자식이 계승자 노드 대체

    if (predecessor != erase -> left){
      // predecessor -> left가 predecssor의 자리를 대신하도록 연결
      change_connection(t, predecessor, predecessor -> left);
      predecessor -> left = erase -> left;
      predecessor -> left -> parent = predecessor;
    } else {
      repNode -> parent = predecessor;
    }
    // predecessor가 erase의 자리를 대신하도록 연결
    change_connection(t, erase, predecessor);
    predecessor -> right = erase -> right;
    predecessor -> right -> parent = predecessor;

    // predecessor의 색을 원래 노드의 색으로 변경
    predecessor -> color = erase -> color;
  } else {
    // 한쪽 자식만 있거나, 자식이 없는 경우 - 남은 자식이 계승. 
    // 사실 둘 다 없으면 차피 nil이 계승하니 상관없음.
    if (erase -> left == t -> nil){
      repNode = erase -> right;
    } else {
      repNode = erase -> left;
    }
    change_connection(t, erase, repNode);
  }
  free(erase);
  if (eraseColor == RBTREE_BLACK){
    // 검정색 노드가 지워진 경우, 이를 해결해야 함!
    delete_fix(t, repNode);
  }
  t -> nil -> parent = t -> nil;  // 부모를 다시 nil 처리
  return 0;
}

key_t *inorder(const rbtree *t, key_t *arr, key_t *target, node_t *curr){
  // 예외 처리
  if (t == NULL || arr == NULL || target == NULL || curr == NULL) return NULL;

  // 중위 순회를 사용하며 *arr에 노드 값 저장
  if (curr == t -> nil || arr >= target) return arr;
  arr = inorder(t, arr, target, curr -> left);
  if (arr < target){
    *arr = curr -> key;
    arr += 1;
  }
  arr = inorder(t, arr, target, curr -> right);
  return arr;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // 예외 처리
  if (t == NULL || arr == NULL) return 1;

  // 전위 순회 사용 시, 값을 오름차순으로 탐색
  // 주소 arr + n 잔까지만 추가하게끔 매개변수 보냄
  inorder(t, arr, arr + n, t -> root);
  return 0;
}
