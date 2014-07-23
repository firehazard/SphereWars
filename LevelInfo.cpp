#include "LevelInfo.h"
#include "FlatSurface.h"
#include "Triangle.h"
#include "Parallelogram.h"
#include "Constants.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <assert.h>
using namespace std;

#define SKYBOX_HDR             "SKYBOX"
#define SURFACES_HDR           "SURFACES"
#define POWERUPS_HDR           "POWERUPS"
#define SPAWNPOINTS_HDR        "SPAWNPOINTS"

#define GRASS_HDR              "GRASS_FLAG"
#define COMMENT_CHAR           '#'


static const string END_OF_SECTION_MARKER  = "||";

LevelInfo::LevelInfo()
{
	m_isGrass = false;
}

LevelInfo::~LevelInfo()
{
}

/**
 * Functions for parsing a level list file
 */
// Parses one file to grab  
bool LevelInfo::loadLevelList(string filename, vector<pair<string, string> > &levelNamesAndPaths)
{
	cout << "Attempting to load level list from \"" << filename << "\"..." << endl;
	int numLinesRead = 0;
	bool success = true;
	ostringstream errors;
	ifstream in (filename.c_str());
	
	if (!in.is_open()) {
		errors << "Unable to open file " << filename << endl;
		success = false;
	} else {
		string line;
		pair<string,string> tmpNameAndPath;
		while(getline(in,line)) {
			string firstWord;
			istringstream iss(line);
			iss >> firstWord;
			if (iss.fail()) {  // Was there an error reading that word?
				// Assume line is whitespace -- skip it.
			} else {
				numLinesRead++;
				tmpNameAndPath.first = "";
				success = parseLevelNameAndPath(line, tmpNameAndPath, errors, numLinesRead);
				if (success) {
					levelNamesAndPaths.push_back(tmpNameAndPath);
				} else {
					errors << "Line " << numLinesRead << ": Error parsing the line \"" << line << "\"" << endl; 
					break;
				}
			}
		}
		in.close();
	}
	if (!success) {
		cout << "Error while parsing level list " << filename << ": " << endl
			 << errors.str() << endl;
	} else {
		// Since success is true, there should have been any errors
		assert(errors.str().length() == 0);
		cout << "Loaded successfully." << endl;
	}
	
	return success;
}

bool LevelInfo::parseLevelNameAndPath(string line, pair<string,string> &nameAndPath, 
                                      ostream &errors, int numLinesRead)
{	
	unsigned int delimPos = line.find(END_OF_SECTION_MARKER);
	if (delimPos == string::npos) {
		errors << "Line " << numLinesRead << ": No delimiter found in level-list." << endl;
		return false;
	}
	
	// Parse the level name
	// --------------------
	// Figure out where level name starts (skipping whitespace)
	int start = getFirstNonWhitespaceIndex(line, 0, false);
	if (start >= (int)delimPos) {
		errors << "Line " << numLinesRead << ": No filename found before delimiter." << endl;
		return false;
	}
	
	int end = getFirstNonWhitespaceIndex(line, delimPos - 1, true);
	assert(end >= start);
		
	nameAndPath.first = line.substr(start, end - start + 1);
	
	
	// Parse the level filename
	// ------------------------
	start = getFirstNonWhitespaceIndex(line, delimPos + END_OF_SECTION_MARKER.size(), false);
	if (start == (int)line.size()) {
		errors << "Line " << numLinesRead << ": No filename found before delimiter." << endl;
		return false;
	}
	
	end = getFirstNonWhitespaceIndex(line, line.size() - 1, true);
	assert(end != (int)line.size());
	assert(end >= start);
	
	nameAndPath.second = line.substr(start, end - start + 1);
	return true;
}

int LevelInfo::getFirstNonWhitespaceIndex(string str, int startPosn, bool isReverse)
{
	int posn = startPosn;
	while(true) {
		// Out of bounds?
		if (posn < 0 && posn >= (int)str.size()) {
			return str.size();
		}
				
		if (str[posn] == ' ' || str[posn] == '\t') {
			// Whitespace -- skip it.
			if (isReverse) {
				posn--;
			} else {
				posn++;
			}
		} else {
			// Not whitespace! yay.
			return posn;
		}
	}
}

/**
 * Functions for parsing a level file
 */
