#include <iostream>
#include "Scene.h"
#include "Sphere.h"
#include "Plane.h"
#include "Renderer.h"
#include "SphericalLight.h"
#include "TriangleMesh.h"

std::string DEFAULT_OUTPUT = "images/result.qoi";
int DEFAULT_WIDTH = 1920;
int DEFAULT_HEIGHT = 1080;

int start(){
    std::ofstream output(DEFAULT_OUTPUT, std::ios::out|std::ios::binary);
    QOIWriter qoi = QOIWriter(output, DEFAULT_WIDTH, DEFAULT_HEIGHT);

    Scene world = Scene();
    world.add_object(std::make_shared<Sphere>(Vector3(-0.95, 3.63261, -0.21884), 0.35, "#FF1D25"));
    world.add_object(std::make_shared<Sphere>(Vector3(-0.4, 4.33013, 0.5), 0.7, "#0071BC"));
    world.add_object(std::make_shared<Sphere>(Vector3(0.72734, 3.19986, -0.35322), 0.45, "#3AA010"));
    world.add_light(std::make_shared<SphericalLight>(Vector3(2,4.5,2), Vector3(1), 80, 10));
    world.add_light(std::make_shared<SphericalLight>(Vector3(-2,1,2.5), Vector3::to_colour("#AAAAFF"), 80, 10));

    Renderer ren = Renderer(&qoi, DEFAULT_WIDTH, DEFAULT_HEIGHT, world);
    ren.render();

    output.close();
    return 1;
}

void test(){
    std::ofstream output(DEFAULT_OUTPUT, std::ios::out|std::ios::binary);
    QOIWriter qoi = QOIWriter(output, DEFAULT_WIDTH, DEFAULT_HEIGHT);

    Scene world = Scene();
    world.add_object(std::make_shared<Sphere>(Vector3(0.55, 3.5, -0.16), 0.5, "#0030BC"));
    world.add_object(std::make_shared<Sphere>(Vector3(-0.55, 5, 0), 0.9, "#FF1122"));
    world.add_light(std::make_shared<SphericalLight>(Vector3(1,2,3), Vector3::to_colour("#B3DDFF"), 80, 10));
    world.add_light(std::make_shared<SphericalLight>(Vector3(-5,4,1), Vector3::to_colour("#FFB0B2"), 80, 10));
    
    world.ambientColour = Vector3::to_colour("#FFFFFF") * 0.05;

    Renderer ren = Renderer(&qoi, DEFAULT_WIDTH, DEFAULT_HEIGHT, world);
    ren.render();

    output.close();
}

void wd40(){
    std::ofstream output(DEFAULT_OUTPUT, std::ios::out|std::ios::binary);
    QOIWriter qoi = QOIWriter(output, DEFAULT_WIDTH, DEFAULT_HEIGHT);

    Scene world = Scene();

    Material lube_mat = parse_material("objs/wd40/lubricant_spray_8k.mtl");
    std::cout << lube_mat.K_d << std::endl;
    lube_mat.K_a = Vector3(1);
    std::shared_ptr<TriangleMesh> lube = std::make_shared<TriangleMesh>("objs/wd40/lubricant_spray_8k.obj", lube_mat);
    // lube->rotate(0, -M_PI_2, 0);
    // lube->rescale(10);
    // lube->translate(Vector3(0, 4, 0));
    // lube->recalcOctree();
    lube->K_Atex = std::make_shared<ImageTexture>("objs/wd40/textures/lubricant_spray_diff_8k.qoi");

    world.add_object(lube);

    world.ambientColour = Vector3::to_colour("#FFFFFF") * 0.3;
    world.add_light(std::make_shared<SphericalLight>(Vector3(4,5,6), Vector3::to_colour("#FFFFFF"), 500, 0));

    Renderer ren = Renderer(&qoi, DEFAULT_WIDTH, DEFAULT_HEIGHT, world);
    ren.render();
    std::cout << triangle_count << std::endl;
    std::cout << AABBIntersectionCount << std::endl;

    output.close();
}

void bust(){
    std::ofstream output(DEFAULT_OUTPUT, std::ios::out|std::ios::binary);
    QOIWriter qoi = QOIWriter(output, DEFAULT_WIDTH, DEFAULT_HEIGHT);

    Scene world = Scene();

    Material lube_mat = parse_material("objs/wd40/lubricant_spray_8k.mtl");
    std::cout << lube_mat.K_d << std::endl;
    lube_mat.K_a = Vector3(1);
    std::shared_ptr<TriangleMesh> rhet = std::make_shared<TriangleMesh>("objs/rhetorican/source/bust.obj", lube_mat);

    Mat4 object_matrix = Mat4::create_translation(Vector3(0, 20, -15)) * Mat4::create_rotation(Vector3(0, 0, M_PI/4));
    rhet->object_matrix = object_matrix;
    rhet->transform();
    rhet->K_Atex = std::make_shared<ImageTexture>("objs/rhetorican/source/retheur_-_LowPoly_u1_v1.qoi");
    rhet->recalc_tree();
    world.add_object(rhet);

    world.ambientColour = Vector3::to_colour("#FFFFFF") * 0.3;
    world.add_light(std::make_shared<SphericalLight>(Vector3(-10,15,6), Vector3::to_colour("#FFFFFF"), 500, 0));

    Renderer ren = Renderer(&qoi, DEFAULT_WIDTH, DEFAULT_HEIGHT, world);
    ren.render();
    std::cout << triangle_count << std::endl;
    std::cout << AABBIntersectionCount << std::endl;

    output.close();
}

