
#version 460

// output
layout (location = 0) out vec2 fragTexCoord;

layout (push_constant) uniform Constants {
    float zNear;
};


void main() {

    fragTexCoord = vec2( gl_VertexIndex  & 2, (gl_VertexIndex & 1) << 1);
	gl_Position = vec4(fragTexCoord * 2.0f - 1.0f, zNear, 1.0f);
}