#version 410

layout(location = 0) in vec3 position;

uniform mat4 MVP;
uniform vec3 cameraPosition;
uniform float heightFactor;
uniform vec3 lightPos;
uniform sampler2D rgbTexture;
uniform sampler2D rgbHeight;

out vec2 textureCoordinate;
out vec3 vertexNormal;
out vec3 ToLightVector;
out vec3 ToCameraVector;

vec3 getVertex(float i, float j) {
    float r = 350;
    float pi = 3.14159265359;
    float alpha = 2*pi*i/250;
    float beta = pi*j/125;
    return vec3(r*sin(beta)*cos(alpha), r*sin(beta)*sin(alpha), r*cos(beta))/2;
}

void main(){
    textureCoordinate.x = float(position.x/250);
    textureCoordinate.y = float(position.z/125);
    vec3 pos = getVertex(position.x, position.z);
    vertexNormal = normalize(pos);
    pos.y += heightFactor*texture(rgbHeight, textureCoordinate).xyz.x*vertexNormal.y;
    ToLightVector = normalize(lightPos - pos);
    ToCameraVector = normalize(cameraPosition - pos);
    gl_Position = MVP*vec4(pos, 1.0f);
}
