#include "lab_m1/Tema3/Tema3.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;

#define PI 3.14


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema3::Tema3()
{
}


Tema3::~Tema3()
{
}


void Tema3::Init()
{
    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "textures");
    lastZ = lastX = lastY = 0;
    angularStepSkier = 0;
    timerNewObjects = 1;
    spotPos = glm::vec3(-0.38, 1.1, 0);
    posUI = glm::vec3(-3.30894, -0.542119, 4.34235);
    afisat = false;
    // Create a simple quad for snow
    {
    vector<glm::vec3> vertices
    {
        glm::vec3(75,   0.0f,75),    // top right
        glm::vec3(75,  0.0f, -75),    // bottom right
        glm::vec3(-75, 0.0f, -75),    // bottom left
        glm::vec3(-75,  0.0f, 75),    // top left
    };

    vector<glm::vec3> normals
    {
        glm::vec3(0, 1, 1),
        glm::vec3(1, 0, 1),
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0)
    };

    vector<glm::vec2> textureCoords
    {
        // TODO(student): Complete texture coordinates for the square
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, 0.0f)

    };

    vector<unsigned int> indices =
    {
        0, 1, 3,
        1, 2, 3
    };

    Mesh* mesh = new Mesh("quad_snow");
    mesh->InitFromData(vertices, normals, textureCoords, indices);
    meshes[mesh->GetMeshID()] = mesh;
    }


    // Load textures
    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "snow_texture.jpg").c_str(), GL_REPEAT);
        mapTextures["snow"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "rom_texture.jpg").c_str(), GL_REPEAT);
        mapTextures["romania"] = texture;
    }


    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader* shader = new Shader("Tema3Shader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    //Create skier
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    //Load cone
    {
        Mesh* mesh = new Mesh("cone");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3"), "cone.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    { //leaves texture
        Texture2D* texture = new Texture2D();
        
        texture->Load2D(PATH_JOIN(sourceTextureDir, "tree_leaves2.jpg").c_str(), GL_REPEAT);
        mapTextures["leaves"] = texture;
    }

    { //yeti texture
        Texture2D* texture = new Texture2D();

        texture->Load2D(PATH_JOIN(sourceTextureDir, "yeti_texture.jpg").c_str(), GL_REPEAT);
        mapTextures["yeti"] = texture;
    }

    { // Grey wood texture
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "greywood_texture.jpg").c_str(), GL_REPEAT);
        mapTextures["greywood"] = texture;
    }

    { // Rocks texture
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "rocks_texture.jpeg").c_str(), GL_REPEAT);
        mapTextures["rocks"] = texture;
    }

    { //random texture
        Texture2D* texture = new Texture2D();
        texture = CreateRandomTexture(25, 25);
        mapTextures["random"] = texture;
    }

    {// Load sphere
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;

    }

    { //create ui
        int points = 20;
        int Raza = 1;
        vector<VertexFormat> circle_Vertex;
        circle_Vertex.push_back(VertexFormat(glm::vec3(0), glm::vec3(1, 0, 0)));
        for (int i = 0; i <= points; i++)
        {
            circle_Vertex.push_back(VertexFormat(glm::vec3(Raza*  cos(2 * PI* i / points), 0, Raza*sin(2 * PI * i / points)), glm::vec3(1, 0, 0)));
                
        }
        vector<unsigned int> circle_indices;
        for (int i = 0; i <= points + 1; i++)
        {
            circle_indices.push_back(i);
        }
        Mesh* mesh = new Mesh("circle");
        mesh->SetDrawMode(GL_TRIANGLE_FAN);
        mesh->InitFromData(circle_Vertex, circle_indices);
        meshes["circle"] = mesh;
    }

    scaleXBody = 0.2f;
    scaleYBody = 0.4f;
    scaleZBody = 0.2f;
    scaleXSKIs = 0.05f;
    scaleYSKIs = 0.0001f;
    scaleZSKIs = 1.0f;

    scaleCone = 0.5f;
    scaleGift = 0.25f;
    scaleTrunk = 0.2f;
    angleSlope = RADIANS(30);

   

    translateX = translateY = translateZ = 0;
    timeY = timeX = 0;
    GetSceneCamera()->m_transform->SetWorldPosition(glm::vec3(0, 3, 10.0f/ sqrt(3)));
    GetSceneCamera()->m_transform->SetWorldRotation(glm::vec3(-40, 0, 0));
    GetSceneCamera()->Update();
    autoCamera = true;

    
    time = 0;
    last_time_object = 0;
    blocat = false;
    blocat2 = false;
    material_kd = 0.3;
    material_ks = 0.3;
    material_shininess = 30;

    positions_objects.push_back(glm::vec3(-10, -10, -10));
    color_light_objects.push_back(glm::vec3(0, 0, 0));
    pos_lighting_poles.push_back(glm::vec3(-10, -10, -10));

    lives = 3;
    score = 0;

}


