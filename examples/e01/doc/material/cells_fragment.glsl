#version 150 core

in vec3 fPosition;
in vec2 fUV;
in vec2 fUV1;
in vec2 fUV2;
in vec3 fNormal;
in vec2 fPos2D;

//in vec3 fPosRad1Adj;
//out color
out vec4 outColor;  //

//tiles meta data , e.g. r is type of tile.
uniform sampler2D tex_t;//0
//
uniform sampler2D tex_o;//1 ocean
uniform sampler2D tex_s;//2 shore
uniform sampler2D tex_p;//3 plain
uniform sampler2D tex_h;//4 hill
uniform sampler2D tex_m;//5 mountain
uniform sampler2D tex_f;//6 frozen
uniform sampler2D tex_b;//7 beach
uniform sampler2D tex_l;//8 lake
uniform sampler2D tex_fo;//9 fogOfWar
uniform sampler2D tex_w2;//10, world tex 2, find cell key of the rect centre.
uniform sampler2D tex_cs;//11 cells
uniform sampler2D tex_i;//12 ice

//
uniform int showCellEdge;
uniform int showRegionEdge;
uniform vec3 cameraPos;
uniform vec2 actorPos2D;
uniform int debug;
uniform int leiout;
uniform int cellsCols;
uniform int cellsRows;
//

//
const int TT_FRZ_MOUNTAIN = 12;
const int TT_FRZ_PLAIN = 11;
const int TT_FRZ_SHORE = 10;
const int TT_RIVER = 9;
const int TT_LAKE = 8;
const int TT_BEACH = 7;// 
const int TT_MOUNTAIN = 5;
const int TT_HILL = 4;
const int TT_PLAIN = 3;
const int TT_SHORE = 2;
const int TT_OCEAN = 1;
const int TT_UNKNOW = 0;
//

const int SHOW_CELL_EDGE = 1 << 0;

const float FRZ_MOUNTAIN_FOOT = 55.0;//y position below which the frozen mountain becomes normal mountain.

const float BEACH_HEAD = 50.1;//y position below which the frozen mountain becomes normal mountain.
const float BEACH_FOOT = 49.56;//y position below which the frozen mountain becomes normal mountain.

const int SHOW_REGION_EDGE_TYPES_1 = 1 << 0;
const int SHOW_REGION_EDGE_TYPES_2 = 1 << 1;
const int SHOW_REGION_EDGE_TYPES_3 = 1 << 2;

const float sqrt3 = sqrt(3.0);
const float sin60 = sqrt3 / 2.0;
const float cos60 = 0.5;
const float sin30 = 0.5;
const float cos30 = sqrt3 / 2.0;
const float tan60 = sqrt3;
const float tan30 = 1.0 / sqrt3;

//
const int LT_POINTY_TOP = 0; //pointy top layout
const int LT_FLAT_TOP = 1;//flat top layout

bool isPointyTop() {
    return leiout == LT_POINTY_TOP;
}

bool isFlatTop() {
    return leiout == LT_FLAT_TOP;
}

float unitWidth = (isPointyTop() ? 2.0 : sqrt3); //norm refers to that rad ==1 and tiles == 1 
float unitHeight = (isPointyTop() ? sqrt3 : 2.0); //norm refers to rad == 1 and tiles == 1

struct light {
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
};
// 
ivec2 cubeRound(float q, float r) {
    // Cube
    float x = q;
    float z = r;
    float y = -x - z;
    // 
    int rx = int(round(x));
    int ry = int(round(y));
    int rz = int(round(z));

    float dx = abs(rx - x);
    float dy = abs(ry - y);
    float dz = abs(rz - z);

    if(dx > dy && dx > dz) {
        rx = -ry - rz;
    } else if(dy > dz) {
        ry = -rx - rz;
    } else {
        rz = -rx - ry;
    }

    // Fix rounding to ensure q + r + s == 0
    if(isPointyTop()) {
        int row = rz;
        int col = rx + (row - (row & 1)) / 2;
        return ivec2(col, row);
    } else {
        int col = rx;
        int row = rz + (col - (col & 1)) / 2;
        return ivec2(col, row);
    }
}

ivec2 getCellKey(vec2 pos2D, float rad) {
    float R = (2.0 / sqrt3) * rad;
    // 
    // Step 1: (q, r)
    if(isPointyTop()) {
        float q = ((sqrt3 / 3.0) * pos2D.x - (1.0 / 3.0) * pos2D.y) / R;
        float r = ((2.0 / 3.0) * pos2D.y) / R;
        return cubeRound(q, r);
    } else {
        float q = ((2.0 / 3.0) * pos2D.x) / R;
        float r = ((sqrt3 / 3.0) * pos2D.y - (1.0 / 3.0) * pos2D.x) / R;
        return cubeRound(q, r);
    }
}

