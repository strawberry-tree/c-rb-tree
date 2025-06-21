// 매크로 _RBTREE_H_가 정의되어 있지 않으면, 아래 모든 코드를 실행한다.

#ifndef _RBTREE_H_
#define _RBTREE_H_

// stddef.h: 포인터 연산, 메모리 관리 관련 타입, 매크로 제공
#include <stddef.h>

// 열거형 color_t: RBTREE_RED는 0, RBTREE_BLACK은 1
typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;

// int형 key_t. 그냥 int형이지만 키를 저장하게끔 식별자를 둔 거라고 생각하자.
typedef int key_t;

typedef struct node_t {
  color_t color;  // RED? BLACK?
  key_t key;      // 저장된 키 값
  struct node_t *parent, *left, *right;
  // 부모, 왼/오 자식
} node_t;

typedef struct {
  node_t *root; // 루트 노드
  node_t *nil;  // 앞으로 쓸 센티넬 노드를 저장? 근데 이걸 하나로 할 수 있나 몰겄다
} rbtree;

// new_rbtree: 새로운 rbtree를 생성하고, 주소를 반환해야 한다.
rbtree *new_rbtree(void);

// delete_rbtree: rbtree의 주소를 인수로 받으면, rbtree를 삭제해야 한다.
void delete_rbtree(rbtree *);


// 함수 원형 선언. 일단 하나씩 하자
node_t *rbtree_insert(rbtree *, const key_t);
node_t *rbtree_find(const rbtree *, const key_t);
node_t *rbtree_min(const rbtree *);
node_t *rbtree_max(const rbtree *);
int rbtree_erase(rbtree *, node_t *);

int rbtree_to_array(const rbtree *, key_t *, const size_t);

#endif  // _RBTREE_H_
