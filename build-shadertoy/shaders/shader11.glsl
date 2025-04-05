// shader_custom.glsl

// This shader uses the ShaderToy uniforms:
// iResolution, iTime, iMouse

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    // Normalize coordinates (0 to 1)
    vec2 uv = fragCoord / iResolution.xy;

    // Create a time-varying color
    vec3 color = vec3(0.5 + 0.5 * cos(iTime + uv.xyx * 6.2831));

    // Mix in some mouse influence if the left button is pressed.
    // iMouse.xy holds the current mouse coordinates.
    if(iMouse.z > 0.0) {
        vec2 mouseUV = iMouse.xy / iResolution.xy;
        // Blend the computed color with a fixed color based on the mouse location.
        color = mix(color, vec3(mouseUV, 0.5), 0.5);
    }

    // Output the final color with full opacity.
    fragColor = vec4(color, 1.0);
}