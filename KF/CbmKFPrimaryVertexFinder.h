/** The CbmKFPrimaryVertexFinder class
 *
 * @author  S.Gorbunov, I.Kisel
 * @version 1.0
 * @since   06.02.06
 * 
 * Class to find primary vertex with the Kalman Filter method
 *
 */
#ifndef CBMKFPRIMARYVERTEXFINDER_H
#define CBMKFPRIMARYVERTEXFINDER_H

#include "CbmKFTrackInterface.h"
#include "CbmKFVertexInterface.h"

#include <vector>

class CbmKFPrimaryVertexFinder:public TObject {
  
  std::vector<CbmKFTrackInterface*> Tracks;

 public:

  CbmKFPrimaryVertexFinder():Tracks() { Clear(); };
  ~CbmKFPrimaryVertexFinder(){};

  void Clear();
  void AddTrack( CbmKFTrackInterface* Track );
  void SetTracks( std::vector<CbmKFTrackInterface*> &vTracks );
  void Fit(CbmKFVertexInterface &vtx );

  ClassDef( CbmKFPrimaryVertexFinder, 1 );
};
#endif /* !CBMKFPRIMARYVERTEXFINDER_H */
