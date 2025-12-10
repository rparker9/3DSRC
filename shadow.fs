#version 330

out vec4 fragColor;

void main()
{
    // Use gl_FragCoord.z which is the depth in [0,1] range after perspective divide
    // For orthographic projection, this is already linear
    float depth = gl_FragCoord.z;
    
    // Write depth to color buffer (R, G, B channels all same value)
    fragColor = vec4(depth, depth, depth, 1.0);
}