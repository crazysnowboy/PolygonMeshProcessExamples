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

#ifndef MYGUI_H_
#define MYGUI_H_

#include <pangolin/pangolin.h>
#include <pangolin/gl/gl.h>
#include <pangolin/gl/gldraw.h>
#include <map>
#include <Shaders.h>

#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

class myGUI
{
public:
    myGUI(bool liveCap, bool showcaseMode)
        : showcaseMode(showcaseMode)
    {
        width = 1280;
        height = 980;
        panel = 205;

        width += panel;

        pangolin::Params windowParams;

        windowParams.Set("SAMPLE_BUFFERS", 0);
        windowParams.Set("SAMPLES", 0);

        pangolin::CreateWindowAndBind("Main", width, height, windowParams);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);


        pangolin::SetFullscreen(showcaseMode);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        s_cam = pangolin::OpenGlRenderState(pangolin::ProjectionMatrix(640, 480, 420, 420, 320, 240, 0.1, 1000),
                                            pangolin::ModelViewLookAt(0, 0, -1, 0, 0, 1, pangolin::AxisNegY));

        pangolin::Display("cam").SetBounds(0, 1.0f, 0, 1.0f, -640 / 480.0)
        .SetHandler(new pangolin::Handler3D(s_cam));



        std::vector<std::string> labels;
        labels.push_back(std::string("residual"));
        labels.push_back(std::string("threshold"));
        resLog.SetLabels(labels);

        resPlot = new pangolin::Plotter(&resLog, 0, 300, 0, 0.0005, 30, 0.5);
        resPlot->Track("$i");

        std::vector<std::string> labels2;
        labels2.push_back(std::string("inliers"));
        labels2.push_back(std::string("threshold"));
        inLog.SetLabels(labels2);

        inPlot = new pangolin::Plotter(&inLog, 0, 300, 0, 40000, 30, 0.5);
        inPlot->Track("$i");

        if(!showcaseMode)
        {
            pangolin::CreatePanel("ui").SetBounds(0.0, 1.0, 0.0, pangolin::Attach::Pix(panel));
            pangolin::Display("multi").SetBounds(pangolin::Attach::Pix(0), 1 / 4.0f, showcaseMode ? 0 : pangolin::Attach::Pix(180), 1.0)
            .SetLayout(pangolin::LayoutEqualHorizontal)
            .AddDisplay(pangolin::Display("ModelImg"))
            .AddDisplay(pangolin::Display("Model"))
            .AddDisplay(*resPlot)
            .AddDisplay(*inPlot);
        }

        pause = new pangolin::Var<bool>("ui.Pause", false, true);
        step = new pangolin::Var<bool>("ui.Step", false, false);
        save = new pangolin::Var<bool>("ui.Save", false, false);
        reset = new pangolin::Var<bool>("ui.Reset", false, false);
        flipColors = new pangolin::Var<bool>("ui.Flip RGB", false, true);

        if(liveCap)
        {
            autoSettings = new pangolin::Var<bool>("ui.Auto Settings", true, true);
        }
        else
        {
            autoSettings = 0;
        }

        pyramid = new pangolin::Var<bool>("ui.Pyramid", true, true);
        so3 = new pangolin::Var<bool>("ui.SO(3)", true, true);
        frameToFrameRGB = new pangolin::Var<bool>("ui.Frame to frame RGB", false, true);
        fastOdom = new pangolin::Var<bool>("ui.Fast Odometry", false, true);
        rgbOnly = new pangolin::Var<bool>("ui.RGB only tracking", false, true);
        confidenceThreshold = new pangolin::Var<float>("ui.Confidence threshold", 10.0, 0.0, 24.0);
        depthCutoff = new pangolin::Var<float>("ui.Depth cutoff", 3.0, 0.0, 12.0);
        icpWeight = new pangolin::Var<float>("ui.ICP weight", 10.0, 0.0, 100.0);

