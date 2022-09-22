// $Id: PhysicsLinkDef.h,v
#ifdef __CLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// decay
#pragma link C++ class BmnTwoParticleDecay++;
#pragma link C++ class BmnParticlePair++;
#pragma link C++ class BmnParticlePairCut++;
#pragma link C++ class BmnParticlePairsInfo++;
#pragma link C++ class BmnMassSpectrumAnal++;
#pragma link C++ class DstEventHeaderExtended+;

// embedding 
#pragma link C++ class BmnLambdaEmbeddingMonitor+;
#pragma link C++ class BmnParticleStore+;
#pragma link C++ class BmnLambdaEmbeddingQa+;
#pragma link C++ class BmnLambdaEmbeddingDrawHistos+;
#pragma link C++ class BmnLambdaMisc+;
#pragma link C++ class BmnInnerTrackerGeometryDraw+;
#pragma link C++ class BmnLambdaEmbedding+;

// efficiency 
#pragma link C++ class BmnDataTriggerInfo+;
#pragma link C++ class BmnEfficiency+;
#pragma link C++ class BmnEfficiencyProbability+;
#pragma link C++ class BmnRealisticMc+;
#pragma link C++ class BmnEfficiencyTools+;
#pragma link C++ class EffStore+;
#pragma link C++ class EffStore2D+;
#pragma link C++ class InnerTrackerParams+;

// run7
#pragma link C++ class BmnLambdaAnalRun7++;
#pragma link C++ class BmnDataAnalRun7++;
#pragma link C++ class BmnTriggerEfficiencyRun7++;
#pragma link C++ class TriggerEfficiency++;
#pragma link C++ class Residuals+;

#endif

