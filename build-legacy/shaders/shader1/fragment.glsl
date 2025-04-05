precision mediump float;

varying vec2 pos;

void main() {
    vec4 tl = vec4(0.5, 0.1, 0.9, 1.0);
    vec4 tr = vec4(0.1, 1.0, 0.8, 1.0);

    vec4 bl = vec4(0.8, 0.6, 0.1, 1.0);
    vec4 br = vec4(0.7, 0.1, 0.2, 1.0);

    vec4 c = mix(mix(bl, br, pos.x),mix(tl, tr, pos.x), pos.y);
    gl_FragColor = c;
}