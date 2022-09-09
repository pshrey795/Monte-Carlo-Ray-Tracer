#include "../include/Mesh.hpp"

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, Vertex centralVertex, Material mat){
    this->vertices = vertices;
    this->mat = mat;
    this->centralVertex = centralVertex;
    this->isLight = !(mat.Ke == color3d(0,0,0));
    for(unsigned int i=0;i<indices.size();i+=3){
        point3d a = vertices[indices[i]].pos;
        point3d b = vertices[indices[i+1]].pos;
        point3d c = vertices[indices[i+2]].pos;
        vec3d centroid = (a + b + c) / 3;
        vec3d normal = unit_vec(cross(b-a, c-a));
        faces.push_back(Face(indices[i], indices[i+1], indices[i+2], normal, centroid));
    }
    nodeList = vector<BVHNode>(2*faces.size(), BVHNode());
    constructBVH();
}

//BVH Construction 
void Mesh::constructBVH(){
    //Populating the list of primitives(same as the list of faces initially)
    for(unsigned int i=0;i<faces.size();i++){
        primitives.push_back(i);
    }

    BVHNode& currNode = nodeList[currentNodeIndex];
    currNode.primCount = faces.size();
    numNodes++;
    updateBounds(currentNodeIndex);
    generateNodes(currentNodeIndex);
}

void Mesh::updateBounds(int nodeIndex){
    BVHNode& currNode = nodeList[nodeIndex];
    for(int i = currNode.firstPrim; i < currNode.primCount; i++){
        Face currFace = faces[primitives[i]];
        point3d a = vertices[currFace.v1].pos;
        point3d b = vertices[currFace.v2].pos;
        point3d c = vertices[currFace.v3].pos;
        currNode.bounds.nodeMin = min(min(min(currNode.bounds.nodeMin, a), b), c);
        currNode.bounds.nodeMax = max(max(max(currNode.bounds.nodeMax, a), b), c);
    }
}

double Mesh::surfaceAreaMeasure(BVHNode& currNode, int axis, double currPos){
    nodeBounds leftBound, rightBound;
    int leftCount = 0, rightCount = 0;
    for(int i=0;i<currNode.primCount;i++){
        Face& currFace = faces[primitives[currNode.firstPrim + i]];
        if(currFace.centroid[axis] < currPos){
            leftCount++;
            leftBound.expand(vertices[currFace.v1].pos);
            leftBound.expand(vertices[currFace.v2].pos);
            leftBound.expand(vertices[currFace.v3].pos);
        }else{
            rightCount++;
            rightBound.expand(vertices[currFace.v1].pos);
            rightBound.expand(vertices[currFace.v2].pos);
            rightBound.expand(vertices[currFace.v3].pos);
        }
    } 
    double cost = leftCount * leftBound.surfaceArea() + rightCount * rightBound.surfaceArea();
    return (cost > 0)? cost : 1e30;
}