        followPose = new pangolin::Var<bool>("ui.Follow pose", true, true);
        drawRawCloud = new pangolin::Var<bool>("ui.Draw raw", false, true);
        drawFilteredCloud = new pangolin::Var<bool>("ui.Draw filtered", false, true);
        drawGlobalModel = new pangolin::Var<bool>("ui.Draw global model", true, true);
        drawUnstable = new pangolin::Var<bool>("ui.Draw unstable points", false, true);
        drawPoints = new pangolin::Var<bool>("ui.Draw points", false, true);
        drawColors = new pangolin::Var<bool>("ui.Draw colors", showcaseMode, true);
        drawFxaa = new pangolin::Var<bool>("ui.Draw FXAA", showcaseMode, true);
        drawWindow = new pangolin::Var<bool>("ui.Draw time window", false, true);
        drawNormals = new pangolin::Var<bool>("ui.Draw normals", false, true);
        drawTimes = new pangolin::Var<bool>("ui.Draw times", false, true);
        drawDefGraph = new pangolin::Var<bool>("ui.Draw deformation graph", false, true);
        drawFerns = new pangolin::Var<bool>("ui.Draw ferns", false, true);
        drawDeforms = new pangolin::Var<bool>("ui.Draw deformations", true, true);

        gpuMem = new pangolin::Var<int>("ui.GPU memory free", 0);

        totalPoints = new pangolin::Var<std::string>("ui.Total points", "0");
        totalNodes = new pangolin::Var<std::string>("ui.Total nodes", "0");
        totalFerns = new pangolin::Var<std::string>("ui.Total ferns", "0");
        totalDefs = new pangolin::Var<std::string>("ui.Total deforms", "0");
        totalFernDefs = new pangolin::Var<std::string>("ui.Total fern deforms", "0");

        trackInliers = new pangolin::Var<std::string>("ui.Inliers", "0");
        trackRes = new pangolin::Var<std::string>("ui.Residual", "0");
        logProgress = new pangolin::Var<std::string>("ui.Log", "0");

        if(showcaseMode)
        {
            pangolin::RegisterKeyPressCallback(' ', pangolin::SetVarFunctor<bool>("ui.Reset", true));
        }
    }

    virtual ~myGUI()
    {
        delete pause;
        delete reset;
        delete inPlot;
        delete resPlot;

        if(autoSettings)
        {
            delete autoSettings;

        }
        delete step;
        delete save;
        delete trackInliers;
        delete trackRes;
        delete confidenceThreshold;
        delete totalNodes;
        delete drawWindow;
        delete so3;
        delete totalFerns;
        delete totalDefs;
        delete depthCutoff;
        delete logProgress;
        delete drawTimes;
        delete drawFxaa;
        delete fastOdom;
        delete icpWeight;
        delete pyramid;
        delete rgbOnly;
        delete totalFernDefs;
        delete drawFerns;
        delete followPose;
        delete drawDeforms;
        delete drawRawCloud;
        delete totalPoints;
        delete frameToFrameRGB;
        delete flipColors;
        delete drawFilteredCloud;
        delete drawNormals;
        delete drawColors;
        delete drawGlobalModel;
        delete drawUnstable;
        delete drawPoints;
        delete drawDefGraph;
        delete gpuMem;


    }

    void preCall()
    {
        glClearColor(0.05 * !showcaseMode, 0.05 * !showcaseMode, 0.3 * !showcaseMode, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        width = pangolin::DisplayBase().v.w;
        height = pangolin::DisplayBase().v.h;

        pangolin::Display("cam").Activate(s_cam);
    }



    void postCall()
    {
        GLint cur_avail_mem_kb = 0;
        glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &cur_avail_mem_kb);

        int memFree = cur_avail_mem_kb / 1024;

        gpuMem->operator=(memFree);

        pangolin::FinishFrame();

        glFinish();
    }



    bool showcaseMode;
    int width;
    int height;
    int panel;

    pangolin::Var<bool> * pause,
             * step,
             * save,
             * reset,
             * flipColors,
             * rgbOnly,
             * pyramid,
             * so3,
             * frameToFrameRGB,
             * fastOdom,
             * followPose,
             * drawRawCloud,
             * drawFilteredCloud,
             * drawNormals,
             * autoSettings,
             * drawDefGraph,
             * drawColors,
             * drawFxaa,
             * drawGlobalModel,
             * drawUnstable,
             * drawPoints,
             * drawTimes,
             * drawFerns,
             * drawDeforms,
             * drawWindow;
    pangolin::Var<int> * gpuMem;
    pangolin::Var<std::string> * totalPoints,
             * totalNodes,
             * totalFerns,
             * totalDefs,
             * totalFernDefs,
             * trackInliers,
             * trackRes,
             * logProgress;
    pangolin::Var<float> * confidenceThreshold,
             * depthCutoff,
             * icpWeight;

    pangolin::DataLog resLog, inLog;
    pangolin::Plotter * resPlot,
             * inPlot;

    pangolin::OpenGlRenderState s_cam;


};


#endif /* GUI_H_ */
