#version 150

in vec3 fNormal;
in vec3 fFaceNormal;
in mat3 fMaterial;
in vec3 fCamSpace;
in vec3 fColor;

out vec4 color;

void main() 
{ 
	color = vec4(fMaterial[0],1);
	color = vec4(fColor,1);
} 

