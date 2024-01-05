#pragma once

#include "Ray.h"
#include "Observable.h"
#include "BVH.h"
#include "OctreeRec.h"
#include "Mat4.h"

#define BUILD_OCTREE 0
uint OCTREE_DEPTH = 7;

std::string replace_slash(std::string str){
    std::string newStr = "";
    for (int i = 0; i < str.length(); i++){
        if (str[i] == '/'){
            newStr += " ";
        }
        else {
            newStr += str[i];
        }
    }
    return newStr;
}

struct TriangleMesh: public Observable{
    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Vector3> texcoords;
    std::vector<std::vector<int>> faces;
    Mat4 object_matrix;
    Vector3 boundingBox[2];
    std::shared_ptr<Observable> tree;

    TriangleMesh(){
        object_matrix = Mat4();
    }

    // TriangleMesh(TriangleMesh& mesh){
    //     vertices = mesh.vertices;
    //     normals = mesh.normals;
    //     texcoords = mesh.texcoords;
    //     faces = mesh.faces;
    //     object_matrix = mesh.object_matrix;
    //     mat = mesh.mat;
    //     boundingBox[0] = mesh.boundingBox[0];
    //     boundingBox[1] = mesh.boundingBox[1];
    //     tree = mesh.tree;
    // }

    TriangleMesh(const std::string& filename, Material material_){
        std::ifstream file(filename);
        if(!file.is_open()){
            std::cerr << "Could not open file " << filename << std::endl;
            return;
        }
        mat = material_;
        std::string line;
        while(std::getline(file, line)){
            if(line.empty()) continue;
            if(line[0] == 'v'){
                std::istringstream iss(line);
                std::string type;
                iss >> type;
                // vertex
                if(type == "v"){
                    Vector3 v;
                    iss >> v.x >> v.y >> v.z;
                    vertices.push_back(v);
                }
                // vertex normal
                else if(type == "vn"){
                    Vector3 vn;
                    iss >> vn.x >> vn.y >> vn.z;                     
                    normals.push_back(vn);
                }
                // vertex texture
                else if(type == "vt"){
                    Vector3 vt;
                    iss >> vt.x >> vt.y;
                    texcoords.push_back(vt);
                }
            }
            // face
            else if (line[0] == 'f'){
                std::vector<int> face;
                parse_face(face, line);
                faces.push_back(face);
            }
        }
        if (normals.size() == 0){
            calculate_normals();
        }
        file.close();
        // calculate normals if they are not given
        recalc_bounding_box();
    }

    void parse_face(std::vector<int>& face, std::string line){
        std::istringstream iss(line);
        std::string type;
        iss >> type;
        std::string fc;

        while (iss >> fc){
            fc = replace_slash(fc);
            int v;
            std::istringstream fcss(fc);
            fcss >> v;
            face.push_back(v);
            fcss >> v;
            face.push_back(v);
            fcss >> v;
            face.push_back(v);
        }

        int ftype = face_type(line);
        if (ftype == 1){
            face[2] = face[0];
            face[5] = face[3];
            face[8] = face[6];
            return;
        }
        else if(ftype == -1){
            std::cerr << "Unknown face type" << std::endl;
            return;
        }
    }

    int face_type(std::string line){
        // type 0 - f 1/2/3 4/5/6 7/8/9
        // type 1 - f 1/2 3/4 5/6
        // type 2 - f 1//2 3//4 5//6
        // type 3 - f 1 2 3
        if (line.find("//") != std::string::npos){
            return 2;
        }
        if (line.find("/") == std::string::npos){
            return 3;
        }
        // count number of "/"
        int count = 0;
        for (int i = 0; i < line.length(); i++){
            if (line[i] == '/'){
                count++;
            }
        }
        if (count == 6){
            return 0;
        }
        else if (count == 3){
            return 1;
        }
        else {
            std::cout << line << std::endl;
            std::cerr << "Unknown face type" << std::endl;
            return -1;
        }
    }


