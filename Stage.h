#ifndef STAGE_H
#define STAGE_H


#include "sdlheaders.h"
#include "Vector3d.h"
#include "Sphere.h"
#include "FlatSurface.h"
#include "Parallelogram.h"
#include "Missile.h"
#include "LineSegment.h"
#include "StageMind.h"
#include <vector>
using namespace std;

class Stage
{
  public:
	Stage(void);
	~Stage(void);
	void setSurfaces(const vector<FlatSurface*> surfaces);
	void drawMe();
	void rotateBy (float angle, const Vector3d& axis);
	double getFrictionAt(const Vector3d &position) const;
	void updateState();
	Vector3d getNormalFor(Shape* shape) const;
	bool isMissileTouchingStage(Missile* missile);
	Vector3d getStageNormalForMissile(Missile* missile);
	Vector3d getRandomPosition();
	void setAutoSpin(bool value);
	bool forceDraw();
	void toggleGrass();
	void resetForceDraw();

  private:
  	static const Vector3d gravity;

	// TODO: Make sure surfaces get freed  	
  	vector<FlatSurface *> m_surfaces;
  	vector<FlatSurface *> m_surfacesOrig;
	Vector3d rot_axis;   // Axis to rotate about
	Vector3d rot_origin; // Origin to rotate about	
	float rot_angle;
	StageMind* stage_mind;
	bool force_draw;
	void initSurfaces();
	void initSimpleSurface();
	void initCurvedSurfaces();
	void initTriangleSurfaces();
	void initBridgeLevelSurfaces();
	void freeSurfaces();
	void autoSpin();
	void handleContactingShape(Shape *shape, const Vector3d &normal) const;
	bool isTouchingShape(Shape *shape, const FlatSurface *surface) const;
	bool isEdgeTouchingShape(Shape *shape, const LineSegment &edge) const;
	bool isVertexTouchingShape(Shape *shape, const Vector3d &vertex) const;
	void backShapeAwayFromEdge(Shape *shape, const LineSegment &edge, float dist) const;
	void backShapeAwayFromVertex(Shape *shape, const Vector3d &vertex, float dist) const;
};

#endif
