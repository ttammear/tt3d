#version 440

struct VertexOut {
    vec4 position;
    vec4 normal;
};

struct TriangleOut
{
    int index[3];
};

struct Line {
    vec4 start;
    vec4 end;
};

struct GenData
{
    uint tunnelCount;
    float firstOctaveMax;
    float secondOctaveMax;
    float padding;
    float dxgoalFirstOctaveMax;
    float dxgoalSecondOctaveMax;
    float dzgoalFirstOctaveMax;
    float dzgoalSecondOctaveMax;
    vec4 chunkOrigin;
};

struct GlobalVert
{
	vec3 coord;
	int index;
};

struct EdgeVertices{
	GlobalVert left[17][17][17][3];
};

// Shader storage buffer objects
layout(std430, binding = 0) readonly buffer InputVertexBuffer {
    vec4 data[];
} inputVertexBuffer;

layout(std430, binding = 1) writeonly buffer OutputVertexBuffer {
    VertexOut data[];
} outputVertexBuffer;

layout(std430, binding = 5) writeonly buffer OutputElementBuffer {
    TriangleOut data[];
} outputElementBuffer;

layout(std430, binding = 2) readonly buffer TunnelData
{
    GenData genData;
    Line tunnels[];
} tunnelData;

layout(std430, binding = 4) buffer EdgeVertexData
{
    EdgeVertices data[];
} edgeVertexData;

layout (binding = 3, offset = 0) uniform atomic_uint triangleCount;
layout (binding = 3, offset = 4) uniform atomic_uint indexCount;

// Uniforms
uniform isampler2D mcubesLookup;
uniform isampler1D mcubesLookup2;
uniform vec3 worldOffset;
uniform float voxelScale;

// Shared values between all the threads in the group
shared float cubeValues[18][18][18]; // the actual used size is 17, but using 18 saves 3 overflow checks

const ivec4 edgeVertexOffset[12] =
{
    ivec4(0, 0, 0, 0), // 0
	 ivec4(1, 0, 0, 1),
	 ivec4(0, 1, 0, 0),
	 ivec4(0, 0, 0, 1),
	 ivec4(0, 0, 1, 0), // 4
	 ivec4(1, 0, 1, 1),
	 ivec4(0, 1, 1, 0),
	 ivec4(0, 0, 1, 1),
	 ivec4(0, 0, 0, 2), // 8
	 ivec4(1, 0, 0, 2),
	 ivec4(1, 1, 0, 2),
	 ivec4(0, 1, 0, 2) // 11
};

vec3 mod289(vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
    return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
{
    const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
    const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

    // First corner
    vec3 i  = floor(v + dot(v, C.yyy) );
    vec3 x0 =   v - i + dot(i, C.xxx) ;

    // Other corners
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min( g.xyz, l.zxy );
    vec3 i2 = max( g.xyz, l.zxy );

    //   x0 = x0 - 0.0 + 0.0 * C.xxx;
    //   x1 = x0 - i1  + 1.0 * C.xxx;
    //   x2 = x0 - i2  + 2.0 * C.xxx;
    //   x3 = x0 - 1.0 + 3.0 * C.xxx;
    vec3 x1 = x0 - i1 + C.xxx;
    vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
    vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

    // Permutations
    i = mod289(i);
    vec4 p = permute( permute( permute(
                                   i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
                               + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
                      + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

    // Gradients: 7x7 points over a square, mapped onto an octahedron.
    // The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    float n_ = 0.142857142857; // 1.0/7.0
    vec3  ns = n_ * D.wyz - D.xzx;

    vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

    vec4 x = x_ *ns.x + ns.yyyy;
    vec4 y = y_ *ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);

    vec4 b0 = vec4( x.xy, y.xy );
    vec4 b1 = vec4( x.zw, y.zw );

    //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
    //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
    vec4 s0 = floor(b0)*2.0 + 1.0;
    vec4 s1 = floor(b1)*2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));

    vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
    vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

    vec3 p0 = vec3(a0.xy,h.x);
    vec3 p1 = vec3(a0.zw,h.y);
    vec3 p2 = vec3(a1.xy,h.z);
    vec3 p3 = vec3(a1.zw,h.w);

    //Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
    m = m * m;
    return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                  dot(p2,x2), dot(p3,x3) ) );
}

float getOffset(float v1, float v2)
{
    float delta = v1 - v2;
    if(delta == 0.0)
        return 0.5;
    return v1/delta;
}

