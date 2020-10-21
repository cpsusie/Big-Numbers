#include "pch.h"
#include <MyUtil.h>
#include <Math.h>
#include <Random.h>
#include <Bpn.h>

/*
#define DEBUG_BPN
*/

#if defined(IS32BIT)
#define ASM_OPTIMIZED
#endif

#pragma warning(disable : 4244)

static inline float frandom(float from, float to) {
  return randFloat(from, to);
}

static void randomVector(float *v, int size)  {
  for(int i = 0; i < size; i++) {
    v[i] = frandom(-0.25, 0.25);
  }
}

static void copyVector(float *dst, const float *src, int size) {
  memcpy(dst, src, size * sizeof(float));
}

static void writeVector(FILE *f, const float *v, int size) {
  for(int i = 0; i < size; i++) {
    _ftprintf(f,_T("%15.12e\n"), v[i]);
  }
}

static void loadVector(FILE *f, float *v, int size) {
  for(int i = 0; i < size; i++) {
    if(fscanf(f,"%e",&v[i]) != 1) {
      throwException(_T("%s:Expected number"), __TFUNCTION__);
    }
  }
}

void Bpn::layerPropagate(BpnLayer *lower, BpnLayer *upper) {
  float  *inputs      = lower->m_outputs;
  int     ninputs     = lower->m_noutputs;
  float  *outputs     = upper->m_outputs;
  int     noutputs    = upper->m_noutputs;
  float  *biasweights = upper->m_biasweights;

  for(int i = 0; i < noutputs; i++ ) {
    float sum = 0;
    float *weights = upper->m_weights[i];

#if !defined(ASM_OPTIMIZED)
    for(int j = 0; j < ninputs; j++ )
      sum += inputs[j] * weights[j];
#else
    __asm {
        fldz
        mov ecx, ninputs
        mov eax, weights
        mov ebx, inputs
startloop:
        cmp ecx,0
        je  exitloop
        fld dword ptr [eax]
        fmul dword ptr [ebx]
        fadd
        sub ecx,1
        add eax,4
        add ebx,4
        jmp startloop
exitloop:
        fstp dword ptr [sum]
    }
#endif

    if(m_bias) {
      sum += biasweights[i];
    }

    outputs[i] = (float)(1.0 / (1.0 + exp(-sum)));
  }
}

void Bpn::propagateForward() {
  layerPropagate(m_inunits, m_hiddenlayers[0]);
  int i;
  for(i = 1; i < m_nhiddenlayers; i++) {
    layerPropagate(m_hiddenlayers[i-1], m_hiddenlayers[i]);
  }
  layerPropagate(m_hiddenlayers[i-1], m_outunits);

  m_bpndraw->drawUnits(this);
}

void Bpn::computeOutputError(const CompactDoubleArray &target) {
  float  *errors  = m_outunits->m_errors;
  float  *outputs = m_outunits->m_outputs;
  const int n     = m_outunits->m_noutputs;

  if(target.size() != n) {
    throwException(_T("%s:Wrong number of target values=%zu. No. of output units=%d")
                  ,__TFUNCTION__, target.size(), n);
  }
  for(int i = 0; i < n; i++ ) {
    errors[i] = ((float)(target[i]) - outputs[i]) * outputs[i] * (1.0F - outputs[i]);
  }
}

static void layerBackPropagateError(BpnLayer *upper, BpnLayer *lower) {
  float  *uppererror = upper->m_errors;
  int     uppercount = upper->m_noutputs;
  float  *lowererror = lower->m_errors;
  int     lowercount = lower->m_noutputs;

  for(int i = 0; i < lowercount; i++ ) {
    float errorsum = 0;
    for(int j = 0; j < uppercount; j++ ) {
      errorsum += uppererror[j] * upper->m_weights[j][i];
    }
    float unit = lower->m_outputs[i];
    lowererror[i] = unit * (1.0F - unit) * errorsum;
  }
}