void Mesh::generateNodes(int nodeIndex){
    BVHNode& currNode = nodeList[nodeIndex];

    if(currNode.primCount < MIN_PRIM_COUNT){
        //Recursion base case
        return;
    }else{
        //Surface Area Heuristic
        int splitAxis = -1;
        double splitPos = 0;
        double splitCost = DBL_MAX;

        for(int axis = 0; axis < 3; axis++){
            for(int i = 0; i < currNode.primCount; i++){
                Face& currFace = faces[primitives[currNode.firstPrim + i]];
                double currPos = currFace.centroid[axis];
                double currCost = surfaceAreaMeasure(currNode, axis, currPos);
                if(currCost < splitCost){
                    splitCost = currCost;
                    splitAxis = axis;
                    splitPos = currPos;
                }
            }
        }

        //Comparing the best cost of splitting with the cost of the parent for early termination 
        double parentCost = currNode.bounds.surfaceArea() * currNode.primCount;
        if(parentCost <= splitCost){
            return;
        }

        //Partition the faces (in place) into two sets 
        int i = currNode.firstPrim;
        int j = currNode.firstPrim + currNode.primCount - 1;
        while(i <= j){
            Face currFace = faces[primitives[i]];
            if(currFace.centroid[splitAxis] < splitPos){
                i++;
            }else{
                swap(primitives[i], primitives[j]);
                j--;
            }
        }

        //Creating the left and right nodes recursively 
        int leftPrimFirst = currNode.firstPrim;
        int leftPrimCount = i - currNode.firstPrim;
        int rightPrimFirst = i;
        int rightPrimCount = currNode.firstPrim + currNode.primCount - i;
        
        if(leftPrimCount == 0 || rightPrimCount == 0){
            //Occurs when the centroids of all the faces are on one side of the split plane
            //While the actual bounds of the plane are across the the plane
            //In this case, we just return since no new children need to be created 
            return;
        }

        //Creating and updating new nodes
        int leftNodeIndex = numNodes++;
        int rightNodeIndex = numNodes++;
        BVHNode& leftNode = nodeList[leftNodeIndex];
        BVHNode& rightNode = nodeList[rightNodeIndex];
        leftNode.firstPrim = leftPrimFirst;
        leftNode.primCount = leftPrimCount;
        updateBounds(leftNodeIndex);
        rightNode.firstPrim = rightPrimFirst;
        rightNode.primCount = rightPrimCount;
        updateBounds(rightNodeIndex);

        //Updating the current node
        currNode.leftChild = leftNodeIndex;
        currNode.rightChild = rightNodeIndex;

        //To differntiate between a child node and a leaf node 
        currNode.primCount = 0;

        //Induction Step of recursion 
        generateNodes(leftNodeIndex);
        generateNodes(rightNodeIndex);
    }
}

double Mesh::intersectTri(Ray r, Face f){
    //Moller Trumbore Algorithm
    point3d a = this->vertices[f.v1].pos;
    point3d b = this->vertices[f.v2].pos;
    point3d c = this->vertices[f.v3].pos;
    
    vec3d edge1 = b - a;
    vec3d edge2 = c - a;
    vec3d pVec = cross(r.direction, edge2);
    double M = dot(pVec, edge1);

    //Rejecting if the the ray is parallel to the triangle
    if(checkEquality(M, 0.0f)){
        return -1;
    }

    double invCosine = 1.0f / M;
    vec3d tVec = r.origin - a;
    double u = invCosine * dot(pVec, tVec);
    vec3d qVec = cross(tVec, edge1);
    double v = invCosine * dot(qVec, r.direction);

    //Rejecting if the intersection point is outside the triangle
    if(u < 0.0f || u > 1.0f || v < 0.0f || u + v > 1.0f){
        return -1;
    }

    double t = invCosine * dot(qVec, edge2);
    return t;
}

double Mesh::triArea(point3d a, point3d b, point3d c){
    vec3d v1 = b - a;
    vec3d v2 = c - a;
    vec3d v = cross(v1, v2);
    return 0.5 * v.length();
}

void Mesh::intersectBVH(Ray r, double t_min, double t_max, int mesh_index, hit_record &rec, int nodeIndex){
    BVHNode& currNode = nodeList[nodeIndex];
    bool isHit = intersectBox(r, t_min, t_max, currNode.bounds.nodeMin, currNode.bounds.nodeMax);
    if(isHit){
        if(currNode.primCount > 0){
            //Leaf Node, so we need to check all primitives manually 
            for(int i=currNode.firstPrim;i<currNode.firstPrim + currNode.primCount;i++){
                int faceIndex = primitives[i];
                double t = intersectTri(r, faces[faceIndex]);
                if(t>0){
                    if(t > t_min && t < t_max){
                        t_max = t;
                        rec.t = t;
                        rec.pt = r.origin + t * r.direction;
                        vec3d n = faces[faceIndex].normal;
                        double k = dot(r.direction,n);
                        rec.normal = (k<0)?n:-n;
                        rec.tangent = unit_vec(vertices[faces[faceIndex].v2].pos - vertices[faces[faceIndex].v1].pos);
                        rec.mat = this->mat;
                        rec.face_index = faceIndex;
                        rec.mesh_index = mesh_index;
                        rec.isHit = true;
                    }
                }
            }
        }else{
            intersectBVH(r, t_min, t_max, mesh_index, rec, currNode.leftChild);
            t_max = min(t_max, rec.t);
            intersectBVH(r, t_min, t_max, mesh_index, rec, currNode.rightChild);
        }
    }else{
        return;
    }
}