vec4 getOffsetv(vec4 v1, vec4 v2)
{
    vec4 delta = v1 - v2;
    vec4 eq = vec4(equal(delta,vec4(0.0)));
    vec4 neq = vec4(notEqual(delta,vec4(0.0)));
	 return eq*vec4(0.5)+neq*(v1/delta); // TODO: division by 0 can happen
}

vec3 getClosestPointOnLine(vec3 a, vec3 b, vec3 from)
{
    vec3 atob = b-a;
    from = from-a;
    float atob2 = dot(atob,atob); // squared length
    float atopDotAtob = dot(from, atob);
    float t = atopDotAtob / atob2;
    t = clamp(t,0.0,1.0);
    atob *= t;
    return a+atob;
}

float voxel(vec3 worldPos)
{
    vec3 curVec = worldPos-tunnelData.genData.chunkOrigin.xyz;
    float progressX = curVec.x/64.0; 	// TODO: 64=chunk size
    float progressZ = curVec.z/64.0;
    //float zom = mix(0.0,tunnelData.genData.dxgoalSecondOctaveMax,progressX) + mix(0.0,tunnelData.genData.dzgoalSecondOctaveMax,progressZ);
    float fom = mix(0.0,tunnelData.genData.dxgoalFirstOctaveMax,progressX) + mix(0.0,tunnelData.genData.dzgoalFirstOctaveMax,progressZ);
    float som = mix(0.0,tunnelData.genData.dxgoalSecondOctaveMax,progressX) + mix(0.0,tunnelData.genData.dzgoalSecondOctaveMax,progressZ);

    float lacunarity = 2.0;

    float warp = (snoise(0.08*worldPos)+1);
    vec3 sampleCoord = vec3(0.2,0.9,0.48)*warp*10 + worldPos;
    //vec3 sampleCoord = worldPos;
    sampleCoord.y = 33.11;

    float h2noise = snoise(0.005*sampleCoord)+1;
    float h2 = (h2noise)*(tunnelData.genData.secondOctaveMax+som);

    float h0 = h2noise*0.5*((snoise(0.005*pow(lacunarity,4.0)*sampleCoord)+1)*(1.0));
    float h1 = h2noise*0.5*((snoise(0.0005*pow(lacunarity,2.0)*sampleCoord)+1)*(tunnelData.genData.firstOctaveMax+fom));

    float minHeight = (h0+h1+h2) - worldPos.y ;

    for(int i = 0; i < tunnelData.genData.tunnelCount; i++)
    {
        vec3 p0 = tunnelData.tunnels[i].start.xyz;
        vec3 p1 = tunnelData.tunnels[i].end.xyz;
        vec3 cp = getClosestPointOnLine(p0,p1,worldPos);
        float sphere = length(worldPos-cp);
        if(sphere < 5.0)
            return (-5.0+sphere)*10.0;
    }

    return minHeight;
}

