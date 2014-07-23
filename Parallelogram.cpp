#include "Parallelogram.h"
#include "SDLEngine.h"
#include "sdlheaders.h"
#include "Utils.h"
#include <assert.h>
#include <math/vector3.h>

#define THICKNESS 1.0
#define NUM_VERTICES 4
#define NUM_EDGES 4

Parallelogram::Parallelogram(const Vector3d &origin, const Vector3d &vecA, 
                             const Vector3d &vecB)
	: FlatSurface(origin, vecA, vecB)
{
	updateEdges();
}

Parallelogram::Parallelogram(const Parallelogram &other)
	: FlatSurface(other)
{
	updateEdges();
}

Vector3d Parallelogram::getVertex(unsigned index) const
{
	assert (index >= 0);
	assert (index < NUM_VERTICES);
	Vector3d returnVal = m_origin;
	switch(index) {
		case 0:
			break;
		case 1:
	 		returnVal = returnVal + m_vecA;
	 		break;
	 	case 2:
	 		returnVal = returnVal + m_vecA + m_vecB;
	 		break;
	 	case 3:
	 		returnVal = returnVal + m_vecB;
	 		break;
		default:
			assert(0);
	}
	return returnVal;
}

unsigned Parallelogram::getNumEdges() const
{
	return NUM_EDGES;
}

unsigned Parallelogram::getNumVertices() const
{
	return NUM_VERTICES;
}


bool Parallelogram::isInBounds(const Vector3d &point) const
{
	return (isPointInTriangleBounds(point, m_origin, m_vecA, 
	                                m_vecB, m_cobMatrix, false) 
	        || 
	        isPointInTriangleBounds(point, getVertex(2), m_vecA, 
	                                m_vecB, m_cobMatrix, true));
}

