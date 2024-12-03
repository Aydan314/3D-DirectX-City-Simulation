// Assign values from inside of GPU //

Texture2D diffuseTex : register(t0);
Texture2D specularTex : register(t1);
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

////////////////////////////// Vertex Shader //////////////////////////////////////////

struct VS_Out
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 worldPos : WPOS;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
};


VS_Out VS_main(float3 Position : POSITION, float3 Normal : NORMAL, float2 TexCoord : TEXCOORD)
{   
    VS_Out output = (VS_Out) 0;

    // Transform model space positions to world space //
    float4 pos4 = float4(Position, 1.0f);
    output.position = mul(pos4, World);
    output.worldPos = output.position;
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    
    // Transform model space normals to world space //
        float4 norm4 = float4(Normal, 0.0f);
    output.normal = normalize(mul(norm4, World));
    
    // Pass texture coordinate to pixel shader //
    output.texCoord = TexCoord;

    return output;
}

///////////////////////// Pixel Shader ///////////////////////////////////////////////

// Transforms a projection space coordinate into a 2d pixel value //

struct PS_LightValues
{
    float4 diffuse;
    float4 ambient;
    float4 specular;
    float4 diffuseMaterial;
    float4 ambientMaterial;
    float4 specularMaterial;
};

PS_LightValues PS_CalculatePrimaryLight(PS_LightValues lightValues, VS_Out input)
{
    // Calculate Diffuse Lighting //
    input.normal = normalize(input.normal);
    float3 ldir = -PrimaryLightData.LightDir;
    float diffuseAmount = saturate(dot(input.normal, ldir));
    
    lightValues.diffuse = diffuseAmount * (lightValues.diffuseMaterial * PrimaryLightData.DiffuseLight);
    
    // Calculate Ambient Lighting //
    lightValues.ambient = lightValues.ambientMaterial * PrimaryLightData.AmbientLight;
    
    // Calculate Specular Lighting //
    if (diffuseAmount > 0.0f)
    {
        float3 toviewer = normalize(CameraPosition - input.worldPos);
        float3 reflectDir = normalize(reflect(PrimaryLightData.LightDir, input.normal));
        float specAmount = pow(saturate(dot(reflectDir, toviewer)), PrimaryLightData.SpecPower);
        
        lightValues.specular = specAmount * (PrimaryLightData.SpecularLight * lightValues.specularMaterial);
    }
    
    return lightValues;
};

PS_LightValues PS_CalculatePointLight(PS_LightValues pixelLightValues, LightValues pointLightValues, VS_Out input)
{
    
    if (distance(input.worldPos, pointLightValues.LightPos) < pointLightValues.Radius)
    {
        // Calculate Diffuse Lighting //
        input.normal = normalize(input.normal);
        float3 ldir = (pointLightValues.LightPos - input.worldPos);
    
        float diffuseAmount = saturate(dot(input.normal, ldir));
        
        float intensity = saturate(1 - distance(input.worldPos, pointLightValues.LightPos) / pointLightValues.Radius);
    
        pixelLightValues.diffuse += diffuseAmount * (pixelLightValues.diffuseMaterial * (pointLightValues.DiffuseLight * intensity));
        
        // Calculate Specular Lighting //
        if (diffuseAmount > 0.0f)
        {
            float3 toviewer = normalize(CameraPosition - input.worldPos);
            float3 reflectDir = normalize(reflect(-ldir, input.normal));
            float specAmount = pow(saturate(dot(reflectDir, toviewer)), pointLightValues.SpecPower);
        
            pixelLightValues.specular += specAmount * ((pointLightValues.SpecularLight * intensity) * pixelLightValues.specularMaterial);
        }
        
    }
  
    return pixelLightValues;
}

float4 PS_main(VS_Out input) : SV_TARGET
{
    PS_LightValues lightValues;
    
    lightValues.diffuseMaterial = PrimaryLightData.DiffuseMaterial;
    lightValues.ambientMaterial = PrimaryLightData.AmbientMaterial;
    lightValues.specularMaterial = PrimaryLightData.SpecularMaterial;
    
    // Handle texture Materials //
    if (TextureState == 1 || TextureState == 2)
    {
        float4 texColor = diffuseTex.Sample(bilinearSampler, input.texCoord);

        lightValues.diffuseMaterial = texColor;
        lightValues.ambientMaterial = texColor;
    }
    
    if (TextureState == 2)
    {
        lightValues.specularMaterial = specularTex.Sample(bilinearSampler, input.texCoord);
    }
    
    // Calculate Main Directional Light //
    
    lightValues = PS_CalculatePrimaryLight(lightValues, input);
    
    // Calculate Point Lights //
    for (int i = 0; i < 16; i++)
    {
        if (LightData[i].Radius != 0)
        {
            lightValues = PS_CalculatePointLight(lightValues, LightData[i], input);
        }
        else
            break;
    }
    
    // Fog calculations //
    float pixelDist = distance(CameraPosition, input.worldPos);
    float fogAmount = 0.f;
    
    float maxFogDrawDist = 500.f;
    float minFogDrawDist = 100.f;
    
    if (pixelDist > minFogDrawDist)
    {
        fogAmount = (pixelDist - minFogDrawDist) / (maxFogDrawDist - minFogDrawDist);
        
        if (fogAmount > 1.f) fogAmount = 1.f;
    }
   
    // Combine lighting calculations //
    input.color = ((lightValues.specular + lightValues.ambient + lightValues.diffuse) * (1 - fogAmount)) + (PrimaryLightData.AmbientLight * fogAmount);
    
    return input.color;
}