void vMarchCube1(int i, int j, int k, uint index)
{	
    int iterate, iFlagIndex;
    vec4 afCubeValuev[2];

    afCubeValuev[0].x = cubeValues[i][j][k];
    afCubeValuev[0].y = cubeValues[i+1][j][k];
    afCubeValuev[0].z = cubeValues[i+1][j+1][k];
    afCubeValuev[0].w = cubeValues[i][j+1][k];
    afCubeValuev[1].x = cubeValues[i][j][k+1];
    afCubeValuev[1].y = cubeValues[i+1][j][k+1];
    afCubeValuev[1].z = cubeValues[i+1][j+1][k+1];
    afCubeValuev[1].w = cubeValues[i][j+1][k+1];

	 // I heard GPU likes vectors...
    iFlagIndex = int(dot(ivec4(1, 2, 4, 8)*ivec4(step(afCubeValuev[0],vec4(0.0))),ivec4(1)) + 
		dot(ivec4(16, 32, 64, 128)*ivec4(step(afCubeValuev[1],vec4(0.0))),ivec4(1)) + vec4(0.1));

	 int edgeConnection[3];
 
    for(iterate = 0; iterate < 5; iterate++)
    {
    	  edgeConnection[0] = texelFetch2D(mcubesLookup, ivec2(3*iterate, iFlagIndex), 0).a;
		  edgeConnection[1] = texelFetch2D(mcubesLookup, ivec2(3*iterate+1, iFlagIndex), 0).a;
		  edgeConnection[2] = texelFetch2D(mcubesLookup, ivec2(3*iterate+2, iFlagIndex), 0).a;

        if(edgeConnection[0] > -1)
        {
            uint indexIndex = atomicCounterIncrement(indexCount);

            // calculate vertex positions and normal of the triangle
            vec3 verts[3];
            for(int curVert = 0; curVert < 3; curVert++)
            {
                ivec4 indexOffset = edgeVertexOffset[edgeConnection[curVert]];
                verts[curVert] = edgeVertexData.data[index].left[i+indexOffset.x][j+indexOffset.y][k+indexOffset.z][indexOffset.w].coord;
            }
            vec3 normal = normalize(cross(verts[1].xyz -verts[0].xyz, verts[2].xyz - verts[0].xyz));

            // add missing vertices and indices
            for(int curVert = 0; curVert < 3; curVert++)
            {
                ivec4 indexOffset = edgeVertexOffset[edgeConnection[curVert]];
                int res = -2;
                do
                {
                    res = atomicCompSwap(edgeVertexData.data[index].left[i+indexOffset.x][j+indexOffset.y][k+indexOffset.z][indexOffset.w].index, -1, -2);
                    if(res == -1) // new vertex
                    {
                        // create new vertex
                        uint vertexIndex = atomicCounterIncrement(triangleCount);
                        // write new vertex index so others can read it
                        atomicExchange(edgeVertexData.data[index].left[i+indexOffset.x][j+indexOffset.y][k+indexOffset.z][indexOffset.w].index, int(vertexIndex));                      
                        outputVertexBuffer.data[vertexIndex].position = vec4(verts[curVert], 1.0);
                        outputVertexBuffer.data[vertexIndex].normal = vec4(normal,1.0);
                        // add new index
                        outputElementBuffer.data[indexIndex].index[curVert] = int(vertexIndex);
                    }
                    else if(res != -2)	// some other thread already created this vertex, just use the stored value
                    {
                        // add new index
                        outputElementBuffer.data[indexIndex].index[curVert] = res;
                        // TODO: atomic?
                        outputVertexBuffer.data[res].normal += vec4(normal,1.0);
                    }
                } while (res == -2);		
            }
        }
    }
}

// cant use 17 17 17 because "local work size runs out of limitaion"
layout(local_size_x = 17, local_size_y = 17, local_size_z = 1) in;
void main() {
    ivec2 location = ivec2(gl_WorkGroupID.xy);
    ivec2 itemID = ivec2(gl_LocalInvocationID.xy);
    ivec2 tileNumber = ivec2(gl_NumWorkGroups.xy);
    uint index = location.x;

	 // take all the samples we will need
    for(int i = 0; i < 17; i++) {
        int zv = i;
        vec3 worldPosition = inputVertexBuffer.data[index].xyz + vec3(itemID.x*voxelScale,itemID.y*voxelScale,i*voxelScale);
        cubeValues[itemID.x][itemID.y][zv] = voxel(worldPosition+worldOffset);
    }

	 int dummy = 3; // i don't understand the dynamically uniform expression thing, so I use these
    dummy;
    barrier();

	 // calculate all possible vertex positions
    for(int i = 0; i < 17; i++) {
        int zv = i;

        vec3 worldPosition = inputVertexBuffer.data[index].xyz + vec3(itemID.x*voxelScale,itemID.y*voxelScale,zv*voxelScale)/* + worldOffset*/;

        float fOffset = getOffset(cubeValues[itemID.x][itemID.y][zv], cubeValues[itemID.x+1][itemID.y][zv]);
        edgeVertexData.data[index].left[itemID.x][itemID.y][zv][0].coord = worldPosition + fOffset*vec3(voxelScale,0.0,0.0);
        edgeVertexData.data[index].left[itemID.x][itemID.y][zv][0].index = -1;

        fOffset = getOffset(cubeValues[itemID.x][itemID.y][zv], cubeValues[itemID.x][itemID.y+1][zv]);
        edgeVertexData.data[index].left[itemID.x][itemID.y][zv][1].coord = worldPosition + fOffset*vec3(0.0,voxelScale,0.0);
        edgeVertexData.data[index].left[itemID.x][itemID.y][zv][1].index = -1;

        fOffset = getOffset(cubeValues[itemID.x][itemID.y][zv], cubeValues[itemID.x][itemID.y][zv+1]);
        edgeVertexData.data[index].left[itemID.x][itemID.y][zv][2].coord = worldPosition + fOffset*vec3(0.0,0.0,voxelScale);
        edgeVertexData.data[index].left[itemID.x][itemID.y][zv][2].index = -1;
    }

	dummy = 4;
    dummy;
    barrier();

    if(itemID.x == 16 || itemID.y == 16)
	    return;

	 for(int i = 0; i < 16; i++)
	 {
    	 vMarchCube1(itemID.x, itemID.y, i, index);
	 }

}
