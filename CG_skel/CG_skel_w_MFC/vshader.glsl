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

out vec4 fNormal;
out vec4 fFaceNormal;
out mat3 fMaterial;

void main()
{
	if (vPosition.w == 0){
		gl_Position = uProjMtx * uNormModelviewMtx * uModelviewMtx * (vPosition + vNormal + vFaceNormal + vec4(vAmbient,0) + vec4(vDiffuse,0) + vec4(vSpecular,0));
		return;
	}
	vec4 camSpace = uModelviewMtx * vPosition;
    
	gl_Position = uProjMtx * camSpace;
	fNormal = uNormModelviewMtx * vNormal;
	fFaceNormal = uNormModelviewMtx * vFaceNormal;
	
	fMaterial = mat3(vAmbient,vDiffuse,vSpecular);
}