    void calculate_normals(){
        normals.clear();
        for (int i = 0; i < faces.size(); i++){
            normals.push_back(Vector3(0,0,0));
        }
        for (auto face: faces){
            Vector3 v0 = vertices[face[0] - 1];
            Vector3 v1 = vertices[face[3] - 1];
            Vector3 v2 = vertices[face[6] - 1];
            Vector3 normal = Vector3::normalize(Vector3::cross((v1 - v0),(v2 - v0)));
            normals[face[2] - 1] += normal;
            normals[face[5] - 1] += normal;
            normals[face[8] - 1] += normal;
        }
        for (int i = 0; i < normals.size(); i++){
            normals[i] = Vector3::normalize(normals[i]);
        }
    }

    void recalc_bounding_box(){
        // calculate bounding box for quick intersection testing
        Vector3 vmax = Vector3(-1e8);
        Vector3 vmin = Vector3(1e8);
        for(const auto& v : vertices){
            vmax = Vector3::max(vmax, v);
            vmin = Vector3::min(vmin, v);
        }
        std::cout << "vertices: " << vertices.size() << std::endl;
        std::cout << "normals: " << normals.size() << std::endl;
        std::cout << "texcoords: " << texcoords.size() << std::endl;
        std::cout << "faces: " << faces.size() << std::endl;
        std::cout << "vmin: " << vmin << std::endl;
        std::cout << "vmax: " << vmax << std::endl;
        boundingBox[0] = vmin;
        boundingBox[1] = vmax;
    }

    void recalc_tree(){
        std::vector<std::shared_ptr<Observable>> triangles;
        for (int i = 0; i < faces.size(); i++){
            std::vector<int> face = faces[i];
            Triangle tri = Triangle(vertices[face[0] - 1], vertices[face[3] - 1], vertices[face[6] - 1], i);
            triangles.push_back(std::make_shared<Triangle>(tri));
        }
#if BUILD_OCTREE
        tree = std::make_shared<Octree>(boundingBox, faces, vertices, OCTREE_DEPTH);
#else
        tree = std::make_shared<BVH>(triangles);
#endif
    }

    Vector3 centroid(){
        return (boundingBox[1] + boundingBox[0]) * 0.5f;
    }

    Vector3 max_vertex(){
        return boundingBox[1];
    }

    Vector3 min_vertex(){
        return boundingBox[0];
    }

    void transform(){
        for (int i = 0; i < vertices.size(); i++){
            vertices[i] = Mat4::transform_point(object_matrix, vertices[i]);
        }
        recalc_bounding_box();

        Mat4 inv = Mat4::invert(object_matrix);
        Mat4 norm_matrix = Mat4::transpose(inv);

        for (int i = 0; i < normals.size(); i++){
            normals[i] = Mat4::transform_direction(norm_matrix, normals[i]);
        }
    }


    ~TriangleMesh(){
        vertices.clear();
        normals.clear();
        texcoords.clear();
        faces.clear();
    }

    bool intersect(const Ray& ray, RayHit& inter){
        bool hit = tree->intersect(ray, inter);
        // check if we had a intersection of a triangle
        if (!hit){
            return false;
        }
        
        // index is greater than -1 if there is an intersection
        inter.point = ray.at(inter.distance);
        auto face = faces[inter.index];
        // if (Ntex->implemented){
        //     inter.normal = Ntex->get_colour(inter.u, inter.v);
        // }
        // else{
            inter.normal = Vector3::normalize(normals[face[2] - 1] * (1 - inter.hu - inter.hv) + normals[face[5] - 1] * inter.hu + normals[face[8] - 1] * inter.hv);
        //}
        inter.mat = std::make_shared<Material>(mat);
        Vector3 uv = texcoords[face[1] - 1] * (1 - inter.hu - inter.hv) + texcoords[face[4] - 1] * inter.hu + texcoords[face[7] - 1] * inter.hv;
        inter.u = uv.x;
        inter.v = uv.y;
        return true;
    }
};