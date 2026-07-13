// Metal Shading Language port of the engine's HLSL shaders (shaders/VS.hlsl +
// shaders/PS.hlsl). The Metal renderer loads and compiles this at runtime,
// just as the D3D11 backend compiles the HLSL. Lambertian shading over a list
// of lights with tangent-space normal mapping and a PCF-filtered shadow map
// for the directional light.

#include <metal_stdlib>
using namespace metal;

struct VSIn {
    float3 position [[attribute(0)]];
    float3 normal   [[attribute(1)]];
    float3 tangent  [[attribute(2)]];
    float2 uv       [[attribute(3)]];
};

struct VSOut {
    float4 position [[position]];
    float3 worldPos;
    float3 worldNormal;
    float3 worldTangent;
    float4 lightSpacePos;
    float2 uv;
};

struct Uniforms {
    float4x4 worldViewProj;
    float4x4 world;
    float4x4 lightViewProj;
};

// Layout matches ap::GpuLight (tightly packed, 48 bytes): packed_float3 keeps
// the 12-byte vector packing instead of Metal's default 16-byte float3.
struct Light {
    int           type;       // 0 directional, 1 point, 2 spot
    packed_float3 position;
    float         range;
    packed_float3 direction;
    packed_float3 color;
    float         intensity;
};

// Keeps unlit and shadowed surfaces visible instead of pitch black.
constant float ambient = 0.15;

vertex VSOut vertex_main(VSIn in [[stage_in]],
                         constant Uniforms& u [[buffer(1)]]) {
    VSOut out;
    out.position = u.worldViewProj * float4(in.position, 1.0);

    const float3x3 world3 = float3x3(u.world[0].xyz, u.world[1].xyz, u.world[2].xyz);
    out.worldPos = (u.world * float4(in.position, 1.0)).xyz;
    out.worldNormal = normalize(world3 * in.normal);
    out.worldTangent = world3 * in.tangent;
    // lightViewProj is all zeros when no light casts shadows; the fragment
    // shader detects that via lightSpacePos.w == 0 and skips shadowing.
    out.lightSpacePos = u.lightViewProj * float4(out.worldPos, 1.0);
    out.uv = in.uv;
    return out;
}

// Percentage-closer-filtered shadow test for the directional light.
// Returns 1 when fully lit and 0 when fully in shadow.
static float ShadowFactor(float4 lightSpacePos,
                          depth2d<float> shadowMap,
                          sampler shadowSamp) {
    if (lightSpacePos.w <= 0.0)
        return 1.0; // no shadow-casting light this frame

    const float3 proj = lightSpacePos.xyz / lightSpacePos.w;
    const float2 uv = float2(proj.x * 0.5 + 0.5, proj.y * -0.5 + 0.5);
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0 || proj.z > 1.0)
        return 1.0; // outside the shadow volume

    const float bias = 0.002;
    const float2 texel = float2(1.0 / shadowMap.get_width(),
                                1.0 / shadowMap.get_height());

    float lit = 0.0;
    for (int y = -1; y <= 1; ++y)
        for (int x = -1; x <= 1; ++x)
            lit += shadowMap.sample_compare(shadowSamp, uv + float2(x, y) * texel,
                                            proj.z - bias);
    return lit / 9.0;
}

fragment float4 fragment_main(VSOut in [[stage_in]],
                              constant float4&     diffuseColor [[buffer(0)]],
                              const device Light*  lights       [[buffer(1)]],
                              constant uint&       lightCount   [[buffer(2)]],
                              texture2d<float>     tex          [[texture(0)]],
                              texture2d<float>     normalMap    [[texture(1)]],
                              depth2d<float>       shadowMap    [[texture(2)]],
                              sampler              samp         [[sampler(0)]],
                              sampler              shadowSamp   [[sampler(1)]]) {
    // Tangent-space normal mapping: rebuild an orthonormal TBN basis and
    // perturb the interpolated normal by the normal-map sample. Meshes
    // without a normal map bind a flat (0.5, 0.5, 1) texture.
    float3 N = normalize(in.worldNormal);
    float3 T = in.worldTangent - N * dot(N, in.worldTangent);
    if (dot(T, T) > 1e-8) {
        T = normalize(T);
        const float3 B = cross(N, T);
        const float3 sampled = normalMap.sample(samp, in.uv).xyz * 2.0 - 1.0;
        N = normalize(sampled.x * T + sampled.y * B + sampled.z * N);
    }

    const float shadow = ShadowFactor(in.lightSpacePos, shadowMap, shadowSamp);

    float3 result = float3(ambient);
    for (uint i = 0; i < lightCount; ++i) {
        const Light light = lights[i];

        float3 L;
        float intensity = light.intensity;
        if (light.type == 0) {
            L = normalize(-float3(light.direction));
            intensity *= shadow;
        } else {
            const float3 toLight = float3(light.position) - in.worldPos;
            L = normalize(toLight);
            intensity *= saturate(1.0 - length(toLight) / light.range);
        }

        const float NdotL = max(dot(N, L), 0.0);
        result += NdotL * float3(light.color) * intensity;
    }

    return float4(result * diffuseColor.rgb, 1.0) * tex.sample(samp, in.uv);
}

// --- Debug line rendering (collider outlines) ---

struct DebugVSIn {
    float3 position [[attribute(0)]];
    float3 color    [[attribute(1)]];
};

struct DebugVSOut {
    float4 position [[position]];
    float3 color;
};

vertex DebugVSOut debug_vertex_main(DebugVSIn in [[stage_in]],
                                    constant float4x4& viewProj [[buffer(1)]]) {
    DebugVSOut out;
    out.position = viewProj * float4(in.position, 1.0);
    out.color = in.color;
    return out;
}

fragment float4 debug_fragment_main(DebugVSOut in [[stage_in]]) {
    return float4(in.color, 1.0);
}
