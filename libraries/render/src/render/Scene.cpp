//
//  Scene.cpp
//  render/src/render
//
//  Created by Sam Gateau on 1/11/15.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "Scene.h"

using namespace render;

void ItemBucketMap::insert(const ItemID& id, const ItemKey& key) {
    // Insert the itemID in every bucket where it filters true
    for (auto& bucket : (*this)) {
        if (bucket.first.test(key)) {
            bucket.second.insert(id);
        }
    }
}
void ItemBucketMap::erase(const ItemID& id, const ItemKey& key) {
    // Remove the itemID in every bucket where it filters true
    for (auto& bucket : (*this)) {
        if (bucket.first.test(key)) {
            bucket.second.erase(id);
        }
    }
}

void ItemBucketMap::reset(const ItemID& id, const ItemKey& oldKey, const ItemKey& newKey) {
    // Reset the itemID in every bucket,
    // Remove from the buckets where oldKey filters true AND newKey filters false
    // Insert into the buckets where newKey filters true
    for (auto& bucket : (*this)) {
        if (bucket.first.test(oldKey)) {
            if (!bucket.first.test(newKey)) {
                bucket.second.erase(id);
            }
        } else if (bucket.first.test(newKey)) {
            bucket.second.insert(id);
        }
    }
}

void ItemBucketMap::allocateStandardOpaqueTranparentBuckets() {
    (*this)[ItemFilter::Builder::opaqueShape()];
    (*this)[ItemFilter::Builder::transparentShape()];
}


void Item::resetPayload(const PayloadPointer& payload) {
    if (!payload) {
        kill();
    } else {
        _payload = payload;
        _key = _payload->getKey();
    }
}

void Item::kill() {
    _payload.reset();
    _key._flags.reset();
}

void Item::move() {
    
}

void PendingChanges::resetItem(ItemID id, const PayloadPointer& payload) {
    _resetItems.push_back(id);
    _resetPayloads.push_back(payload);
}

void PendingChanges::removeItem(ItemID id) {
    _removedItems.push_back(id);
}

void PendingChanges::moveItem(ItemID id) {
    _movedItems.push_back(id);
}

        
void PendingChanges::merge(PendingChanges& changes) {
    _resetItems.insert(_resetItems.end(), changes._resetItems.begin(), changes._resetItems.end());
    _resetPayloads.insert(_resetPayloads.end(), changes._resetPayloads.begin(), changes._resetPayloads.end());
    _removedItems.insert(_removedItems.end(), changes._removedItems.begin(), changes._removedItems.end());
    _movedItems.insert(_movedItems.end(), changes._movedItems.begin(), changes._movedItems.end());
}

Scene::Scene() {
    _items.push_back(Item()); // add the itemID #0 to nothing
    _masterBucketMap.allocateStandardOpaqueTranparentBuckets();
}

ItemID Scene::allocateID() {
    // Just increment and return the proevious value initialized at 0
    return _IDAllocator.fetch_add(1);
}

/// Enqueue change batch to the scene
void Scene::enqueuePendingChanges(const PendingChanges& pendingChanges) {
    _changeQueueMutex.lock();
    _changeQueue.push(pendingChanges);
    _changeQueueMutex.unlock();
}

void consolidateChangeQueue(PendingChangesQueue& queue, PendingChanges& singleBatch) {
    while (!queue.empty()) {
        auto pendingChanges = queue.front();
        singleBatch.merge(pendingChanges);
        queue.pop();
    };
}
 
void Scene::processPendingChangesQueue() {
    _changeQueueMutex.lock();
    PendingChanges consolidatedPendingChanges;
    consolidateChangeQueue(_changeQueue, consolidatedPendingChanges);
    _changeQueueMutex.unlock();
    
    _itemsMutex.lock();
        // Here we should be able to check the value of last ItemID allocated 
        // and allocate new items accordingly
        ItemID maxID = _IDAllocator.load();
        if (maxID > _items.size()) {
            _items.resize(maxID + 100); // allocate the maxId and more
        }
        // Now we know for sure that we have enough items in the array to
        // capture anything coming from the pendingChanges
        resetItems(consolidatedPendingChanges._resetItems, consolidatedPendingChanges._resetPayloads);
        removeItems(consolidatedPendingChanges._removedItems);
        moveItems(consolidatedPendingChanges._movedItems);

     // ready to go back to rendering activities
    _itemsMutex.unlock();
}

void Scene::resetItems(const ItemIDs& ids, Payloads& payloads) {
    auto resetID = ids.begin();
    auto resetPayload = payloads.begin();
    for (;resetID != ids.end(); resetID++, resetPayload++) {
        auto& item = _items[(*resetID)];
        auto oldKey = item.getKey();
        item.resetPayload(*resetPayload);

        _masterBucketMap.reset((*resetID), oldKey, item.getKey());
    }

}

void Scene::removeItems(const ItemIDs& ids) {
    for (auto removedID :ids) {
        _masterBucketMap.erase(removedID, _items[removedID].getKey());
        _items[removedID].kill();
    }
}

void Scene::moveItems(const ItemIDs& ids) {
    for (auto movedID :ids) {
        _items[movedID].move();
    }
}

void Scene::registerObserver(ObserverPointer& observer) {
    // make sure it's a valid observer
    if (observer && (observer->getScene() == nullptr)) {
        // Then register the observer
        _observers.push_back(observer);

        // And let it do what it wants to do
        observer->registerScene(this);
    }
}

void Scene::unregisterObserver(ObserverPointer& observer) {
    // make sure it's a valid observer currently registered
    if (observer && (observer->getScene() == this)) {
        // let it do what it wants to do
        observer->unregisterScene();

        // Then unregister the observer
        auto it = std::find(_observers.begin(), _observers.end(), observer);
        _observers.erase(it);
    }
}
