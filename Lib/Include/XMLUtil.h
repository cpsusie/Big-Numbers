#pragma once

#include "XmlDoc.h"
#include "NumberInterval.h"

class Date;
class Time;
class Timestamp;

void setValue(XMLDoc &doc, const XMLNodePtr &n, const Date      &d);
void getValue(XMLDoc &doc, const XMLNodePtr &n,       Date      &d);

void setValue(XMLDoc &doc, const XMLNodePtr &n, const Time      &t);
void setValue(XMLDoc &doc, const XMLNodePtr &n,       Time      &t);

void setValue(XMLDoc &doc, const XMLNodePtr &n, const Timestamp &t);
void getValue(XMLDoc &doc, const XMLNodePtr &n,       Timestamp &t);

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const NumberInterval<T> &interval) {
  setValue(doc, n, _T("from"), interval.getFrom());
  setValue(doc, n, _T("to"  ), interval.getTo());
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, NumberInterval<T> &interval) {
  T from, to;
  getValue(doc, n, _T("from"), from);
  getValue(doc, n, _T("to"  ), to  );
  interval.setFrom(from);
  interval.setTo(to);
}

class BitSet;

void setValue(XMLDoc &doc, XMLNodePtr n, const BitSet &set);
void getValue(XMLDoc &doc, XMLNodePtr n,       BitSet &set);