void Bpn::backPropagateError( ) {
  layerBackPropagateError( m_outunits, m_hiddenlayers[m_nhiddenlayers-1] );
  for(int i = m_nhiddenlayers - 1; i > 0; i-- )
    layerBackPropagateError( m_hiddenlayers[i], m_hiddenlayers[i-1] );
}

static void layerAdjustWeights( BpnLayer *upper, BpnLayer *lower,
                                double momentum,
                                double learning_rate  ) {
  float *uppererror = upper->m_errors;
  int    uppercount = upper->m_noutputs;
  int    lowercount = lower->m_noutputs;
  float *inputs     = lower->m_outputs;
  float  moment     = momentum;
  for(int j = 0; j < uppercount; j++ ) {
    float *weights   = upper->m_weights[j];
    float *lastdelta = upper->m_lastdelta[j];
    float tmp        = (float)learning_rate * uppererror[j];

#if !defined(ASM_OPTIMIZED)
    for(int k = 0; k < lowercount; k++ ) {
      lastdelta[k] *= (float)momentum;
      lastdelta[k] += inputs[k] * tmp;
      weights[k]   += lastdelta[k];
    }
#else
    __asm {
        mov ecx,lowercount
        mov eax,inputs
        mov ebx,weights
        mov edx,lastdelta
startloop:
        cmp ecx,0
        je exitloop
        fld  dword ptr [ebx]
        fld  dword ptr [edx]
        fmul dword ptr [moment]
        fld  dword ptr [tmp]
        fmul dword ptr [eax]
        fadd
        fst  dword ptr [edx]
        fadd
        fstp dword ptr [ebx]

        sub ecx,1
        add eax,4
        add ebx,4
        add edx,4
        jmp startloop
exitloop:
    }
#endif

    if(upper->m_bias) {
      upper->m_biasweights[j] += tmp;
    }
  }
}

void Bpn::adjustWeights() {
  layerAdjustWeights(m_outunits, m_hiddenlayers[m_nhiddenlayers-1], m_momentum, m_learningrate);
  for(int i = m_nhiddenlayers - 1 ; i > 0; i--) {
    layerAdjustWeights(m_hiddenlayers[i], m_hiddenlayers[i-1], m_momentum, m_learningrate);
  }
  layerAdjustWeights(m_hiddenlayers[0], m_inunits, m_momentum, m_learningrate);
  m_bpndraw->drawWeights(this);
}

void BpnLayer::allocate(LayerType type, int noutputs, int ninputs, bool bias) {
  m_noutputs = noutputs;
  m_ninputs  = ninputs;
  m_outputs  = new float[m_noutputs]; TRACE_NEW(m_outputs);
  m_type     = type;
  m_bias     = bias;

  if(type != LAYER_INPUT) {
    m_errors    = new float[ m_noutputs]; TRACE_NEW(m_errors   );
    m_weights   = new float*[m_noutputs]; TRACE_NEW(m_weights  );
    m_lastdelta = new float*[m_noutputs]; TRACE_NEW(m_lastdelta);
    for(int i = 0; i < m_noutputs; i++ ) {
      m_weights[i]   = new float[m_ninputs]; TRACE_NEW(m_weights[i]);
      m_lastdelta[i] = new float[m_ninputs]; TRACE_NEW(m_lastdelta[i]);
    }
  }
  if(m_bias) {
    m_biasweights = new float[m_noutputs]; TRACE_NEW(m_biasweights);
  }
}

void BpnLayer::deallocate() {
  SAFEDELETEARRAY(m_outputs);

  if(m_type != LAYER_INPUT) {
    for(int i = 0; i < m_noutputs; i++) {
      SAFEDELETEARRAY(m_weights[i]  );
      SAFEDELETEARRAY(m_lastdelta[i]);
    }
    SAFEDELETEARRAY(m_weights  );
    SAFEDELETEARRAY(m_lastdelta);
    SAFEDELETEARRAY(m_errors   );
  }
  if(m_bias) {
    SAFEDELETEARRAY(m_biasweights);
  }
}

