#include "stdafx.h"
#include <Console.h>
#include <ctype.h>
#include <CountedByteStream.h>
#include "AdaptiveHuffmanTree.h"

//#define DEBUGMODULE

void AdaptivHuffmanTree::compress(ResetableByteInputStream &input, ByteOutputStream &output, int quality) {
  ByteCounter inputCounter, outputCounter;

  initialize();
  CountedByteInputStream   in(inputCounter   , input );
  CountedByteOutputStream  outc(outputCounter, output);
  BitOutputStream          out(outc);

  int c;
#ifdef DEBUGMODULE
  dump();
#endif
  while((c = in.getByte()) != EOF) {
//    fprintf(stderr,"read %d\n",c); fflush(stderr);
    encode(out,c);
    updateModel(c);

#ifdef DEBUGMODULE
  dump();
#endif
//    check();
  }
  encode(out,endOfStream);

  m_rawSize        = inputCounter.getCount();
  m_compressedSize = outputCounter.getCount();
}

void AdaptivHuffmanTree::expand(ByteInputStream &input, ByteOutputStream &output) {
  ByteCounter inputCounter, outputCounter;

  initialize();
  CountedByteInputStream   inc(inputCounter , input );
  CountedByteOutputStream  out(outputCounter, output);

  BitInputStream in(inc);

  int c;
  while((c = decode(in)) != endOfStream) {
//    fprintf(stderr,"write %d\n",c); fflush(stderr);
    out.putByte(c);
    updateModel(c);
  }

  m_compressedSize = inputCounter.getCount();
  m_rawSize        = outputCounter.getCount();
}

void AdaptivHuffmanTree::addNode(int parent, int weight, bool child_is_leaf, int child) {
  HuffmanNode &n    = m_nodes[m_nextFree];
  n.m_parent        = parent;
  n.m_weight        = weight;
  n.m_childIsLeaf   = child_is_leaf;
  n.m_child         = child;
  if(child_is_leaf) {
    m_leaf[child] = m_nextFree;
  }
  m_nextFree++;
}

void AdaptivHuffmanTree::initialize() {
  m_nextFree = 0;
  for(int i = 0; i < ARRAYSIZE(m_leaf); i++) {
    m_leaf[i] = -1;
  }
  addNode(-1,2,false,1);
  addNode(0,1,true,endOfStream);
  addNode(0,1,true,ESCAPE);
}

void AdaptivHuffmanTree::encode(BitOutputStream &output, UINT ch) {
  ULONG code       = 0;
  ULONG currentBit = 1;
  int codeSize             = 0;
  int currentNode = m_leaf[ch];
  if(currentNode == -1) {
    currentNode = m_leaf[ESCAPE];
  }
  while(currentNode) {
    if((currentNode & 1) == 0) {
      code |= currentBit;
    }
    currentBit <<= 1;
    codeSize++;
    currentNode = m_nodes[currentNode].m_parent;
  }
  output.putBits(code,codeSize);
  if(m_leaf[ch] == -1) {
    output.putBits(ch,8);
    addNewNode(ch);
  }
}

UINT AdaptivHuffmanTree::decode(BitInputStream &input) {
  int currentNode = 0;
  while(!m_nodes[currentNode].m_childIsLeaf) {
    currentNode = m_nodes[currentNode].m_child;
    currentNode += input.getBit();
  }
  UINT ch = m_nodes[currentNode].m_child;
  if(ch == ESCAPE) {
    ch = input.getBits(8);
    addNewNode((unsigned char)ch);
  }
  return ch;
}

void AdaptivHuffmanTree::updateModel(UINT ch) {
  if(m_nodes[0].m_weight == MAX_WEIGHT) {
    rebuildTree();
  }
  int currentNode = m_leaf[ch];
  while(currentNode != -1) {
    m_nodes[currentNode].m_weight++;
    int newNode;
    for(newNode = currentNode; newNode; newNode--) {
      if(m_nodes[newNode-1].m_weight >= m_nodes[currentNode].m_weight) {
        break;
      }
    }
    if(newNode != currentNode) {
      swapNodes(currentNode,newNode);
      currentNode = newNode;
    }
    currentNode = m_nodes[currentNode].m_parent;
  }
}

void AdaptivHuffmanTree::rebuildTree() {
  int j = m_nextFree - 1;
  for(int i = j; i >= 0; i--) {
    if(m_nodes[i].m_childIsLeaf) {
      m_nodes[j] = m_nodes[i];
      m_nodes[j].m_weight = (m_nodes[j].m_weight + 1) / 2;
      j--;
    }
  }
  for(int i = m_nextFree - 2; j >= 0; i-=2, j--) {
    int k = i+1;
    m_nodes[j].m_weight = m_nodes[i].m_weight + m_nodes[k].m_weight;
    UINT weight = m_nodes[j].m_weight;
    m_nodes[j].m_childIsLeaf = false;
    for(k = j+1; weight < m_nodes[k].m_weight; k++);
    k--;
    memmove(&m_nodes[j],&m_nodes[j+1], (k-j)*sizeof(HuffmanNode));
    m_nodes[k].m_weight = weight;
    m_nodes[k].m_child = i;
    m_nodes[k].m_childIsLeaf = false;
  }

  for(int i = m_nextFree - 1; i >= 0; i--) {
    HuffmanNode &n = m_nodes[i];
    if(n.m_childIsLeaf) {
      m_leaf[n.m_child] = i;
    } else {
      int k = n.m_child;
      m_nodes[k].m_parent = m_nodes[k+1].m_parent = i;
    }
  }
}

