import sys
import re

def ints(line):
    return list(map(int, re.findall(r"(\d+)", line)))

def split(filename): 
    filepath = filename.rsplit("/", 1)[0] + "/"
    vertices = {"v": [], "vn": [], "vt": []}
    objects = {}
    extra_info = {}
    obj_file = open(filename, "r")
    current_object = ""
    for line in obj_file:
        if line.startswith("o"):
            current_object = line.split()[1]
            objects[current_object] = []
        else:
            start = line.split()[0]
            if start in vertices:
                vertices[start].append(line)
            elif start == "f":
                objects[current_object].append(line)
            else:
                if current_object not in extra_info:
                    extra_info[current_object] = []
                else:
                    extra_info[current_object].append(line) 
    obj_file.close()
    
    for object_name in objects:
        faces = objects[object_name]
        unique_vs = set()
        unique_vts = set()
        unique_vns = set()
        for face in faces:
            for vertex in face.split()[1:]:
                data = ints(vertex)
                if len(data) == 1:
                    unique_vs.add(data[0])
                elif len(data) == 3:
                    unique_vs.add(data[0])
                    unique_vts.add(data[1])
                    unique_vns.add(data[2])
                elif len(data) == 2:
                    if "//" in vertex:
                        unique_vs.add(data[0])
                        unique_vns.add(data[1])
                    else:
                        unique_vs.add(data[0])
                        unique_vts.add(data[1])
        unique_vs = sorted(list(unique_vs))
        unique_vts = sorted(list(unique_vts))
        unique_vns = sorted(list(unique_vns))
        maps = {"v": {}, "vt": {}, "vn": {}} # maps old indices to new ones
        for i in range(len(unique_vs)):
            maps["v"][unique_vs[i]] = i + 1
        for i in range(len(unique_vts)):
            maps["vt"][unique_vts[i]] = i + 1
        for i in range(len(unique_vns)):
            maps["vn"][unique_vns[i]] = i + 1
        
        with open(filepath + object_name + ".obj", "w") as f:
            for line in extra_info[object_name]:
                f.write(line)
            for vertex in unique_vs:
                f.write(vertices["v"][vertex - 1])
            for vertex in unique_vts:
                f.write(vertices["vt"][vertex - 1])
            for vertex in unique_vns:
                f.write(vertices["vn"][vertex - 1])
            for face in faces:
                new_face = "f "
                for vertex in face.split()[1:]:
                    data = ints(vertex)
                    if len(data) == 1:
                        new_face += str(maps["v"][data[0]]) + " "
                    elif len(data) == 3:
                        new_face += str(maps["v"][data[0]]) + "/" + str(maps["vt"][data[1]]) + "/" + str(maps["vn"][data[2]]) + " "
                    elif len(data) == 2:
                        if "//" in vertex:
                            new_face += str(maps["v"][data[0]]) + "//" + str(maps["vn"][data[1]]) + " "
                        else:
                            new_face += str(maps["v"][data[0]]) + "/" + str(maps["vt"][data[1]]) + " "
                f.write(new_face + "\n")
        
            


if __name__ == "__main__":
    filename = sys.argv[1]
    split(filename)