uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform bool lighting;
uniform bool bumpmap;
uniform bool shouldTexture;
uniform bool shouldParticle;
uniform bool shouldModulate;
varying float lightIntensity;
varying vec3 tspaceLight0Vec;
varying vec3 tspaceEyeVec;

void main()
{
	vec4 baseColor;
	if (shouldTexture) {
		if (shouldModulate) {
			baseColor = gl_Color * texture2D(tex0, gl_TexCoord[0].st);
		} else {
			baseColor = texture2D(tex0, gl_TexCoord[0].st);
		}
	} else {
		baseColor = gl_Color;
	}
	
	if (lighting) {
		vec4 ambient = baseColor * 0.15;
		if (bumpmap) {
			vec3 bumpNormal = normalize((texture2D(tex1, gl_TexCoord[0].st).xyz * 2.0) - 1.0);
			vec3 normEye = normalize(tspaceEyeVec);
			
			/**
			// incomplete parallax mapping
			float scale = 10.0;
			float bias = 0.0;
			float newHeight = (((texture2D(tex2, gl_TexCoord[0].st).r) / 256.0) * scale) + bias;
			vec2 newTexCoords = (gl_TexCoord[0].st) + (normEye.xy * newHeight);
			baseColor = texture2D(tex0, newTexCoords);
			bumpNormal = normalize((texture2D(tex1, gl_TexCoord[0].st).xyz * 2.0) - 1.0);
			**/
			
			float diffuse = max(dot(tspaceLight0Vec, bumpNormal), 0.0);
			vec3 lightVec = normalize(tspaceLight0Vec);
			float specular = pow(clamp(dot(reflect(-lightVec, bumpNormal), normEye), 0.0, 1.0), 20.0);
			vec4 specColor = gl_FrontMaterial.specular;
			
			gl_FragColor = ambient + 0.75 * (diffuse * baseColor) + 0.5 * (specular * specColor);
		} else {
			vec4 litTexture = lightIntensity * baseColor;
			gl_FragColor = ambient + litTexture;
		}
	} else {
		gl_FragColor = baseColor;
	}
}