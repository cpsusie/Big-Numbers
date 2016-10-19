#include "stdafx.h"
#include "Chess.h"

Options &OptionsAccessor::getOptions() {
  return theApp.m_options;
}

const Options &OptionsAccessor::getOptions() const {
  return theApp.m_options;
}