bool isOcean(int type) {
    return type == TT_OCEAN;
}

bool isShore(int type) {
    return type == TT_SHORE;
}

bool isPlain(int type) {

    return type == TT_PLAIN;
}

bool isMountain(int type) {
    return type == TT_MOUNTAIN;
}

bool isHill(int type) {
    return type == TT_HILL;
}

bool isFRZShore(int type) {
    return type == TT_FRZ_SHORE;
}
bool isFRZPlain(int type) {
    return type == TT_FRZ_PLAIN;

}

bool isFRZMountain(int type) {
    return type == TT_FRZ_MOUNTAIN;
}

bool isLake(int type) {
    return type == TT_LAKE;
}

bool isBeach(int type) {
    return type == TT_BEACH;
}

bool isOneOfType(ivec3 types, int type) {
    return types.x == type || types.y == type || types.z == type;
}

//ckey to centre point.
vec2 getOrigin2D(ivec2 cKey, float rad) {
    if(leiout == LT_POINTY_TOP) {

        float centerX = cKey.x * rad * unitWidth + (cKey.y % 2 == 0 ? 0 : rad);
        float centerY = cKey.y * rad * unitHeight;
        return vec2(centerX, centerY);
    } else if(leiout == LT_FLAT_TOP) {
        float centerX = cKey.x * rad * unitWidth;
        float centerY = cKey.y * rad * unitHeight + (cKey.x % 2 == 0 ? 0 : rad);
        return vec2(centerX, centerY);
    }
}

float distanceToCell() {
    float rad = 1.0;
    ivec2 cKey = getCellKey(fPos2D, rad);
    vec2 cIn2D = getOrigin2D(cKey, rad);

    return distance(cIn2D, fPos2D) / rad;
}

float rotateNeg60GetX(vec2 p) {
    const float c = cos60;               // cos(-60°)
    const float s = -sin60;   // sin(-60°)
    return c * p.x - s * p.y;
}

float rotateNeg30GetX(vec2 p) {
    const float c = cos30;               // cos(-30°)
    const float s = -sin30;   // sin(-30°)
    return c * p.x - s * p.y;
}

//
float distanceToEdgeByCKey(ivec2 cKey) {
    float rad = 1.0;
    vec2 cIn2D = getOrigin2D(cKey, rad);
    vec2 p = fPos2D - cIn2D;
    p = abs(p);

    if(isPointyTop()) {
        if(p.y / p.x > tan30) {
            return rotateNeg60GetX(p) - rad;
        }
        return p.x - rad; // < 0 inner, > 0 outer.
    } else {
        if(p.y / p.x < tan60) {
            return rotateNeg30GetX(p) - rad;
        }
        return p.y - rad;
    }

}

float distanceToActorCross(float rad1, float rad2) {
    vec2 p = abs(fPos2D - actorPos2D);
    float len = length(p);
    if(len < rad1) {
        return -len;
    }
    if(len > rad2) {
        return len;
    }

    if(p.y > p.x) {
        return p.x;
    }

    return p.y;
}

//TODO use a mask? 16 bits?
//TODO use a struct to store type's attributes, material, spec
vec4 getTypeColorFromTex(int type0) {
    vec4 color;
    if(isFRZMountain(type0)) {
        color = texture(tex_f, fUV1);
    } else if(isFRZShore(type0)) {
        color = texture(tex_f, fUV1);
    } else if(isFRZPlain(type0)) {
        color = texture(tex_f, fUV1);
    } else if(isMountain(type0)) {
        color = texture(tex_m, fUV1);
    } else if(isHill(type0)) {
       color = texture(tex_h, fUV1);
    } else if(isPlain(type0)) {
        color = texture(tex_p, fUV1);
    } else if(isShore(type0)) {
        color = texture(tex_s, fUV1); //shore itself
    } else if(isOcean(type0)) {
        color = texture(tex_o, fUV1);
    } else if(isLake(type0)) {
        color = texture(tex_l, fUV1);
    } else if(isBeach(type0)) {
        color = texture(tex_b, fUV1);
    }
    return color;
}

// MIT License - from https://github.com/ashima/webgl-noise
vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

float simplexNoise(vec2 v) {
    const vec4 C = vec4(0.211324865405187, 0.366025403784439,
                        -0.577350269189626, 0.024390243902439);
    vec2 i  = floor(v + dot(v, C.yy));
    vec2 x0 = v -   i + dot(i, C.xx);

    vec2 i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;

    i = mod289(i); // Avoid truncation effects in permutation
    vec3 p = permute(permute(i.y + vec3(0.0, i1.y, 1.0))
                   + i.x + vec3(0.0, i1.x, 1.0));

    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
                            dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;

    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;

    m *= 1.79284291400159 - 0.85373472095314 * (a0*a0 + h*h);

    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}