void Tema3::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema3::Update(float deltaTimeSeconds)
{

    //Render snow
    if (lives <= 0)
    {
        blocat2 = true;
    }
    if (!blocat && !blocat2)
    {
        timeY += deltaTimeSeconds;
        translateZ += deltaTimeSeconds;
        time += deltaTimeSeconds;
    }
    if (blocat2)
    {
        angularStepSkier = 0;
    }

    if (blocat2 && !afisat)
    {
        afisat = true;
        cout << "Your score: " << score << endl;
        cout << "Press K to restart" << endl;
    }

    if (time - last_time_object > timerNewObjects)
    {
        last_time_object = time;
        float translateZObject = translateZ + 6.5;
        int randomNumber = rand();
        glm::vec3 poz = glm::vec3(rand() % 13 - 6.5f + translateX, -translateZObject * tan(angleSlope), translateZObject);
        if (randomNumber % 4 == 0)
        {
            pos_trees.push_back(poz);
        }
        else if (randomNumber % 4 == 1)
        {
            pos_gifts.push_back(poz);
        }
        else if (randomNumber % 4 == 2)
        {
            pos_lighting_poles.push_back(poz);
        }
        else if (randomNumber % 4 == 3)
        {
            pos_rocks.push_back(poz);
        }

        if (randomNumber % 4 < 2)
        {
            positions_objects.push_back(poz);
            color_light_objects.push_back(glm::vec3(rand() % 71 / 100.0f, rand() % 71 / 100.0f, rand() % 71 / 100.0f));
        }
    }
    
    
    
    {
        translateY = -translateZ * tan(angleSlope);
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1), glm::vec3(translateX, translateY , translateZ));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
        modelMatrix = glm::rotate(modelMatrix, angleSlope, glm::vec3(1, 0, 0));
    
        quad = 1;
        RenderSimpleMesh(meshes["quad_snow"], shaders["Tema3Shader"], modelMatrix, mapTextures["snow"]);
        quad = 0;
    }

    
    //Render skier
    {
        float translateXNext = translateX;
        translateXNext += cos(angularStepSkier - RADIANS(90)) * deltaTimeSeconds;
        if (translateX == 0)
        {
           angleGoDown = angleSlope;
        }
        else
        {
            glm::vec3 dir = glm::vec3(translateX - lastX, translateY- lastY, translateZ- lastZ);
            dir = glm::normalize(dir);
            angleGoDown = acos(glm::dot(dir, glm::normalize(glm::vec3(dir.x, 0, dir.z))));
            angleRotateDown = acos(glm::dot(dir, glm::normalize(glm::vec3(0, dir.y, dir.z))));
        }
        bool gasit = false;
        for (glm::vec3 vec : pos_trees)
        {
            if (distance(glm::vec3(vec.x, 0, vec.z), glm::vec3(translateXNext, 0, translateZ)) < (scaleXBody/2 + scaleTrunk/2) * sqrt(2))
            {
                if (last_obj_pos != vec)
                {
                    lives--;
                    last_obj_pos = vec;
                }
                gasit = true;
            }     
        }

        for (glm::vec3 vec : pos_lighting_poles)
        {
            if (distance(glm::vec3(vec.x, 0, vec.z), glm::vec3(translateXNext, 0, translateZ)) < (scaleXBody  + 0.15 ) * sqrt(2)/2)
            {
                if (last_obj_pos != vec)
                {
                    lives--;
                    last_obj_pos = vec;
                }
                gasit = true;
            }
        }

        for (glm::vec3 vec : pos_rocks)
        {
            if (distance(glm::vec3(vec.x, 0, vec.z), glm::vec3(translateXNext, 0, translateZ)) < (scaleXBody + 0.5) * sqrt(2) / 2)
            {
                if (last_obj_pos != vec)
                {
                    lives--;
                    last_obj_pos = vec;
                }
                gasit = true;
            }
        }
            if (gasit)
            {
                blocat = true;
            }
            else
            {
                blocat = false;
            }
        
        
            translateX = translateXNext;
            timeX += cos(angularStepSkier - RADIANS(90)) * deltaTimeSeconds;
        

        glm::mat4 modelMatrixSkier = glm::translate(glm::mat4(1), glm::vec3(translateX, -translateZ * tan(angleSlope) + 0.18f, translateZ));
        modelMatrixSkier = glm::rotate(modelMatrixSkier, angularStepSkier, glm::vec3(0, 1, 0));
        modelMatrixSkier = glm::rotate(modelMatrixSkier,angleSlope, glm::vec3(1,0,0)) ;
        modelMatrixSkier = glm::rotate(modelMatrixSkier, RADIANS(180), glm::vec3(0, 1, 0));
        glm::mat4 modelMatrixBody = glm::translate(modelMatrixSkier, glm::vec3(0, scaleYBody / 2, 0));
         modelMatrixBody = glm::scale(modelMatrixBody, glm::vec3(scaleXBody, scaleYBody, scaleZBody));
        
        RenderSimpleMesh(meshes["box"], shaders["Tema3Shader"], modelMatrixBody, mapTextures["yeti"]);

        glm::mat4 modelMatrixSKIL = modelMatrixSkier;
        modelMatrixSKIL = glm::translate(modelMatrixSKIL, glm::vec3(-0.08f, 0.001f, -0.02f));
            modelMatrixSKIL = glm::scale(modelMatrixSKIL, glm::vec3(scaleXSKIs, scaleYSKIs, scaleZSKIs));
        
        RenderSimpleMesh(meshes["box"], shaders["Tema3Shader"], modelMatrixSKIL, mapTextures["romania"]);

        glm::mat4 modelMatrixSKIR = modelMatrixSkier;
        modelMatrixSKIR = glm::translate(modelMatrixSKIR, glm::vec3(0.08f, 0.001f, -0.02f));
        modelMatrixSKIR = glm::scale(modelMatrixSKIR, glm::vec3(scaleXSKIs, scaleYSKIs, scaleZSKIs));

        RenderSimpleMesh(meshes["box"], shaders["Tema3Shader"], modelMatrixSKIR, mapTextures["romania"]);
    }


    //Render trees
    {
        int i = 0;
        for (glm::vec3 vec : pos_trees)
        {
            if (translateZ - vec.z < 6.5 && fabs(translateX - vec.x) < 6.5)
            {
                glm::mat4 modelMatrixTree = glm::translate(glm::mat4(1), vec);
                glm::mat4 modelMatrixTreeCone = glm::translate(modelMatrixTree, glm::vec3(0, 1.5, 0));
                modelMatrixTreeCone = glm::scale(modelMatrixTreeCone, glm::vec3(scaleCone, scaleCone, scaleCone));
                RenderSimpleMesh(meshes["cone"], shaders["Tema3Shader"], modelMatrixTreeCone, mapTextures["leaves"]);

                glm::mat4 modelMatrixTrunk = glm::translate(modelMatrixTree, glm::vec3(0, 0.5, 0));
                   modelMatrixTrunk  = glm::scale(modelMatrixTrunk, glm::vec3(scaleTrunk, 1, scaleTrunk));
                RenderSimpleMesh(meshes["box"], shaders["Tema3Shader"], modelMatrixTrunk, mapTextures["romania"]);
            }
            else
            {
                  pos_trees.erase(pos_trees.begin() + i);
                  for (int k = 1; k < positions_objects.size(); k++)
                  {
                      if (vec == positions_objects.at(k))
                      {
                          positions_objects.erase(positions_objects.begin() + k);
                          color_light_objects.erase(color_light_objects.begin() + k);
                      }
                  }
            }
            i++;
        }
    }

    {//Render gifts
        int i = 0;
        for (glm::vec3 vec : pos_gifts)
        {
            if (translateZ - vec.z < 6.5 && fabs(translateX - vec.x) < 6.5)
            {
                glm::mat4 modelMatrixGift = glm::translate(glm::mat4(1), vec);
                modelMatrixGift = glm::translate(modelMatrixGift, glm::vec3(0, scaleGift * 0.5, 0));  // ridicam cu jum de latura * scale
                modelMatrixGift = glm::rotate(modelMatrixGift, angleSlope, glm::vec3(1, 0, 0));
                modelMatrixGift = glm::scale(modelMatrixGift, glm::vec3(scaleGift, scaleGift, scaleGift));
                RenderSimpleMesh(meshes["box"], shaders["Tema3Shader"], modelMatrixGift, mapTextures["random"]);
            }
            else
            {
                pos_gifts.erase(pos_gifts.begin() + i);
                for (int k = 1; k < positions_objects.size(); k++)
                {
                    if (vec == positions_objects.at(k))
                    {
                        positions_objects.erase(positions_objects.begin() + k);
                        color_light_objects.erase(color_light_objects.begin() + k);
                    }
                }
            }
            i++;
        }
    }

    {// Render lighting_poles
        int i = 0;
        //for (glm::vec3 vec : pos_lighting_poles.)
        for(int i = 1; i < pos_lighting_poles.size(); i++)
        {
            glm::vec3 vec = pos_lighting_poles.at(i);
            if (translateZ - vec.z < 6.5 && fabs(translateX - vec.x) < 6.5)
            {
                glm::mat4 modelMatrixPoles = glm::translate(glm::mat4(1),vec); 
                glm::mat4 modelMatrixPolesVert = glm::translate(modelMatrixPoles, glm::vec3(0, 0.5 * 1.25, 0)); // ridicam cu jum de latura * factor scalare
                modelMatrixPolesVert = glm::scale(modelMatrixPolesVert, glm::vec3(0.15, 1.25, 0.15));
                RenderSimpleMesh(meshes["box"], shaders["Tema3Shader"], modelMatrixPolesVert, mapTextures["greywood"]);

                glm::mat4 modelMatrixPolesHoriz = glm::translate(modelMatrixPoles, glm::vec3(0, 1.25 + 0.075, 0)); // ridicam barna oriz cu cat are h cea verticala + jumatate din grosimea celei orizontale
                modelMatrixPolesHoriz = glm::rotate(modelMatrixPolesHoriz, RADIANS(90), glm::vec3(0, 0, 1));
                modelMatrixPolesHoriz = glm::scale(modelMatrixPolesHoriz, glm::vec3(0.15, 1, 0.15));
                RenderSimpleMesh(meshes["box"], shaders["Tema3Shader"], modelMatrixPolesHoriz, mapTextures["greywood"]);


            }
            else
            {
                pos_lighting_poles.erase(pos_lighting_poles.begin() + i);
                i--;
            }
          
        }
    }

    {//Render rocks

        for (glm::vec3 vec : pos_rocks)
        {
            if (translateZ - vec.z < 6.5 && fabs(translateX - vec.x) < 6.5)
            {
                for (int i = -1; i <= 1; i++)
                {
                    glm::mat4 modelMatrixRocks = glm::translate(glm::mat4(1), vec);
                    modelMatrixRocks = glm::translate(modelMatrixRocks, glm::vec3(0.2 * i, 0.125, 0)); // ridicam cu jum de raza si 
                    modelMatrixRocks = glm::scale(modelMatrixRocks, glm::vec3(0.25, 0.25, 0.25)); //raza devine 0.25
                    RenderSimpleMesh(meshes["sphere"], shaders["Tema3Shader"], modelMatrixRocks, mapTextures["rocks"]);
                }
            }
        }

    }

    {// Intersection with gifts
        int i = 0;
        for (glm::vec3 vec : pos_gifts)
        {
            if (distance(glm::vec3(translateX, 0, translateZ), glm::vec3(vec.x, 0, vec.z)) < (scaleGift/2 + scaleXBody/2)*sqrt(2))
            {
                pos_gifts.erase(pos_gifts.begin() + i);
                score += 50;
                for (int k = 1; k < positions_objects.size(); k++)
                {
                    if (vec == positions_objects.at(k))
                    {
                        positions_objects.erase(positions_objects.begin() + k);
                        color_light_objects.erase(color_light_objects.begin() + k);
                    }
                }
            }
            i++;
        }
    }

    {//Render ui
        for (int i = 0; i < lives; i++)
        {
            glm::mat4 modelMatrixUI = glm::translate(glm::mat4(1), posUI + glm::vec3(0.25 * i, 0, 0));
            modelMatrixUI = glm::translate(modelMatrixUI, glm::vec3(translateX, translateY, translateZ));
            modelMatrixUI = glm::rotate(modelMatrixUI, angleSlope, glm::vec3(1, 0, 0));
            modelMatrixUI = glm::scale(modelMatrixUI, glm::vec3(0.1, 0.1, 0.1));
            RenderMesh(meshes["circle"], shaders["VertexColor"], modelMatrixUI);
        }
    }

      
    if (autoCamera)
    {
        GetSceneCamera()->m_transform->Move(glm::vec3(-lastX + translateX, -lastY + translateY, -lastZ + translateZ)); 
        GetSceneCamera()->Update();
    }
    lastZ = translateZ;// retinem ca sa stim cu cat deplasam camera 
    lastX = translateX;//
    lastY = translateY;//

    
    
}


