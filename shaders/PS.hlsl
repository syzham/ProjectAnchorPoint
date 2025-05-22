struct Light {
    int type;
    float3 position;
    float range;
    float3 direction;
    float3 color;
    float intensity;
};

StructuredBuffer<Light> lights : register(t0);

cbuffer MaterialData : register(b0)
{
    float3 diffuseColor;
}

float4 main(float4 position : SV_POSITION, float3 worldNormal : NORMAL) : SV_TARGET {
    float3 result = float3(0, 0, 0);

    for (uint i = 0; i < lights.Length; ++i) {
        Light light = lights[i];

        float3 L;
        if (light.type == 0) {
            L = normalize(-light.direction);
        } else {
            L = normalize(light.position - (float3)position);
            float dist = length(light.position - (float3)position);
            float attenuation = saturate(1.0 - dist / light.range);
            light.intensity *= attenuation;
        }
        float NdotL = max(dot(worldNormal, L), 0.0);
        result += NdotL * light.color * light.intensity;
    }

    return float4(result * diffuseColor.rgb, 1.0);
}