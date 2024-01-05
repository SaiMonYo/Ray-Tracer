#include <iostream>
#include "Scene.h"
#include "Plane.h"
#include "Renderer.h"
#include "SphericalLight.h"
#include "TriangleMesh.h"
#include "ObjLoader.h"

std::string DEFAULT_OUTPUT = "images/result.qoi";
int DEFAULT_WIDTH = 1920;
int DEFAULT_HEIGHT = 1080;


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
    rhet->mat.K_Dtex = std::make_shared<ImageTexture>("objs/rhetorican/source/retheur_-_LowPoly_u1_v1.qoi");
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

    std::shared_ptr<BVH> bvh = std::make_shared<BVH>(load_obj("objs/Jinx/jinx.obj"));
    world.add_object(bvh);

    world.ambientColour = Vector3::to_colour("#FFFFFF") * 0.35;
    world.add_light(std::make_shared<SphericalLight>(Vector3(-7,10,10), Vector3::to_colour("#FFFFFF"), 300, 0));

    world.cam.setup(Vector3(-4.29, 0.4, 0.8), Vector3(0, 0.4, 0.8));

    Renderer ren = Renderer(&qoi, DEFAULT_WIDTH, DEFAULT_HEIGHT, world);
    ren.render();
    std::cout << triangle_count << std::endl;
    std::cout << AABBIntersectionCount << std::endl;

    output.close();
}

void apple(){
    std::ofstream output(DEFAULT_OUTPUT, std::ios::out|std::ios::binary);
    QOIWriter qoi = QOIWriter(output, DEFAULT_WIDTH, DEFAULT_HEIGHT);

    Scene world = Scene();

    SkySphere sky = SkySphere(std::make_shared<ImageTexture>("SkyTextures/peppermint_powerplant_4k.qoi"));
    world.sky = std::make_shared<SkySphere>(sky);

    std::shared_ptr<BVH> bvh = std::make_shared<BVH>(load_obj("objs/Apple/apple.obj"));
    world.add_object(bvh);

    world.ambientColour = Vector3::to_colour("#FFFFFF") * 0.35;
    world.add_light(std::make_shared<SphericalLight>(Vector3(5,5,5), Vector3::to_colour("#FFFFFF"), 300, 0));

    world.cam.setup(Vector3(0, 0.4, 0), Vector3(0, 0, 0));

    Renderer ren = Renderer(&qoi, DEFAULT_WIDTH, DEFAULT_HEIGHT, world);
    ren.render();
    std::cout << triangle_count << std::endl;
    std::cout << AABBIntersectionCount << std::endl;

    output.close();
}

void cornell(){
    std::ofstream output(DEFAULT_OUTPUT, std::ios::out|std::ios::binary);
    QOIWriter qoi = QOIWriter(output, DEFAULT_WIDTH, DEFAULT_HEIGHT);

    Scene world = Scene();

    std::shared_ptr<BVH> bvh = std::make_shared<BVH>(load_obj("objs/cornell/cornell-box.obj"));
    world.add_object(bvh);

    world.ambientColour = Vector3::to_colour("#FFFFFF") * 0.5;
    world.cam.setup(Vector3(0, 5, 10), Vector3(0, -5, 0));

    Renderer ren = Renderer(&qoi, DEFAULT_WIDTH, DEFAULT_HEIGHT, world);
    ren.render();
    std::cout << triangle_count << std::endl;
    std::cout << AABBIntersectionCount << std::endl;

    output.close();
}



int main(){
    //bust();
    //jinx();
    //jinx();
    cornell();
}