void Tema3::FrameEnd()
{
    //DrawCoordinateSystem();
}


void Tema3::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture1)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // TODO(student): Set any other shader uniforms that you need
    GLint loc = glGetUniformLocation(shader->program, "quad");
    glUniform1i(loc, quad);

    loc = glGetUniformLocation(shader->program, "timeY");
    glUniform1f(loc, timeY);

    loc = glGetUniformLocation(shader->program, "timeX");
    glUniform1f(loc, timeX);

    loc = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(loc, material_kd);

    loc = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(loc, material_ks);

    loc = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(loc, material_shininess);


    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    int eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    loc = glGetUniformLocation(shader->program, "positions");
    glUniform3fv(loc, 150, glm::value_ptr(positions_objects.at(0)));
    loc = glGetUniformLocation(shader->program, "N");
    glUniform1i(loc, positions_objects.size());

    loc = glGetUniformLocation(shader->program, "pos_lighting");
    glUniform3fv(loc, 150, glm::value_ptr(pos_lighting_poles.at(0)));

    loc = glGetUniformLocation(shader->program, "M");
    glUniform1i(loc, pos_lighting_poles.size());

    loc = glGetUniformLocation(shader->program, "colors");
    glUniform3fv(loc, 150, glm::value_ptr(color_light_objects.at(0)));

    loc = glGetUniformLocation(shader->program, "spotPos");
    glUniform3f(loc, spotPos.x, spotPos.y, spotPos.z);

    if (texture1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

    }

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


