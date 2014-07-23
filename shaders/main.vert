#define PI 3.1415926535
uniform vec3 light0Pos;
uniform bool lighting;
uniform bool bumpmap;
uniform bool shouldParticle;
uniform float time;
uniform bool isSphere;

varying float lightIntensity;
varying vec3 tspaceLight0Vec;
varying vec3 tspaceEyeVec;

attribute vec3 vertexTangent;
attribute vec3 vertexVelocity;
attribute float vertexStartTime;

void main()
{
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

	if (shouldParticle) {
		// multiply velocity by the time difference
		vec4 particlePos = gl_Vertex + vec4(vertexVelocity * (time - vertexStartTime), 0.0);
		
		// move world position into eye coord position
		gl_Position = gl_ModelViewProjectionMatrix * particlePos;
		gl_FrontColor = gl_Color;
	} else {
		// transform the position using fixed functionality transform
		gl_Position = ftransform();
		gl_FrontColor = gl_Color;

		if (lighting) {
			// get the vertex coord in eye coordinate space
			vec4 eyeCoordPosition = gl_ModelViewMatrix * gl_Vertex;
			// transform and normalize the normal
			vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
			
			// grab the vector from our lightsource to this vertex
			vec4 eyeSpaceLightLoc = gl_LightSource[0].position;
			vec4 lightVector = normalize(eyeSpaceLightLoc - eyeCoordPosition);
			
			// get viewing vector, because camera is at 0,0,0 this is the negated eyeCoordPosition
			vec3 viewVector = normalize(vec3(-eyeCoordPosition));
	
			if (bumpmap) {
				vec3 vertexTangentTemp;
				if (isSphere) {
					// assume all vertex tangents lie in the x z plane
					float angle = acos(gl_Vertex.x);
					if (gl_Vertex.z > 0.0) {
						angle =  (2.0 * PI) - angle;
					}
					// derivative in the x z plane creates our texture tangent
					vertexTangentTemp = normalize(vec3(-sin(angle), 0, -cos(angle)));
				} else {
					vertexTangentTemp = vertexTangent;
				}
				vec3 t = normalize(gl_NormalMatrix * vertexTangentTemp);
				vec3 b = cross(normal, t);
				
				// get a vector to the lightsource in tangent space
				tspaceLight0Vec.x = dot(lightVector.xyz, t);
				tspaceLight0Vec.y = dot(lightVector.xyz, b);
				tspaceLight0Vec.z = dot(lightVector.xyz, normal);
				
				// get a vector to the camera in tangent space
				eyeCoordPosition = -eyeCoordPosition;
				tspaceEyeVec.x = dot(eyeCoordPosition.xyz, t);
				tspaceEyeVec.y = dot(eyeCoordPosition.xyz, b);
				tspaceEyeVec.z = dot(eyeCoordPosition.xyz, normal);
			} else {
				// get the outgoing reflection vector, negating the light vector
				vec3 reflectionVector = reflect(-lightVector.xyz, normal);
				
				/**
				// compute ambient component
				vec4 ambientComponent = gl_FrontLightProduct[0].ambient;

				// compute diffuse component
				float vD = max(dot(lightVector.xyz, normal), 0.0);
				vec4 diffuseComponent = gl_FrontLightProduct[0].diffuse * vD;

				// compute specular component
				vec4 specularComponent = vec4(0.0);
				if (vD > 0.0) {
					specularComponent = gl_FrontLightProduct[0].specular * 
						pow(clamp(dot(reflectionVector, viewVector), 0.0, 1.0), gl_FrontMaterial.shininess); 
				}
				**/
				float specular = 0.0;
				float diffuse = max(dot(lightVector.xyz, normal), 0.0);
				if (diffuse > 0.0) {
					specular = pow(clamp(dot(reflectionVector, viewVector), 0.0, 1.0), 16.0);	
				}
				
				//gl_FrontColor = gl_FrontLightModelProduct.sceneColor + ambientComponent + diffuseComponent + specularComponent;
				
				// lightIntensity = diffuse component + specular component
				lightIntensity = 0.75 * diffuse + 0.5 * specular;
			}
			
		}

	}
}
