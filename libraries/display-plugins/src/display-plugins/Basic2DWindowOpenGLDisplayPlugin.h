//
//  Created by Bradley Austin Davis on 2015/05/29
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once

#include "MainWindowOpenGLDisplayPlugin.h"

class Basic2DWindowOpenGLDisplayPlugin : public MainWindowOpenGLDisplayPlugin {
    Q_OBJECT

public:
    virtual void activate() override;
    virtual void deactivate() override;

    virtual const QString & getName() const override;

private:
    static const QString NAME;
};