void AdaptivHuffmanTree::swapNodes(int i, int j) {
  HuffmanNode &ni = m_nodes[i];

  if(ni.m_childIsLeaf) {
    m_leaf[ni.m_child] = j;
  } else {
    m_nodes[ni.m_child].m_parent = j;
    m_nodes[ni.m_child+1].m_parent = j;
  }

  HuffmanNode &nj = m_nodes[j];
  if(nj.m_childIsLeaf) {
    m_leaf[nj.m_child] = i;
  } else {
    m_nodes[nj.m_child].m_parent = i;
    m_nodes[nj.m_child+1].m_parent = i;
  }

  HuffmanNode temp = ni;
  ni            = nj;
  ni.m_parent   = temp.m_parent;
  temp.m_parent = nj.m_parent;
  nj            = temp;
}

void AdaptivHuffmanTree::addNewNode(UINT ch) {
  int lighestNode    = m_nextFree - 1;
  int newNode        = m_nextFree;
  int zeroWeightNode = m_nextFree + 1;
  m_nextFree += 2;

  HuffmanNode &nn    = m_nodes[newNode];
  HuffmanNode &ln    = m_nodes[lighestNode];
  HuffmanNode &zn    = m_nodes[zeroWeightNode];

  nn                 = ln;
  nn.m_parent        = lighestNode;

  ln.m_child         = newNode;
  ln.m_childIsLeaf   = false;

  zn.m_child         = ch;
  zn.m_childIsLeaf   = true;
  zn.m_weight        = 0;
  zn.m_parent        = lighestNode;

  m_leaf[ch]         = zeroWeightNode;
  m_leaf[nn.m_child] = newNode;
}

String AdaptivHuffmanTree::getDisplayString(UINT ch) const {
  if(ch <= 255) {
    if(isprint(ch)) {
      return format(_T("'%c'"),ch);
    } else {
      return format(_T("#%d"),ch);
    }
  } else if(ch == ESCAPE) {
    return _T("esc");
  } else if(ch == endOfStream) {
    return _T("eos");
  } else {
    return format(_T("#%d"), ch);
  }
}

String AdaptivHuffmanTree::getCodeString(UINT ch) const {
  int currentNode = m_leaf[ch];
  String code;
  while(currentNode) {
    code += (currentNode & 1) ? _T("1") : _T("0");
    currentNode = m_nodes[currentNode].m_parent;
  }
  return rev(code);
}

void AdaptivHuffmanTree::dump(int index, int level) const {
  const HuffmanNode &n = m_nodes[index];
  _tprintf(_T("%*.*sindex:%-3d parent:%-3d weight:%-3d"),level,level,_T(""),index, n.m_parent, n.m_weight);
  if(n.m_childIsLeaf) {
    _tprintf(_T(" %s%*.*s%s"), getDisplayString(n.m_child).cstr(),40-level,40-level,_T(""),getCodeString(n.m_child).cstr());
  }

  _tprintf(_T("\n"));
  if(!n.m_childIsLeaf) {
    dump(n.m_child,level+2);
    dump(n.m_child+1,level+2);
  }
}

void AdaptivHuffmanTree::dump() const {
  Console::clear();
  Console::setCursorPos(0,0);
  dump(0,0);

  int x,y;
  Console::getCursorPos(x,y);
  Console::setCursorPos(0,y+1);
  int count = 0;
  for(int i = 0; i < ARRAYSIZE(m_leaf); i++) {
    if(m_leaf[i] != -1) {
      _tprintf(_T("[%3s] = %-3d   "), getDisplayString(i).cstr(),m_leaf[i]);
      if(++count == 5) {
        printf("\n");
        count = 0;
      }
    }
  }
  Console::getKey();
}

unsigned int AdaptivHuffmanTree::checkTree(int index) const {
  const HuffmanNode &n = m_nodes[index];
  if(!n.m_childIsLeaf) {
    unsigned int w = checkTree(n.m_child);
    w += checkTree(n.m_child+1);
    if(w != n.m_weight) {
      throwException(_T("weight of node %d (=%d) is different from sum of childnodes (=%d)")
                    ,index,n.m_weight,w);
    }
    if(m_nodes[n.m_child].m_parent != index) {
      throwException(_T("node %d has wrong parent (=%d). should be %d")
                    ,n.m_child,m_nodes[n.m_child].m_parent,index);
    }
    if(m_nodes[n.m_child+1].m_parent != index) {
      throwException(_T("node %d has wrong parent (=%d). should be %d")
                    ,n.m_child+1,m_nodes[n.m_child+1].m_parent,index);
    }
  } else if(m_leaf[n.m_child] == -1) {
    throwException(_T("leaf[%d] is -1. Child of %d")
                  ,n.m_child,index);
  }
  return n.m_weight;
}

void AdaptivHuffmanTree::check() const {
  checkTree(0);
  for(int i = 0; i < ARRAYSIZE(m_leaf); i++) {
    if(m_leaf[i] != -1) {
      const HuffmanNode &n = m_nodes[m_leaf[i]];
      if(!n.m_childIsLeaf) {
        throwException(_T("node[leaf[%d]] has false in child_is_leaf"), i);
        if(n.m_child != i) {
          throwException(_T("node[leaf[%d]].child = %d"),i,n.m_child);
        }
      }
    }
  }
}
