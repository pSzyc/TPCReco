#ifndef _TrackSegment2D_H_
#define _TrackSegment2D_H_

#include <string>
#include <vector>
#include <memory>

#include "TVector3.h"
#include "TH2D.h"
#include "CommonDefinitions.h"

class TrackSegment2D{

public:

  TrackSegment2D(int strip_dir = DIR_U){ myStripDir = strip_dir;};

  ~TrackSegment2D() {};

  void setBiasTangent(const TVector3 & aBias, const TVector3 & aTangent);

  void setStartEnd(const TVector3 & aStart, const TVector3 & aEnd);

  void setRecHits(const TH2D & aRecHits) {myRecHits = aRecHits;}

  ///Unit tangential vector along segment.
  const TVector3 & getTangent() const { return myTangent;}

  ///Bias vector perpendicular to tangential vector.
  const TVector3 & getBias() const { return myBias;}

   ///Bias vector at the beggining of the segment.
  const TVector3 & getStart() const { return myStart;}

  ///Bias vector at the end of the segment.
  const TVector3 & getEnd() const { return myEnd;}

  ///Bias vector with T=0.
  const TVector3 & getBiasAtT0() const { return myBiasAtT0;}

  ///Tangent vector along time arrow, normalised to unit value along time.
  const TVector3 & getTangentWithT1() const { return myTangentWithT1;}

  ///Bias vector with Wire=0.
  const TVector3 & getBiasAtWire0() const { return myBiasAtWire0;}

  double getLength() const { return myLenght;}

  ///Rec hits assigned to this projection.
  const TH2D & getRecHits() const {return myRecHits;}

  ///Return rec hits chi2.
  double getRecHitChi2(const TH2D & hRecHits) const;

private:

  ///Calculate vector for different parametrisations.
  void initialize();

  int myStripDir;
  double myLenght;

  TVector3 myTangent, myBias;
  TVector3 myStart, myEnd;    
  TVector3 myBiasAtT0, myBiasAtWire0;
  TVector3 myTangentWithT1;

  TH2D myRecHits;
    
};


typedef std::vector<TrackSegment2D> TrackSegment2DCollection;

#endif

