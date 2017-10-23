/*
 * This file is part of ElasticFusion.
 *
 * Copyright (C) 2015 Imperial College London
 *
 * The use of the code within this file and all code within files that
 * make up the software that is ElasticFusion is permitted for
 * non-commercial purposes only.  The full terms and conditions that
 * apply to the code within this file are detailed within the LICENSE.txt
 * file and at <http://www.imperial.ac.uk/dyson-robotics-lab/downloads/elastic-fusion/elastic-fusion-license/>
 * unless explicitly stated.  By downloading this file you agree to
 * comply with these terms.
 *
 * If you wish to use any of this code for commercial purposes then
 * please email researchcontracts.engineering@imperial.ac.uk.
 *
 */

#include "myRender.h"





const std::string myRender::Test = "Test";

bool myRender::loadOBJ(const char * path, std::vector < glm::vec3 > & out_vertices, std::vector < int> & out_tri,std::vector < glm::vec2> & out_uv)
{

    std::vector< unsigned int > Triangle_vertexIndices;
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;

    FILE * file = fopen(path, "r");
    if( file == NULL )
    {
        printf("loadOBJ : Impossible to open the file !\n");
        return false;
    }
    while( 1 )
    {

        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if ( res == EOF)
        {
            break;    // EOF = End Of File. Quit the loop.
        }

        // else : parse lineHeader
        if ( strcmp(lineHeader, "v") == 0 )
        {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if ( strcmp(lineHeader, "vt") == 0 )
        {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        else if ( strcmp(lineHeader, "vn") == 0 )
        {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if ( strcmp(lineHeader, "f") == 0 )
        {

            unsigned int vertexIndex[3];
            int matches = fscanf(file, "%d/%d %d/%d %d/%d\n", &vertexIndex[0], &vertexIndex[0],
                                 &vertexIndex[1], &vertexIndex[1], &vertexIndex[2], &vertexIndex[2]
                                );

            if ( matches != 6 )
            {
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                return false;
            }
            Triangle_vertexIndices.push_back(vertexIndex[0]);
            Triangle_vertexIndices.push_back(vertexIndex[1]);
            Triangle_vertexIndices.push_back(vertexIndex[2]);

        }
    }
    // For each vertex
    for( unsigned int i=0; i<temp_vertices.size(); i++ )
    {
        glm::vec3 vertex = temp_vertices[i];
        out_vertices.push_back(vertex);
    }
    // For each vertex of each triangle
    for( unsigned int i=0; i<Triangle_vertexIndices.size(); i++ )
    {
        unsigned int tri_v_Index = Triangle_vertexIndices[i];
        out_tri.push_back(tri_v_Index);
    }
    for( unsigned int i=0; i<temp_uvs.size(); i++ )
    {
        glm::vec2 uv = temp_uvs[i];
        out_uv.push_back(uv);
    }
    fclose(file);

    return true;
}


void myRender::UploadObj(const char *obj_path,float Scale)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<int> tri_list;

    bool res = loadOBJ(obj_path, vertices, tri_list,uvs);

    if(res==true)
    {
        printf("vertices size = %d \r\n",(int)vertices.size());
        printf("tri_list size = %d \r\n",(int)tri_list.size());
        printf("uvs size = %d \r\n",(int)uvs.size());
        Triangle_Num=tri_list.size()/3;
    }

    GLfloat *g_vertex_buffer_data = new GLfloat[Triangle_Num*3*3];
    GLfloat *g_uv_buffer_data = new GLfloat[Triangle_Num*3*2];


    for (int j = 0;  j < tri_list.size();  j++)
    {
        g_vertex_buffer_data[j*3+0]=vertices[tri_list[j]-1].x/Scale;
        g_vertex_buffer_data[j*3+1]=-vertices[tri_list[j]-1].y/Scale;
        g_vertex_buffer_data[j*3+2]=-vertices[tri_list[j]-1].z/Scale;

        g_uv_buffer_data[j*2+0]=uvs[tri_list[j]-1].x;
        g_uv_buffer_data[j*2+1]=uvs[tri_list[j]-1].y;

    }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, Triangle_Num*3*3*sizeof(GLfloat), g_vertex_buffer_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);



    glGenBuffers(1, &uvo);
    glBindBuffer(GL_ARRAY_BUFFER, uvo);
    glBufferData(GL_ARRAY_BUFFER, Triangle_Num*3*2*sizeof(GLfloat), g_uv_buffer_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}
myRender::~myRender()
{

}
void myRender::UploadTexture(const char*image_path)
{

    cv::Mat image=cv::imread(image_path);
    if(image.empty())
    {
        printf("opencv read image failed \r\n");
        exit(-2);
    }
    cv::flip(image,image,0);
    cv::cvtColor(image,image,CV_BGR2RGB);
    RGB_Image = new pangolin::GlTexture(image.cols, image.rows, GL_RGBA, true, 0, GL_RGB, GL_UNSIGNED_BYTE);
    RGB_Image->Upload(image.data, GL_RGB, GL_UNSIGNED_BYTE);
}

void myRender::render(pangolin::OpenGlMatrix mvp, const Eigen::Matrix4f & pose)
{




    drawProgram->Bind();

    drawProgram->setUniform(Uniform("MVP", mvp));
    drawProgram->setUniform(Uniform("pose", pose));



    //　把纹理ID和纹理单元绑定在一起。
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, RGB_Image->tid);

    GLint texLoc  = glGetUniformLocation(drawProgram->programId(), "myTextureSampler");
    glUniform1i(texLoc, 0);

    // 1rst attribute buffer : vertices
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        0            // array buffer offset
    );

    // 2nd attribute buffer : UVs
    glBindBuffer(GL_ARRAY_BUFFER, uvo);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );





    glDrawArrays(GL_TRIANGLES, 0, Triangle_Num*3); // Triangle_Num*3 = vertex num

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    drawProgram->Unbind();
}