BpnLayer::BpnLayer(LayerType type, int noutputs, int ninputs, bool bias) {
  allocate(type,noutputs,ninputs,bias);

  if(m_type != LAYER_INPUT) {
    for(int i = 0; i < m_noutputs; i++) {
      randomVector(m_weights[i]  , m_ninputs);
      randomVector(m_lastdelta[i], m_ninputs);
    }
  }
  if(m_bias) {
    randomVector(m_biasweights, m_noutputs);
  }
}

void BpnLayer::copyVectors(const BpnLayer &src) {
  if(m_type != LAYER_INPUT) {
    for(int i = 0; i < m_noutputs; i++ ) {
      copyVector(m_weights[i]  , src.m_weights[i]  , m_ninputs);
      copyVector(m_lastdelta[i], src.m_lastdelta[i], m_ninputs);
    }
  }
  if(m_bias) {
    copyVector( m_biasweights, src.m_biasweights, m_noutputs );
  }
}

BpnLayer::BpnLayer(const BpnLayer &src) {
  allocate(src.m_type, src.m_noutputs, src.m_ninputs, src.m_bias);
  copyVectors(src);
}

BpnLayer::~BpnLayer() {
  deallocate();
}

BpnLayer &BpnLayer::operator=(const BpnLayer &rhs) {
  if(this == &rhs) {
    return *this;
  }
  if(rhs.m_ninputs  != m_ninputs || rhs.m_noutputs != m_noutputs || rhs.m_bias != m_bias) {
    deallocate();
    allocate(rhs.m_type,rhs.m_noutputs,rhs.m_ninputs,rhs.m_bias);
  }
  copyVectors(rhs);
  return *this;
}

void BpnLayer::load(FILE *f) {
  for(int i = 0; i < m_noutputs; i++ ) {
    loadVector(f,m_weights[i]  ,m_ninputs);
    loadVector(f,m_lastdelta[i],m_ninputs);
  }
  if(m_bias) {
    loadVector(f,m_biasweights,m_noutputs);
  }
}

void BpnLayer::save(FILE *f) const {
  for(int i = 0; i < m_noutputs; i++ ) {
    writeVector(f,m_weights[i]  ,m_ninputs);
    writeVector(f,m_lastdelta[i],m_ninputs);
  }
  if(m_bias) {
    writeVector(f,m_biasweights,m_noutputs);
  }
}

void Bpn::setInput(const CompactDoubleArray &in) {
  const int n   = m_inunits->m_noutputs;
  if(in.size() != n) {
    throwException(_T("%s:Wrong number of input-values=%zu. No. of input units=%d")
                  ,__TFUNCTION__, in.size(),n);
  }
  float *inputs = m_inunits->m_outputs;
  for(int i = 0; i < n; i++ ) {
    inputs[i] = (float)(in[i]);
  }
}

CompactDoubleArray &Bpn::getOutput(CompactDoubleArray &out) const {
  int    n       = m_outunits->m_noutputs;
  float *outputs = m_outunits->m_outputs;
  out.clear();
  for(int i = 0; i < n; i++) {
    out.add(outputs[i]);
  }
  return out;
}

void Bpn::learnLast(const CompactDoubleArray &output) {
  computeOutputError(output);
  backPropagateError();
  adjustWeights();
}

void Bpn::learn(const CompactDoubleArray &input, const CompactDoubleArray &output) {
  setInput(input);
  propagateForward();
  learnLast(output);
}

CompactDoubleArray &Bpn::recognize(const CompactDoubleArray &input, CompactDoubleArray &output) {
  setInput(input);
  propagateForward();
  return getOutput(output);
}

double Bpn::getPatternError(const CompactDoubleArray &target) const {
  const float *outputs = m_outunits->m_outputs;
  const int    n       = m_outunits->m_noutputs;
  double       sum     = 0.0;

  if(target.size() != n) {
    throwException(_T("%s:Wrong number of target values=%zu. No. of output units=%d")
                  ,__TFUNCTION__, target.size(), n);
  }

  for(int  i = 0; i < n; i++ ) {
    double temp = target[i] - outputs[i];
    sum += temp * temp;
  }
  return sum/2.0;
}

