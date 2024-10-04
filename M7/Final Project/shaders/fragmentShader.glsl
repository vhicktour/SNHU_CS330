#version 330 core
out vec4 fragmentColor;

in vec3 fragmentPosition;
in vec3 fragmentVertexNormal;
in vec2 fragmentTextureCoordinate;

struct Material {
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;
}; 

struct DirectionalLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    bool bActive;
};

struct PointLight {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    bool bActive;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       

    bool bActive;
};

#define TOTAL_POINT_LIGHTS 1  // Adjusted to 1 since we have one point light

uniform bool bUseTexture=false;
uniform bool bUseLighting=false;
uniform vec4 objectColor = vec4(1.0f);
uniform vec3 viewPosition;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[TOTAL_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform sampler2D objectTexture;
uniform vec2 UVscale = vec2(1.0f, 1.0f);

// function prototypes
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    if(bUseLighting == true)
    {
        vec3 phongResult = vec3(0.0f);
        // properties
        vec3 norm = normalize(fragmentVertexNormal);
        vec3 viewDir = normalize(viewPosition - fragmentPosition);
    
        // phase 1: directional lighting
        if(directionalLight.bActive == true)
        {
            phongResult += CalcDirectionalLight(directionalLight, norm, viewDir);
        }
        // phase 2: point lights
        for(int i = 0; i < TOTAL_POINT_LIGHTS; i++)
        {
            if(pointLights[i].bActive == true)
            {
                phongResult += CalcPointLight(pointLights[i], norm, fragmentPosition, viewDir);   
            }
        } 
        // phase 3: spot light (if used)
        if(spotLight.bActive == true)
        {
            phongResult += CalcSpotLight(spotLight, norm, fragmentPosition, viewDir);    
        }
    
        if(bUseTexture == true)
        {
            vec3 texColor = vec3(texture(objectTexture, fragmentTextureCoordinate * UVscale));
            fragmentColor = vec4(texColor * phongResult, (texture(objectTexture, fragmentTextureCoordinate * UVscale)).a);
        }
        else
        {
            fragmentColor = vec4(vec3(objectColor) * phongResult, objectColor.a);
        }
    }
    else
    {
        if(bUseTexture == true)
        {
            fragmentColor = texture(objectTexture, fragmentTextureCoordinate * UVscale);
        }
        else
        {
            fragmentColor = objectColor;
        }
    }
}

// calculates the color when using a directional light.
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 ambient = vec3(0.0f);
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);

    vec3 lightDirection = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDirection), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    if(bUseTexture == true)
    {
        vec3 texColor = vec3(texture(objectTexture, fragmentTextureCoordinate * UVscale));
        ambient = light.ambient * texColor;
        diffuse = light.diffuse * diff * texColor;
        specular = light.specular * spec * material.specularColor;
    }
    else
    {
        ambient = light.ambient * vec3(objectColor);
        diffuse = light.diffuse * diff * material.diffuseColor * vec3(objectColor);
        specular = light.specular * spec * material.specularColor;
    }
    
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 ambient = vec3(0.0f);
    vec3 diffuse = vec3(0.0f);
    vec3 specular= vec3(0.0f);

    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    // Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    if(bUseTexture == true)
    {
        vec3 texColor = vec3(texture(objectTexture, fragmentTextureCoordinate * UVscale));
        ambient = light.ambient * texColor;
        diffuse = light.diffuse * diff * texColor;
        specular = light.specular * specularComponent * material.specularColor;
    }
    else
    {
        ambient = light.ambient * vec3(objectColor);
        diffuse = light.diffuse * diff * material.diffuseColor * vec3(objectColor);
        specular = light.specular * specularComponent * material.specularColor;
    }
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    
    return vec3(0.0);
}
