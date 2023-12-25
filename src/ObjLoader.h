#pragma once

#include "TriangleMesh.h"
#include "BVH.h"
#include <map>

void parse_face(std::vector<int>& face, std::string line);
int face_type(std::string line);
void load_mtllib(std::string filename, std::map<std::string, Material>& materials);

BVH load_obj(const std::string& filename, Material mat){
    std::vector<TriangleMesh> meshes;
    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Vector3> texcoords;
    std::map<std::string, Material> materials;

    std::string path;
    int last_slash = filename.find_last_of("/\\");
    if (last_slash != std::string::npos){
        path = filename.substr(0, last_slash + 1);
    }
    else{
        path = "./";
    }

    std::ifstream file(filename);
    if(!file.is_open()){
        std::cerr << "Could not open file " << filename << std::endl;
        return BVH();
    }

    TriangleMesh mesh;
    std::string line;
    while(std::getline(file, line)){
        if(line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        std::string type;
        iss >> type;
        if(type == "o"){
            if (mesh.faces.size() > 0){
                meshes.push_back(mesh);
            }
            mesh = TriangleMesh();
        }
        else if (type == "mtllib"){
            std::string mtllib_name;
            iss >> mtllib_name;
            load_mtllib(path + mtllib_name, materials);
        }
        else if (type == "usemtl"){
            std::string mat_name;
            iss >> mat_name;
            if (materials.find(mat_name) != materials.end()){
                mesh.mat = materials[mat_name];
            }
        }
        else if(line[0] == 'v'){
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
            mesh.faces.push_back(face);
        }
    }
    if (mesh.faces.size() > 0){
        meshes.push_back(mesh);
    }

    if (texcoords.size() == 0){
        texcoords.resize(vertices.size(), Vector3(0,0,0));
    }
    bool need_to_calc_normals = false;
    if (normals.size() == 0){
        normals.resize(vertices.size(), Vector3(0,0,0));
        need_to_calc_normals = true;
    }

    for (auto& mesh: meshes){
        std::vector<std::vector<int>> new_faces;
        for (auto face: mesh.faces){
            std::vector<int> new_face;
            // maps
            std::vector<int> vmap(vertices.size(), -1);
            std::vector<int> vtmap(texcoords.size(), -1);
            std::vector<int> vnmap(normals.size(), -1);
            for (int i = 0; i < face.size(); i += 3){
                int v = face[i];
                int vt = face[i + 1];
                int vn = face[i + 2];
                if (vmap[v-1] == -1){
                    mesh.vertices.push_back(vertices[v-1]);
                    vmap[v-1] = mesh.vertices.size();
                }
                if (vtmap[vt-1] == -1){
                    mesh.texcoords.push_back(texcoords[vt-1]);
                    vtmap[vt-1] = mesh.texcoords.size();
                }
                if (vnmap[vn-1] == -1){
                    mesh.normals.push_back(normals[vn-1]);
                    vnmap[vn-1] = mesh.normals.size();
                }
                new_face.push_back(vmap[v-1]);
                new_face.push_back(vtmap[vt-1]);
                new_face.push_back(vnmap[vn-1]);
            }
            new_faces.push_back(new_face);
        }
        mesh.faces = new_faces;
        if (need_to_calc_normals){
            mesh.calculate_normals();
        }
        mesh.recalc_bounding_box();
        mesh.recalc_tree();
    }
    file.close();

    long long int vsum = 0;
    long long int vnsum = 0;
    long long int vtsum = 0;
    long long int fsum = 0;
    for (auto mesh: meshes){
        vsum += mesh.vertices.size();
        vnsum += mesh.normals.size();
        vtsum += mesh.texcoords.size();
        fsum += mesh.faces.size();
    }
    std::cout << "vertices: " << vsum << std::endl;
    std::cout << "normals: " << vnsum << std::endl;
    std::cout << "texcoords: " << vtsum << std::endl;
    std::cout << "faces: " << fsum << std::endl;

    std::vector<std::shared_ptr<Observable>> meshes_ptrs;
    for (auto mesh: meshes){
        std::shared_ptr<Observable> mesh_ptr = std::make_shared<TriangleMesh>(mesh);
        meshes_ptrs.push_back(mesh_ptr);
    }
    BVH bvh(meshes_ptrs);
    return bvh;
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

void load_mtllib(std::string filename, std::map<std::string, Material>& materials){
    std::ifstream file(filename);
    if(!file.is_open()){
        std::cerr << "Could not open file " << filename << std::endl;
        return;
    }

    std::string path;
    int last_slash = filename.find_last_of("/\\");
    if (last_slash != std::string::npos){
        path = filename.substr(0, last_slash + 1);
    }
    else{
        path = "./";
    }

    std::string line;
    Material mat;
    std::string mat_name = "";
    while (std::getline(file, line)){
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string component;
        iss >> component;

        if (component == "newmtl"){
            if (mat_name != ""){
                materials[mat_name] = mat;
            }
            iss >> mat_name;
        }
        else if (component == "map_Kd"){
            std::string texure_name = "";
            iss >> texure_name;
            try{
                mat.K_Dtex = std::make_shared<ImageTexture>(path + texure_name);
            }
            catch(std::runtime_error& e){
                std::cout << "Missing texture: " << path + texure_name << std::endl;
            }
        }
        else if (component == "Ka"){
            Vector3 v;
            iss >> v.x >> v.y >> v.z;
            mat.K_a = v;
        }
        else if (component == "Kd"){
            Vector3 v;
            iss >> v.x >> v.y >> v.z;
            mat.K_d = v;
        }
        else if (component == "Ks"){
            Vector3 v;
            iss >> v.x >> v.y >> v.z;
            mat.K_s = v;
        }
        else if (component == "Ns"){
            float v;
            iss >> v;
            mat.N_s = v;
        }
        else if (component == "Ni"){
            float v;
            iss >> v;
            mat.N_i = v;
        }
        else if (component == "d"){
            float v;
            iss >> v;
            mat.d = v;
        }
        else{}
    }
    if (mat_name != ""){
        materials[mat_name] = mat;
    }
}