void Bpn::allocateLayers(const CompactIntArray &layerunits, bool bias) {
  DEFINEMETHODNAME;
  const int layerCount = (int)layerunits.size();
  if(layerCount > MAXLAYERCOUNT) {
    throwException(_T("%s called with %d layers. max=%d"), method, layerCount, MAXLAYERCOUNT);
  }
  if(layerunits.size() < 3) {
    throwException(_T("%s called with %d layers. min=3"), method, layerCount);
  }
  m_nhiddenlayers = layerCount - 2;
  m_bias          = bias;
  m_inunits       = new BpnLayer(LAYER_INPUT,layerunits[0], 0, false ); TRACE_NEW(m_inunits     ); // no bias weights on inputlayer
  m_hiddenlayers  = new BpnLayer*[m_nhiddenlayers];                     TRACE_NEW(m_hiddenlayers);
  for(int i = 0; i < m_nhiddenlayers; i++) {
    m_hiddenlayers[i] = new BpnLayer(LAYER_HIDDEN,layerunits[i+1],layerunits[i], bias); TRACE_NEW(m_hiddenlayers[i]);
  }
  m_outunits = new BpnLayer(LAYER_OUTPUT,layerunits[layerCount-1], layerunits[layerCount-2], bias); TRACE_NEW(m_outunits);
}

CompactIntArray Bpn::getLayerUnits() const {
  CompactIntArray result;
  if(m_inunits == 0) {
    return result;
  }
  result.add(m_inunits->size());
  for(int i = 0; i < m_nhiddenlayers; i++) {
    result.add(m_hiddenlayers[i]->size());
  }
  result.add(m_outunits->size());
  return result;
}

void Bpn::copyLayers(const Bpn &src) {
  if(src.m_inunits == nullptr) {
    return; // nothing to copy
  }
  *m_inunits = *src.m_inunits;
  for(int i = 0; i < src.m_nhiddenlayers; i++) {
    *m_hiddenlayers[i] = *src.m_hiddenlayers[i];
  }
  *m_outunits = *src.m_outunits;
}

void Bpn::initLayers() {
  m_inunits      = nullptr;
  m_hiddenlayers = nullptr;
  m_outunits     = nullptr;
}

void Bpn::deallocateLayers() {
  SAFEDELETE(m_inunits);
  for( int i = 0; i < m_nhiddenlayers; i++ ) {
    SAFEDELETE(m_hiddenlayers[i]);
  }
  SAFEDELETEARRAY(m_hiddenlayers);
  SAFEDELETE(m_outunits);
  initLayers();
}

void Bpn::save(const String &fileName) const {
  save(fileName.cstr());
}

void Bpn::save(const TCHAR *fileName) const {
  FILE *f = MKFOPEN(fileName, "w");
  try {
    save(f);
    fclose(f);
  } catch(...) {
    fclose(f);
    throw;
  }
}

void Bpn::save(FILE *f) const {
  _ftprintf(f, _T("%c "), m_bias ? 'B' : 'U' );
  _ftprintf(f, _T("%d "), m_inunits->size());
  for(int i = 0; i < m_nhiddenlayers; i++) {
    _ftprintf(f, _T("%d "), m_hiddenlayers[i]->size());
  }
  _ftprintf(f, _T("%d "), m_outunits->size());
  _ftprintf(f, _T("0 \n"));

  for(int i = 0; i < m_nhiddenlayers; i++) {
    m_hiddenlayers[i]->save(f);
  }
  m_outunits->save(f);
}

