#version 150

in vec4 vPosition;
in vec4 vNormal;
in vec4 vFaceNormal;
in mat3 vMaterial;

uniform mat4 uModelviewMtx;
uniform mat4 uNormModelviewMtx;
uniform mat4 uProjMtx;

out vec4 fNormal;
out vec4 fFaceNormal;
out mat3 fMaterial;

void main()
{
	if (vPosition.w == 0){
		gl_Position = uProjMtx * uNormModelviewMtx * uModelviewMtx * (vPosition + vNormal + vFaceNormal + vec4(vMaterial[0],0) + vec4(vMaterial[1],0) + vec4(vMaterial[2],0));
		return;
	}
	vec4 camSpace = uModelviewMtx * vPosition;
    
	gl_Position = uProjMtx * camSpace;
	fNormal = uNormModelviewMtx * vNormal;
	fFaceNormal = uNormModelviewMtx * vFaceNormal;
	
	fMaterial = vMaterial;
}
