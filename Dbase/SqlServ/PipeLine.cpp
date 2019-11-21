#include "stdafx.h"

#define PIPELINESIZE 10
bool PipeLine::full()  const {
  return m_count == PIPELINESIZE;
}

PipeLine::PipeLine() : m_emptysem(0) {
  m_buffer = new Tuple[PIPELINESIZE];
  m_head = m_tail = m_count = 0;
}

PipeLine::~PipeLine() {
  delete[] m_buffer;
}

void PipeLine::writeeof() {
  Tuple t(0);
  write(t);
}

void PipeLine::write(const Tuple &v) {
  for(;;) {
    m_gate.wait();
    if(!full()) break;
    m_gate.notify(); // open gate
    m_fullsem.wait();
  }
  m_buffer[m_tail] = v;
  m_tail = (m_tail + 1) % PIPELINESIZE;
  m_count++;
  m_emptysem.notify(); // tell any thread, waiting in remove that we are not empty anymore
  m_gate.notify(); // open gate
}

bool PipeLine::read(Tuple &v) {
  for(;;) {
    m_gate.wait();
    if(!isEmpty()) break;
    m_gate.notify(); // open gate
    m_emptysem.wait();
  }
  if(m_buffer[m_head].size() > 0) {
    v = m_buffer[m_head];
    m_head = (m_head + 1) % PIPELINESIZE;
    m_count--;
    m_fullsem.notify(); // tell any thread waiting in insert, that we are not full anymore
    m_gate.notify(); // open gate
    return true;
  }
  else { // eof
    m_gate.notify();
    return false;
  }
}
