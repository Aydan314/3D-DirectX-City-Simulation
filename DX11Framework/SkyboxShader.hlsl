TextureCube skybox : register(t0);
SamplerState bilinearSampler : register(s0);

struct LightValues
{
    float4 DiffuseLight;
    float4 DiffuseMaterial;
    float4 AmbientLight;
    float4 AmbientMaterial;
    float4 SpecularLight;
    float4 SpecularMaterial;
    float SpecPower;
    float3 LightDir;
    float Radius;
    float3 LightPos;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 Projection;
    float4x4 View;
    float4x4 World;
    LightValues PrimaryLightData;
    LightValues LightData[16];
    uint TextureState;
    float3 CameraPosition;
}

struct SkyboxVS_Out
{
    float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD;
};


SkyboxVS_Out VS_main(float4 Position : POSITION)
{
    SkyboxVS_Out output = (SkyboxVS_Out) 0;
    
    // Transform model space positions to world space //
    output.position = mul(Position, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    
    // Creates infinate depth illusion //
    output.position = output.position.xyww;
    output.texCoord = Position;
    
    return output;
}

float4 PS_main(SkyboxVS_Out input) : SV_Target
{
    float4 texColor = skybox.Sample(bilinearSampler, input.texCoord);
    texColor *= PrimaryLightData.DiffuseLight;
    
    return texColor;
}