#include "stdafx.h"

#if !defined(TABLEBASE_BUILDER)

#define HASH_CAPACITY 1187

GameKeyHashStack::GameKeyHashStack() {
  init();
}

GameKeyHashStack::GameKeyHashStack(const GameKeyHashStack &src) {
  init(src.m_stackCapacity);
  for(UINT i = 0; i < src.m_stackTop; i++) {
    push(*src.m_stack[i]);
  }
}

GameKeyHashStack &GameKeyHashStack::operator=(const GameKeyHashStack &src) {
  clear();
  for(UINT i = 0; i < src.m_stackTop; i++) {
    push(*src.m_stack[i]);
  }
  return *this;
}

GameKeyHashStack::~GameKeyHashStack() {
  cleanUp();
}

void GameKeyHashStack::init(UINT stackCapacity) {
  m_hashCapacity  = HASH_CAPACITY;
  m_stackCapacity = stackCapacity;
  m_stackTop      = 0;

  m_hashTable = new GameKeyHashElement*[m_hashCapacity]; TRACE_NEW(m_hashTable);
  for(UINT i = 0; i < m_hashCapacity; i++) {
    m_hashTable[i] = NULL;
  }

  m_stack = new GameKeyHashElement*[m_stackCapacity]; TRACE_NEW(m_stack);
  for(UINT i = 0; i < m_stackCapacity; i++) {
    m_stack[i] = NULL;
  }
  initFreeList();
}

void GameKeyHashStack::cleanUp() {
  clear();
  SAFEDELETEARRAY(m_hashTable);
  SAFEDELETEARRAY(m_stack    );
  deleteFreeList();
}

void GameKeyHashStack::initFreeList() {
  m_freeList = NULL;
  for(int i = 0; i < 100; i++) {
    GameKeyHashElement *p = new GameKeyHashElement; TRACE_NEW(p);
    p->m_next = m_freeList;
    m_freeList = p;
  }
}

void GameKeyHashStack::deleteFreeList() {
  for(GameKeyHashElement *p = m_freeList, *next = NULL; p; p = next) {
    next = p->m_next;
    SAFEDELETE(p);
  }
  m_freeList = NULL;
}

GameKeyHashElement *GameKeyHashStack::fetchElement() {
  if(m_freeList != NULL) {
    GameKeyHashElement *result = m_freeList;
    m_freeList = result->m_next;
    return result;
  } else {
    GameKeyHashElement *result = new GameKeyHashElement; TRACE_NEW(result);
    return result;
  }
}

void GameKeyHashStack::clear() {
  while(!isEmpty()) {
    pop();
  }
}

int GameKeyHashStack::push(const GameKey &gameKey) {
  const ULONG index = gameKey.hashCode() % m_hashCapacity;
  GameKeyHashElement *q = m_hashTable[index], *n;
  for(n = q; n; n = n->m_next) {
    if(*n == gameKey) {
      n->m_count++;
      break;
    }
  }

  if(n == NULL) {
    n = fetchElement();
    (GameKey&)(*n) = gameKey;

    m_hashTable[index] = n;
    n->m_next          = q;
    if(q != NULL) {
      q->m_prev = &n->m_next;
    }
    n->m_prev = &m_hashTable[index];
  }

  if(m_stackTop >= m_stackCapacity) {
    expandStack();
  }
  return (m_stack[m_stackTop++] = n)->m_count;
}

int GameKeyHashStack::getRepeatCount() const {
  return (m_stackTop == 0) ? 0 : m_stack[m_stackTop-1]->m_count;
}

void GameKeyHashStack::expandStack() {
  const UINT newStackCapacity = 2 * m_stackCapacity;
  GameKeyHashElement **newStack = new GameKeyHashElement*[newStackCapacity]; TRACE_NEW(newStack);
  for(UINT i = 0; i < m_stackCapacity; i++) {
    newStack[i] = m_stack[i];
  }
  SAFEDELETEARRAY(m_stack);
  m_stack         = newStack;
  m_stackCapacity = newStackCapacity;
}

void GameKeyHashStack::pop() {
  GameKeyHashElement *n = m_stack[--m_stackTop];
  if(n->m_count > 1) {
    n->m_count--;
  } else {
    GameKeyHashElement *next = n->m_next;
    if(next) {
      next->m_prev = n->m_prev;
    }
    *n->m_prev = next; // n->m_prev always != NULL !

    n->m_next = m_freeList;
    m_freeList = n;
  }
}

#endif
