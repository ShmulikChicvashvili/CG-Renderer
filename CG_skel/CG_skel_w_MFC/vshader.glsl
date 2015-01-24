#version 150

in vec4 vPosition;
in vec4 vNormal;
in vec4 vFaceNormal;

uniform mat4 uModelviewMtx;
uniform mat4 uNormModelviewMtx;
uniform mat4 uProjMtx;

out vec4 fNormal;
out vec4 fFaceNormal;

void main()
{
	vec4 camSpace = uModelviewMtx * vPosition;
    
	gl_Position = uProjMtx * camSpace;
	fNormal = uNormModelviewMtx * vNormal;
	fFaceNormal = uNormModelviewMtx * vFaceNormal;
}
