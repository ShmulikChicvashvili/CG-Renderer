#version 150

in vec4 vPosition;
in vec4 vNormal;
in vec4 vTangent;
in vec4 vBitangent;

in vec4 vFaceMid;
in vec4 vFaceNormal;

in vec3 vAmbient;
in vec3 vDiffuse;
in vec3 vSpecular;

in vec2 vTexCoords;

uniform mat4 uModelviewMtx;
uniform mat4 uNormModelviewMtx;
uniform mat4 uProjMtx;

uniform struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec4 pos;
} lights[10];
uniform int numLights;

uniform int ticks;
uniform bool animateVertex;

out vec3 fCamSpace;
out vec3 fNormal;

out vec3 fFaceMid;
out vec3 fFaceNormal;

out mat3 TBN;

out mat3 fMaterial;

out vec2 fTexCoords;

out vec3 fColor;

float rand(vec2 co){
    return fract(sin(dot(co ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 calculateIlluminationIntensity(vec3 ka, vec3 kd, vec3 ks, vec3 la, vec3 ld, vec3 ls,
	vec3 lightDirection, vec3 norm, vec3 viewDirection) {
	
	float SPEC_SHININESS = 3;
	
	/*assert(cmpFloat(length(viewDirection), 1) && viewDirection.w == 0);
	assert((cmpFloat(length(lightDirection), 1) || lightDirection == vec4(0, 0, 0, 0)) && lightDirection.w == 0);
	assert(cmpFloat(length(norm), 1) && norm.w == 0);*/

	vec3 illuminationIntensity = vec3(0.0); 
	// Ambient illumination
	illuminationIntensity += ka * la;
	// Defuse illumination
	illuminationIntensity += kd * max((dot(lightDirection, norm)),0) * ld;
	// Specular illumination
	vec3 R = normalize(reflect(-lightDirection, norm));
	float temp = max(dot(R, viewDirection), 0);
	illuminationIntensity += ks * (pow(temp, SPEC_SHININESS)) * ls;
	return illuminationIntensity;
}

vec3 calcLightDirection(int i, vec3 point ){
	vec3 dir;
	
	if(lights[i].pos.w == 0){
		dir = -lights[i].pos.xyz;
	} else {
		dir = lights[i].pos.xyz - point;
		if (dir != vec3(0.0)){
			dir = normalize(dir);
		}
	}
	
	return dir;
}

vec3 calcEyeDirection(vec3 point){
	vec3 dir = -point;
	if (dir != vec3(0.0)){
		dir = normalize(dir);
	}
	
	return dir;
}

vec3 calcColor(vec3 point, vec3 norm, vec3 ka, vec3 kd, vec3 ks){
	vec3 eyeVec = calcEyeDirection(point);
	vec3 color = vec3(0.0);
	for(int i = 0; i < numLights; i++){
		vec3 lightDir = calcLightDirection(i, point);
		
		color += calculateIlluminationIntensity(ka, kd, ks, 
			lights[i].ambient, lights[i].diffuse, lights[i].specular,
			lightDir, norm, eyeVec);
	}
	return vec3(min(1,color.x),min(1,color.y),min(1,color.z));
}

void main()
{
	fTexCoords = vTexCoords;
	//if (vPosition.w == 0){
	//	gl_Position = uProjMtx * uNormModelviewMtx * uModelviewMtx * (vPosition + vNormal + vFaceNormal + vec4(vAmbient,0) + vec4(vDiffuse,0) + vec4(vSpecular,0) + vFaceMid);
	//	return;
	//}
	
	vec4 pos = vPosition;
	vec4 faceMid = vFaceMid;
	vec4 norm = vNormal;
	vec4 faceNorm = vFaceNormal;
	vec4 tangent = vTangent;
	vec4 bitangent = vBitangent;
	
	if (animateVertex){
		mat4 resize = mat4(1,0,0,0,
							0,1,0,0,
							0,0,1,0,
							0,0,0,1);
		//resize = resize * (0.5 + ticks / 100.0);
		
		resize = resize * (sin(radians((ticks / 200.0) * 360.0)) + 2) * 0.5;
				
		float deg = ticks * 360.0 / 100.0;
		float rad = radians(deg);
		
		mat4 rotate = mat4(cos(rad), 0, sin(rad), 0,
							0, 1, 0, 0,
							-sin(rad),0, cos(rad), 0,
							0, 0, 0, 1);
		
		pos = vec4((rotate * resize * pos).xyz,1);
		faceMid = vec4((rotate * resize * faceMid).xyz,1);
		norm = rotate * norm;
		faceNorm = rotate * faceNorm;
		tangent = rotate * tangent;
		bitangent = rotate * bitangent;
	}
	
	vec4 camSpace = uModelviewMtx * pos;
    
	gl_Position = uProjMtx * camSpace;

	fCamSpace = camSpace.xyz;
	fNormal = normalize((uNormModelviewMtx * norm).xyz);
	
	fFaceMid = (uModelviewMtx * faceMid).xyz;
	fFaceNormal = normalize((uNormModelviewMtx * faceNorm).xyz);
	
	
	TBN[0] = normalize((uNormModelviewMtx * tangent).xyz);
	TBN[1] = normalize((uNormModelviewMtx * bitangent).xyz);
	TBN[2] = fNormal;
		
	fMaterial = mat3(vAmbient,vDiffuse,vSpecular);
	
	
	fColor = calcColor(fCamSpace, fNormal, vAmbient, vDiffuse, vSpecular);
}