// Returns true if successful
bool LevelInfo::loadFromFile(string filename)
{
	cout << "Attempting to load level \"" << filename << "\"..." << endl;
	int numLinesRead = 0;  // Functions as line numbering system (1-based) for labelling errors
	bool success = true;
	ostringstream errors;
	ifstream in (filename.c_str());
	
	if (!in.is_open()) {
		errors << "Unable to open file " << filename << endl;
		success = false;
	} else {
		string line;
		while(getline(in,line)) {
			numLinesRead++;
			// Get the first word on the line. (If all's well, it should be a header)
			string firstWord;
			istringstream iss(line);
			iss >> firstWord;
			if (iss.fail()) {  // Was there an error reading that word?
				// Assume line is whitespace -- skip it.
			} else {
				// Read the next section, depending on what the header is.
				if (firstWord == SURFACES_HDR) {     			
					success = loadSurfacesSection(in, errors, numLinesRead);					
				} else if (firstWord == POWERUPS_HDR) {
					success = loadPowerupsSection(in, errors, numLinesRead);
				} else if (firstWord == SPAWNPOINTS_HDR) {
					success = loadSpawnPointsSection(in, errors, numLinesRead);
				} else if (firstWord == SKYBOX_HDR) {
					success = loadSkyBoxSection(in, errors, numLinesRead);
				} else if (firstWord == GRASS_HDR) {
					m_isGrass = true;
				} else {
					errors << "Line " << numLinesRead << ": Expected a section header, but got " << line;
					success = false;
				}
			}			
			// Check if there was an error reading the section
			if (!success) {
				break;
			}
		}
		in.close();
	}
	if (!success) {
		cerr << "Error while parsing level file " << filename << ": " << endl
			 << errors.str()
			 << endl;
		
		// TODO DWH Clean up any structures that were loaded in
		
	} else {
		// Since success is true, there should have been any errors
		assert(errors.str().length() == 0);
		cout << "Loaded successfully." << endl;
	}
	return success;
}

// Gets the next line of non-whitespace text (skipping over blank and whitespace lines)
bool LevelInfo::getLineOrReportError(istream &in, string &line, ostream &errors, int &numLinesRead)
{
	while(true) {
		bool lineRead = getline(in,line);
		if (!lineRead) {
			errors << "Line " << numLinesRead << ": Unexpected end of file."
                   << "(May be missing an end-of-section marker, \"" 
                   << END_OF_SECTION_MARKER << "\")." << endl;
			return false;
		} else {
			numLinesRead++;
			// Only return if the line has some non-whitespace text
			// and it's not a comment
			string str;
			istringstream iss(line);
			iss >> str; // Only succeeds if there's non-whitespace text
			if (!iss.fail()) {				
				if (str[0] != COMMENT_CHAR) {
					// Found a line of non-whitespace, non-comment text
					return true;
				}
			}
		}
	}	
}

bool LevelInfo::loadSurfacesSection(istream &in, ostream &errors, int &numLinesRead)
{
	// Make sure we haven't already read the surfaces
	if (!m_surfaces.empty()) {
		errors << "Line " << numLinesRead << ": Encountered a second " 
		       << SURFACES_HDR << " section." << endl;
		return false;
	}
	
	// Read until you get an end-of-section marker
	while(true) {
		// Read next line and see if it's an end-of-section marker
		string line;
		if (!getLineOrReportError(in, line, errors, numLinesRead)) {
			return false;
		} 
		if (isEndOfSection(line)) {
			return true;
		}
		
		// Read the geometric info -- type and defining vectors
		FlatSurface::FlatSurfaceType type;
		Vector3d origin, vecA, vecB;
		Vector3d originText, vecAText, vecBText, vectCText;
		if (!loadSurfaceType(line, type, errors, numLinesRead)   ||
	       (!loadVector3d(in, origin, errors, numLinesRead))     ||
		   (!loadVector3d(in, vecA, errors, numLinesRead))       ||
		   (!loadVector3d(in, vecB, errors, numLinesRead)))
		{
		   	return false;
		}

		FlatSurface *surface;
		if (type == FlatSurface::TRIANGLE) {
			surface = new Triangle(origin, vecA, vecB);
		} else if (type == FlatSurface::PARALLELOGRAM) {
			surface = new Parallelogram(origin, vecA, vecB);
		} else {
			// shouldn't be able to get here
			assert(0);
		}

		vector<Vector3d> textCoords;
		// Load Texture Coordinates -- 1 per vertex.
		for (int i = 0; i < (int)surface->getNumVertices(); i++)
		{
			Vector3d curCoord;
			if (!loadVector2d(in, curCoord, errors, numLinesRead)) {
				return false;
			}
			surface->setTextureCoord(i, curCoord);
		}
		// Read texture + material properties stuff
		/* TODO DWH
		Appearance appearance;
		if (!loadSurfaceAppearance(in, appearance errors, numLinesRead)) {
			return false;
		} 
		*/
		
		
		m_surfaces.push_back(surface);
	}
}

