#ifndef MESH_H
#define MESH_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "vertex.h"

class UV_Mesh
{
public:
    std::vector<UV_Vertex> vertices;
    std::vector<unsigned int> indices;
    // constructor
    UV_Mesh(std::vector<UV_Vertex> vertices, std::vector<unsigned int> indices)
    {
        this->vertices = vertices;
        this->indices = indices;
    }
};

namespace Meshes
{
    UV_Mesh cube
    {
        std::vector<UV_Vertex>{
            // Front face
            {-0.5f, -0.5f,  0.5f,   0.0f, 0.0f},  // Bottom left  
            { 0.5f, -0.5f,  0.5f,   1.0f, 0.0f},  // Bottom right 
            { 0.5f,  0.5f,  0.5f,   1.0f, 1.0f},  // Top right    
            {-0.5f,  0.5f,  0.5f,   0.0f, 1.0f},  // Top left     
            
            // Back face
            {-0.5f, -0.5f, -0.5f,   0.0f, 0.0f},  // Bottom left  
            { 0.5f, -0.5f, -0.5f,   1.0f, 0.0f},  // Bottom right 
            { 0.5f,  0.5f, -0.5f,   1.0f, 1.0f},  // Top right    
            {-0.5f,  0.5f, -0.5f,   0.0f, 1.0f},  // Top left     

            // Left face
            {-0.5f, -0.5f, -0.5f,   0.0f, 0.0f},  // Bottom left
            {-0.5f, -0.5f,  0.5f,   1.0f, 0.0f},  // Bottom right
            {-0.5f,  0.5f,  0.5f,   1.0f, 1.0f},  // Top right
            {-0.5f,  0.5f, -0.5f,   0.0f, 1.0f},  // Top left

            // Right face
            { 0.5f, -0.5f, -0.5f,   0.0f, 0.0f},  // Bottom left
            { 0.5f, -0.5f,  0.5f,   1.0f, 0.0f},  // Bottom right
            { 0.5f,  0.5f,  0.5f,   1.0f, 1.0f},  // Top right
            { 0.5f,  0.5f, -0.5f,   0.0f, 1.0f},  // Top left

            // Top face
            {-0.5f,  0.5f, -0.5f,   0.0f, 0.0f},  // Bottom left
            { 0.5f,  0.5f, -0.5f,   1.0f, 0.0f},  // Bottom right
            { 0.5f,  0.5f,  0.5f,   1.0f, 1.0f},  // Top right
            {-0.5f,  0.5f,  0.5f,   0.0f, 1.0f},  // Top left

            // Bottom face
            {-0.5f, -0.5f, -0.5f,   0.0f, 0.0f},  // Bottom left
            { 0.5f, -0.5f, -0.5f,   1.0f, 0.0f},  // Bottom right
            { 0.5f, -0.5f,  0.5f,   1.0f, 1.0f},  // Top right
            {-0.5f, -0.5f,  0.5f,   0.0f, 1.0f}   // Top left
        },
        std::vector<unsigned int>{
            // Front
            0, 1, 2,   2, 3, 0,
            // Back
            4, 5, 6,   6, 7, 4,
            // Left
            8, 9, 10,  10, 11, 8,
            // Right
            12, 13, 14, 14, 15, 12,
            // Top
            16, 17, 18, 18, 19, 16,
            // Bottom
            20, 21, 22, 22, 23, 20
        }
    };
    UV_Mesh block
    {
        std::vector<UV_Vertex>{
            // Front face
            {-0.5f, -0.5f,  0.5f,   1.0f/3, 0.0f},  // Bottom left  
            { 0.5f, -0.5f,  0.5f,   2.0f/3, 0.0f},  // Bottom right 
            { 0.5f,  0.5f,  0.5f,   2.0f/3, 1.0f/4},  // Top right    
            {-0.5f,  0.5f,  0.5f,   1.0f/3, 1.0f/4},  // Top left     
            
            // Back face
            {-0.5f, -0.5f, -0.5f,   1.0f/3, 1.0f/4},  // Bottom left  
            { 0.5f, -0.5f, -0.5f,   2.0f/3, 1.0f/4},  // Bottom right 
            { 0.5f,  0.5f, -0.5f,   2.0f/3, 2.0f/4},  // Top right    
            {-0.5f,  0.5f, -0.5f,   1.0f/3, 2.0f/4},  // Top left     

            // Left face
            {-0.5f, -0.5f, -0.5f,   0.0f, 1.0f/4},  // Bottom left
            {-0.5f, -0.5f,  0.5f,   1.0f/3, 1.0f/4},  // Bottom right
            {-0.5f,  0.5f,  0.5f,   1.0f/3, 2.0f/4},  // Top right
            {-0.5f,  0.5f, -0.5f,   0.0f, 2.0f/4},  // Top left

            // Right face
            { 0.5f, -0.5f, -0.5f,   2.0f/3, 1.0f/4},  // Bottom left
            { 0.5f, -0.5f,  0.5f,   1.0f, 1.0f/4},  // Bottom right
            { 0.5f,  0.5f,  0.5f,   1.0f, 2.0f/4},  // Top right
            { 0.5f,  0.5f, -0.5f,   2.0f/3, 2.0f/4},  // Top left

            // Top face
            {-0.5f,  0.5f, -0.5f,   1.0f/3, 2.0f/4},  // Bottom left
            { 0.5f,  0.5f, -0.5f,   2.0f/3, 2.0f/4},  // Bottom right
            { 0.5f,  0.5f,  0.5f,   2.0f/3, 3.0f/4},  // Top right
            {-0.5f,  0.5f,  0.5f,   1.0f/3, 3.0f/4},  // Top left

            // Bottom face
            {-0.5f, -0.5f, -0.5f,   1.0f/3, 0.0f},  // Bottom left
            { 0.5f, -0.5f, -0.5f,   2.0f/3, 0.0f},  // Bottom right
            { 0.5f, -0.5f,  0.5f,   2.0f/3, 1.0f/4},  // Top right
            {-0.5f, -0.5f,  0.5f,   1.0f/3, 1.0f/4}   // Top left
        },
        std::vector<unsigned int>{
            // Front
            0, 1, 2,   2, 3, 0,
            // Back
            4, 5, 6,   6, 7, 4,
            // Left
            8, 9, 10,  10, 11, 8,
            // Right
            12, 13, 14, 14, 15, 12,
            // Top
            16, 17, 18, 18, 19, 16,
            // Bottom
            20, 21, 22, 22, 23, 20
        }
    };
}

#endif // MESH_H
