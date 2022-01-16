#version 330 core
out vec4 FragColor;

in vec2 fragTexCoord;
in vec4 fragColor;

uniform vec3 coord; // vec3: coordinate.xy, direction
uniform vec2 mapSize; 

#define PI 3.14159265359

void main()
{
    float alpha = 0.95f;

    vec2 pos;
    pos.x = gl_FragCoord.x;
    pos.y = abs(gl_FragCoord.y - mapSize.y);

    float fragAngle = atan(coord.y - pos.y, coord.x - pos.x);
    fragAngle -= PI;
 

    bool ust = (fragAngle > (coord.z - PI/6) ) || (fragAngle < ( (coord.z + PI/6) - 2 * PI) );
    bool alt = (fragAngle < (coord.z + PI/6) ) || (fragAngle > ( (coord.z - PI/6) + 2 * PI) );


    if( ust && alt)
    
    {
       
        vec2 newMapSize = mapSize;

        alpha = length(distance(pos, coord.xy) / newMapSize );
        alpha = clamp(alpha, 0.0f, 0.95f);
    }

    FragColor = vec4(vec3(0.0f), alpha);
}