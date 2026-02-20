#ifdef GL_ES
precision mediump float;
#endif

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture0;
uniform float time;

void main()
{
    vec4 texColor = texture2D(texture0, fragTexCoord);
    float wave = (gl_FragCoord.x - gl_FragCoord.y) * 0.02;
    float t = time;
    float r = 0.5 + 0.5 * sin(wave - t);
    float g = 0.5 + 0.5 * sin(wave - t + 2.094);
    float b = 0.5 + 0.5 * sin(wave - t + 4.188);
    vec3 rainbow = vec3(r, g, b);
    gl_FragColor = vec4(rainbow, texColor.a);
}
