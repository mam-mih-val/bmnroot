/********************************************************************
* /home/merz/BMN/GIT/trunk_220814/build/base/G__BaseDict.h
* CAUTION: DON'T CHANGE THIS FILE. THIS FILE IS AUTOMATICALLY GENERATED
*          FROM HEADER FILES LISTED IN G__setup_cpp_environmentXXX().
*          CHANGE THOSE HEADER FILES AND REGENERATE THIS FILE.
********************************************************************/
#ifdef __CINT__
#error /home/merz/BMN/GIT/trunk_220814/build/base/G__BaseDict.h/C is only for compilation. Abort cint.
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define G__ANSIHEADER
#define G__DICTIONARY
#define G__PRIVATE_GVALUE
#include "G__ci.h"
#include "FastAllocString.h"
extern "C" {
extern void G__cpp_setup_tagtableG__BaseDict();
extern void G__cpp_setup_inheritanceG__BaseDict();
extern void G__cpp_setup_typetableG__BaseDict();
extern void G__cpp_setup_memvarG__BaseDict();
extern void G__cpp_setup_globalG__BaseDict();
extern void G__cpp_setup_memfuncG__BaseDict();
extern void G__cpp_setup_funcG__BaseDict();
extern void G__set_cpp_environmentG__BaseDict();
}


#include "TObject.h"
#include "TMemberInspector.h"
#include "FairBaseContFact.h"
#include "FairBaseParSet.h"
#include "FairLink.h"
#include "FairMultiLinkedData.h"
#include "FairDetector.h"
#include "FairEventHeader.h"
#include "FairFileHeader.h"
#include "FairGenerator.h"
#include "FairHit.h"
#include "FairIon.h"
#include "FairMCApplication.h"
#include "FairGeaneApplication.h"
#include "FairMCEventHeader.h"
#include "FairMCPoint.h"
#include "FairModule.h"
#include "FairParticle.h"
#include "FairPrimaryGenerator.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRunAna.h"
#include "FairRunIdGenerator.h"
#include "FairRunSim.h"
#include "FairGenericStack.h"
#include "FairTask.h"
#include "FairTrackParam.h"
#include "FairTrajFilter.h"
#include "FairVolume.h"
#include "FairVolumeList.h"
#include "FairField.h"
#include "FairFieldFactory.h"
#include "FairRadLenPoint.h"
#include "FairRadMapManager.h"
#include "FairRadMapPoint.h"
#include "FairRadLenManager.h"
#include "FairRadGridManager.h"
#include "FairMesh.h"
#include "FairRKPropagator.h"
#include "FairTimeStamp.h"
#include "FairTSBufferFunctional.h"
#include "FairFileInfo.h"
#include "FairRunInfo.h"
#include "FairWriteoutBuffer.h"
#include "FairRingSorter.h"
#include "FairRingSorterTask.h"
#include "FairAnaSelector.h"
#include <algorithm>
namespace std { }
using namespace std;

#ifndef G__MEMFUNCBODY
#endif

