#include <iostream>
#include "Scene.h"
#include "Sphere.h"
#include "Plane.h"
#include "Renderer.h"
#include "SphericalLight.h"
#include "TriangleMesh.h"

std::string DEFAULT_OUTPUT = "images/result.qoi";
int DEFAULT_WIDTH = 1080;
int DEFAULT_HEIGHT = 2340;

int start(){
    std::ofstream output(DEFAULT_OUTPUT, std::ios::out|std::ios::binary);
    QOIWriter qoi = QOIWriter(output, DEFAULT_WIDTH, DEFAULT_HEIGHT);

    Scene world = Scene();
    world.add_object(std::make_shared<Sphere>(Vector3(-0.95, 3.63261, -0.21884), 0.35, "#FF1D25"));
    world.add_object(std::make_shared<Sphere>(Vector3(-0.4, 4.33013, 0.5), 0.7, "#0071BC"));
    world.add_object(std::make_shared<Sphere>(Vector3(0.72734, 3.19986, -0.35322), 0.45, "#3AA010"));
    world.add_object(std::make_shared<Plane>(Vector3(0,4.68013,-0.10622), Vector3(0,-2.180126190185547, 4.2239089012146), "#111111"));
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



int main(){
    //test();
    //start();
    //wd40();
    bust();
}
