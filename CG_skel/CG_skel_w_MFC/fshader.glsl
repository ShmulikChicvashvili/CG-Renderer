#version 150

in vec3 fCamSpace;
in vec3 fNormal;

in vec3 fFaceMid;
in vec3 fFaceNormal;

in mat3 fMaterial;

in vec2 fTexCoords;

in vec3 fColor;

uniform struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec4 pos;
} lights[10];
uniform int numLights;

uniform int uColorMethod; // 0 - flat, 1 - gouraud, 2 - phong

uniform int uTexType; // 0 - No texture. 1 - Color texture. 2 - Normal texture
uniform sampler2D uTexMap;

uniform bool uConstColor;

uniform bool uToon;
uniform bool uSilhouette;

uniform int ticks;
uniform bool animateColor;

out vec4 color;


vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 calculateIlluminationIntensity(vec3 ka, vec3 kd, vec3 ks, vec3 la, vec3 ld, vec3 ls,
	vec3 lightDirection, vec3 norm, vec3 viewDirection) {
	
	float SPEC_SHININESS = 3;
	
	//assert(cmpFloat(length(viewDirection), 1) && viewDirection.w == 0);
	//assert((cmpFloat(length(lightDirection), 1) || lightDirection == vec4(0, 0, 0, 0)) && lightDirection.w == 0);
	//assert(cmpFloat(length(norm), 1) && norm.w == 0);

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
	
	//return vec3(0,0,1);
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
	vec3 lightDir;
	for(int i = 0; i < numLights; i++){
		lightDir = calcLightDirection(i, point);

		if(lightDir == vec3(10,10,10)) return vec3(1,0,1);
		
		color += calculateIlluminationIntensity(ka, kd, ks, 
			lights[i].ambient, lights[i].diffuse, lights[i].specular,
			lightDir, norm, eyeVec);
	}
	return vec3(min(1,color.x),min(1,color.y),min(1,color.z));
}

void main() 
{ 
	vec3 norm;
	vec3 point;
	vec3 ka = fMaterial[0], kd = fMaterial[1], ks = fMaterial[2];
	if (uConstColor){
		color = vec4(0,1,0,1);
		return;
	}
	
	if (uSilhouette){
		color = vec4(0,0,0,1);
		return;
	}
	
	if(uTexType == 1){
		vec3 tex = texture(uTexMap, fTexCoords).xyz;
		//ka = vec3(0.05);
		ka = tex * 0.1;
		kd = tex;
		ks = tex * 0.7;
		//ks = vec3(0.6);
	}
	
	if (uColorMethod == 1) { // Gouraud
		color = vec4(fColor,1);
	}
	
	if(uColorMethod == 0) { // Flat
		norm = fFaceNormal;
		point = fFaceMid;
	} else {
		norm = fNormal;
		point = fCamSpace;
	}
	if (norm != vec3(0.0)){
		norm = normalize(norm);
	}
	
	if (uColorMethod == 0 || uColorMethod == 2){
		color = vec4(calcColor(point, norm, ka, kd, ks),1); 
	}
	
	if (animateColor){
		vec3 tmpColor = rgb2hsv(color.xyz);
		tmpColor.x += ticks * 0.005;
		color = vec4(hsv2rgb(tmpColor),1);
	}
	
	if (uToon){
		int bits = 6;
		float r = floor(color.x * (2 ^ bits)) / (2 ^ bits);
		float g = floor(color.y * (2 ^ bits)) / (2 ^ bits);
		float b = floor(color.z * (2 ^ bits)) / (2 ^ bits);
		color = vec4(r, g, b, 1);
	}
	//if(uTexType == 1){
		//if(kd.x > 0.5 && kd.y > 0.5 && kd.z > 0.5) color = vec4(1,0,0,1);
		//if(kd.x < 0.5 && kd.y < 0.5 && kd.z < 0.5) color = vec4(0,1,0,1);
		//if(kd == vec3(0.0)) color = vec4(0,0,1,1);
	//}
} 

