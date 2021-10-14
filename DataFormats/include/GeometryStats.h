#ifndef __GEOMETRYSTATS_H__
#define __GEOMETRYSTATS_H__
#include <map>
#include <vector> // added by MC - 4 Aug 2021

//Helper struct holding strip numbers of min/max strips and number of strips
struct SectionStats{
  SectionStats(int num):min(num),max(num),n(1){}
  int min;
  int max;
  int n;
};

typedef std::map<int,SectionStats> DirectionStats;
typedef std::vector<int> SectionIndexList; // added by MC - 4 Aug 2021

//Wrapper around stats maps. 
//Provides interface for strip number for min/max strip and number of strips in direction->section 
//or direction with merged sections
class GeometryStats{
  public:
    GeometryStats()=default;
    inline  void Fill(int dir,int section,int num){FillSections(dir,section,num);FillMerged(dir,num);}
    void print();

    inline  SectionIndexList GetDirSectionIndexList(int dir){return directionsSectionIndexList.at(dir);} // added by MC - 4 Aug 2021
    
    inline  int GetDirNStrips(int dir,int section){return directionsStats.at(dir).at(section).n;}
    inline  int GetDirMinStrip(int dir,int section){return directionsStats.at(dir).at(section).min;}
    inline  int GetDirMaxStrip(int dir,int section){return directionsStats.at(dir).at(section).max;}
    inline  int GetDirNStripsMerged(int dir){return mergedStats.at(dir).n;}
    inline  int GetDirMinStripMerged(int dir){return mergedStats.at(dir).min;}
    inline  int GetDirMaxStripMerged(int dir){return mergedStats.at(dir).max;}
private:
  std::map<int,DirectionStats> directionsStats;
  std::map<int,SectionIndexList> directionsSectionIndexList; // added by MC - 4 Aug 2021
  DirectionStats mergedStats;
  void FillSections(int dir, int section,int num);
  //this works only with directions without gaps
  void FillMerged(int dir,int num);
};

#endif
