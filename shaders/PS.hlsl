struct Light {
    int type;
    float3 position;
    float range;
    float3 direction;
    float3 color;
    float intensity;
};

StructuredBuffer<Light> lights : register(t0);
Texture2D tex : register(t1);
Texture2D normalMap : register(t2);
Texture2D shadowMap : register(t3);
SamplerState samp : register(s0);
SamplerComparisonState shadowSamp : register(s1);

cbuffer MaterialData : register(b0)
{
    float3 diffuseColor;
}

// Keeps unlit and shadowed surfaces visible instead of pitch black.
static const float ambient = 0.15;

struct PSInput {
    float4 pos : SV_POSITION;
    float3 worldPos : POSITION1;
    float3 worldNormal : NORMAL;
    float3 worldTangent : TANGENT;
    float4 lightSpacePos : POSITION2;
    float2 uv : TEXCOORD;
};

// Percentage-closer-filtered shadow test for the directional light.
// Returns 1 when fully lit and 0 when fully in shadow.
float ShadowFactor(float4 lightSpacePos) {
    if (lightSpacePos.w <= 0.0)
        return 1.0; // no shadow-casting light this frame

    float3 proj = lightSpacePos.xyz / lightSpacePos.w;
    float2 uv = float2(proj.x * 0.5 + 0.5, proj.y * -0.5 + 0.5);
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0 || proj.z > 1.0)
        return 1.0; // outside the shadow volume

    float bias = 0.002;
    float width, height;
    shadowMap.GetDimensions(width, height);
    float2 texel = float2(1.0 / width, 1.0 / height);

    float lit = 0.0;
    [unroll]
    for (int y = -1; y <= 1; ++y) {
        [unroll]
        for (int x = -1; x <= 1; ++x) {
            lit += shadowMap.SampleCmpLevelZero(shadowSamp, uv + float2(x, y) * texel,
                                                proj.z - bias);
        }
    }
    return lit / 9.0;
}

float4 main(PSInput input) : SV_TARGET {
    // Tangent-space normal mapping: rebuild an orthonormal TBN basis and
    // perturb the interpolated normal by the normal-map sample. Meshes
    // without a normal map bind a flat (0.5, 0.5, 1) texture.
    float3 N = normalize(input.worldNormal);
    float3 T = input.worldTangent - N * dot(N, input.worldTangent);
    if (dot(T, T) > 1e-8) {
        T = normalize(T);
        float3 B = cross(N, T);
        float3 sampled = normalMap.Sample(samp, input.uv).xyz * 2.0 - 1.0;
        N = normalize(sampled.x * T + sampled.y * B + sampled.z * N);
    }

    float shadow = ShadowFactor(input.lightSpacePos);

    float3 result = float3(ambient, ambient, ambient);
    for (uint i = 0; i < lights.Length; ++i) {
        Light light = lights[i];

        float3 L;
        if (light.type == 0) {
            L = normalize(-light.direction);
            light.intensity *= shadow;
        } else {
            float3 toLight = light.position - input.worldPos;
            L = normalize(toLight);
            float attenuation = saturate(1.0 - length(toLight) / light.range);
            light.intensity *= attenuation;
        }
        float NdotL = max(dot(N, L), 0.0);
        result += NdotL * light.color * light.intensity;
    }

    return float4(result * diffuseColor.rgb, 1.0) * tex.Sample(samp, input.uv);
}
