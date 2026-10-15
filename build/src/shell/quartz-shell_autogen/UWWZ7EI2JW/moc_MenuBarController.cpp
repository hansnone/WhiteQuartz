/****************************************************************************
** Meta object code from reading C++ file 'MenuBarController.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.12.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/shell/menubar/MenuBarController.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MenuBarController.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN6Quartz5Shell17MenuBarControllerE_t {};
} // unnamed namespace

template <> constexpr inline auto Quartz::Shell::MenuBarController::qt_create_metaobjectdata<qt_meta_tag_ZN6Quartz5Shell17MenuBarControllerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Quartz::Shell::MenuBarController",
        "timeChanged",
        "",
        "activeAppTitleChanged",
        "altPressedChanged",
        "pressed",
        "notificationCenterToggled",
        "setActiveAppTitle",
        "title",
        "triggerAppleMenuAction",
        "actionId",
        "setAltPressed",
        "toggleNotificationCenter",
        "updateClock",
        "formattedTime",
        "activeAppTitle",
        "isAltPressed"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'timeChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'activeAppTitleChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'altPressedChanged'
        QtMocHelpers::SignalData<void(bool)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 5 },
        }}),
        // Signal 'notificationCenterToggled'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setActiveAppTitle'
        QtMocHelpers::SlotData<void(const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Slot 'triggerAppleMenuAction'
        QtMocHelpers::SlotData<void(const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Slot 'setAltPressed'
        QtMocHelpers::SlotData<void(bool)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 5 },
        }}),
        // Slot 'toggleNotificationCenter'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'updateClock'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'formattedTime'
        QtMocHelpers::PropertyData<QString>(14, QMetaType::QString, QMC::DefaultPropertyFlags, 0),
        // property 'activeAppTitle'
        QtMocHelpers::PropertyData<QString>(15, QMetaType::QString, QMC::DefaultPropertyFlags, 1),
        // property 'isAltPressed'
        QtMocHelpers::PropertyData<bool>(16, QMetaType::Bool, QMC::DefaultPropertyFlags, 2),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MenuBarController, qt_meta_tag_ZN6Quartz5Shell17MenuBarControllerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Quartz::Shell::MenuBarController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6Quartz5Shell17MenuBarControllerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6Quartz5Shell17MenuBarControllerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN6Quartz5Shell17MenuBarControllerE_t>.metaTypes,
    nullptr
} };

void Quartz::Shell::MenuBarController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MenuBarController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->timeChanged(); break;
        case 1: _t->activeAppTitleChanged(); break;
        case 2: _t->altPressedChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->notificationCenterToggled(); break;
        case 4: _t->setActiveAppTitle((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->triggerAppleMenuAction((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->setAltPressed((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 7: _t->toggleNotificationCenter(); break;
        case 8: _t->updateClock(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MenuBarController::*)()>(_a, &MenuBarController::timeChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (MenuBarController::*)()>(_a, &MenuBarController::activeAppTitleChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (MenuBarController::*)(bool )>(_a, &MenuBarController::altPressedChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (MenuBarController::*)()>(_a, &MenuBarController::notificationCenterToggled, 3))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->formattedTime(); break;
        case 1: *reinterpret_cast<QString*>(_v) = _t->activeAppTitle(); break;
        case 2: *reinterpret_cast<bool*>(_v) = _t->isAltPressed(); break;
        default: break;
        }
    }
}

const QMetaObject *Quartz::Shell::MenuBarController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Quartz::Shell::MenuBarController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6Quartz5Shell17MenuBarControllerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Quartz::Shell::MenuBarController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void Quartz::Shell::MenuBarController::timeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Quartz::Shell::MenuBarController::activeAppTitleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Quartz::Shell::MenuBarController::altPressedChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void Quartz::Shell::MenuBarController::notificationCenterToggled()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
