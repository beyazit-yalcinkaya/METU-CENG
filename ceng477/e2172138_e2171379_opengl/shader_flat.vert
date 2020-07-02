#version 410

layout(location = 0) in vec3 position;

uniform mat4 MVP;
uniform vec3 cameraPosition;
uniform float heightFactor;
uniform vec3 lightPos;
uniform sampler2D rgbTexture;
uniform sampler2D rgbHeight;
uniform int widthTexture;
uniform int heightTexture;

out vec2 textureCoordinate;
out vec3 vertexNormal;
out vec3 ToLightVector;
out vec3 ToCameraVector;

vec3 getVertex(float x, float z) {
    vec2 temp;
    temp.x = 1.0f - float(x)/float(widthTexture);
    temp.y = 1.0f - float(z)/float(heightTexture);
    return vec3(x, heightFactor*texture(rgbHeight, temp).xyz.x, z);
}

void main(){
    textureCoordinate.x = 1.0f - float(position.x)/float(widthTexture);
    textureCoordinate.y = 1.0f - float(position.z)/float(heightTexture);
    vec3 pos = getVertex(position.x, position.z);
    vec2 textureCoordinateNeighbor;
    vec3 normal;
    vec3 neighbor0, neighbor1, neighbor2, neighbor3, neighbor4, neighbor5;
    vec3 v0, v1, v2, v3, v4, v5;
    if (position.x == 0) {
        if (position.z == 0) {
            vec3 neighbor0 = getVertex(position.x + 1, position.z);
            vec3 neighbor1 = getVertex(position.x,     position.z + 1);
            vec3 v0 = neighbor0 - pos;
            vec3 v1 = neighbor1 - pos;
            normal = normalize(cross(v1, v0));
        } else if (position.z == heightTexture - 1) {
            vec3 neighbor0 = getVertex(position.x + 1, position.z);
            vec3 neighbor1 = getVertex(position.x + 1, position.z - 1);
            vec3 neighbor2 = getVertex(position.x,     position.z - 1);
            vec3 v0 = neighbor0 - pos;
            vec3 v1 = neighbor1 - pos;
            vec3 v2 = neighbor2 - pos;
            normal = normalize((normalize(cross(v0, v1))
                              + normalize(cross(v1, v2)))/2);
        } else {
            neighbor0 = getVertex(position.x,     position.z - 1);
            neighbor1 = getVertex(position.x + 1, position.z - 1);
            neighbor2 = getVertex(position.x + 1, position.z);
            neighbor3 = getVertex(position.x,     position.z + 1);
            vec3 v0 = neighbor0 - pos;
            vec3 v1 = neighbor1 - pos;
            vec3 v2 = neighbor2 - pos;
            vec3 v3 = neighbor3 - pos;
            normal = normalize((normalize(cross(v1, v0))
                              + normalize(cross(v2, v1))
                              + normalize(cross(v3, v2)))/3);
        }

    } else if (position.x == widthTexture - 1) {
        if (position.z == 0) {
            vec3 neighbor0 = getVertex(position.x - 1, position.z);
            vec3 neighbor1 = getVertex(position.x - 1, position.z + 1);
            vec3 neighbor2 = getVertex(position.x,     position.z + 1);
            vec3 v0 = neighbor0 - pos;
            vec3 v1 = neighbor1 - pos;
            vec3 v2 = neighbor2 - pos;
            normal = normalize((normalize(cross(v0, v1)
                              + normalize(cross(v1, v2))))/2);
        } else if (position.z == heightTexture - 1) {
            vec3 neighbor0 = getVertex(position.x - 1, position.z);
            vec3 neighbor1 = getVertex(position.x,     position.z - 1);
            vec3 v0 = neighbor0 - pos;
            vec3 v1 = neighbor1 - pos;
            normal = normalize(cross(v1, v0));
        } else {
            neighbor0 = getVertex(position.x,     position.z + 1);
            neighbor1 = getVertex(position.x - 1, position.z + 1);
            neighbor2 = getVertex(position.x - 1, position.z);
            neighbor3 = getVertex(position.x,     position.z - 1);
            vec3 v0 = neighbor0 - pos;
            vec3 v1 = neighbor1 - pos;
            vec3 v2 = neighbor2 - pos;
            vec3 v3 = neighbor3 - pos;
            normal = normalize((normalize(cross(v1, v0))
                              + normalize(cross(v2, v1))
                              + normalize(cross(v3, v2)))/3);
        }
    } else {
        if (position.z == 0) {
            neighbor0 = getVertex(position.x + 1, position.z);
            neighbor1 = getVertex(position.x,     position.z + 1);
            neighbor2 = getVertex(position.x - 1, position.z + 1);
            neighbor3 = getVertex(position.x - 1, position.z);
            vec3 v0 = neighbor0 - pos;
            vec3 v1 = neighbor1 - pos;
            vec3 v2 = neighbor2 - pos;
            vec3 v3 = neighbor3 - pos;
            normal = normalize((normalize(cross(v1, v0))
                              + normalize(cross(v2, v1))
                              + normalize(cross(v3, v2)))/3);
        } else if (position.z == heightTexture - 1) {
            neighbor0 = getVertex(position.x - 1, position.z);
            neighbor1 = getVertex(position.x, position.z - 1);
            neighbor2 = getVertex(position.x + 1, position.z - 1);
            neighbor3 = getVertex(position.x + 1, position.z);
            vec3 v0 = neighbor0 - pos;
            vec3 v1 = neighbor1 - pos;
            vec3 v2 = neighbor2 - pos;
            vec3 v3 = neighbor3 - pos;
            normal = normalize((normalize(cross(v1, v0))
                              + normalize(cross(v2, v1))
                              + normalize(cross(v3, v2)))/3);
        } else {
            neighbor0 = getVertex(position.x - 1, position.z);
            neighbor1 = getVertex(position.x - 1, position.z + 1);
            neighbor2 = getVertex(position.x,     position.z + 1);
            neighbor3 = getVertex(position.x + 1, position.z);
            neighbor4 = getVertex(position.x + 1, position.z - 1);
            neighbor5 = getVertex(position.x,     position.z - 1);
            vec3 v0 = neighbor0 - pos;
            vec3 v1 = neighbor1 - pos;
            vec3 v2 = neighbor2 - pos;
            vec3 v3 = neighbor3 - pos;
            vec3 v4 = neighbor4 - pos;
            vec3 v5 = neighbor5 - pos;
            normal = normalize((normalize(cross(v0, v1))
                              + normalize(cross(v1, v2))
                              + normalize(cross(v2, v3))
                              + normalize(cross(v3, v4))
                              + normalize(cross(v4, v5))
                              + normalize(cross(v5, v0)))/6);
        }
    }
    ToLightVector = normalize(lightPos - pos);
    ToCameraVector = normalize(cameraPosition - pos);
    vertexNormal = normal;
   gl_Position = MVP*vec4(pos, 1.0f);
}