bool LevelInfo::loadPowerupsSection(istream &in, ostream &errors, int &numLinesRead)
{
	// Make sure we haven't already loaded powerups
	if (!m_powerups.empty()) {
		errors << "Line " << numLinesRead << ": Encountered a second " 
		       << POWERUPS_HDR << " section." << endl;
		return false;
	} 

	// Read until you get an end-of-section marker
	while(true) {
		// Read next line and see if it's an end-of-section marker
		string line;
		if (!getLineOrReportError(in, line, errors, numLinesRead)) {
			return false;
		} 
		if (isEndOfSection(line)) {
			return true;
		}
		
		// Read the powerup type and position
		Powerup::PowerupType type;
		Vector3d position;
		if (!loadPowerupType(line, type, errors, numLinesRead) ||
	        !loadVector3d(in, position, errors, numLinesRead))
		{
		   	return false;
		}
		
		m_powerups.push_back(make_pair<Powerup::PowerupType, Vector3d>(type, position));
	}
}

// TODO DWH: Check that no 2 spawn points are within a sphere-diameter from each other
bool LevelInfo::loadSpawnPointsSection(istream &in, ostream &errors, int &numLinesRead)
{
	// Make sure we haven't already loaded spawn points
	if (!m_spawnPoints.empty()) {
		errors << "Line " << numLinesRead << ": Encountered a second " 
		       << SPAWNPOINTS_HDR << " section." << endl;
		return false;
	} 

	// Read until you get an end-of-section marker
	while(true) {
		// Read next line and see if it's an end-of-section marker
		string line;
		if (!getLineOrReportError(in, line, errors, numLinesRead)) {
			return false;
		} 
		if (isEndOfSection(line)) {
			// Make sure we have exactly one spawn point for each player
			if (m_spawnPoints.size() != MAX_NUM_PLAYERS) {
				errors << "Line " << numLinesRead << ": Wrong number of spawn points.  " 
				       << "Expected " << MAX_NUM_PLAYERS << ", but found " 
				       << m_spawnPoints.size() << "." << endl;
				return false;
			}
			return true;
		}
		
		// Read the spawn position
		Vector3d position;
		if (!loadVector3d(line, position, errors, numLinesRead))
		{
		   	return false;
		}
		
		m_spawnPoints.push_back(position); 
	}
}

bool LevelInfo::loadSkyBoxSection(istream &in, ostream &errors, int &numLinesRead)
{
	if (!m_skyBoxFaces.empty()) {
		errors << "Line " << numLinesRead << ": Encountered a second " 
		       << SKYBOX_HDR << " section." << endl;
		return false;
	}
	
	// Read until you get an end-of-section marker
	while(true) {
		// Read next line and see if it's an end-of-section marker
		string line;
		if (!getLineOrReportError(in, line, errors, numLinesRead)) {
			return false;
		}
		if (isEndOfSection(line)) {
			return true;
		}

		string textureName;
		SkyBoxInfo sbInfo;
		
		// READ FIRST LINE
		// ---------------
		// Parse the data
		istringstream textureStream(line);
		textureStream >> textureName;
		sbInfo.texture = Utils::loadTexture(textureName.c_str());
		//cout << textureName.c_str() << " " << sbInfo.texture << endl;
		
		// READ SECOND LINE (x & y repetitions)
		// ------------------------------------
		if (!getLineOrReportError(in, line, errors, numLinesRead)) {
			return false;
		} 
		if (isEndOfSection(line)) {
			if (m_spawnPoints.size() != MAX_NUM_PLAYERS) {
				errors << "Line " << numLinesRead << ":  " << SKYBOX_HDR 
				       << " section was prematurely terminated.  "
				       << "Missing num repetitions." << endl;
				return false;
			}
		}
		
		istringstream repStream(line);
		repStream >> sbInfo.xReps >> sbInfo.yReps;
		
		if (textureStream.fail() || repStream.fail()) {
			errors << "Line " << numLinesRead << ": Error parsing " << SKYBOX_HDR << " section." << endl;
			return false;
		} else if (sbInfo.xReps <= 0 || sbInfo.yReps <= 0) {
			errors << "Line " << numLinesRead << ": Number of texture repetitions is 0 or lower." << endl;
			return false;		
		}
	
		// Parsed Section correctly!!
		m_skyBoxFaces.push_back(sbInfo);
	}
	return true;
}

