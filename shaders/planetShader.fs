#version 330 core
out vec4 FragColor;

in vec2 fragTexCoord;
in vec4 fragColor;

uniform vec2 coord; // vec3: coordinate.xy
uniform float rOuter;
uniform float rInner;

#define PI 3.14159265359

bool isInCircle(vec2 center, float r, vec2 point){
    return (point.x - center.x)*(point.x - center.x) + (point.y - center.y)*(point.y - center.y) < radius*radius;
}

void main()
{
    
    bool innerCirlceCheck = isInCircle(coord,rInner,gl_FragCoord); // Should be false
    bool outerCirlceCheck = isInCircle(coord,rOuter,gl_FragCoord); // Should be true

    float newBrightness = fragColor.a;

    if(innerCirlceCheck == false && outerCirlceCheck == true){
                
    }
    
    FragColor = vec4(fragColor.xyz, newBrightness);
}