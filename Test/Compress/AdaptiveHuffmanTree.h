#pragma once

#include "Compressor.h"

#define ESCAPE        (endOfStream + 1)
#define SYMBOL_COUNT  258
#define NODE_TABLE_COUNT ((SYMBOL_COUNT * 2) - 1)
#define MAX_WEIGHT    0x8000

class HuffmanNode {
public:
  UINT m_weight;
  int  m_parent;
  int  m_childIsLeaf;
  int  m_child;
};

class AdaptivHuffmanTree : public Compressor {
private:
  int         m_leaf[SYMBOL_COUNT];
  int         m_nextFree;
  HuffmanNode m_nodes[NODE_TABLE_COUNT];
  void   addNode(int parent, int weight, bool childIsLeaf, int child);
  void   addNewNode(UINT ch);
  void   rebuildTree();
  void   swapNodes(int i, int j);
  void   dump(int index, int level) const;
  UINT   checkTree(int index = 0) const;
  String getCodeString(UINT ch) const;
  String getDisplayString(UINT ch) const;
  void   initialize();
  void   updateModel(UINT ch);
  void   encode(BitOutputStream &output, UINT ch);
  UINT   decode(BitInputStream &input);
public:
  void dump() const;
  void check() const;
  void compress(ResetableByteInputStream &input, ByteOutputStream &output, int quality);
  void expand(  ByteInputStream          &input, ByteOutputStream &output);
};