void Parallelogram::drawMe() const
{
	glEnable(GL_NORMALIZE);
	glPushMatrix();
	// Texturing
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, SDLEngine::textures[0]);


	Vector3d surfVrt[NUM_VERTICES];
	for (int i = 0; i < NUM_VERTICES; i++) {
		surfVrt[i] = getVertex(i);
	}
    GLdouble vertices[] = {         
            surfVrt[0].getX(), surfVrt[0].getY(),          surfVrt[0].getZ(),
            surfVrt[1].getX(), surfVrt[1].getY(),          surfVrt[1].getZ(),
            surfVrt[2].getX(), surfVrt[2].getY(),          surfVrt[2].getZ(),
            surfVrt[3].getX(), surfVrt[3].getY(),          surfVrt[3].getZ(),
            surfVrt[0].getX(), surfVrt[0].getY() - THICKNESS, surfVrt[0].getZ(),
            surfVrt[1].getX(), surfVrt[1].getY() - THICKNESS, surfVrt[1].getZ(),
            surfVrt[2].getX(), surfVrt[2].getY() - THICKNESS, surfVrt[2].getZ(),
            surfVrt[3].getX(), surfVrt[3].getY() - THICKNESS, surfVrt[3].getZ(),
    };

	GLdouble texCoordsFaces[] = {
		m_texCoords[0].getX(), m_texCoords[0].getY(), 
		m_texCoords[1].getX(), m_texCoords[1].getY(), 
		m_texCoords[2].getX(), m_texCoords[2].getY(), 
		m_texCoords[3].getX(), m_texCoords[3].getY()
	}; 

	// TODO DWH: This is looking a little weird on every other edge.  Check this.
	float edgeHeightScale = ((GLdouble)THICKNESS/(GLdouble)m_vecA.getMagnitude());
	GLdouble texCoordsEdges[] = {
		m_texCoords[0].getX(), m_texCoords[0].getY() * edgeHeightScale, 
		m_texCoords[1].getX(), m_texCoords[1].getY() * edgeHeightScale, 
		m_texCoords[3].getX(), m_texCoords[3].getY() * edgeHeightScale, 
		m_texCoords[2].getX(), m_texCoords[2].getY() * edgeHeightScale
	};

	GLubyte topIndices[] = {0, 1, 2, 3};
	GLubyte bottomIndices[] = {7, 6, 5, 4};
	GLubyte frontIndices[] = {4, 5, 1, 0};
	GLubyte backIndices[] = {3, 2, 6, 7};
	GLubyte leftIndices[] = {4, 0, 3, 7};
	GLubyte rightIndices[] = {1, 5, 6, 2};

	if (m_texture == INVALID_TEXTURE) {
		glUniform1iARB(S_UNI_POS[S_UNI_bumpmap], 1.0);
	} else {
		glUniform1iARB(S_UNI_POS[S_UNI_bumpmap], 0.0);
	}
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	if (m_texture == INVALID_TEXTURE) {
		glBindTexture(GL_TEXTURE_2D, SDLEngine::textures[0]);
	} else {
		glBindTexture(GL_TEXTURE_2D, m_texture);
	}
	
	glUniform1iARB(S_UNI_POS[S_UNI_tex0], 0);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, SDLEngine::textures[5]);
	glUniform1iARB(S_UNI_POS[S_UNI_tex1], 1);

	// setup the material
	float specular[] = {0.75, 0.75, 0.75};
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);


	//glEnableClientState(GL_VERTEX_ARRAY);
	//glVertexPointer(3, GL_DOUBLE, 0, vertices);
	glBegin(GL_QUADS);

	Vector topNormal = m_normal.toVector();
	topNormal.normalize();
	Vector bottomNormal = -topNormal;
	Vector rightNormal = m_vecA.toVector();
	rightNormal.normalize();
	Vector leftNormal = -rightNormal;
	Vector backNormal = m_vecB.toVector();
	backNormal.normalize();
	Vector frontNormal = -backNormal;

	for (int i = 0; i < 4; ++i) {
		glTexCoord2dv(&texCoordsFaces[i * 2]);
		topNormal.glNormal();
		glVertexAttrib3fARB(S_ATT_vertexTangent, rightNormal.x(), rightNormal.y(), rightNormal.z());
		//glArrayElement(topIndices[i]);
		glVertex3f(surfVrt[i].getX(), surfVrt[i].getY(), surfVrt[i].getZ());
	}

	for (int i = 0; i < 4; ++i) {
		glTexCoord2dv(&texCoordsFaces[i * 2]);
		bottomNormal.glNormal();
		glVertexAttrib3fARB(S_ATT_vertexTangent, rightNormal.x(), rightNormal.y(), rightNormal.z());
		double* t = &vertices[bottomIndices[i]*3];
		glVertex3d(*t, *(t+1), *(t+2));
		//glArrayElement(bottomIndices[i]);
	}
	for (int i = 0; i < 4; ++i) {
		glTexCoord2dv(&texCoordsEdges[i * 2]);
		frontNormal.glNormal();
		glVertexAttrib3fARB(S_ATT_vertexTangent, rightNormal.x(), rightNormal.y(), rightNormal.z());
		double* t = &vertices[frontIndices[i]*3];
		glVertex3d(*t, *(t+1), *(t+2));
		//glArrayElement(frontIndices[i]);
	}
	for (int i = 0; i < 4; ++i) {
		glTexCoord2dv(&texCoordsEdges[i * 2]);
		backNormal.glNormal();
		glVertexAttrib3fARB(S_ATT_vertexTangent, rightNormal.x(), rightNormal.y(), rightNormal.z());
		double* t = &vertices[backIndices[i]*3];
		glVertex3d(*t, *(t+1), *(t+2));
		//glArrayElement(backIndices[i]);
	}
	for (int i = 0; i < 4; ++i) {
		glTexCoord2dv(&texCoordsEdges[i * 2]);
		leftNormal.glNormal();
		glVertexAttrib3fARB(S_ATT_vertexTangent, topNormal.x(), topNormal.y(), topNormal.z());
		double* t = &vertices[leftIndices[i]*3];
		glVertex3d(*t, *(t+1), *(t+2));
		//glArrayElement(leftIndices[i]);
	}
	for (int i = 0; i < 4; ++i) {
		glTexCoord2dv(&texCoordsEdges[i * 2]);
		rightNormal.glNormal();
		glVertexAttrib3fARB(S_ATT_vertexTangent, bottomNormal.x(), bottomNormal.y(), bottomNormal.z());
		double* t = &vertices[rightIndices[i]*3];
		glVertex3d(*t, *(t+1), *(t+2));
		//glArrayElement(rightIndices[i]);
	}
	glEnd();
	//glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glUniform1iARB(S_UNI_POS[S_UNI_bumpmap], 0.0);
}

FlatSurface::FlatSurfaceType Parallelogram::getType() const
{
	return PARALLELOGRAM;
}

void Parallelogram::updateEdges()
{
	m_edges.clear();
	for (unsigned i = 0; i < getNumVertices() - 1; i++) {
		m_edges.push_back(LineSegment(getVertex(i), getVertex(i + 1)));
	}
	m_edges.push_back(LineSegment(getVertex(getNumVertices() - 1), getVertex(0)));
}
