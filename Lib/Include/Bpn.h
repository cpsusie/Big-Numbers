#pragma once

#include "Array.h"

typedef enum {
  LAYER_INPUT
 ,LAYER_HIDDEN
 ,LAYER_OUTPUT
} LayerType;

#define MAXLAYERCOUNT 5

class BpnLayer {
private:
  void allocate(LayerType type, int noutputs, int ninputs, bool bias);
  void copyVectors(const BpnLayer &src);
  void deallocate();
public:
  LayerType m_type;
  bool      m_bias;
  int       m_noutputs, m_ninputs;
  float    *m_outputs;
  float   **m_weights, *m_biasweights;
  float    *m_errors;
  float   **m_lastdelta;
  BpnLayer(LayerType type, int noutputs, int ninputs, bool bias);
  BpnLayer(const BpnLayer &src);
  BpnLayer &operator=(const BpnLayer &rhs);
  ~BpnLayer();
  void load(FILE *f);
  void save(FILE *f) const;
  void list() const;
  int size() const { return m_noutputs; }
};

class Bpn;

class BpnGraphic {
public:
  virtual void drawWeights(Bpn *b);
  virtual void drawUnits(Bpn *b);
};

extern BpnGraphic *BPN_NODISP;
extern BpnGraphic *BPN_DISP;
extern BpnGraphic *BPN_GRAPHICS;

class Bpn {
private:
  bool         m_bias;
  int          m_nhiddenlayers;
  BpnLayer    *m_inunits, *m_outunits, **m_hiddenlayers;
  float        m_momentum, m_learningrate;
  BpnGraphic  *m_bpndraw;
  void   allocateLayers(const Array<int> &layerunits, bool bias);
  void   deallocateLayers();
  void   setInput(const Array<double> &in);
  void   layerPropagate(BpnLayer *lower, BpnLayer *upper);
  void   propagateForward();
  void   computeOutputError(const Array<double> &target);
  void   backPropagateError();
  void   adjustWeights();
  void   initLayers();
  void   copyLayers(const Bpn &src);
public:
  Bpn(const Bpn &src);
  Bpn(const Array<int> &nunits, bool bias = false);
  Bpn(char *fname);
  ~Bpn();
  Bpn &operator=(const Bpn &rhs);
  void   save(const TCHAR  *fileName) const;
  void   save(const String &fileName) const;
  void   load(const TCHAR  *fileName);
  void   load(const String &fileName);
  void   save(FILE *f) const;
  void   load(FILE *f);
  void   learn(            const Array<double> &input, const Array<double> &output);
  void   learnLast(        const Array<double> &output);
  Array<double> &recognize(const Array<double> &input, Array<double> &output);
  Array<double> &getOutput(Array<double> &out) const;
  double getPatternError(  const Array<double> &target) const;
  void   list() const;
  BpnGraphic *setGraphic(BpnGraphic *gt);
  float  getLearningRate() const         { return m_learningrate;     }
  void   setLearningRate(float newvalue) { m_learningrate = newvalue; }
  float  getMomentum() const             { return m_momentum;         }
  void   setMomentum(float newvalue)     { m_momentum     = newvalue; }
  int    getInputCount()  const          { return m_inunits->size();  }
  int    getOutputCount() const          { return m_outunits->size(); }
  int    getHiddenLayerCount() const     { return m_nhiddenlayers;    }
  Array<int> getLayerUnits() const;
  BpnLayer *getInputLayer()           { return m_inunits;             }
  BpnLayer *getHiddenLayer(int index) { return m_hiddenlayers[index]; }
  BpnLayer *getOutputLayer()          { return m_outunits;            }
};