void jinx(){
    std::ofstream output(DEFAULT_OUTPUT, std::ios::out|std::ios::binary);
    QOIWriter qoi = QOIWriter(output, DEFAULT_WIDTH, DEFAULT_HEIGHT);

    Scene world = Scene();

    Material lube_mat = parse_material("objs/wd40/lubricant_spray_8k.mtl");
    std::cout << lube_mat.K_d << std::endl;
    lube_mat.K_a = Vector3(1);
    std::shared_ptr<TriangleMesh> rhet = std::make_shared<TriangleMesh>("objs/Jinx/jinx.obj", lube_mat);

    //Mat4 object_matrix = Mat4::create_translation(Vector3(0.5, 3.8, -0.7)) * Mat4::create_rotation(Vector3(0.02, 0, M_PI/2));
    //rhet->object_matrix = object_matrix;
    //rhet->transform();
    rhet->recalc_tree();
    world.add_object(rhet);

    world.ambientColour = Vector3::to_colour("#FFFFFF") * 0.3;
    world.add_light(std::make_shared<SphericalLight>(Vector3(-5,20,10), Vector3::to_colour("#FFFFFF"), 500, 0));

    world.cam.setup(Vector3(-4.29, 0.4, 0.8), Vector3(0, 0.4, 0.8));

    Renderer ren = Renderer(&qoi, DEFAULT_WIDTH, DEFAULT_HEIGHT, world);
    ren.render();
    std::cout << triangle_count << std::endl;
    std::cout << AABBIntersectionCount << std::endl;

    output.close();
}

