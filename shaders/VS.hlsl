cbuffer ObjectData : register(b1)
{
    row_major float4x4 worldViewProj;
    row_major float4x4 world;
    row_major float4x4 lightViewProj;
}

struct VSInput {
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD;
};

struct VSOutput {
    float4 pos : SV_POSITION;
    float3 worldPos : POSITION1;
    float3 worldNormal : NORMAL;
    float3 worldTangent : TANGENT;
    float4 lightSpacePos : POSITION2;
    float2 uv : TEXCOORD;
};

VSOutput main(VSInput input) {
    VSOutput output;
    output.pos = mul(worldViewProj, float4(input.pos, 1.0f));
    output.worldPos = mul(world, float4(input.pos, 1.0f)).xyz;
    output.worldNormal = normalize(mul((float3x3)world, input.normal));
    output.worldTangent = mul((float3x3)world, input.tangent);
    // lightViewProj is all zeros when no light casts shadows; the pixel
    // shader detects that via lightSpacePos.w == 0 and skips shadowing.
    output.lightSpacePos = mul(lightViewProj, float4(output.worldPos, 1.0f));
    output.uv = input.uv;
    return output;
}
