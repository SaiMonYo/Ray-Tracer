#pragma once

#include "Vector.h"

struct Mat4{
    float cells[16];

    inline Mat4() {
		memset(cells, 0, sizeof(cells));
		cells[0]  = 1.0f;
		cells[5]  = 1.0f;
		cells[10] = 1.0f;
		cells[15] = 1.0f;
	}

    inline float& operator()(int row, int column){
        return cells[column + (row << 2)];
    }

    inline const float& operator()(int row, int column) const {
        return cells[column + (row << 2)];
    }

    inline static Mat4 create_translation(Vector3 trans){
        Mat4 matrix = Mat4();
        matrix(0, 3) = trans.x;
        matrix(1, 3) = trans.y;
        matrix(2, 3) = trans.z;

        return matrix;
    }

    inline static Mat4 create_scalar(Vector3 scale){
        Mat4 matrix = Mat4();
        matrix(0, 0) = scale.x;
        matrix(1, 1) = scale.y;
        matrix(2, 2) = scale.z; 

        return matrix;
    }

    
};