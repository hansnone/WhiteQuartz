/****************************************************************************
** Meta object code from reading C++ file 'stagemanagereffect.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.12.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/compositor/stagemanagereffect.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'stagemanagereffect.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.12.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN6Quartz10Compositor18StageManagerEffectE_t {};
} // unnamed namespace

template <> constexpr inline auto Quartz::Compositor::StageManagerEffect::qt_create_metaobjectdata<qt_meta_tag_ZN6Quartz10Compositor18StageManagerEffectE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Quartz::Compositor::StageManagerEffect",
        "D-Bus Interface",
        "org.quartz.Compositor.StageManager",
        "stageGroupActivated",
        "",
        "groupId",
        "stateChanged",
        "enabled",
        "spatialModeChanged",
        "newMode",
        "setEnabled",
        "isEnabledSlot",
        "activateStageGroup",
        "addWindowToCurrentStage",
        "windowInternalId",
        "activeStageCount",
        "toggleMissionControl",
        "toggleAppExpose",
        "activeAppId"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'stageGroupActivated'
        QtMocHelpers::SignalData<void(const QString &)>(3, 4, QMC::AccessPublic | QMC::MethodScriptable, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Signal 'stateChanged'
        QtMocHelpers::SignalData<void(bool)>(6, 4, QMC::AccessPublic | QMC::MethodScriptable, QMetaType::Void, {{
            { QMetaType::Bool, 7 },
        }}),
        // Signal 'spatialModeChanged'
        QtMocHelpers::SignalData<void(int)>(8, 4, QMC::AccessPublic | QMC::MethodScriptable, QMetaType::Void, {{
            { QMetaType::Int, 9 },
        }}),
        // Slot 'setEnabled'
        QtMocHelpers::SlotData<void(bool)>(10, 4, QMC::AccessPublic | QMC::MethodScriptable, QMetaType::Void, {{
            { QMetaType::Bool, 7 },
        }}),
        // Slot 'isEnabledSlot'
        QtMocHelpers::SlotData<bool() const>(11, 4, QMC::AccessPublic | QMC::MethodScriptable, QMetaType::Bool),
        // Slot 'activateStageGroup'
        QtMocHelpers::SlotData<void(const QString &)>(12, 4, QMC::AccessPublic | QMC::MethodScriptable, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Slot 'addWindowToCurrentStage'
        QtMocHelpers::SlotData<void(const QString &)>(13, 4, QMC::AccessPublic | QMC::MethodScriptable, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Slot 'activeStageCount'
        QtMocHelpers::SlotData<int() const>(15, 4, QMC::AccessPublic | QMC::MethodScriptable, QMetaType::Int),
        // Slot 'toggleMissionControl'
        QtMocHelpers::SlotData<void()>(16, 4, QMC::AccessPublic | QMC::MethodScriptable, QMetaType::Void),
        // Slot 'toggleAppExpose'
        QtMocHelpers::SlotData<void(const QString &)>(17, 4, QMC::AccessPublic | QMC::MethodScriptable, QMetaType::Void, {{
            { QMetaType::QString, 18 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    QtMocHelpers::UintData qt_constructors {};
    QtMocHelpers::ClassInfos qt_classinfo({
            {    1,    2 },
    });
    return QtMocHelpers::metaObjectData<StageManagerEffect, qt_meta_tag_ZN6Quartz10Compositor18StageManagerEffectE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums, qt_constructors, qt_classinfo);
}
Q_CONSTINIT const QMetaObject Quartz::Compositor::StageManagerEffect::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6Quartz10Compositor18StageManagerEffectE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6Quartz10Compositor18StageManagerEffectE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN6Quartz10Compositor18StageManagerEffectE_t>.metaTypes,
    nullptr
} };

void Quartz::Compositor::StageManagerEffect::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<StageManagerEffect *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->stageGroupActivated((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->stateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->spatialModeChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->setEnabled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 4: { bool _r = _t->isEnabledSlot();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 5: _t->activateStageGroup((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->addWindowToCurrentStage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: { int _r = _t->activeStageCount();
            if (_a[0]) *reinterpret_cast<int*>(_a[0]) = std::move(_r); }  break;
        case 8: _t->toggleMissionControl(); break;
        case 9: _t->toggleAppExpose((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (StageManagerEffect::*)(const QString & )>(_a, &StageManagerEffect::stageGroupActivated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (StageManagerEffect::*)(bool )>(_a, &StageManagerEffect::stateChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (StageManagerEffect::*)(int )>(_a, &StageManagerEffect::spatialModeChanged, 2))
            return;
    }
}

const QMetaObject *Quartz::Compositor::StageManagerEffect::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Quartz::Compositor::StageManagerEffect::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6Quartz10Compositor18StageManagerEffectE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Quartz::Compositor::StageManagerEffect::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void Quartz::Compositor::StageManagerEffect::stageGroupActivated(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void Quartz::Compositor::StageManagerEffect::stateChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void Quartz::Compositor::StageManagerEffect::spatialModeChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
