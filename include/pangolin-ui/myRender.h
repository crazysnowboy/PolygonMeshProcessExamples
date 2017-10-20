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

#ifndef MYRENDER_H_
#define MYRENDER_H_

#include "Shaders.h"
#include "Uniform.h"

#include <pangolin/gl/gl.h>
#include <pangolin/display/opengl_render_state.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Defines.h"
#include <zconf.h>

#include "opencv2/opencv.hpp"


class myRender
{
public:

    myRender(std::shared_ptr<Shader> program):drawProgram(program)
    {

    }

    virtual ~myRender();

    void UploadTexture(const char*image_path);
    void UploadObj(const char *obj_path,float Scale);
    void render( pangolin::OpenGlMatrix mvp, const Eigen::Matrix4f & pose);
    bool loadOBJ(const char * path, std::vector < glm::vec3 > & out_vertices, std::vector < int> & out_tri,std::vector < glm::vec2> & out_uv);

    static const std::string Test;

private:

    GLuint vbo;
    GLuint fid;
    GLuint uvo;
    int Triangle_Num;
    std::shared_ptr<Shader> drawProgram;

    pangolin::GlTexture* RGB_Image;
};

#endif /* FEEDBACKBUFFER_H_ */
