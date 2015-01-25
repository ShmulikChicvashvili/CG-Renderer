#version 150

in vec4 fNormal;
in vec4 fFaceNormal;
in mat3 fMaterial;

out vec4 fColor;

void main() 
{ 
	fColor = vec4(fMaterial[0],1);
} 