extern G__linked_taginfo G__G__BaseDictLN_TClass;
extern G__linked_taginfo G__G__BaseDictLN_TBuffer;
extern G__linked_taginfo G__G__BaseDictLN_TMemberInspector;
extern G__linked_taginfo G__G__BaseDictLN_TObject;
extern G__linked_taginfo G__G__BaseDictLN_TNamed;
extern G__linked_taginfo G__G__BaseDictLN_TCollection;
extern G__linked_taginfo G__G__BaseDictLN_TString;
extern G__linked_taginfo G__G__BaseDictLN_vectorlElongcOallocatorlElonggRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_vectorlEunsignedsPintcOallocatorlEunsignedsPintgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_vectorlEdoublecOallocatorlEdoublegRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_basic_ostreamlEcharcOchar_traitslEchargRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_string;
extern G__linked_taginfo G__G__BaseDictLN_vectorlEROOTcLcLTSchemaHelpercOallocatorlEROOTcLcLTSchemaHelpergRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_reverse_iteratorlEvectorlEROOTcLcLTSchemaHelpercOallocatorlEROOTcLcLTSchemaHelpergRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__BaseDictLN_TList;
extern G__linked_taginfo G__G__BaseDictLN_TObjArray;
extern G__linked_taginfo G__G__BaseDictLN_TClonesArray;
extern G__linked_taginfo G__G__BaseDictLN_vectorlETVirtualArraymUcOallocatorlETVirtualArraymUgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_reverse_iteratorlEvectorlETVirtualArraymUcOallocatorlETVirtualArraymUgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__BaseDictLN_FairLogger;
extern G__linked_taginfo G__G__BaseDictLN_TIterator;
extern G__linked_taginfo G__G__BaseDictLN_iteratorlEbidirectional_iterator_tagcOTObjectmUcOlongcOconstsPTObjectmUmUcOconstsPTObjectmUaNgR;
extern G__linked_taginfo G__G__BaseDictLN_FairParSet;
extern G__linked_taginfo G__G__BaseDictLN_FairContainer;
extern G__linked_taginfo G__G__BaseDictLN_FairContFact;
extern G__linked_taginfo G__G__BaseDictLN_FairBaseContFact;
extern G__linked_taginfo G__G__BaseDictLN_FairParamList;
extern G__linked_taginfo G__G__BaseDictLN_FairParGenericSet;
extern G__linked_taginfo G__G__BaseDictLN_maplEintcOintcOlesslEintgRcOallocatorlEpairlEconstsPintcOintgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_maplEintcOintcOlesslEintgRcOallocatorlEpairlEconstsPintcOintgRsPgRsPgRcLcLiterator;
extern G__linked_taginfo G__G__BaseDictLN_maplElongcOintcOlesslElonggRcOallocatorlEpairlEconstsPlongcOintgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_maplEintcOTGeoElementRNmUcOlesslEintgRcOallocatorlEpairlEconstsPintcOTGeoElementRNmUgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_TGeoMatrix;
extern G__linked_taginfo G__G__BaseDictLN_TGeoVolume;
extern G__linked_taginfo G__G__BaseDictLN_TGeoNode;
extern G__linked_taginfo G__G__BaseDictLN_TGeoManager;
extern G__linked_taginfo G__G__BaseDictLN_vectorlETGeoVolumeAssemblycLcLThreadData_tmUcOallocatorlETGeoVolumeAssemblycLcLThreadData_tmUgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_reverse_iteratorlEvectorlETGeoVolumeAssemblycLcLThreadData_tmUcOallocatorlETGeoVolumeAssemblycLcLThreadData_tmUgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__BaseDictLN_maplElongcOTGeoNavigatorArraymUcOlesslElonggRcOallocatorlEpairlEconstsPlongcOTGeoNavigatorArraymUgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_FairPrimaryGenerator;
extern G__linked_taginfo G__G__BaseDictLN_FairBaseParSet;
extern G__linked_taginfo G__G__BaseDictLN_FairLink;
extern G__linked_taginfo G__G__BaseDictLN_TArrayI;
extern G__linked_taginfo G__G__BaseDictLN_TFile;
extern G__linked_taginfo G__G__BaseDictLN_TMap;
extern G__linked_taginfo G__G__BaseDictLN_TTree;
extern G__linked_taginfo G__G__BaseDictLN_TBranch;
extern G__linked_taginfo G__G__BaseDictLN_maplEstringcOTObjArraymUcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOTObjArraymUgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_TSelector;
extern G__linked_taginfo G__G__BaseDictLN_TChain;
extern G__linked_taginfo G__G__BaseDictLN_FairGeoNode;
extern G__linked_taginfo G__G__BaseDictLN_FairFileHeader;
extern G__linked_taginfo G__G__BaseDictLN_FairMCEventHeader;
extern G__linked_taginfo G__G__BaseDictLN_FairEventHeader;
extern G__linked_taginfo G__G__BaseDictLN_FairTSBufferFunctional;
extern G__linked_taginfo G__G__BaseDictLN_BinaryFunctor;
extern G__linked_taginfo G__G__BaseDictLN_FairWriteoutBuffer;
extern G__linked_taginfo G__G__BaseDictLN_TFolder;
extern G__linked_taginfo G__G__BaseDictLN_TF1;
extern G__linked_taginfo G__G__BaseDictLN_FairRootManager;
extern G__linked_taginfo G__G__BaseDictLN_maplETStringcOTObjectmUcOlesslETStringgRcOallocatorlEpairlEconstsPTStringcOTObjectmUgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_dequelETClonesArraymUcOallocatorlETClonesArraymUgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_queuelETClonesArraymUcOdequelETClonesArraymUcOallocatorlETClonesArraymUgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_maplETStringcOqueuelETClonesArraymUcOdequelETClonesArraymUcOallocatorlETClonesArraymUgRsPgRsPgRcOlesslETStringgRcOallocatorlEpairlEconstsPTStringcOqueuelETClonesArraymUcOdequelETClonesArraymUcOallocatorlETClonesArraymUgRsPgRsPgRsPgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_maplETStringcOTClonesArraymUcOlesslETStringgRcOallocatorlEpairlEconstsPTStringcOTClonesArraymUgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_maplETStringcOFairTSBufferFunctionalmUcOlesslETStringgRcOallocatorlEpairlEconstsPTStringcOFairTSBufferFunctionalmUgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_maplETStringcOFairWriteoutBuffermUcOlesslETStringgRcOallocatorlEpairlEconstsPTStringcOFairWriteoutBuffermUgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_maplEintcOTBranchmUcOlesslEintgRcOallocatorlEpairlEconstsPintcOTBranchmUgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_maplETStringcOintcOlesslETStringgRcOallocatorlEpairlEconstsPTStringcOintgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_maplETStringcOintcOlesslETStringgRcOallocatorlEpairlEconstsPTStringcOintgRsPgRsPgRcLcLiterator;
extern G__linked_taginfo G__G__BaseDictLN_listlETStringcOallocatorlETStringgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_maplETStringcOTChainmUcOlesslETStringgRcOallocatorlEpairlEconstsPTStringcOTChainmUgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_maplETStringcOlistlETStringcOallocatorlETStringgRsPgRmUcOlesslETStringgRcOallocatorlEpairlEconstsPTStringcOlistlETStringcOallocatorlETStringgRsPgRmUgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_multimaplETStringcOTArrayIcOlesslETStringgRcOallocatorlEpairlEconstsPTStringcOTArrayIgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_maplETStringcOmultimaplETStringcOTArrayIcOlesslETStringgRcOallocatorlEpairlEconstsPTStringcOTArrayIgRsPgRsPgRcOlesslETStringgRcOallocatorlEpairlEconstsPTStringcOmultimaplETStringcOTArrayIcOlesslETStringgRcOallocatorlEpairlEconstsPTStringcOTArrayIgRsPgRsPgRsPgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_maplEunsignedsPintcOTChainmUcOlesslEunsignedsPintgRcOallocatorlEpairlEconstsPunsignedsPintcOTChainmUgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_maplEunsignedsPintcOdoublecOlesslEunsignedsPintgRcOallocatorlEpairlEconstsPunsignedsPintcOdoublegRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_maplEunsignedsPintcOunsignedsPintcOlesslEunsignedsPintgRcOallocatorlEpairlEconstsPunsignedsPintcOunsignedsPintgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_FairMultiLinkedData;
extern G__linked_taginfo G__G__BaseDictLN_setlEFairLinkcOlesslEFairLinkgRcOallocatorlEFairLinkgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_vectorlEintcOallocatorlEintgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_reverse_iteratorlEvectorlEintcOallocatorlEintgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__BaseDictLN_TRefArray;
extern G__linked_taginfo G__G__BaseDictLN_FairVolumeList;
extern G__linked_taginfo G__G__BaseDictLN_FairVolume;
extern G__linked_taginfo G__G__BaseDictLN_FairModule;
extern G__linked_taginfo G__G__BaseDictLN_FairDetector;
extern G__linked_taginfo G__G__BaseDictLN_FairFileInfo;
extern G__linked_taginfo G__G__BaseDictLN_FairGenerator;
extern G__linked_taginfo G__G__BaseDictLN_FairTimeStamp;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTBaselEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTBaselEdoublegR;
extern G__linked_taginfo G__G__BaseDictLN_TVectorTlEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TVectorTlEdoublegR;
extern G__linked_taginfo G__G__BaseDictLN_TElementActionTlEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TElementPosActionTlEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTlEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTSymlEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTRow_constlEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTRowlEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTDiag_constlEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTColumn_constlEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTFlat_constlEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTSub_constlEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTSparseRow_constlEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTSparseDiag_constlEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTColumnlEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTDiaglEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTFlatlEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTSublEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTSparseRowlEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TMatrixTSparseDiaglEfloatgR;
extern G__linked_taginfo G__G__BaseDictLN_TVector3;
extern G__linked_taginfo G__G__BaseDictLN_FairHit;
extern G__linked_taginfo G__G__BaseDictLN_FairIon;
extern G__linked_taginfo G__G__BaseDictLN_TVirtualMCApplication;
extern G__linked_taginfo G__G__BaseDictLN_TLorentzVector;
extern G__linked_taginfo G__G__BaseDictLN_TTask;
extern G__linked_taginfo G__G__BaseDictLN_InitStatus;
extern G__linked_taginfo G__G__BaseDictLN_FairTask;
extern G__linked_taginfo G__G__BaseDictLN_vectorlETStringcOallocatorlETStringgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_reverse_iteratorlEvectorlETStringcOallocatorlETStringgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__BaseDictLN_CpuInfo_t;
extern G__linked_taginfo G__G__BaseDictLN_MemInfo_t;
extern G__linked_taginfo G__G__BaseDictLN_ProcInfo_t;
extern G__linked_taginfo G__G__BaseDictLN_TTimeStamp;
extern G__linked_taginfo G__G__BaseDictLN_FairRunInfo;
extern G__linked_taginfo G__G__BaseDictLN_FairGenericStack;
extern G__linked_taginfo G__G__BaseDictLN_FairField;
extern G__linked_taginfo G__G__BaseDictLN_FairTrajFilter;
extern G__linked_taginfo G__G__BaseDictLN_FairRadLenManager;
extern G__linked_taginfo G__G__BaseDictLN_FairRadGridManager;
extern G__linked_taginfo G__G__BaseDictLN_FairRadMapManager;
extern G__linked_taginfo G__G__BaseDictLN_FairMCApplication;
extern G__linked_taginfo G__G__BaseDictLN_multimaplEintcOFairVolumemUcOlesslEintgRcOallocatorlEpairlEconstsPintcOFairVolumemUgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_multimaplEintcOFairVolumemUcOlesslEintgRcOallocatorlEpairlEconstsPintcOFairVolumemUgRsPgRsPgRcLcLiterator;
extern G__linked_taginfo G__G__BaseDictLN_FairGeaneApplication;
extern G__linked_taginfo G__G__BaseDictLN_FairMCPoint;
extern G__linked_taginfo G__G__BaseDictLN_TRef;
extern G__linked_taginfo G__G__BaseDictLN_TMCParticleType;
extern G__linked_taginfo G__G__BaseDictLN_TParticle;
extern G__linked_taginfo G__G__BaseDictLN_FairParticle;
extern G__linked_taginfo G__G__BaseDictLN_FairRuntimeDb;
extern G__linked_taginfo G__G__BaseDictLN_FairRun;
extern G__linked_taginfo G__G__BaseDictLN_TProof;
extern G__linked_taginfo G__G__BaseDictLN_maplETStringcOTProofcLcLMD5Mod_tcOlesslETStringgRcOallocatorlEpairlEconstsPTStringcOTProofcLcLMD5Mod_tgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_FairRunAna;
extern G__linked_taginfo G__G__BaseDictLN_FairRunIdGenerator;
extern G__linked_taginfo G__G__BaseDictLN_FairRunIdGeneratorcLcLuuid;
extern G__linked_taginfo G__G__BaseDictLN_FairMesh;
extern G__linked_taginfo G__G__BaseDictLN_FairRunSim;
extern G__linked_taginfo G__G__BaseDictLN_TMCProcess;
extern G__linked_taginfo G__G__BaseDictLN_TVirtualMCStack;
extern G__linked_taginfo G__G__BaseDictLN_FairTrackParam;
extern G__linked_taginfo G__G__BaseDictLN_TGeoTrack;
extern G__linked_taginfo G__G__BaseDictLN_TVirtualMagField;
extern G__linked_taginfo G__G__BaseDictLN_FairFieldFactory;
extern G__linked_taginfo G__G__BaseDictLN_FairRadLenPoint;
extern G__linked_taginfo G__G__BaseDictLN_FairRadMapPoint;
extern G__linked_taginfo G__G__BaseDictLN_TH2D;
extern G__linked_taginfo G__G__BaseDictLN_FairRKPropagator;
extern G__linked_taginfo G__G__BaseDictLN_vectorlEFairTimeStampmUcOallocatorlEFairTimeStampmUgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_reverse_iteratorlEvectorlEFairTimeStampmUcOallocatorlEFairTimeStampmUgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__BaseDictLN_pairlEdoublecOFairTimeStampmUgR;
extern G__linked_taginfo G__G__BaseDictLN_vectorlEpairlEdoublecOFairTimeStampmUgRcOallocatorlEpairlEdoublecOFairTimeStampmUgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_reverse_iteratorlEvectorlEpairlEdoublecOFairTimeStampmUgRcOallocatorlEpairlEdoublecOFairTimeStampmUgRsPgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__BaseDictLN_multimaplEdoublecOpairlEdoublecOFairTimeStampmUgRcOlesslEdoublegRcOallocatorlEpairlEconstsPdoublecOpairlEdoublecOFairTimeStampmUgRsPgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_multimaplEdoublecOFairTimeStampmUcOlesslEdoublegRcOallocatorlEpairlEconstsPdoublecOFairTimeStampmUgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_FairRingSorter;
extern G__linked_taginfo G__G__BaseDictLN_vectorlEmultimaplEdoublecOFairTimeStampmUcOlesslEdoublegRcOallocatorlEpairlEconstsPdoublecOFairTimeStampmUgRsPgRsPgRcOallocatorlEmultimaplEdoublecOFairTimeStampmUcOlesslEdoublegRcOallocatorlEpairlEconstsPdoublecOFairTimeStampmUgRsPgRsPgRsPgRsPgR;
extern G__linked_taginfo G__G__BaseDictLN_reverse_iteratorlEvectorlEmultimaplEdoublecOFairTimeStampmUcOlesslEdoublegRcOallocatorlEpairlEconstsPdoublecOFairTimeStampmUgRsPgRsPgRcOallocatorlEmultimaplEdoublecOFairTimeStampmUcOlesslEdoublegRcOallocatorlEpairlEconstsPdoublecOFairTimeStampmUgRsPgRsPgRsPgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__BaseDictLN_pairlEintcOdoublegR;
extern G__linked_taginfo G__G__BaseDictLN_FairRingSorterTask;
extern G__linked_taginfo G__G__BaseDictLN_TProofOutputFile;
extern G__linked_taginfo G__G__BaseDictLN_FairAnaSelector;

/* STUB derived class for protected member access */
