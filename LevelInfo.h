#ifndef LEVELINFO_H_
#define LEVELINFO_H_

#include "FlatSurface.h"
#include "Powerup.h"
#include "MaterialProperties.h"
#include "SkyBoxInfo.h"

#include <string>
using namespace std;

// TODO DWH: Make a copy constructor
// TODO DWH: Make a destructor
// TODO DWH: Load textures with a SDLEngine method that caches stuff?
class LevelInfo
{
  public:
	LevelInfo();
	~LevelInfo();
		
	// Returns true if successful
	bool loadFromFile(string filename);
	
	// Accessors
	const vector<FlatSurface *>  &getSurfaces() const
		{ return m_surfaces; }
	const vector<Vector3d>       &getSpawnPoints() const
		{ return m_spawnPoints; }
	const vector<pair<Powerup::PowerupType, Vector3d> > &getPowerups() const
		{ return m_powerups; }	
	const vector<SkyBoxInfo> &getSkyBoxFaces() const
		{ return m_skyBoxFaces; }
	
	const bool isGrass() const
		{ return m_isGrass; }
		
	static bool loadLevelList(string filename, vector<pair<string, string> > &levelNamesAndPaths);
	
  private:
	vector<FlatSurface *> m_surfaces;      // Surface info
	vector<Vector3d>  m_spawnPoints;       // Spawn point info	
	vector<pair<Powerup::PowerupType, Vector3d> > m_powerups; 	// Powerup info
	vector<SkyBoxInfo> m_skyBoxFaces;
	bool m_isGrass;

	// Utility function
	static bool getLineOrReportError(istream &in, string &line, ostream &errors, int &numLinesRead);
	
	// Section-loading functions
	bool loadSurfacesSection(istream &in, ostream &errors, int &numLinesRead);
	bool loadPowerupsSection(istream &in, ostream &errors, int &numLinesRead);
	bool loadSpawnPointsSection(istream &in, ostream &errors, int &numLinesRead);	
	bool loadSkyBoxSection(istream &in, ostream &errors, int &numLinesRead);
	
	bool isEndOfSection(const string &line) const;
	
	// Functions to load just one type of data
	bool loadSurfaceType(const string &line, FlatSurface::FlatSurfaceType &type, 
	                     ostream &errors, int numLinesRead) const;
	
	bool loadPowerupType(const string &line, Powerup::PowerupType &type, 
	                     ostream &errors, int numLinesRead) const;                     
	
	bool loadVector3d(const string &line, Vector3d &vec, 
	                  ostream &errors, int numLinesRead) const;
	
	bool loadVector3d(istream &in, Vector3d &vec, 
	                  ostream &errors, int &numLinesRead) const;
	                  
	bool loadVector2d(const string &line, Vector3d &vec, 
	                  ostream &errors, int numLinesRead) const;
	
	bool loadVector2d(istream &in, Vector3d &vec, 
	                  ostream &errors, int &numLinesRead) const;
	                  
	static bool parseLevelNameAndPath(string line, pair<string,string> &nameAndPath, 
	                                  ostream &errors, int numLinesRead);
	static int getFirstNonWhitespaceIndex(string str, int startPosn, bool isReverse); 
	                  
};

#endif /*LEVELINFO_H_*/
