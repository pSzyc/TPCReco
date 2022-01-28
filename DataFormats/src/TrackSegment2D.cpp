#include "TrackSegment2D.h"
#include "colorText.h"
#include <iostream>

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void TrackSegment2D::setBiasTangent(const TVector3 & aBias, const TVector3 & aTangent){

  myBias = aBias;
  myTangent = aTangent.Unit();

  double lambda = 10;//FIXME what value should be here?
  myStart = myBias; 
  myEnd = myBias+lambda*myTangent;
  initialize();
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void TrackSegment2D::setStartEnd(const TVector3 & aStart, const TVector3 & aEnd){

  myStart = aStart;
  myEnd = aEnd;

  myTangent = (myEnd - myStart).Unit();
  myBias = myStart;

  initialize();
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void TrackSegment2D::initialize(){
  
  double lambda = -myBias.X()/myTangent.X();
  myBiasAtT0 = myBias + lambda*myTangent;
  
  lambda = -myBias.Y()/myTangent.Y();
  myBiasAtWire0 = myBias + lambda*myTangent;

  ///Set tangent direction along time arrow with unit time component,
  ///so vector components can be compared between projections.
  myTangentWithT1 = myTangent;
  if(myTangentWithT1.X()<0) myTangentWithT1 *= -1;
  
  if(std::abs(myTangentWithT1.X())>1E-5){
    myTangentWithT1 *= 1.0/myTangentWithT1.X();
  }
  else myTangentWithT1 *=0.0;
  
  myLenght = (myEnd - myStart).Mag();
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
double TrackSegment2D::getIntegratedCharge(double lambdaCut, const Hit2DCollection & aRecHits) const{

  double x = 0.0, y = 0.0;
  double totalCharge = 0.0;
  double radiusCut = 4.0;//FIXME put into configuration. Value 4.0 abtaine looking athe plots.
  double distance = 0.0;
  TVector3 aPoint;
  
  for(const auto aHit:aRecHits){
    x = aHit.getPosTime();
    y = aHit.getPosWire();
    aPoint.SetXYZ(x, y, 0.0);    
    distance = getPointTransverseDistance(aPoint);
    if(distance>0 && distance<radiusCut) totalCharge += aHit.getCharge();
  }
  return totalCharge;
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
double TrackSegment2D::getIntegratedHitDistance(double lambdaCut, const Hit2DCollection & aRecHits) const{

  double x = 0.0, y = 0.0;
  double totalCharge = 0.0;
  double distance = 0.0;
  double sum = 0.0;
  TVector3 aPoint;
  //const TVector3 & tangent = getTangentWithT1();
  
  for(const auto aHit:aRecHits){
    x = aHit.getPosTime();
    y = aHit.getPosWire();
    aPoint.SetXYZ(x, y, 0.0);    
    distance = getPointTransverseDistance(aPoint);
    if(distance>0 && distance<5){
      int sign = 1.0;//std::copysign(1.0,tangent.Cross(aPoint).Z());
      sum += distance*sign*aHit.getCharge();
      totalCharge += aHit.getCharge();
    }
  }
  //FIXME divide by total segment charge, not total charge up to lambda
  return sum;
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
double TrackSegment2D::getPointTransverseDistance(const TVector3 & aPoint) const{

  const TVector3 & bias = getBias();
  const TVector3 & tangent = getTangent();
  const TVector3 & start = getStart();

  double lambda = (aPoint - start)*tangent/tangent.Mag();
  if(lambda<0 || lambda>getLength()) return -999;
  TVector3 transverseComponent = aPoint - bias - lambda*tangent;
  return transverseComponent.Mag();
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
double TrackSegment2D::getRecHitChi2(const Hit2DCollection & aRecHits) const {

  double dummyChi2 = 1E9;

  if(getTangent().Mag()<1E-3){
    std::cout<<__FUNCTION__<<KRED<< " TrackSegment2D has null tangent "<<RST
	     <<" for direction: "<<getStripDir()<<std::endl;
    std::cout<<KGRN<<"Start point: "<<RST;
    getStart().Print();
    std::cout<<KGRN<<"End point: "<<RST;
    getEnd().Print();
    return dummyChi2;
  }

  TVector3 aPoint;
  double chi2 = 0.0;
  double chargeSum = 0.0;
  double distance = 0.0;
  int pointCount = 0;
  
  double x = 0.0, y = 0.0;
  double charge = 0.0;
  double weight = 0.0;

  for(const auto aHit:aRecHits){
    x = aHit.getPosTime();
    y = aHit.getPosWire();
    charge = aHit.getCharge();
    aPoint.SetXYZ(x, y, 0.0);
    distance = getPointTransverseDistance(aPoint);
    weight = 1.0;//Place holder for something more complicated
    if(distance<0) continue;
    ++pointCount;    
    chi2 += std::pow(distance, 2)*charge*weight;
    chargeSum +=charge*weight;
  }
  if(!pointCount) return dummyChi2;

  chi2 /= chargeSum;
  return chi2;
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
std::ostream & operator << (std::ostream &out, const TrackSegment2D &aSegment){

  const TVector3 & start = aSegment.getStart();
  const TVector3 & end = aSegment.getEnd();
  
  out<<"direction: "<<aSegment.getStripDir()
     <<" ("<<start.X()<<", "<<start.Y()<<")"
     <<" -> "
     <<"("<<end.X()<<", "<<end.Y()<<") "<<std::endl
     <<"\t N Hough accumulator hits: "
     <<"["<<aSegment.getNAccumulatorHits()<<"]";
  return out;
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
