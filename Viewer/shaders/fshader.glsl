#version 330 core

// Inputs from vertex shader (after interpolation was applied)
in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoords;
in vec3 orig_fragPos;
in mat4 model_view;

struct Material
{
//  Material properties
    sampler2D textureMap;
     vec3 model_ambient_color;
     vec3 model_diffuse_color;
     vec3 model_specular_color;
     float shininess;
};
uniform Material material;

uniform vec3 camera_position;

//in case of point light.
uniform int point_light_count;
uniform vec3 point_light_position[5];
uniform vec3 point_ambient_intensity[5];
uniform vec3 point_diffuse_intensity[5];
uniform vec3 point_specular_intensity[5];

//in case of parallel light.
uniform int parallel_light_count;
uniform vec3 parallel_light_direction[5];
uniform vec3 parallel_ambient_intensity[5];
uniform vec3 parallel_diffuse_intensity[5];
uniform vec3 parallel_specular_intensity[5];

uniform bool draw_light;
uniform bool draw_toon;
uniform int num_colors;


// The final color of the fragment (pixel)
out vec4 frag_color;

void main()
{
    if(!draw_light){
        if(!draw_toon){
            vec3 texColor = texture(material.textureMap,fragTexCoords).xyz;

            vec3 ambient_color = {0,0,0};
            vec3 diffuse_color = {0,0,0};
            vec3 specular_color = {0,0,0};

            for (int i = 0; i < point_light_count ; ++i){
                //ambient
                ambient_color += texColor*point_ambient_intensity[i] * material.model_ambient_color;

                //diffuse
                vec3 N = normalize(fragNormal);
                vec3 I = normalize(point_light_position[i] - fragPos);
                float diffuse_factor = max(dot(N,I),0.f);
                diffuse_color += texColor*diffuse_factor * point_diffuse_intensity[i] * material.model_diffuse_color;

                //specular

                vec3 V = normalize(camera_position - fragPos);
                vec3 R = reflect(-I,fragNormal);

                float specular_factor = pow(max(dot(V,R),0.f),material.shininess);

                specular_color += texColor*point_specular_intensity[i]*material.model_specular_color*specular_factor;

            }


            for (int i = 0; i < parallel_light_count ; ++i){
                //ambient
                ambient_color += texColor*parallel_ambient_intensity[i] * material.model_ambient_color;

                //diffuse
                vec3 N = normalize(fragNormal);
                vec3 I = normalize(parallel_light_direction[i]);
                float diffuse_factor = max(dot(N,I),0.f);
                diffuse_color += texColor*diffuse_factor * parallel_diffuse_intensity[i] * material.model_diffuse_color;

                //specular

                vec3 V = normalize(camera_position - fragPos);
                vec3 R = reflect(-I,fragNormal);

                float specular_factor = pow(max(dot(V,R),0.f),material.shininess);

                specular_color += texColor*parallel_specular_intensity[i]*material.model_specular_color*specular_factor;

            }
            vec3 final_color = clamp(ambient_color + diffuse_color + specular_color,0,1);

            frag_color = vec4(final_color,1.f);
        }
        else{
            //toon shading
            vec3 texColor = texture(material.textureMap,fragTexCoords).xyz;
            vec3 N = normalize(fragNormal);
            vec3 I = normalize(point_light_position[0] - fragPos);
            vec3 V = normalize(camera_position - fragPos);
            vec3 R = normalize(reflect(-I,N));

            float diffuse_factor = max(dot(N,I),0.f);
            float color_choice = floor(diffuse_factor*num_colors);
            float cut_by = color_choice/num_colors;

            vec3 ambient = material.model_ambient_color * point_ambient_intensity[0] * cut_by;
            vec3 diffuse = material.model_diffuse_color * point_diffuse_intensity[0] * cut_by;
            vec3 specular = material.model_specular_color * point_specular_intensity[0] * cut_by;

            texColor *=cut_by;
            frag_color = vec4(texColor,1);
        }
    }
    else{
        frag_color = vec4(1.f,1.f,1.f,1.f);
    }

}