void jinx2(){
    std::ofstream output(DEFAULT_OUTPUT, std::ios::out|std::ios::binary);
    QOIWriter qoi = QOIWriter(output, DEFAULT_WIDTH, DEFAULT_HEIGHT);

    Scene world = Scene();

    Material lube_mat = parse_material("objs/wd40/lubricant_spray_8k.mtl");
    std::cout << lube_mat.K_d << std::endl;
    lube_mat.K_a = Vector3(1);

    std::shared_ptr<TriangleMesh> Ears_mat0_Ears_0 = std::make_shared<TriangleMesh>("objs/Jinx/Ears_mat0_Ears_0.obj", lube_mat);
    Ears_mat0_Ears_0->K_Atex = std::make_shared<ImageTexture>("objs/Jinx/textures/mat0_Ears_baseColor.qoi");
    Ears_mat0_Ears_0->recalc_tree();
    world.add_object(Ears_mat0_Ears_0);

    std::shared_ptr<TriangleMesh> Ears_mat0_Ears_1 = std::make_shared<TriangleMesh>("objs/Jinx/Ears_mat0_Ears_1.obj", lube_mat);
    Ears_mat0_Ears_1->K_Atex = std::make_shared<ImageTexture>("objs/Jinx/textures/mat0_Chest_baseColor.qoi");
    Ears_mat0_Ears_1->recalc_tree();
    world.add_object(Ears_mat0_Ears_1);

    std::shared_ptr<TriangleMesh> Ears_mat0_Ears_10 = std::make_shared<TriangleMesh>("objs/Jinx/Ears_mat0_Ears_10.obj", lube_mat);
    Ears_mat0_Ears_10->K_Atex = std::make_shared<ImageTexture>("objs/Jinx/textures/mat0_Lashes_baseColor.qoi");
    Ears_mat0_Ears_10->recalc_tree();
    world.add_object(Ears_mat0_Ears_10);

    std::shared_ptr<TriangleMesh> Ears_mat0_Ears_11 = std::make_shared<TriangleMesh>("objs/Jinx/Ears_mat0_Ears_11.obj", lube_mat);
    Ears_mat0_Ears_11->K_Atex = std::make_shared<ImageTexture>("objs/Jinx/textures/mat0_FX_baseColor.qoi");
    Ears_mat0_Ears_11->recalc_tree();
    world.add_object(Ears_mat0_Ears_11);

    std::shared_ptr<TriangleMesh> Ears_mat0_Ears_12 = std::make_shared<TriangleMesh>("objs/Jinx/Ears_mat0_Ears_12.obj", lube_mat);
    Ears_mat0_Ears_12->K_Atex = std::make_shared<ImageTexture>("objs/Jinx/textures/mat0_Blast_baseColor.qoi");
    Ears_mat0_Ears_12->recalc_tree();
    world.add_object(Ears_mat0_Ears_12);

    std::shared_ptr<TriangleMesh> Ears_mat0_Ears_13 = std::make_shared<TriangleMesh>("objs/Jinx/Ears_mat0_Ears_13.obj", lube_mat);
    Ears_mat0_Ears_13->K_Atex = std::make_shared<ImageTexture>("objs/Jinx/textures/mat0_BG_baseColor.qoi");
    Ears_mat0_Ears_13->recalc_tree();
    world.add_object(Ears_mat0_Ears_13);

    std::shared_ptr<TriangleMesh> Ears_mat0_Ears_2 = std::make_shared<TriangleMesh>("objs/Jinx/Ears_mat0_Ears_2.obj", lube_mat);
    Ears_mat0_Ears_2->K_Atex = std::make_shared<ImageTexture>("objs/Jinx/textures/mat0_Hair_baseColor.qoi");
    Ears_mat0_Ears_2->recalc_tree();
    world.add_object(Ears_mat0_Ears_2);

    std::shared_ptr<TriangleMesh> Ears_mat0_Ears_3 = std::make_shared<TriangleMesh>("objs/Jinx/Ears_mat0_Ears_3.obj", lube_mat);
    Ears_mat0_Ears_3->K_Atex = std::make_shared<ImageTexture>("objs/Jinx/textures/mat0_Head_baseColor.qoi");
    Ears_mat0_Ears_3->recalc_tree();
    world.add_object(Ears_mat0_Ears_3);

    std::shared_ptr<TriangleMesh> Ears_mat0_Ears_4 = std::make_shared<TriangleMesh>("objs/Jinx/Ears_mat0_Ears_4.obj", lube_mat);
    Ears_mat0_Ears_4->K_Atex = std::make_shared<ImageTexture>("objs/Jinx/textures/mat0_Eyes_baseColor.qoi");
    Ears_mat0_Ears_4->recalc_tree();
    world.add_object(Ears_mat0_Ears_4);

    std::shared_ptr<TriangleMesh> Ears_mat0_Ears_5 = std::make_shared<TriangleMesh>("objs/Jinx/Ears_mat0_Ears_5.obj", lube_mat);
    Ears_mat0_Ears_5->K_Atex = std::make_shared<ImageTexture>("objs/Jinx/textures/mat0_Teeth_baseColor.qoi");
    Ears_mat0_Ears_5->recalc_tree();
    world.add_object(Ears_mat0_Ears_5);

    std::shared_ptr<TriangleMesh> Ears_mat0_Ears_6 = std::make_shared<TriangleMesh>("objs/Jinx/Ears_mat0_Ears_6.obj", lube_mat);
    Ears_mat0_Ears_6->K_Atex = std::make_shared<ImageTexture>("objs/Jinx/textures/mat0_Bangs_baseColor.qoi");
    Ears_mat0_Ears_6->recalc_tree();
    world.add_object(Ears_mat0_Ears_6);

    std::shared_ptr<TriangleMesh> Ears_mat0_Ears_7 = std::make_shared<TriangleMesh>("objs/Jinx/Ears_mat0_Ears_7.obj", lube_mat);
    Ears_mat0_Ears_7->K_Atex = std::make_shared<ImageTexture>("objs/Jinx/textures/mat0_Hair_Bracelet_2_baseColor.qoi");
    Ears_mat0_Ears_7->recalc_tree();
    world.add_object(Ears_mat0_Ears_7);

    std::shared_ptr<TriangleMesh> Ears_mat0_Ears_8 = std::make_shared<TriangleMesh>("objs/Jinx/Ears_mat0_Ears_8.obj", lube_mat);
    Ears_mat0_Ears_8->K_Atex = std::make_shared<ImageTexture>("objs/Jinx/textures/mat0_Pin_baseColor.qoi");
    Ears_mat0_Ears_8->recalc_tree();
    world.add_object(Ears_mat0_Ears_8);

    std::shared_ptr<TriangleMesh> Ears_mat0_Ears_9 = std::make_shared<TriangleMesh>("objs/Jinx/Ears_mat0_Ears_9.obj", lube_mat);
    Ears_mat0_Ears_9->K_Atex = std::make_shared<ImageTexture>("objs/Jinx/textures/mat0_Braids_baseColor.qoi");
    Ears_mat0_Ears_9->recalc_tree();
    world.add_object(Ears_mat0_Ears_9);


    world.ambientColour = Vector3::to_colour("#FFFFFF") * 0.45;
    //world.add_light(std::make_shared<SphericalLight>(Vector3(-10,10,10), Vector3::to_colour("#FFFFFF"), 5000, 0));

    world.cam.setup(Vector3(-4.29, 0.4, 0.8), Vector3(0, 0.4, 0.8));

    Renderer ren = Renderer(&qoi, DEFAULT_WIDTH, DEFAULT_HEIGHT, world);
    ren.render();
    std::cout << triangle_count << std::endl;
    std::cout << AABBIntersectionCount << std::endl;

    output.close();
}



int main(){
    //test();
    //start();
    //wd40();
    //bust();
    jinx2();
}
