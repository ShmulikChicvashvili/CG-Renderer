#version 150

in vec4 vPosition;
in vec4 vNormal;
in vec4 vFaceNormal;
in vec3 vAmbient;
in vec3 vDiffuse;
in vec3 vSpecular;

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

out vec3 fFaceNormal;
out vec3 fNormal;
out mat3 fMaterial;
out vec3 fCamSpace;
out vec3 fColor;


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

void main()
{
	if (vPosition.w == 0){
		gl_Position = uProjMtx * uNormModelviewMtx * uModelviewMtx * (vPosition + vNormal + vFaceNormal + vec4(vAmbient,0) + vec4(vDiffuse,0) + vec4(vSpecular,0));
		return;
	}
	vec4 camSpace = uModelviewMtx * vPosition;
    
	gl_Position = uProjMtx * camSpace;
	
	fMaterial = mat3(vAmbient,vDiffuse,vSpecular);
	
	fFaceNormal = normalize((uNormModelviewMtx * vFaceNormal).xyz);
	
	fNormal = normalize((uNormModelviewMtx * vNormal).xyz);
	
	fCamSpace = camSpace.xyz;
	
	
	vec3 eyeVec = calcEyeDirection(fCamSpace);
	fColor = vec3(0.0);
	for(int i = 0; i < numLights; i++){
		vec3 lightDir = calcLightDirection(i, fCamSpace);
		
		fColor += calculateIlluminationIntensity(vAmbient, vDiffuse, vSpecular, 
			lights[i].ambient, lights[i].diffuse, lights[i].specular,
			lightDir, fNormal, eyeVec);
	}
	fColor = vec3(min(1,fColor.x),min(1,fColor.y),min(1,fColor.z));
}
