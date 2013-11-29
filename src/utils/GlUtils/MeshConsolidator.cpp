#include <MeshConsolidator.hpp>
#include <GlUtilsException.hpp>
#include <cstring>
#include <cstdlib>

using namespace GlUtils;

//----------------------------------------------------------------------------------------
/**
 * Default constructor
 */
MeshConsolidator::MeshConsolidator()
        : totalPositionBytes(0),
          totalNormalBytes(0),
          vertexPositionDataPtr_head(nullptr),
          vertexPositionDataPtr_tail(nullptr),
          normalDataPtr_head(nullptr),
          normalDataPtr_tail(nullptr) { }


//----------------------------------------------------------------------------------------
/**
 * Constructs a \c MeshConsolidator object from an \c unordered_map with keys equal to
 * c-string identifiers, and mapped values equal to Mesh pointers.
 *
 * @param meshMap
 */
MeshConsolidator::MeshConsolidator(initializer_list<pair<const char *, const Mesh *> > list)
        : totalPositionBytes(0), totalNormalBytes(0) {

    unordered_map<const char *, const Mesh *> meshMap;
    for(auto key_value : list) {
        meshMap[key_value.first] = key_value.second;
    }

    processMeshes(meshMap);
}

//----------------------------------------------------------------------------------------
/**
 * Constructs a \c MeshConsolidator object from an \c unordered_map with keys equal to
 * c-string identifiers, and mapped values equal to Wavefront .obj file names.
 *
 * @param meshMap
 */
MeshConsolidator::MeshConsolidator(initializer_list<pair<const char *, const char *> > list)
        : totalPositionBytes(0), totalNormalBytes(0) {

    // Need to keep Mesh objects in memory for processing until the end of this block.
    // Use vector<shared_ptr<Mesh>> as memory requirements could be large for some Meshes.
    // Meshes will auto-destruct at the end of this method when vector goes out of scope.
    vector<shared_ptr<Mesh> > meshVector;
    meshVector.resize(list.size());

    unordered_map<const char *, const Mesh *> meshMap;
    int i = 0;
    for(auto key_value : list) {
        const char * meshId = key_value.first;
        const char * meshFileName = key_value.second;
        meshVector[i] = make_shared<Mesh>(meshFileName);
        meshMap[meshId] = meshVector[i].get();
        i++;
    }

    processMeshes(meshMap);
}

//----------------------------------------------------------------------------------------
void MeshConsolidator::processMeshes(const unordered_map<const char *, const Mesh *> & meshMap) {

    // Calculate the total number of bytes for both vertex and normal data.
    for(auto key_value: meshMap) {
        const Mesh & mesh = *(key_value.second);
        totalPositionBytes += mesh.getNumVertexPositionBytes();
        totalNormalBytes += mesh.getNumVertexNormalBytes();
    }

    // Allocate memory for vertex position data.
    vertexPositionDataPtr_head = shared_ptr<float>((float *)malloc(totalPositionBytes), free);
    if (vertexPositionDataPtr_head.get() == (float *)0) {
        throw GlUtilsException("Unable to allocate system memory within method MeshConsolidator::processMeshes");
    }

    // Allocate memory for normal data.
    normalDataPtr_head = shared_ptr<float>((float *)malloc(totalNormalBytes), free);
    if (vertexPositionDataPtr_head.get() == (float *)0) {
        throw GlUtilsException("Unable to allocate system memory within method MeshConsolidator::processMeshes");
    }

    // Assign pointers to beginning of memory blocks.
    vertexPositionDataPtr_tail = vertexPositionDataPtr_head.get();
    normalDataPtr_tail = normalDataPtr_head.get();

    for(auto key_value : meshMap) {
        const char * meshId = key_value.first;
        const Mesh & mesh = *(key_value.second);
        consolidateMesh(meshId, mesh);
    }
}

//----------------------------------------------------------------------------------------
MeshConsolidator::~MeshConsolidator() {
    // All resources auto freed by shared pointers.
}

//----------------------------------------------------------------------------------------
void MeshConsolidator::consolidateMesh(const char * meshId, const Mesh & mesh) {
    unsigned int startIndex = (vertexPositionDataPtr_tail - vertexPositionDataPtr_head.get()) / num_floats_per_vertex;
    unsigned int numIndices = mesh.getNumVertexPositions();

    memcpy(vertexPositionDataPtr_tail, mesh.getVertexPositionDataPtr(), mesh.getNumVertexPositionBytes());
    vertexPositionDataPtr_tail += mesh.getNumVertexPositionBytes() / sizeof(float);

    memcpy(normalDataPtr_tail, mesh.getVertexNormalDataPtr(), mesh.getNumVertexNormalBytes());
    normalDataPtr_tail += mesh.getNumVertexNormalBytes() / sizeof(float);

    batchInfoMap[meshId] = BatchInfo(startIndex, numIndices);
}

//----------------------------------------------------------------------------------------
/**
 * Appends to the back of \c outVector a series of \c BatchInfo objects that
 * specify the starting index location of each Mesh object's data (e.g. vertices
 * and normals) within contiguous memory, and the number of data elements each
 * Mesh is composed of.
 *
 * Each \c BatchInfo object corresponds to the data layout for a single \c Mesh
 * object, and the order of the appended \c BatchInfo objects matches the order of
 * the \c MeshConsolidator constructor arguments.
 *
 * @param outVector
 */
void MeshConsolidator::getBatchInfo(unordered_map<const char *, BatchInfo> & batchInfoMap) const {
    for(const auto & key_value : this->batchInfoMap) {
        const char * meshId = key_value.first;
        BatchInfo batchInfo = key_value.second;
        batchInfoMap[meshId] = batchInfo;
    }
}

//----------------------------------------------------------------------------------------
/**
 * @return the starting memory location for all consolidated \c Mesh vertex data.
 */
const float * MeshConsolidator::getVertexPositionDataPtr() const {
    return vertexPositionDataPtr_head.get();
}

//----------------------------------------------------------------------------------------
/**
 * @return the starting memory location for all consolidated \c Mesh normal data.
 */
const float * MeshConsolidator::getVertexNormalDataPtr() const {
    return normalDataPtr_head.get();
}

//----------------------------------------------------------------------------------------
/**
 * @return the total number of bytes of all consolidated \c Mesh vertex data.
 */
unsigned long MeshConsolidator::getNumVertexPositionBytes() const {
    return totalPositionBytes;
}

//----------------------------------------------------------------------------------------
/**
 * @return the total number of bytes of all consolidated \c Mesh normal data.
 */
unsigned long MeshConsolidator::getNumVertexNormalBytes() const {
    return totalNormalBytes;
}
