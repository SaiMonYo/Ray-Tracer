#pragma once

#include "Vector.h"

struct Mat4;
inline Mat4 operator*(const Mat4& left, const Mat4& right);

struct Mat4{
    float cells[16];

    inline Mat4() {
		for (int i = 0; i < 16; i++){
            cells[i] = 0.0f;
        }
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

    inline static Mat4 create_scalar(float scale){
        Mat4 matrix = Mat4();
        matrix(0, 0) = scale;
        matrix(1, 1) = scale;
        matrix(2, 2) = scale; 

        return matrix;
    }

    inline static Mat4 create_rotation(Vector3 rotation){
        float x = rotation.x;
        float y = rotation.y;
        float z = rotation.z;
        // x rotation
        Mat4 x_matrix = Mat4();
        x_matrix(1, 1) = cos(x);
        x_matrix(2, 1) = -sin(x);
        x_matrix(1, 2) = sin(x);
        x_matrix(2, 2) = cos(x);

        // y rotation
        Mat4 y_matrix = Mat4();
        y_matrix(0, 0) = cos(y);
        y_matrix(2, 0) = sin(y);
        y_matrix(0, 2) = -sin(y);
        y_matrix(2, 2) = cos(y);

        // z rotation
        Mat4 z_matrix = Mat4();
        z_matrix(0, 0) = cos(z);
        z_matrix(0, 1) = -sin(z);
        z_matrix(1, 0) = sin(z);
        z_matrix(1, 1) = cos(z);

        return x_matrix * y_matrix * z_matrix;
    }


    inline static Vector3 transform_point(const Mat4& matrix, const Vector3& pos){
        Vector3 result;
        result.x = pos.x * matrix(0, 0) + pos.y * matrix(0, 1) + pos.z * matrix(0, 2) + matrix(0, 3);
        result.y = pos.x * matrix(1, 0) + pos.y * matrix(1, 1) + pos.z * matrix(1, 2) + matrix(1, 3);
        result.z = pos.x * matrix(2, 0) + pos.y * matrix(2, 1) + pos.z * matrix(2, 2) + matrix(2, 3);
        return result;
    }

    inline static Vector3 transform_direction(const Mat4& matrix, const Vector3& dir){
        Vector3 result;
        result.x = dir.x * matrix(0, 0) + dir.y * matrix(0, 1) + dir.z * matrix(0, 2);
        result.y = dir.x * matrix(1, 0) + dir.y * matrix(1, 1) + dir.z * matrix(1, 2);
        result.z = dir.x * matrix(2, 0) + dir.y * matrix(2, 1) + dir.z * matrix(2, 2);
        return result;
    }

    // using https://github.com/jan-van-bergen/CPU-Raytracer/blob/master/Matrix4.h
    // Which is based on: http://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
	inline static Mat4 invert(const Mat4& matrix) {		
		float inv[16] = {
			 matrix.cells[5] * matrix.cells[10] * matrix.cells[15] - matrix.cells[5]  * matrix.cells[11] * matrix.cells[14] - matrix.cells[9]  * matrix.cells[6] * matrix.cells[15] +
			 matrix.cells[9] * matrix.cells[7]  * matrix.cells[14] + matrix.cells[13] * matrix.cells[6]  * matrix.cells[11] - matrix.cells[13] * matrix.cells[7] * matrix.cells[10],
			-matrix.cells[1] * matrix.cells[10] * matrix.cells[15] + matrix.cells[1]  * matrix.cells[11] * matrix.cells[14] + matrix.cells[9]  * matrix.cells[2] * matrix.cells[15] -
			 matrix.cells[9] * matrix.cells[3]  * matrix.cells[14] - matrix.cells[13] * matrix.cells[2]  * matrix.cells[11] + matrix.cells[13] * matrix.cells[3] * matrix.cells[10],
			 matrix.cells[1] * matrix.cells[6]  * matrix.cells[15] - matrix.cells[1]  * matrix.cells[7]  * matrix.cells[14] - matrix.cells[5]  * matrix.cells[2] * matrix.cells[15] +
			 matrix.cells[5] * matrix.cells[3]  * matrix.cells[14] + matrix.cells[13] * matrix.cells[2]  * matrix.cells[7]  - matrix.cells[13] * matrix.cells[3] * matrix.cells[6],
			-matrix.cells[1] * matrix.cells[6]  * matrix.cells[11] + matrix.cells[1]  * matrix.cells[7]  * matrix.cells[10] + matrix.cells[5]  * matrix.cells[2] * matrix.cells[11] -
			 matrix.cells[5] * matrix.cells[3]  * matrix.cells[10] - matrix.cells[9]  * matrix.cells[2]  * matrix.cells[7]  + matrix.cells[9]  * matrix.cells[3] * matrix.cells[6],
			-matrix.cells[4] * matrix.cells[10] * matrix.cells[15] + matrix.cells[4]  * matrix.cells[11] * matrix.cells[14] + matrix.cells[8]  * matrix.cells[6] * matrix.cells[15] -
			 matrix.cells[8] * matrix.cells[7]  * matrix.cells[14] - matrix.cells[12] * matrix.cells[6]  * matrix.cells[11] + matrix.cells[12] * matrix.cells[7] * matrix.cells[10],
			 matrix.cells[0] * matrix.cells[10] * matrix.cells[15] - matrix.cells[0]  * matrix.cells[11] * matrix.cells[14] - matrix.cells[8]  * matrix.cells[2] * matrix.cells[15] +
			 matrix.cells[8] * matrix.cells[3]  * matrix.cells[14] + matrix.cells[12] * matrix.cells[2]  * matrix.cells[11] - matrix.cells[12] * matrix.cells[3] * matrix.cells[10],
			-matrix.cells[0] * matrix.cells[6]  * matrix.cells[15] + matrix.cells[0]  * matrix.cells[7]  * matrix.cells[14] + matrix.cells[4]  * matrix.cells[2] * matrix.cells[15] -
			 matrix.cells[4] * matrix.cells[3]  * matrix.cells[14] - matrix.cells[12] * matrix.cells[2]  * matrix.cells[7]  + matrix.cells[12] * matrix.cells[3] * matrix.cells[6],
			 matrix.cells[0] * matrix.cells[6]  * matrix.cells[11] - matrix.cells[0]  * matrix.cells[7]  * matrix.cells[10] - matrix.cells[4]  * matrix.cells[2] * matrix.cells[11] +
			 matrix.cells[4] * matrix.cells[3]  * matrix.cells[10] + matrix.cells[8]  * matrix.cells[2]  * matrix.cells[7]  - matrix.cells[8]  * matrix.cells[3] * matrix.cells[6],
			 matrix.cells[4] * matrix.cells[9]  * matrix.cells[15] - matrix.cells[4]  * matrix.cells[11] * matrix.cells[13] - matrix.cells[8]  * matrix.cells[5] * matrix.cells[15] +
			 matrix.cells[8] * matrix.cells[7]  * matrix.cells[13] + matrix.cells[12] * matrix.cells[5]  * matrix.cells[11] - matrix.cells[12] * matrix.cells[7] * matrix.cells[9],
			-matrix.cells[0] * matrix.cells[9]  * matrix.cells[15] + matrix.cells[0]  * matrix.cells[11] * matrix.cells[13] + matrix.cells[8]  * matrix.cells[1] * matrix.cells[15] -
			 matrix.cells[8] * matrix.cells[3]  * matrix.cells[13] - matrix.cells[12] * matrix.cells[1]  * matrix.cells[11] + matrix.cells[12] * matrix.cells[3] * matrix.cells[9],
			 matrix.cells[0] * matrix.cells[5]  * matrix.cells[15] - matrix.cells[0]  * matrix.cells[7]  * matrix.cells[13] - matrix.cells[4]  * matrix.cells[1] * matrix.cells[15] +
			 matrix.cells[4] * matrix.cells[3]  * matrix.cells[13] + matrix.cells[12] * matrix.cells[1]  * matrix.cells[7]  - matrix.cells[12] * matrix.cells[3] * matrix.cells[5],
			-matrix.cells[0] * matrix.cells[5]  * matrix.cells[11] + matrix.cells[0]  * matrix.cells[7]  * matrix.cells[9]  + matrix.cells[4]  * matrix.cells[1] * matrix.cells[11] -
			 matrix.cells[4] * matrix.cells[3]  * matrix.cells[9]  - matrix.cells[8]  * matrix.cells[1]  * matrix.cells[7]  + matrix.cells[8]  * matrix.cells[3] * matrix.cells[5],
			-matrix.cells[4] * matrix.cells[9]  * matrix.cells[14] + matrix.cells[4]  * matrix.cells[10] * matrix.cells[13] + matrix.cells[8]  * matrix.cells[5] * matrix.cells[14] -
			 matrix.cells[8] * matrix.cells[6]  * matrix.cells[13] - matrix.cells[12] * matrix.cells[5]  * matrix.cells[10] + matrix.cells[12] * matrix.cells[6] * matrix.cells[9],
			 matrix.cells[0] * matrix.cells[9]  * matrix.cells[14] - matrix.cells[0]  * matrix.cells[10] * matrix.cells[13] - matrix.cells[8]  * matrix.cells[1] * matrix.cells[14] +
			 matrix.cells[8] * matrix.cells[2]  * matrix.cells[13] + matrix.cells[12] * matrix.cells[1]  * matrix.cells[10] - matrix.cells[12] * matrix.cells[2] * matrix.cells[9],
			-matrix.cells[0] * matrix.cells[5]  * matrix.cells[14] + matrix.cells[0]  * matrix.cells[6]  * matrix.cells[13] + matrix.cells[4]  * matrix.cells[1] * matrix.cells[14] -
			 matrix.cells[4] * matrix.cells[2]  * matrix.cells[13] - matrix.cells[12] * matrix.cells[1]  * matrix.cells[6]  + matrix.cells[12] * matrix.cells[2] * matrix.cells[5],
			 matrix.cells[0] * matrix.cells[5]  * matrix.cells[10] - matrix.cells[0]  * matrix.cells[6]  * matrix.cells[9]  - matrix.cells[4]  * matrix.cells[1] * matrix.cells[10] +
			 matrix.cells[4] * matrix.cells[2]  * matrix.cells[9]  + matrix.cells[8]  * matrix.cells[1]  * matrix.cells[6]  - matrix.cells[8]  * matrix.cells[2] * matrix.cells[5]
		};
		
		Mat4 result;

		float det = 
			matrix.cells[0] * inv[0] + matrix.cells[1] * inv[4] + 
			matrix.cells[2] * inv[8] + matrix.cells[3] * inv[12];

		if (det != 0.0f) {
			const float inv_det = 1.0f / det;
			for (int i = 0; i < 16; i++) {
				result.cells[i] = inv[i] * inv_det;
			}
		}

		return result;
	}

    inline static Mat4 transpose(const Mat4& matrix){
        Mat4 result;
        for (int i = 0; i < 4; i++){
            for (int j = 0; j < 4; j++){ 
                result(i, j) = matrix(j, i);
            }
        }

        return result;
    }
};

inline Mat4 operator*(const Mat4 & left, const Mat4 & right) {
	Mat4 result;
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			result(i, j) = 
				left(i, 0) * right(0, j) +
				left(i, 1) * right(1, j) +
				left(i, 2) * right(2, j) +
				left(i, 3) * right(3, j);
		}
	}

	return result;
}