const float u_NoiseScale = 10;
const float u_NoiseStrength = 0.6;
float getHeightOffset(vec2 pos) {
    float h = simplexNoise(pos * u_NoiseScale);
    return (h - 0.5) * u_NoiseStrength; // [-0.5, 0.5] → [-s/2, s/2]
}
vec3 computePerturbedNormal(vec3 worldPos) {
    const float eps = 0.01; // 微小偏移量

    float h0 = getHeightOffset(worldPos.xz);

    // X 方向偏移
    float hx = getHeightOffset(worldPos.xz + vec2(eps, 0.0));
    // Z 方向偏移
    float hz = getHeightOffset(worldPos.xz + vec2(0.0, eps));

    // 梯度 = (dh/dx, 1, dh/dz) 的反方向（因为法线垂直于表面）
    vec3 normal = normalize(vec3(
        (h0 - hx) / eps,   // -dh/dx
        1.0,
        (h0 - hz) / eps    // -dh/dz
    ));

    return normal;
}
vec3 applyLight(int type, vec3 rgb, bool applySpec) {
    //aply light
    vec3 theNormal = fNormal;
    if(isPlain(type) || isHill(type)){
        theNormal = computePerturbedNormal(fPosition);
    }
    vec3 N = normalize(theNormal);           //
    // light direction.
    vec3 L = normalize(vec3(0.25, 1.0, -1.5)); 

    //view direction, depends on camera position.
    vec3 V = normalize(cameraPos - fPosition);
    //amb
    float matAmb = 0.5;
    //diff
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * rgb;
    //ambient 
    vec3 ambient = matAmb * rgb;
    //apply light.
    vec3 rgb2 = ambient + diffuse;//;
    if(applySpec) {

    //spec
        vec3 LV = normalize(L + V);
        float matShiness = 0.1;
        float matSpec = 0.2;
        if(isOcean(type) || isShore(type) || isLake(type) || isShore(type)) {
            matShiness = 300;
            matSpec = 1.5;
            matAmb = 0.2;
        } else if(isFRZMountain(type) || isFRZPlain(type) || isFRZShore(type)) {
            matShiness = 75;
            matSpec = .2;
            matAmb = 0.5;
        } else if(isPlain(type) || isMountain(type) || isHill(type)) {
            matShiness = 50;
            matSpec = 0.1;
            matAmb = 0.2;
        }
        float spec = pow(max(dot(N, LV), 0.0), matShiness);
        vec3 specular = matSpec * spec * rgb;
        rgb2 += specular;
    }

    return rgb2;
}

void main() {

    ivec2 cKey = getCellKey(fPos2D, 1.0);//
    vec2 cKeyUV = vec2(float(cKey.x) / (cellsCols - 1), float(cKey.y) / (cellsRows - 1));
    vec4 cKeyTex = texture(tex_cs, cKeyUV);//cell key is here.
    int type = int(cKeyTex.r * 255.0);

    if(isFRZMountain(type)) {
        
    } else if(isFRZShore(type)) {
        
    }

    if(isShore(type) || isOcean(type) || isLake(type) || isFRZShore(type)) {
        if(fPosition.y > BEACH_FOOT) {
            type = TT_BEACH;
        }
    }

    vec4 color = getTypeColorFromTex(type);
    
    if(debug > 0) {
        float dis = distanceToEdgeByCKey(cKey);
        if(dis < 0) {
            dis = abs(dis);
            if(dis < 0.02) {
                color = vec4(1, 1, 1, 1);
            }
        }
    }
    vec4 fogTex = texture(tex_fo, fUV2);
    float fog = fogTex.r;//

    //fog of war
    if(fog < 0.2) {
            //full fog
        color = vec4(0, 0, 0, 1);
    } else if(fog < 0.7) {
        //half fog, mix color + amb + diffuse, no specular
        color.rgb = applyLight(type, color.rgb, false);
        color = mix(color, vec4(0, 0, 0, 1), 0.5);
    } else {//no fog.
        // color + light, amb + diffuse + specular.
        color.rgb = applyLight(type, color.rgb, true);
    }

    // debug zero position
    if(abs(fPosition.x) < 0.2 || abs(fPosition.z) < 0.2) {
        color = vec4(1, 1, 1, 1);
    }
    //actor position indicator
    float disAC = distanceToActorCross(0.02, 0.10);

    if(disAC > 0 && disAC < 0.005) {

        color = vec4(1, 1, 1, 1);

    }

    outColor = color;

}//end of main()
