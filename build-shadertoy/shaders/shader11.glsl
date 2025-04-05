vec3 palette(float d) {
    return mix(vec3(0.2,0.7,0.9), vec3(1.,0.,1.), d);
}

vec2 rotate(vec2 p, float a) {
    float c = cos(a);
    float s = sin(a);
    return p*mat2(c,s,-s,c);
}

vec2 fold(vec2 p) {
  p.xy = abs(p.xy);
  p -= 0.5; 
  return p;
}

float sdOctahedron(vec3 p, float s) {
  p = abs(p);
  float m = p.x + p.y + p.z - s;
  vec3 q;
  if(3.0*p.x < m) q = p.xyz;
  else if(3.0*p.y < m) q = p.yzx;
  else if(3.0*p.z < m) q = p.zxy;
  else return m*0.57735027;
  
  float k = clamp(0.5*(q.z-q.y+s),0.0,s); 
  return length(vec3(q.x,q.y-s+k,q.z-k)); 
}

float map(vec3 p) {
    vec3 q = p;
    float time = iTime*0.5;
    
    for (int i=0; i<3; i++) {
        q = abs(q) - 0.4;
        q.xy = rotate(q.xy,time*0.3);
        q.yz = rotate(q.yz,time*0.1);
    }
    
    float d = sdOctahedron(q,0.4);
    
    return d;
}

vec4 rm(vec3 ro, vec3 rd) {
    float t = 0.0;
    vec3 col = vec3(0.0);
    float d;

    for (float i=0.; i<64.; i++) {
        vec3 p = ro + rd*t;
        d = map(p)*0.5;
        
        if (d<0.02) break;
        if (d>100.) break;
        
        vec2 v = p.xy;
        for (float j=0.; j<3.; j++) {  
          v = fold(v);
          v += 0.7*sin(v.yx*j+iTime)/j + 0.5;
        }
        
        float disk = dot(sign(p), p)/5.;
        float rim = abs(length(p) - 0.7);
        
        float centerDist = length(p);
        float blackHoleFactor = 1.0 - 1.0/(1.0 + exp(3.0 - centerDist*0.5));
        
        col += blackHoleFactor * (
            palette(length(p)*.1)/(400.*(d))
            + vec3(0.3,0.4,1.0)/(1. + 10000.*disk*disk) 
            + vec3(1.0,0.8,0.5)/(1. + 10.*rim*rim)
        );
        
        t += d;
    }
    
    return vec4(col, 1./(d*100.));
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = (fragCoord - (iResolution.xy/2.)) / iResolution.x;
    
    vec3 ro = vec3(0., 0., -50.);
    ro.xz = rotate(ro.xz, iTime);
    vec3 cf = normalize(-ro);
    vec3 cs = normalize(cross(cf, vec3(0.,1.,0.)));
    vec3 cu = normalize(cross(cf,cs));
    
    vec3 uuv = ro + cf*3. + uv.x*cs + uv.y*cu;
    vec3 rd = normalize(uuv - ro);
    
    vec4 col = rm(ro, rd);
    
    float screenDist = length(uv);
    float centerDarkness = 1.0 - 0.95/(1.0 + exp(5.0 - screenDist*15.0));
    
    fragColor = col * centerDarkness;
    
    // Rotating octahedrons
    vec3 octaPos = vec3(sin(iTime*0.5)*2.0, 0.0, cos(iTime*0.5)*2.0);
    float octaDist = sdOctahedron(ro - octaPos, 0.3);
    if (octaDist < 0.1) {
      vec3 octaCol = vec3(0.8,0.6,1.0);
      fragColor.rgb = mix(fragColor.rgb, octaCol, 0.5);
    }
}