void Bpn::load(FILE *f) {
  DEFINEMETHODNAME;
  char biaschar;
  if(fscanf(f, "%c",&biaschar) != 1) {
    throwException(_T("%s:Expected biaschar"), method);
  }
  if(biaschar == 'B') {
    m_bias = true;
  } else if(biaschar == 'U') {
    m_bias = false;
  } else {
    throwException(_T("%s:Expected biaschar B/U"), method);
  }

  CompactIntArray nunits;
  for(int i = 0;; i++ ) {
    if(i > MAXLAYERCOUNT) {
      throwException(_T("%s:nhiddenlayers = %d too big (max = %d)"), method, i, MAXLAYERCOUNT);
    }
    int units;

    if(fscanf(f, "%u", &units) != 1 ) {
      throwException(_T("%s:Illegal input:layerunits"), method);
    }
    if(units == 0) {
      break;
    } else {
      nunits.add(units);
    }
  }
  if(m_inunits != nullptr) {
    deallocateLayers();
  }
  allocateLayers(nunits,m_bias);

  for(int i = 0; i < m_nhiddenlayers; i++) {
    m_hiddenlayers[i]->load(f);
  }
  m_outunits->load(f);
}

void Bpn::load(const String &fileName) {
  load(fileName.cstr());
}

void Bpn::load(const TCHAR *fileName) {
  FILE *f = FOPEN(fileName, "r");
  try {
    load(f);
    fclose(f);
  } catch(...) {
    fclose(f);
    throw;
  }
}

Bpn::Bpn(const Bpn &src) {
  CompactIntArray layerunits = src.getLayerUnits();
  initLayers();
  setGraphic(src.m_bpndraw);
  m_momentum     = src.m_momentum;
  m_learningrate = src.m_learningrate;
  if(layerunits.size() > 0) {
    allocateLayers(layerunits,src.m_bias);
    copyLayers(src);
  }
}

Bpn::Bpn(const CompactIntArray &layerunits, bool bias) {
  initLayers();
  setGraphic(BPN_NODISP);
  m_learningrate  = 0.2F;
  m_momentum      = 0.9F;
  allocateLayers(layerunits,bias);
}

Bpn::Bpn(char *fname) {
  initLayers();
  setGraphic(BPN_NODISP);
  m_learningrate  = 0.2F;
  m_momentum      = 0.9F;
  load(fname);
  m_bpndraw->drawWeights(this);
}

Bpn::~Bpn() {
  deallocateLayers();
}

Bpn &Bpn::operator=(const Bpn &rhs) {
  if(this == &rhs) {
    return *this;
  }
  deallocateLayers();
  CompactIntArray layerunits = rhs.getLayerUnits();
  setGraphic( rhs.m_bpndraw);
  m_momentum     = rhs.m_momentum;
  m_learningrate = rhs.m_learningrate;
  if(layerunits.size() > 0) {
    allocateLayers(layerunits,rhs.m_bias);
    copyLayers(rhs);
  }
  return *this;
}

BpnGraphic *Bpn::setGraphic(BpnGraphic *gt) {
  BpnGraphic *old = m_bpndraw;
  m_bpndraw = gt;
  return old;
}

void BpnLayer::list() const {
  for(int i = 0; i < m_noutputs; i++) {
    _tprintf(_T("output:%lg\n"), m_outputs[i]);
    for(int j = 0; j < m_ninputs; j++) {
      _tprintf(_T("    Weight: %e %e\n"), m_weights[i][j], m_lastdelta[i][j]);
    }
  }
}

void Bpn::list() const {

  _tprintf( _T("----------------- BPN_LIST --------------\n") );

  m_inunits->list();
  m_hiddenlayers[0]->list();
  m_outunits->list();

  _tprintf( _T("-----------------------------------------\n") );
}

void BpnGraphic::drawWeights(Bpn *b) {
}

void BpnGraphic::drawUnits(Bpn *b) {
}

static BpnGraphic BPNNODISP;
BpnGraphic *BPN_NODISP = &BPNNODISP;
BpnGraphic *BPN_DISP   = &BPNNODISP;
BpnGraphic *BPN_GRAFIK = &BPNNODISP;
