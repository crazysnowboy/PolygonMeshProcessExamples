//
// Created by snowboy on 17-6-23.
//
#include "myGUI.h"
#include <zconf.h>
#include "myRender.h"

using namespace std;


class myShader
{
public:


    std::map<std::string, myRender*> RenderTool;
    myShader()
    {

        RenderTool[myRender::Test] = new myRender(loadProgramFromFile("TransformVertexShader.vert", "TextureFragmentShader.frag"));


        float obj_scale=100.0;
        RenderTool[myRender::Test]->UploadObj("data/current_merged.obj",obj_scale);

//        RenderTool[myRender::Test]->UploadTexture("data/current_merged.isomap.png");
        RenderTool[myRender::Test]->UploadTexture("data/timg2.jpg");

    }



};
class  myGUItest
{
public:

    myGUI * gui;

    myGUItest()
    {

        gui = new myGUI(GL_FALSE, GL_FALSE);
    }
    void Launch(void);
    void gui_run(void);

private:
    myShader *myshaer_;



};
void myGUItest::Launch(void)
{

    gui_run();
}

void myGUItest::gui_run(void)
{


    myshaer_=new myShader();
    while(!pangolin::ShouldQuit())
    {
        /////////////////////////
        gui->preCall();
        /////////////////////////

        myshaer_->RenderTool[myRender::Test]->render(gui->s_cam.GetProjectionModelViewMatrix(), Eigen::Matrix4f::Identity());


        /////////////////////////
        gui->postCall();
        /////////////////////////



    }
    printf(" close down \r\n");
}

int main(int argc, char *argv[])
{



    myGUItest ui_test;
    ui_test.Launch();

    return 0;
}
