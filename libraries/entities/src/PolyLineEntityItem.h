//
//  PolyLineEntityItem.h
//  libraries/entities/src
//
//  Created by Eric Levin on 8/3/15.
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_PolyLineEntityItem_h
#define hifi_PolyLineEntityItem_h

#include "EntityItem.h" 

class PolyLineEntityItem : public EntityItem {
 public:
    static EntityItemPointer factory(const EntityItemID& entityID, const EntityItemProperties& properties);

    PolyLineEntityItem(const EntityItemID& entityItemID, const EntityItemProperties& properties);
    
    ALLOW_INSTANTIATION // This class can be instantiated
    
    // methods for getting/setting all properties of an entity
    virtual EntityItemProperties getProperties() const;
    virtual bool setProperties(const EntityItemProperties& properties);

    // TODO: eventually only include properties changed since the params.lastViewFrustumSent time
    virtual EntityPropertyFlags getEntityProperties(EncodeBitstreamParams& params) const;

    virtual void appendSubclassData(OctreePacketData* packetData, EncodeBitstreamParams& params, 
                                    EntityTreeElementExtraEncodeData* modelTreeElementExtraEncodeData,
                                    EntityPropertyFlags& requestedProperties,
                                    EntityPropertyFlags& propertyFlags,
                                    EntityPropertyFlags& propertiesDidntFit,
                                    int& propertyCount, 
                                    OctreeElement::AppendState& appendState) const;

    virtual int readEntitySubclassDataFromBuffer(const unsigned char* data, int bytesLeftToRead, 
                                                 ReadBitstreamToTreeParams& args,
                                                 EntityPropertyFlags& propertyFlags, bool overwriteLocalData);

    const rgbColor& getColor() const { return _color; }
    xColor getXColor() const { xColor color = { _color[RED_INDEX], _color[GREEN_INDEX], _color[BLUE_INDEX] }; return color; }

    void setColor(const rgbColor& value) {
        memcpy(_color, value, sizeof(_color));
    }
    void setColor(const xColor& value) {
        
        _color[RED_INDEX] = value.red;
        _color[GREEN_INDEX] = value.green;
        _color[BLUE_INDEX] = value.blue;
    }
    
    void setLineWidth(float lineWidth){ _lineWidth = lineWidth; }
    float getLineWidth() const{ return _lineWidth; }
    
    bool setLinePoints(const QVector<glm::vec3>& points);
    bool appendPoint(const glm::vec3& point);
    const QVector<glm::vec3>& getLinePoints() const{ return _points; }
    
    bool setNormals(const QVector<glm::vec3>& normals);
    const QVector<glm::vec3>& getNormals() const{ return _normals; }
    
    bool setStrokeWidths(const QVector<float>& strokeWidths);
    const QVector<float>& getStrokeWidths() const{ return _strokeWidths; }
    
    
    virtual ShapeType getShapeType() const { return SHAPE_TYPE_LINE; }

    // never have a ray intersection pick a PolyLineEntityItem.
    virtual bool supportsDetailedRayIntersection() const { return true; }
    virtual bool findDetailedRayIntersection(const glm::vec3& origin, const glm::vec3& direction,
                         bool& keepSearching, OctreeElement*& element, float& distance, BoxFace& face, 
                         void** intersectedObject, bool precisionPicking) const { return false; }

    virtual void debugDump() const;
    static const float DEFAULT_LINE_WIDTH;
    static const int MAX_POINTS_PER_LINE;

 protected:
    rgbColor _color;
    float _lineWidth;
    bool _pointsChanged;
    QVector<glm::vec3> _points;
    QVector<glm::vec3> _vertices;
    QVector<glm::vec3> _normals;
    QVector<float> _strokeWidths;
    mutable QReadWriteLock _quadReadWriteLock;
};

#endif // hifi_PolyLineEntityItem_h
