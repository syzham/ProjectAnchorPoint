// Metal Shading Language port of the engine's HLSL shaders (shaders/VS.hlsl +
// shaders/PS.hlsl). The Metal renderer loads and compiles this at runtime,
// just as the D3D11 backend compiles the HLSL. Lambertian shading over a list
// of lights, modulated by a per-material diffuse colour and a texture.

#include <metal_stdlib>
using namespace metal;

struct VSIn {
    float3 position [[attribute(0)]];
    float3 normal   [[attribute(1)]];
    float2 uv       [[attribute(2)]];
};

struct VSOut {
    float4 position [[position]];
    float3 worldNormal;
    float3 worldPos;
    float2 uv;
};

struct Uniforms {
    float4x4 worldViewProj;
    float4x4 world;
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

vertex VSOut vertex_main(VSIn in [[stage_in]],
                         constant Uniforms& u [[buffer(1)]]) {
    VSOut out;
    out.position = u.worldViewProj * float4(in.position, 1.0);

    const float3x3 world3 = float3x3(u.world[0].xyz, u.world[1].xyz, u.world[2].xyz);
    out.worldNormal = normalize(world3 * in.normal);
    out.worldPos = (u.world * float4(in.position, 1.0)).xyz;
    out.uv = in.uv;
    return out;
}

fragment float4 fragment_main(VSOut in [[stage_in]],
                              constant float4&     diffuseColor [[buffer(0)]],
                              const device Light*  lights       [[buffer(1)]],
                              constant uint&       lightCount   [[buffer(2)]],
                              texture2d<float>     tex          [[texture(0)]],
                              sampler              samp         [[sampler(0)]]) {
    float3 result = float3(0.0);

    for (uint i = 0; i < lightCount; ++i) {
        const Light light = lights[i];

        float3 L;
        float intensity = light.intensity;
        if (light.type == 0) {
            L = normalize(-float3(light.direction));
        } else {
            const float3 toLight = float3(light.position) - in.worldPos;
            L = normalize(toLight);
            const float dist = length(toLight);
            intensity *= saturate(1.0 - dist / light.range);
        }

        const float NdotL = max(dot(in.worldNormal, L), 0.0);
        result += NdotL * float3(light.color) * intensity;
    }

    return float4(result * diffuseColor.rgb, 1.0) * tex.sample(samp, in.uv);
}