bool LevelInfo::isEndOfSection(const string &line) const
{
	string str;
	istringstream iss(line);
	
	iss >> str;
	if (!iss.fail() && str == END_OF_SECTION_MARKER) {
		iss >> str; // this one should fail
		if (iss.fail()) {
			return true;
		}
	}
	return false;
}




bool LevelInfo::loadSurfaceType(const string &line, FlatSurface::FlatSurfaceType &type, 
                                ostream &errors, int numLinesRead) const
{
	string typeName = "";
	istringstream iss(line);		
	iss >> typeName;
	type = FlatSurface::getTypeForName(typeName);
	if (type == FlatSurface::NUM_FLAT_SURFACE_TYPES) {
		errors << "Line " << numLinesRead << ": The string \"" << typeName << "\" is not the name" 
		       << " of a Flat Surface Type." << endl;
		return false;
	} else {
		return true;
	}
}

/* DWH
bool LevelInfo::loadSurfaceType(const string &line, SurfaceAppearance app,
                                ostream &errors, int numLinesRead) const
{
	if (!loadSurfaceAppearance(in, appearance errors, numLinesRead)) {
		return false;
	}
} 
*/

bool LevelInfo::loadPowerupType(const string &line, Powerup::PowerupType &type, 
                                ostream &errors, int numLinesRead) const
{
	string typeName = "";
	istringstream iss(line);		
	iss >> typeName;
	type = Powerup::getTypeForName(typeName);
	if (type == Powerup::NUM_POWERUP_TYPES) {
		errors << "Line " << numLinesRead << ": The string \"" << typeName << "\" is not the name" 
		       << " of a Powerup Type." << endl;
		return false;
	} else {
		return true;
	}
}

bool LevelInfo::loadVector3d(istream &in, Vector3d &vec,
                             ostream &errors, int &numLinesRead) const
{
	string line;
	if (!getLineOrReportError(in, line, errors, numLinesRead)) {
		return false;
	}
	return loadVector3d(line, vec, errors, numLinesRead);
}

bool LevelInfo::loadVector3d(const string &line, Vector3d &vec, 
                             ostream &errors, int numLinesRead) const
{
	float x,y,z;
	istringstream iss(line);
	iss >> x >> y >> z;
	if (!iss.fail()) {
		// Make sure there's nothing else on the line
		string tmp;
		iss >> tmp; // should fail
		if (iss.fail()) {
			vec.setX(x);
			vec.setY(y);
			vec.setZ(z);
			return true;
		}
	}
	errors << "Line " << numLinesRead << ": Couldn't parse a 3d vector from line \"" << line << "\". "
	       << "(May be missing an end-of-section marker, \"" << END_OF_SECTION_MARKER << "\".)" << endl;
	       
	return false;
}

bool LevelInfo::loadVector2d(istream &in, Vector3d &vec,
                             ostream &errors, int &numLinesRead) const
{
	string line;
	if (!getLineOrReportError(in, line, errors, numLinesRead)) {
		return false;
	}
	return loadVector2d(line, vec, errors, numLinesRead);
}

bool LevelInfo::loadVector2d(const string &line, Vector3d &vec, 
                             ostream &errors, int numLinesRead) const
{
	float x,y;
	istringstream iss(line);
	iss >> x >> y;
	if (!iss.fail()) {
		// Make sure there's nothing else on the line
		string tmp;
		iss >> tmp; // should fail
		if (iss.fail()) {
			vec.setX(x);
			vec.setY(y);
			vec.setZ(0);
			return true;
		}
	}
	errors << "Line " << numLinesRead << ": Couldn't parse a 2d vector from line \"" << line << "\". "
	       << "(May be missing an end-of-section marker, \"" << END_OF_SECTION_MARKER << "\".)" << endl;
	       
	return false;
}
