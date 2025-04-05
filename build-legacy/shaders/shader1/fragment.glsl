precision mediump float;
varying vec2 pos;
uniform sampler2D background;
uniform float millis; 
void main() {

    vec2 newPos = pos;
    newPos.y = 1. - newPos.y; // flip y axis
    
    newPos.x = newPos.x + (sin(millis / 1000.0) * 0.1);

    vec4 col = texture2D(background, newPos);


    gl_FragColor = vec4(col);
}
