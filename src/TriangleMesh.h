#pragma once

#include "Ray.h"
#include "Observable.h"
#include "OctreeRec.h"


std::string replaceSlash(std::string str){
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
            recalcNormals();
        }
        std::cout << "vertices: " << vertices.size() << std::endl;
        std::cout << "normals: " << normals.size() << std::endl;
        std::cout << "texcoords: " << texcoords.size() << std::endl;
        file.close();
        // calculate normals if they are not given
        recalcBoundingBox();
    }

    void parse_face(std::vector<int>& face, std::string line){
        std::istringstream iss(line);
        std::string type;
        iss >> type;
        std::string fc;

        while (iss >> fc){
            fc = replaceSlash(fc);
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

    void rescale(float factor){
        Vector3 centroid = getCentroid();
        for (int i = 0; i < vertices.size(); i++){
            vertices[i] = (vertices[i] - centroid) * factor + centroid;
        }
        recalcBoundingBox();
    }

    void recalcNormals(){
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

    void recalcBoundingBox(){
        // calculate bounding box for quick intersection testing
        Vector3 vmax = Vector3(-1e8);
        Vector3 vmin = Vector3(1e8);
        for(const auto& v : vertices){
            vmax = Vector3::max(vmax, v);
            vmin = Vector3::min(vmin, v);
        }
        std::cout << "vmin: " << vmin << std::endl;
        std::cout << "vmax: " << vmax << std::endl;
        Vector3 extend = (vmax - vmin) * 0.01;
        boundingBox[0] = vmin - extend;
        boundingBox[1] = vmax + extend;
    }

    void recalcOctree(){
        recalcBoundingBox();
        tree = Octree(boundingBox, faces, vertices, 7);
    }

    Vector3 getCentroid(){
        Vector3 centroid = Vector3(0,0,0);
        for (const auto& v : vertices){
            centroid += v;
        }
        centroid / vertices.size();
        return centroid / vertices.size();
    }

    void center(){
        Vector3 cent = (boundingBox[1] + boundingBox[0]) * 0.5f;
        for (int i = 0; i < vertices.size(); i++){
            vertices[i] -= cent;
        }
        recalcBoundingBox();
    }

    void floor(){
        recalcBoundingBox();
        float miny = boundingBox[0].y;
        translate(Vector3(0, -miny, 0));
    }
    
    void floor(float floorHeight){
        float miny = boundingBox[0].y;
        translate(Vector3(0, -miny + floorHeight, 0));
    }

    void rotate(float pitch, float roll, float yaw){
        float cosa = cos(yaw);
        float sina = sin(yaw);

        float cosb = cos(pitch);
        float sinb = sin(pitch);

        float cosc = cos(roll);
        float sinc = sin(roll);

        float Axx = cosa * cosb;
        float Axy = cosa * sinb * sinc - sina * cosc;
        float Axz = cosa * sinb * cosc + sina * sinc;

        float Ayx = sina * cosb;
        float Ayy = sina * sinb * sinc + cosa * cosc;
        float Ayz = sina * sinb * cosc - cosa * sinc;

        float Azx = -sinb;
        float Azy = cosb * sinc;
        float Azz = cosb * cosc;

        for (int i = 0; i < vertices.size(); i++){
            Vector3 v = vertices[i];
            vertices[i] = Vector3(
                v.x * Axx + v.y * Ayx + v.z * Azx,
                v.x * Axy + v.y * Ayy + v.z * Azy,
                v.x * Axz + v.y * Ayz + v.z * Azz
            );
        }
        recalcNormals();
        recalcBoundingBox();
    }

    void translate(const Vector3& translation){
        for(auto& v : vertices){
            v += translation;
        }
        recalcBoundingBox();
    }

    ~TriangleMesh(){
        vertices.clear();
        normals.clear();
        texcoords.clear();
        faces.clear();
    }

    bool intersect(Ray& ray, RayHit& inter){
        if (AABBIntersection(boundingBox[0], boundingBox[1], ray) < 0){
            return false;
        }
        bool in = false;
        bool hit = tree.intersection(ray, inter);
        // check if all elements in possibleFaces are in faces
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
        Vector3 uv = texcoords[face[1] - 1] * (1 - inter.hu - inter.hv) + texcoords[face[4] - 1] * inter.hu + texcoords[face[7] - 1] * inter.hv;
        inter.u = uv.x;
        inter.v = uv.y;
        return true;
    }

    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Vector3> texcoords;
    std::vector<Vector3> faceNormals;
    std::vector<std::vector<int>> faces;
    Vector3 boundingBox[2];
    Octree tree;
};