#if !defined(MYLIB_CONSTANTS_H)
#define MYLIB_CONSTANTS_H 1


#include "TString.h"


const float lumi50ns_fb = 0.040;
const float lumi25ns_fb = 1.269;


const int ELECTRON_FLAVOUR = 11;
const int MUON_FLAVOUR     = 13;
const int TAU_FLAVOUR      = 15;
const int Z_FLAVOUR        = 23;

const float ELECTRON_MASS =  0.000511;  // [GeV]
const float MUON_MASS     =  0.106;     // [GeV]
const float TAU_MASS      =  1.777;     // [GeV]
const float Z_MASS        = 91.188;     // [GeV]


const int njetbin = 3;  // 0jet, 1jet, 2+jet, all


const int nchannel = 9;

enum {
  ee,
  em,
  mm,
  ll,
  eee,
  eem,
  emm,
  mmm,
  lll
};

const TString schannel[nchannel] = {
  "ee",
  "em",
  "mm",
  "ll",
  "eee",
  "eem",
  "emm",
  "mmm",
  "lll"
};

TString lchannel[nchannel] = {
  "ee",
  "e#mu",
  "#mu#mu",
  "ll",
  "eee",
  "ee#mu",
  "e#mu#mu",
  "#mu#mu#mu",
  "lll"
};


const int ncut = 10;

enum {
  WW00_Exactly2Leptons,
  WW01_ZVeto,
  WW02_BVeto,
  WZ00_Exactly3Leptons,
  WZ01_HasZ,
  WZ02_HasW,
  WZ03_BVeto,
  Top00_Has2Leptons,
  Top01_Has2Jets,
  Top02_Has1BJet
};

const TString scut[ncut] = {
  "WW00_Exactly2Leptons",
  "WW01_ZVeto",
  "WW02_BVeto",
  "WZ00_Exactly3Leptons",
  "WZ01_HasZ",
  "WZ02_HasW",
  "WZ03_BVeto",
  "Top00_Has2Leptons",
  "Top01_Has2Jets",
  "Top02_Has1BJet"
};


// https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideBTagging#Preliminary_working_or_operating
const float csvv2ivf_looseWP  = 0.423;
const float csvv2ivf_mediumWP = 0.814;
const float csvv2ivf_tightWP  = 0.941;


#endif
