//
//  VoxelPacketProcessor.cpp
//  interface
//
//  Created by Brad Hefta-Gaub on 8/12/13.
//  Copyright (c) 2013 High Fidelity, Inc. All rights reserved.
//
//  Threaded or non-threaded voxel packet receiver for the Application
//

#include <PerfStat.h>

#include "Application.h"
#include "Menu.h"
#include "VoxelPacketProcessor.h"

VoxelPacketProcessor::VoxelPacketProcessor(Application* app) :
    _app(app) {
}

void VoxelPacketProcessor::processPacket(sockaddr& senderAddress, unsigned char* packetData, ssize_t packetLength) {
    PerformanceWarning warn(Menu::getInstance()->isOptionChecked(MenuOption::PipelineWarnings),
                            "VoxelPacketProcessor::processPacket()");
    ssize_t messageLength = packetLength;

    // check to see if the UI thread asked us to kill the voxel tree. since we're the only thread allowed to do that
    if (_app->_wantToKillLocalVoxels) {
        _app->_voxels.killLocalVoxels();
        _app->_wantToKillLocalVoxels = false;
    }
    
    // note: PACKET_TYPE_VOXEL_STATS can have PACKET_TYPE_VOXEL_DATA or PACKET_TYPE_VOXEL_DATA_MONOCHROME
    // immediately following them inside the same packet. So, we process the PACKET_TYPE_VOXEL_STATS first
    // then process any remaining bytes as if it was another packet
    if (packetData[0] == PACKET_TYPE_VOXEL_STATS) {
    
        int statsMessageLength = _app->parseVoxelStats(packetData, messageLength, senderAddress);
        if (messageLength > statsMessageLength) {
            packetData += statsMessageLength;
            messageLength -= statsMessageLength;
            if (!packetVersionMatch(packetData)) {
                return; // bail since piggyback data doesn't match our versioning
            }
        } else {
            return; // bail since no piggyback data
        }
    } // fall through to piggyback message

    if (Menu::getInstance()->isOptionChecked(MenuOption::Voxels)) {
        Node* voxelServer = NodeList::getInstance()->nodeWithAddress(&senderAddress);
        if (voxelServer && socketMatch(voxelServer->getActiveSocket(), &senderAddress)) {
            voxelServer->lock();
            if (packetData[0] == PACKET_TYPE_ENVIRONMENT_DATA) {
                _app->_environment.parseData(&senderAddress, packetData, messageLength);
            } else {
                _app->_voxels.setDataSourceID(voxelServer->getNodeID());
                _app->_voxels.parseData(packetData, messageLength);
                _app->_voxels.setDataSourceID(UNKNOWN_NODE_ID);
            }
            voxelServer->unlock();
        }
    }
}

