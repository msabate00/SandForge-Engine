#version 330 core
in vec2 uv;
out vec4 o;

uniform usampler2D uTex;   // índices R8UI
uniform vec2 uGrid;        // (w,h)
uniform vec2 uView;        // viewport px
uniform vec2 uCamPos;
uniform vec2 uCamSize;
layout(std140) uniform Palette { vec4 colors[256]; vec4 extra[256]; };

// hash determinista por celda
float hash2(ivec2 p){
    uint x = uint(p.x)*374761393u ^ uint(p.y)*668265263u;
    x = (x ^ (x>>13)) * 1274126177u;
    x ^= x >> 16u;
    return float(x & 1023u) / 1023.0; // [0,1]
}

void main(){
  vec2 scale = floor(uView / uCamSize);
  float s = max(1.0, min(scale.x, scale.y));
  vec2 size = uCamSize * s;
  vec2 off  = (uView - size) * 0.5;

  vec2 frag = gl_FragCoord.xy - off;
  if (any(lessThan(frag, vec2(0))) || any(greaterThanEqual(frag, size)))
    discard;

  vec2 uvCam = frag / size;
  vec2 cellf = uCamPos + uvCam * uCamSize;

  if (any(lessThan(cellf, vec2(0))) || any(greaterThanEqual(cellf, uGrid)))
    discard;

  vec2 uvTex = cellf / uGrid;
  ivec2 cellI = ivec2(floor(cellf));
  ivec2 texel = ivec2(cellI.x, int(uGrid.y) - 1 - cellI.y);
  uint m = texelFetch(uTex, texel, 0).r;
  if (m==0u) discard;

  vec4 c = colors[int(m)];
  if (c.a <= 0.0) discard;

  vec3 base_lin = pow(c.rgb, vec3(2.2));

  // -------- variacion de color por celda --------
  ivec2 cellId = ivec2(clamp(floor(cellf), vec2(0), uGrid - 1.0));
  float n = hash2(cellId)*2.0 - 1.0;  // [-1,1]
  float k = 0.15;                     // intensidad
  base_lin = clamp(base_lin * (1.0 + k*n), 0.0, 1.0);
  // ----------------------------------------------

  vec2 cell = fract(cellf); 
  vec2 p = cell - vec2(0.5);        
  float r = length(p);

  float emis = max(extra[int(m)].x, 0.0);

   // --------- Parametros de los puntos ----------
  float radius  = 0.35;
  float feather = 0.30;
  // ----------------------------------------------

  float alpha = 1.0 - smoothstep(radius, radius + feather, r);
  vec3 rgb_lin = base_lin * emis;
  o = vec4(rgb_lin, c.a * alpha);
  if (o.a <= 0.001) discard;
}