Texture2D* Tema3::CreateRandomTexture(unsigned int width, unsigned int height)
{
    GLuint textureID = 0;
    unsigned int channels = 3;
    unsigned int size = width * height * channels;
    unsigned char* data = new unsigned char[size];

    // TODO(student): Generate random texture data
    for (int i = 0; i < size; i++)
    {
        data[i] = rand() % 256;
    }

    // TODO(student): Generate and bind the new texture ID
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    if (GLEW_EXT_texture_filter_anisotropic) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
    }
    // TODO(student): Set the texture parameters (MIN_FILTER, MAG_FILTER and WRAPPING MODE) using glTexParameteri
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    CheckOpenGLError();

    // Use glTexImage2D to set the texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // TODO(student): Generate texture mip-maps

    glGenerateMipmap(GL_TEXTURE_2D);

    CheckOpenGLError();

    // Save the texture into a wrapper Texture2D class for using easier later during rendering phase
    Texture2D* texture = new Texture2D();
    texture->Init(textureID, width, height, channels);

    SAFE_FREE_ARRAY(data);
    return texture;
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema3::OnInputUpdate(float deltaTime, int mods)
{
 
     
}


void Tema3::OnKeyPress(int key, int mods)
{
    // Add key press event

    if (key == GLFW_KEY_M)
    {
        autoCamera = !autoCamera;
    }
    if (key == GLFW_KEY_K)
    {
        Init();
    }
}


void Tema3::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
   
        if (mouseY > resolution.y / 4)
        {
            float tg_X = -mouseX + resolution.x / 2;
            float tg_Y = -resolution.y / 4 + mouseY;
            angularStepSkier = -atan(tg_X / tg_Y);
        }
        else
        {
            angularStepSkier = 0;
        }

        if (angularStepSkier > RADIANS(60))
        {
            angularStepSkier = RADIANS(60);
        }
        else if (angularStepSkier < RADIANS(-60))
        {
            angularStepSkier = RADIANS(-60);
        }
    
   

}


void Tema3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema3::OnWindowResize(int width, int height)
{
}

