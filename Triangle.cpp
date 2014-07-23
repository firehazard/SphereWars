#include "Triangle.h"
#include "SDLEngine.h"
#include "sdlheaders.h"
#include "Utils.h"
#include <assert.h>
#include <math/vector3.h>

#define THICKNESS 1.0
#define NUM_VERTICES 3
#define NUM_EDGES 3

Triangle::Triangle(const Vector3d &origin, const Vector3d &vecA, 
                   const Vector3d &vecB)
	: FlatSurface(origin, vecA, vecB)
{
	updateEdges();
}

Triangle::Triangle(const Triangle &other)
	: FlatSurface(other)
{
	updateEdges();
}

Vector3d Triangle::getVertex(unsigned index) const
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
	 		returnVal = returnVal + m_vecB;
	 		break;
		default:
			assert(0);
	}
	return returnVal;
}

unsigned Triangle::getNumEdges() const
{
	return NUM_EDGES;
}

unsigned Triangle::getNumVertices() const
{
	return NUM_VERTICES;
}


bool Triangle::isInBounds(const Vector3d &point) const
{
	return FlatSurface::isPointInTriangleBounds(point, m_origin, m_vecA, 
	                                            m_vecB, m_cobMatrix, false);
}

// Looks crappy right now, but can be improved later, based on Parallelogram::drawMe
void Triangle::drawMe() const
{
	glEnable(GL_NORMALIZE);
	glPushMatrix();
	// Texturing
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if (m_texture == INVALID_TEXTURE) {
		glBindTexture(GL_TEXTURE_2D, SDLEngine::textures[0]);
	} else {
		glBindTexture(GL_TEXTURE_2D, m_texture);
	}
	

	Vector3d surfVrt[NUM_VERTICES];
	for (int i = 0; i < NUM_VERTICES; i++) {
		surfVrt[i] = getVertex(i);
	}
    GLdouble vertices[] = {         
            surfVrt[0].getX(), surfVrt[0].getY(),          surfVrt[0].getZ(),
            surfVrt[1].getX(), surfVrt[1].getY(),          surfVrt[1].getZ(),
            surfVrt[2].getX(), surfVrt[2].getY(),          surfVrt[2].getZ(),
            surfVrt[0].getX(), surfVrt[0].getY() - THICKNESS, surfVrt[0].getZ(),
            surfVrt[1].getX(), surfVrt[1].getY() - THICKNESS, surfVrt[1].getZ(),
            surfVrt[2].getX(), surfVrt[2].getY() - THICKNESS, surfVrt[2].getZ()
    };

	GLdouble texCoordsFaces[] = {
		m_texCoords[0].getX(), m_texCoords[0].getY(), 
		m_texCoords[1].getX(), m_texCoords[1].getY(), 
		m_texCoords[2].getX(), m_texCoords[2].getY() 
	};

	float edgeHeightScale = ((GLdouble)THICKNESS/(GLdouble)m_vecA.getMagnitude());
	GLdouble texCoordsEdges[] = {
		m_texCoords[0].getX(), m_texCoords[0].getY() * edgeHeightScale, 
		m_texCoords[1].getX(), m_texCoords[1].getY() * edgeHeightScale, 
		m_texCoords[2].getX(), m_texCoords[2].getY() * edgeHeightScale
	};

	// TODO: Set these correctly so that the texture "bends around" the edges
	GLubyte topIndices[] = {0, 1, 2};
	GLubyte bottomIndices[] = {3, 4, 5};
	GLubyte rightIndices[] = {1, 0, 3, 4}; // side a
	GLubyte backIndices[] = {5, 3, 0, 2}; // side b
	GLubyte leftIndices[] = {2, 1, 4, 5}; // side c


	if (m_texture == INVALID_TEXTURE) {
		glUniform1iARB(S_UNI_POS[S_UNI_bumpmap], 1.0);
	} else {
		glUniform1iARB(S_UNI_POS[S_UNI_bumpmap], 0.0);
	}
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, SDLEngine::textures[0]);
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


	Vector topNormal = m_normal.toVector();
	topNormal.normalize();
	Vector bottomNormal = -topNormal;
	Vector sideANormal = m_vecA.crossProduct(m_normal).toVector();
	sideANormal.normalize();
	Vector sideBNormal = -m_vecB.crossProduct(m_normal).toVector();
	sideBNormal.normalize();
	Vector sideCNormal = (m_vecB -m_vecA).crossProduct(m_normal).toVector();

	glBegin(GL_TRIANGLES);

	for (int i = 0; i < NUM_VERTICES; ++i) {
		glTexCoord2dv(&texCoordsFaces[i * 2]);
		topNormal.glNormal();
		glVertexAttrib3fARB(S_ATT_vertexTangent, sideANormal.x(), sideANormal.y(), sideANormal.z());
		//glArrayElement(topIndices[i]);
		glVertex3f(surfVrt[i].getX(), surfVrt[i].getY(), surfVrt[i].getZ());
	}

	for (int i = 0; i < NUM_VERTICES; ++i) {
		glTexCoord2dv(&texCoordsFaces[i * 2]);
		bottomNormal.glNormal();
		glVertexAttrib3fARB(S_ATT_vertexTangent, sideANormal.x(), sideANormal.y(), sideANormal.z());
		double* t = &vertices[bottomIndices[i]*3];
		glVertex3d(*t, *(t+1), *(t+2));
		//glArrayElement(bottomIndices[i]);
	}
	glEnd();	
	glBegin(GL_QUADS);	
	for (int i = 0; i < 4; ++i) {
		glTexCoord2dv(&texCoordsEdges[i * 2]);
		sideANormal.glNormal();
		glVertexAttrib3fARB(S_ATT_vertexTangent, bottomNormal.x(), bottomNormal.y(), bottomNormal.z());
		double* t = &vertices[rightIndices[i]*3];
		glVertex3d(*t, *(t+1), *(t+2));
		//glArrayElement(rightIndices[i]);
	}
	
	for (int i = 0; i < 4; ++i) {
		glTexCoord2dv(&texCoordsEdges[i * 2]);
		sideBNormal.glNormal();
		glVertexAttrib3fARB(S_ATT_vertexTangent, sideANormal.x(), sideANormal.y(), sideANormal.z());
		double* t = &vertices[backIndices[i]*3];
		glVertex3d(*t, *(t+1), *(t+2));
		//glArrayElement(backIndices[i]);
	}

	for (int i = 0; i < 4; ++i) {
		glTexCoord2dv(&texCoordsEdges[i * 2]);
		sideCNormal.glNormal();
		glVertexAttrib3fARB(S_ATT_vertexTangent, topNormal.x(), topNormal.y(), topNormal.z());
		double* t = &vertices[leftIndices[i]*3];
		glVertex3d(*t, *(t+1), *(t+2));
		//glArrayElement(leftIndices[i]);
	}
	glEnd();
	//glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glUniform1iARB(S_UNI_POS[S_UNI_bumpmap], 0.0);	
}

FlatSurface::FlatSurfaceType Triangle::getType() const
{
	return TRIANGLE;
}

void Triangle::updateEdges()
{
	m_edges.clear();
	for (unsigned i = 0; i < getNumVertices() - 1; i++) {
		m_edges.push_back(LineSegment(getVertex(i), getVertex(i + 1)));
	}
	m_edges.push_back(LineSegment(getVertex(getNumVertices() - 1), getVertex(0)));
}
